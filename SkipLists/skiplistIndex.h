////////////////////////////////////////////////////////////////////////////////
/// @brief unique hash index
///
/// @file
///
/// DISCLAIMER
///
/// Copyright by triAGENS GmbH - All rights reserved.
///
/// The Programs (which include both the software and documentation)
/// contain proprietary information of triAGENS GmbH; they are
/// provided under a license agreement containing restrictions on use and
/// disclosure and are also protected by copyright, patent and other
/// intellectual and industrial property laws. Reverse engineering,
/// disassembly or decompilation of the Programs, except to the extent
/// required to obtain interoperability with other independently created
/// software or as specified by law, is prohibited.
///
/// The Programs are not intended for use in any nuclear, aviation, mass
/// transit, medical, or other inherently dangerous applications. It shall
/// be the licensee's responsibility to take all appropriate fail-safe,
/// backup, redundancy, and other measures to ensure the safe use of such
/// applications if the Programs are used for such purposes, and triAGENS
/// GmbH disclaims liability for any damages caused by such use of
/// the Programs.
///
/// This software is the confidential and proprietary information of
/// triAGENS GmbH. You shall not disclose such confidential and
/// proprietary information and shall use it only in accordance with the
/// terms of the license agreement you entered into with triAGENS GmbH.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. O
/// @author Copyright 2011, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_DURHAM_VOC_BASE_SKIPLIST_INDEX_H
#define TRIAGENS_DURHAM_VOC_BASE_SKIPLIST_INDEX_H 1

#include <BasicsC/common.h>
#include "SkipLists/skiplist.h"
#include "ShapedJson/shaped-json.h"

#ifdef __cplusplus
extern "C" {
#endif

// ...............................................................................
// Define the structure of a unique or non-unique hashindex
// ...............................................................................

typedef struct {
  union {
    TRI_skiplist_t* uniqueSkiplist;
    TRI_skiplist_multi_t* nonUniqueSkiplist;
  } skiplist;   
  bool unique; 
} SkiplistIndex;


typedef struct {
  size_t numFields;          // the number of fields
  TRI_shaped_json_t* fields; // list of shaped json objects which the collection should know about
  void* data;                // master document pointer
  void* collection;          // pointer to the collection;
} SkiplistIndexElement;

typedef struct {
  size_t _numElements;
  SkiplistIndexElement* _elements; // simple list of elements
} SkiplistIndexElements;  


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Unique skiplist indexes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SkiplistIndex* SkiplistIndex_new (void);

int SkiplistIndex_add (SkiplistIndex*, SkiplistIndexElement*);

SkiplistIndexElements* SkiplistIndex_find (SkiplistIndex*, SkiplistIndexElement*); 

int SkiplistIndex_insert (SkiplistIndex*, SkiplistIndexElement*);

bool SkiplistIndex_remove (SkiplistIndex*, SkiplistIndexElement*); 

bool SkiplistIndex_update (SkiplistIndex*, const SkiplistIndexElement*, const SkiplistIndexElement*);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Multi-skiplist non-unique skiplist indexes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


SkiplistIndex* MultiSkiplistIndex_new (void);

int MultiSkiplistIndex_add (SkiplistIndex*, SkiplistIndexElement*);

SkiplistIndexElements* MultiSkiplistIndex_find (SkiplistIndex*, SkiplistIndexElement*); 

int MultiSkiplistIndex_insert (SkiplistIndex*, SkiplistIndexElement*);

bool MultiSkiplistIndex_remove (SkiplistIndex*, SkiplistIndexElement*); 

bool MultiSkiplistIndex_update (SkiplistIndex*, SkiplistIndexElement*, SkiplistIndexElement*);


#ifdef __cplusplus
}
#endif

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:

