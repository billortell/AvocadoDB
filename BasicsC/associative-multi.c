////////////////////////////////////////////////////////////////////////////////
/// @brief associative multi array implementation
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Martin Schoenert
/// @author Copyright 2006-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "associative-multi.h"


// -----------------------------------------------------------------------------
// --SECTION--                                                 ASSOCIATIVE ARRAY
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a new element
////////////////////////////////////////////////////////////////////////////////

static void AddNewElement(TRI_multi_array_t* array, void* element) {
  uint64_t hash;
  uint64_t i;
  

  // ...........................................................................
  // compute the hash
  // ...........................................................................

  hash = array->hashElement(array, element);


  // ...........................................................................
  // search the table
  // ...........................................................................

  i = hash % array->_nrAlloc;
  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesR++;
  }


  // ...........................................................................
  // add a new element to the associative array
  // ...........................................................................

  memcpy(array->_table + i * array->_elementSize, element, array->_elementSize);
  array->_nrUsed++;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief resizes the array
////////////////////////////////////////////////////////////////////////////////

static void ResizeMultiArray (TRI_multi_array_t* array) {
  char* oldTable;
  uint64_t oldAlloc;
  uint64_t j;

  oldTable = array->_table;
  oldAlloc = array->_nrAlloc;

  array->_nrAlloc = 2 * array->_nrAlloc + 1;
  array->_nrUsed = 0;
  array->_nrResizes++;

  array->_table = TRI_Allocate(array->_nrAlloc * array->_elementSize);

  for (j = 0; j < array->_nrAlloc; j++) {
    array->clearElement(array, array->_table + j * array->_elementSize);
  }

  for (j = 0; j < oldAlloc; j++) {
    if (! array->isEmptyElement(array, oldTable + j * array->_elementSize)) {
      AddNewElement(array, oldTable + j * array->_elementSize);
    }
  }

  TRI_Free(oldTable);
}


////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialises an array
////////////////////////////////////////////////////////////////////////////////

void TRI_InitMultiArray(TRI_multi_array_t* array,
                        size_t elementSize,
                        uint64_t (*hashKey) (TRI_multi_array_t*, void*),
                        uint64_t (*hashElement) (TRI_multi_array_t*, void*),
                        void (*clearElement) (TRI_multi_array_t*, void*),
                        bool (*isEmptyElement) (TRI_multi_array_t*, void*),
                        bool (*isEqualKeyElement) (TRI_multi_array_t*, void*, void*),
                        bool (*isEqualElementElement) (TRI_multi_array_t*, void*, void*)) {
  char* p;
  char* e;

  array->hashKey = hashKey;
  array->hashElement = hashElement;
  array->clearElement = clearElement;
  array->isEmptyElement = isEmptyElement;
  array->isEqualKeyElement = isEqualKeyElement;
  array->isEqualElementElement = isEqualElementElement;

  array->_elementSize = elementSize;
  array->_nrAlloc = 10;

  array->_table = TRI_Allocate(array->_elementSize * array->_nrAlloc);

  p = array->_table;
  e = p + array->_elementSize * array->_nrAlloc;

  for (;  p < e;  p += array->_elementSize) {
    array->clearElement(array, p);
  }

  array->_nrUsed = 0;
  array->_nrFinds = 0;
  array->_nrAdds = 0;
  array->_nrRems = 0;
  array->_nrResizes = 0;
  array->_nrProbesF = 0;
  array->_nrProbesA = 0;
  array->_nrProbesD = 0;
  array->_nrProbesR = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array, but does not free the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyMultiArray (TRI_multi_array_t* array) {
  TRI_Free(array->_table);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array and frees the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeMultiArray (TRI_multi_array_t* array) {
  TRI_DestroyMultiArray(array);
  TRI_Free(array);
}


////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief lookups an element given a key
////////////////////////////////////////////////////////////////////////////////

TRI_vector_pointer_t TRI_LookupByKeyMultiArray (TRI_multi_array_t* array, void* key) {
  TRI_vector_pointer_t result;
  uint64_t hash;
  uint64_t i;

  
  // ...........................................................................
  // initialise the vector which will hold the result if any
  // ...........................................................................
 
   TRI_InitVectorPointer(&result);
   
   
  // ...........................................................................
  // compute the hash
  // ...........................................................................

  hash = array->hashKey(array, key);
  i = hash % array->_nrAlloc;

  
  // ...........................................................................
  // update statistics
  // ...........................................................................

  array->_nrFinds++;

  
  // ...........................................................................
  // search the table
  // ...........................................................................

  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
  
    if (array->isEqualKeyElement(array, key, array->_table + i * array->_elementSize)) {
      TRI_PushBackVectorPointer(&result, array->_table + i * array->_elementSize);             
    }
    else {
      array->_nrProbesF++;
    }      

    i = (i + 1) % array->_nrAlloc;
  }

  // ...........................................................................
  // return whatever we found -- which could be an empty vector list if nothing
  // matches.
  // ...........................................................................
  return result;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief lookups an element given an element
////////////////////////////////////////////////////////////////////////////////

TRI_vector_pointer_t TRI_LookupByElementMultiArray (TRI_multi_array_t* array, void* element) {
  TRI_vector_pointer_t result;
  uint64_t hash;
  uint64_t i;

  
  // ...........................................................................
  // initialise the vector which will hold the result if any
  // ...........................................................................
 
   TRI_InitVectorPointer(&result);
   
   
  // ...........................................................................
  // compute the hash
  // ...........................................................................

  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;

  
  // ...........................................................................
  // update statistics
  // ...........................................................................

  array->_nrFinds++;

  
  // ...........................................................................
  // search the table
  // ...........................................................................

  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
  
    if (array->isEqualElementElement(array, element, array->_table + i * array->_elementSize)) {
      TRI_PushBackVectorPointer(&result, array->_table + i * array->_elementSize);             
    }
    else {
      array->_nrProbesF++;
    }      

    i = (i + 1) % array->_nrAlloc;
  }

  // ...........................................................................
  // return whatever we found -- which could be an empty vector list if nothing
  // matches. Note that we allow multiple elements (compare with pointer impl).
  // ...........................................................................
  return result;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief adds an element to the array
////////////////////////////////////////////////////////////////////////////////

bool TRI_InsertElementMultiArray (TRI_multi_array_t* array, void* element, bool overwrite) {

  uint64_t hash;
  uint64_t i;

  
  // ...........................................................................
  // compute the hash
  // ...........................................................................

  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;

  
  // ...........................................................................
  // update statistics
  // ...........................................................................

  array->_nrAdds++;

  
  
  // ...........................................................................
  // search the table
  // ...........................................................................

  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)
         && ! array->isEqualElementElement(array, element, array->_table + i * array->_elementSize)) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesA++;
  }

  // ...........................................................................
  // If we found an element, return. While we allow duplicate entries in the hash
  // table, we do not allow duplicate elements. Elements would refer to the 
  // (for example) an actual row in memory. This is different from the 
  // TRI_InsertElementMultiArray function below where we only have keys to
  // differentiate between elements.
  // ...........................................................................
  if (! array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
    if (overwrite) {
      memcpy(array->_table + i * array->_elementSize, element, array->_elementSize);
    }

    return false;
  }

  
  // ...........................................................................
  // add a new element to the associative array
  // ...........................................................................

  memcpy(array->_table + i * array->_elementSize, element, array->_elementSize);
  array->_nrUsed++;


  // ...........................................................................
  // if we were adding and the table is more than half full, extend it
  // ...........................................................................

  if (array->_nrAlloc < 2 * array->_nrUsed) {
    ResizeMultiArray(array);
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds an key/element to the array
////////////////////////////////////////////////////////////////////////////////

bool TRI_InsertKeyMultiArray (TRI_multi_array_t* array, void* key, void* element, bool overwrite) {
  uint64_t hash;
  uint64_t i;

  
  // ...........................................................................
  // compute the hash
  // ...........................................................................

  hash = array->hashKey(array, key);
  i = hash % array->_nrAlloc;

  
  // ...........................................................................
  // update statistics
  // ...........................................................................

  array->_nrAdds++;

  
  // ...........................................................................
  // search the table
  // ...........................................................................

  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesA++;
  }

  // ...........................................................................
  // We do not look for an element (as opposed to the function above). So whether
  // or not there exists a duplicate we do not care.
  // ...........................................................................


  // ...........................................................................
  // add a new element to the associative array
  // ...........................................................................

  memcpy(array->_table + i * array->_elementSize, element, array->_elementSize);
  array->_nrUsed++;


  // ...........................................................................
  // if we were adding and the table is more than half full, extend it
  // ...........................................................................

  if (array->_nrAlloc < 2 * array->_nrUsed) {
    ResizeMultiArray(array);
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief removes an element from the array
////////////////////////////////////////////////////////////////////////////////

bool TRI_RemoveElementMultiArray (TRI_multi_array_t* array, void* element, void* old) {
  uint64_t hash;
  uint64_t i;
  uint64_t k;

  
  // ...........................................................................
  // Obtain the hash
  // ...........................................................................
  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;


  // ...........................................................................
  // update statistics
  // ...........................................................................
  array->_nrRems++;

  
  // ...........................................................................
  // search the table
  // ...........................................................................
  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)
         && ! array->isEqualElementElement(array, element, array->_table + i * array->_elementSize)) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesD++;
  }


  // ...........................................................................
  // if we did not find such an item return false
  // ...........................................................................
  if (array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
    if (old != NULL) {
      memset(old, 0, array->_elementSize);
    }

    return false;
  }


  // ...........................................................................
  // remove item
  // ...........................................................................
  if (old != NULL) {
    memcpy(old, array->_table + i * array->_elementSize, array->_elementSize);
  }


  array->clearElement(array, array->_table + i * array->_elementSize);
  array->_nrUsed--;


  // ...........................................................................
  // and now check the following places for items to move here
  // ...........................................................................
  k = (i + 1) % array->_nrAlloc;

  while (! array->isEmptyElement(array, array->_table + k * array->_elementSize)) {
    uint64_t j = array->hashElement(array, array->_table + k * array->_elementSize) % array->_nrAlloc;

    if ((i < k && !(i < j && j <= k)) || (k < i && !(i < j || j <= k))) {
      memcpy(array->_table + i * array->_elementSize, array->_table + k * array->_elementSize, array->_elementSize);
      array->clearElement(array, array->_table + k * array->_elementSize);
      i = k;
    }

    k = (k + 1) % array->_nrAlloc;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief removes an key/element to the array
////////////////////////////////////////////////////////////////////////////////

bool TRI_RemoveKeyMultiArray (TRI_multi_array_t* array, void* key, void* old) {
  uint64_t hash;
  uint64_t i;
  uint64_t k;

  
  // ...........................................................................
  // generate hash using key
  // ...........................................................................
  hash = array->hashKey(array, key);
  i = hash % array->_nrAlloc;


  // ...........................................................................
  // update statistics
  // ...........................................................................
  array->_nrRems++;


  // ...........................................................................
  // search the table -- we can only match keys
  // ...........................................................................
  while (! array->isEmptyElement(array, array->_table + i * array->_elementSize)
         && ! array->isEqualKeyElement(array, key, array->_table + i * array->_elementSize)) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesD++;
  }

  // ...........................................................................
  // if we did not find such an item return false
  // ...........................................................................
  if (array->isEmptyElement(array, array->_table + i * array->_elementSize)) {
    if (old != NULL) {
      memset(old, 0, array->_elementSize);
    }

    return false;
  }


  // ...........................................................................
  // remove item
  // ...........................................................................
  if (old != NULL) {
    memcpy(old, array->_table + i * array->_elementSize, array->_elementSize);
  }

  array->clearElement(array, array->_table + i * array->_elementSize);
  array->_nrUsed--;


  // ...........................................................................
  // and now check the following places for items to move here
  // ...........................................................................
  k = (i + 1) % array->_nrAlloc;

  while (! array->isEmptyElement(array, array->_table + k * array->_elementSize)) {
    uint64_t j = array->hashElement(array, array->_table + k * array->_elementSize) % array->_nrAlloc;

    if ((i < k && !(i < j && j <= k)) || (k < i && !(i < j || j <= k))) {
      memcpy(array->_table + i * array->_elementSize, array->_table + k * array->_elementSize, array->_elementSize);
      array->clearElement(array, array->_table + k * array->_elementSize);
      i = k;
    }

    k = (k + 1) % array->_nrAlloc;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// --SECTION--                                              ASSOCIATIVE POINTERS
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a new element
////////////////////////////////////////////////////////////////////////////////

static void AddNewElementPointer (TRI_multi_pointer_t* array, void* element) {
  uint64_t hash;
  uint64_t i;

  // compute the hash
  hash = array->hashElement(array, element);

  // search the table
  i = hash % array->_nrAlloc;

  while (array->_table[i] != NULL) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesR++;
  }

  // add a new element to the associative array
  array->_table[i] = element;
  array->_nrUsed++;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief resizes the array
////////////////////////////////////////////////////////////////////////////////

static void ResizeMultiPointer (TRI_multi_pointer_t* array) {
  void** oldTable;
  uint64_t oldAlloc;
  uint64_t j;

  oldTable = array->_table;
  oldAlloc = array->_nrAlloc;

  array->_nrAlloc = 2 * array->_nrAlloc + 1;
  array->_nrUsed = 0;
  array->_nrResizes++;

  array->_table = TRI_Allocate(array->_nrAlloc * sizeof(void*));

  for (j = 0; j < array->_nrAlloc; j++) {
    array->_table[j] = 0;
  }

  for (j = 0; j < oldAlloc; j++) {
    if (oldTable[j] != NULL) {
      AddNewElementPointer(array, oldTable[j]);
    }
  }

  TRI_Free(oldTable);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialises an array
////////////////////////////////////////////////////////////////////////////////

void TRI_InitMultiPointer (TRI_multi_pointer_t* array,
                           uint64_t (*hashKey) (TRI_multi_pointer_t*, void const*),
                           uint64_t (*hashElement) (TRI_multi_pointer_t*, void const*),
                           bool (*isEqualKeyElement) (TRI_multi_pointer_t*, void const*, void const*),
                           bool (*isEqualElementElement) (TRI_multi_pointer_t*, void const*, void const*)) {
  void** p;
  void** e;

  array->hashKey = hashKey;
  array->hashElement = hashElement;
  array->isEqualKeyElement = isEqualKeyElement;
  array->isEqualElementElement = isEqualElementElement;

  array->_nrAlloc = 10;

  array->_table = TRI_Allocate(sizeof(void*) * array->_nrAlloc);

  p = array->_table;
  e = p + array->_nrAlloc;

  for (;  p < e;  ++p) {
    *p = 0;
  }

  array->_nrUsed = 0;
  array->_nrFinds = 0;
  array->_nrAdds = 0;
  array->_nrRems = 0;
  array->_nrResizes = 0;
  array->_nrProbesF = 0;
  array->_nrProbesA = 0;
  array->_nrProbesD = 0;
  array->_nrProbesR = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array, but does not free the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyMultiPointer (TRI_multi_pointer_t* array) {
  TRI_Free(array->_table);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array and frees the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeMultiPointer (TRI_multi_pointer_t* array) {
  TRI_DestroyMultiPointer(array);
  TRI_Free(array);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Collections
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief lookups an element given a key
////////////////////////////////////////////////////////////////////////////////

TRI_vector_pointer_t TRI_LookupByKeyMultiPointer (TRI_multi_pointer_t* array, void const* key) {
  TRI_vector_pointer_t result;
  uint64_t hash;
  uint64_t i;

  // initialises the result vector
  TRI_InitVectorPointer(&result);

  // compute the hash
  hash = array->hashKey(array, key);
  i = hash % array->_nrAlloc;

  // update statistics
  array->_nrFinds++;

  // search the table
  while (array->_table[i] != NULL) {
    if (array->isEqualKeyElement(array, key, array->_table[i])) {
      TRI_PushBackVectorPointer(&result, array->_table[i]);
    }
    else {
      array->_nrProbesF++;
    }

    i = (i + 1) % array->_nrAlloc;
  }

  // return whatever we found
  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief lookups an element given an element
////////////////////////////////////////////////////////////////////////////////

void* TRI_LookupByElementMultiPointer (TRI_multi_pointer_t* array, void const* element) {
  uint64_t hash;
  uint64_t i;

  // compute the hash
  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;

  // update statistics
  array->_nrFinds++;

  // search the table
  while (array->_table[i] != NULL && ! array->isEqualElementElement(array, element, array->_table[i])) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesF++;
  }

  // return whatever we found
  return array->_table[i];
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds an key/element to the array
////////////////////////////////////////////////////////////////////////////////

void* TRI_InsertElementMultiPointer (TRI_multi_pointer_t* array, void* element, bool overwrite) {
  uint64_t hash;
  uint64_t i;
  void* old;

  // compute the hash
  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;

  // update statistics
  array->_nrAdds++;

  // search the table
  while (array->_table[i] != NULL && ! array->isEqualElementElement(array, element, array->_table[i])) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesA++;
  }

  old = array->_table[i];

  // if we found an element, return
  if (old != NULL) {
    if (overwrite) {
      array->_table[i] = element;
    }

    return old;
  }

  // add a new element to the associative array
  array->_table[i] = element;
  array->_nrUsed++;

  // if we were adding and the table is more than half full, extend it
  if (array->_nrAlloc < 2 * array->_nrUsed) {
    ResizeMultiPointer(array);
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief removes an element from the array
////////////////////////////////////////////////////////////////////////////////

void* TRI_RemoveElementMultiPointer (TRI_multi_pointer_t* array, void const* element) {
  uint64_t hash;
  uint64_t i;
  uint64_t k;
  void* old;

  hash = array->hashElement(array, element);
  i = hash % array->_nrAlloc;

  // update statistics
  array->_nrRems++;

  // search the table
  while (array->_table[i] != NULL && ! array->isEqualElementElement(array, element, array->_table[i])) {
    i = (i + 1) % array->_nrAlloc;
    array->_nrProbesD++;
  }

  // if we did not find such an item return 0
  if (array->_table[i] == NULL) {
    return NULL;
  }

  // remove item
  old = array->_table[i];
  array->_table[i] = NULL;
  array->_nrUsed--;

  // and now check the following places for items to move here
  k = (i + 1) % array->_nrAlloc;

  while (array->_table[k] != NULL) {
    uint64_t j = array->hashElement(array, array->_table[k]) % array->_nrAlloc;

    if ((i < k && !(i < j && j <= k)) || (k < i && !(i < j || j <= k))) {
      array->_table[i] = array->_table[k];
      array->_table[k] = NULL;
      i = k;
    }

    k = (k + 1) % array->_nrAlloc;
  }

  // return success
  return old;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
