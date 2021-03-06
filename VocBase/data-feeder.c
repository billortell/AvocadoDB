////////////////////////////////////////////////////////////////////////////////
/// @brief data-feeder
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2012 triagens GmbH, Cologne, Germany
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
/// @author Jan Steemann
/// @author Copyright 2012, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "VocBase/data-feeder.h"
#include "VocBase/join.h"
#include "V8/v8-c-utils.h"
#include "QL/optimize.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        type independent functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief create a new base data data feeder struct
////////////////////////////////////////////////////////////////////////////////

static TRI_data_feeder_t* CreateDataFeeder (const TRI_data_feeder_type_e type,
                                            const TRI_doc_collection_t* collection,
                                            const TRI_join_t* join,
                                            size_t level) {
  TRI_data_feeder_t* feeder;
  
  feeder = (TRI_data_feeder_t*) TRI_Allocate(sizeof(TRI_data_feeder_t));
  if (!feeder) {
    return NULL;
  }
  
  feeder->_type       = type;
  feeder->_level      = level;
  feeder->_join       = (TRI_select_join_t*) join;
  feeder->_part       = (TRI_join_part_t*) ((TRI_select_join_t*) join)->_parts._buffer[level];
  feeder->_collection = collection;
  feeder->_ranges     = NULL;
  feeder->_state      = NULL;

  feeder->init        = NULL;
  feeder->rewind      = NULL;
  feeder->current     = NULL;
  feeder->eof         = NULL;
  feeder->free        = NULL;
  
  return feeder;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                        table scan
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief init table scan data feeder
////////////////////////////////////////////////////////////////////////////////

static void InitFeederTableScan (TRI_data_feeder_t* feeder) {
  TRI_sim_collection_t* collection;
  TRI_doc_mptr_t* document;
  TRI_data_feeder_table_scan_t* state;

  feeder->_accessType = ACCESS_ALL;

  collection = (TRI_sim_collection_t*) feeder->_collection;
  state = (TRI_data_feeder_table_scan_t*) feeder->_state;

  if (collection->_primaryIndex._nrAlloc == 0) {
    state->_start   = NULL;
    state->_end     = NULL;
    state->_current = NULL;
    return;
  }

  state->_start = (void**) collection->_primaryIndex._table;
  state->_end   = (void**) (state->_start + collection->_primaryIndex._nrAlloc);

  // collections contain documents in a hash table
  // some of the entries are empty, and some contain deleted documents
  // it is invalid to use every entry from the hash table but the invalid documents
  // must be skipped.
  // adjust starts to first valid document in collection
  while (state->_start < state->_end) {
    document = (TRI_doc_mptr_t*) *(state->_start);
    if (document != NULL && !document->_deletion) {
      break;
    }

    state->_start++;
  }

  // iterate from end of document hash table to front and skip all documents
  // that are either deleted or empty
  while (state->_end > state->_start) {
    document = (TRI_doc_mptr_t*) *(state->_end - 1);
    if (document != NULL && !document->_deletion) {
      break;
    }
    state->_end--;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief rewind table scan data feeder
////////////////////////////////////////////////////////////////////////////////

static void RewindFeederTableScan (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_table_scan_t* state;
  
  state = (TRI_data_feeder_table_scan_t*) feeder->_state;
  state->_current = state->_start;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get current item from table scan data feeder and advance next pointer
////////////////////////////////////////////////////////////////////////////////

static bool CurrentFeederTableScan (TRI_data_feeder_t* feeder) {
  TRI_doc_mptr_t* document;
  TRI_data_feeder_table_scan_t* state;
  TRI_join_part_t* part;
  
  state = (TRI_data_feeder_table_scan_t*) feeder->_state;
  part = (TRI_join_part_t*) feeder->_part;

  while (state->_current < state->_end) {
    document = (TRI_doc_mptr_t*) *(state->_current);

    state->_current++;
    if (document && document->_deletion == 0) {
      part->_singleDocument = document;
      return true;
    }
  }
    
  part->_singleDocument = NULL;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief check if table scan data feeder is at eof
////////////////////////////////////////////////////////////////////////////////

static bool EofFeederTableScan (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_table_scan_t* state;

  state = (TRI_data_feeder_table_scan_t*) feeder->_state;
  return (state->_current < state->_end);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free table scan data feeder
////////////////////////////////////////////////////////////////////////////////

static void FreeFeederTableScan (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_table_scan_t* state;

  state = (TRI_data_feeder_table_scan_t*) feeder->_state;

  if (state) {
    TRI_Free(state);
  }

  if (feeder->_ranges) {
    TRI_DestroyVectorPointer(feeder->_ranges);
    TRI_Free(feeder->_ranges);
  }

  TRI_Free(feeder);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief create a new table scan data feeder
////////////////////////////////////////////////////////////////////////////////

TRI_data_feeder_t* TRI_CreateDataFeederTableScan (const TRI_doc_collection_t* collection,
                                                  TRI_join_t* join,
                                                  size_t level) {
  TRI_data_feeder_t* feeder;

  feeder = CreateDataFeeder(FEEDER_TABLE_SCAN, collection, join, level);
  if (!feeder) {
    return NULL;
  }

  feeder->_state = (TRI_data_feeder_table_scan_t*) 
    TRI_Allocate(sizeof(TRI_data_feeder_table_scan_t));

  if (!feeder->_state) {
    TRI_Free(feeder);
    return NULL;
  }

  feeder->init        = InitFeederTableScan;
  feeder->rewind      = RewindFeederTableScan;
  feeder->current     = CurrentFeederTableScan;
  feeder->eof         = EofFeederTableScan;
  feeder->free        = FreeFeederTableScan;

  return feeder;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                     primary index
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief init primary index data feeder
////////////////////////////////////////////////////////////////////////////////

static void InitFeederPrimaryLookup (TRI_data_feeder_t* feeder) {
  QL_optimize_range_t* range;
  TRI_vector_string_t parts;
  TRI_data_feeder_primary_lookup_t* state;
  TRI_string_buffer_t* buffer;
  
  state = (TRI_data_feeder_primary_lookup_t*) feeder->_state;
  state->_isEmpty  = true;
  state->_context  = NULL;

  assert(feeder->_ranges); 
  assert(feeder->_ranges->_length == 1);

  range = (QL_optimize_range_t*) feeder->_ranges->_buffer[0];

  if (range->_valueType == RANGE_TYPE_FIELD) {
    // ref access
    feeder->_accessType = ACCESS_REF;

    buffer = (TRI_string_buffer_t*) TRI_Allocate(sizeof(TRI_string_buffer_t));
    if (!buffer) {
      return;
    }

    TRI_AppendStringStringBuffer(buffer, "(function($) { return [");
    TRI_AppendStringStringBuffer(buffer, "$['");
    TRI_AppendStringStringBuffer(buffer, range->_refValue._collection);
    TRI_AppendStringStringBuffer(buffer, "'].");
    TRI_AppendStringStringBuffer(buffer, range->_refValue._field);
    TRI_AppendStringStringBuffer(buffer, "] })");
    state->_context = TRI_CreateExecutionContext(buffer->_buffer);

    TRI_FreeStringBuffer(buffer);
    TRI_Free(buffer);

    if (!state->_context) {
      return;
    }
    state->_isEmpty = false;
  }
  if (range->_valueType == RANGE_TYPE_STRING) {
    // const access
    feeder->_accessType = ACCESS_CONST;
    parts = TRI_SplitString(range->_minValue._stringValue, ':');
    if (parts._length > 0) {
      if (TRI_UInt64String(parts._buffer[0]) == 
          ((TRI_collection_t*) feeder->_collection)->_cid) {
        state->_didValue = TRI_UInt64String(parts._buffer[1]);
        state->_isEmpty = false;
      }
    }
    TRI_DestroyVectorString(&parts);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief rewind primary index data feeder
////////////////////////////////////////////////////////////////////////////////

static void RewindFeederPrimaryLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_primary_lookup_t* state;
  TRI_vector_string_t parts;
  TRI_json_t* parameters;
  TRI_json_t* value;
  
  state = (TRI_data_feeder_primary_lookup_t*) feeder->_state;
  state->_hasCompared = true;

  if (feeder->_accessType == ACCESS_REF) {
    if (!state->_context) {
      return;
    }

    parameters = TRI_CreateListJson();
    if (!parameters) {
      return;
    }
    TRI_DefineWhereExecutionContext(state->_context, 
                                    (TRI_join_t*) feeder->_join, 
                                    feeder->_level, 
                                    true);
    if (TRI_ExecuteRefExecutionContext (state->_context, parameters)) {
      if (parameters->_type != TRI_JSON_LIST) {
        TRI_FreeJson(parameters);
        return;
      }
      if (parameters->_value._objects._length != 1) {
        TRI_FreeJson(parameters);
        return;
      }

      value = TRI_AtVector(&parameters->_value._objects, 0);
      parts = TRI_SplitString(value->_value._string.data, ':');
      if (parts._length == 2) {
        if (TRI_UInt64String(parts._buffer[0]) == 
            ((TRI_collection_t*) feeder->_collection)->_cid) {
          state->_didValue = TRI_UInt64String(parts._buffer[1]);
        }
      }
      TRI_DestroyVectorString(&parts);
    }
    TRI_FreeJson(parameters);
  }

  state->_hasCompared = false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get current item from primary index feeder
////////////////////////////////////////////////////////////////////////////////

static bool CurrentFeederPrimaryLookup (TRI_data_feeder_t* feeder) {
  TRI_sim_collection_t* collection;
  QL_optimize_range_t* range;
  TRI_data_feeder_primary_lookup_t* state;
  TRI_join_part_t* part;
  
  state = (TRI_data_feeder_primary_lookup_t*) feeder->_state;
  part = (TRI_join_part_t*) feeder->_part;

  if (state->_hasCompared || state->_isEmpty) {
    part->_singleDocument = NULL;
    return false;
  }
  
  state->_hasCompared = true;
  range = (QL_optimize_range_t*) feeder->_ranges->_buffer[0];
  collection = (TRI_sim_collection_t*) feeder->_collection;
  part->_singleDocument = (TRI_doc_mptr_t*) 
    TRI_LookupByKeyAssociativePointer(&collection->_primaryIndex, &state->_didValue);

  return (part->_singleDocument != NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief check if primary index data feeder is at eof
////////////////////////////////////////////////////////////////////////////////

static bool EofFeederPrimaryLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_primary_lookup_t* state;
  
  state = (TRI_data_feeder_primary_lookup_t*) feeder->_state;

  return state->_hasCompared;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free primary index data feeder
////////////////////////////////////////////////////////////////////////////////

static void FreeFeederPrimaryLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_primary_lookup_t* state;

  state = (TRI_data_feeder_primary_lookup_t*) feeder->_state;
  
  if (state->_context) {
    TRI_FreeExecutionContext(state->_context);
  }

  if (state) {
    TRI_Free(state);
  }

  if (feeder->_ranges) {
    TRI_DestroyVectorPointer(feeder->_ranges);
    TRI_Free(feeder->_ranges);
  }

  if (feeder) {
    TRI_Free(feeder);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief create a new primary index data feeder
////////////////////////////////////////////////////////////////////////////////

TRI_data_feeder_t* TRI_CreateDataFeederPrimaryLookup (const TRI_doc_collection_t* collection,
                                                      TRI_join_t* join,
                                                      size_t level) {
  TRI_data_feeder_t* feeder;

  feeder = CreateDataFeeder(FEEDER_PRIMARY_LOOKUP, collection, join, level);
  if (!feeder) {
    return NULL;
  }

  feeder->_state = (TRI_data_feeder_primary_lookup_t*) 
    TRI_Allocate(sizeof(TRI_data_feeder_primary_lookup_t));
  if (!feeder->_state) {
    TRI_Free(feeder);
    return NULL;
  }

  // init feeder
  feeder->init        = InitFeederPrimaryLookup; 
  feeder->rewind      = RewindFeederPrimaryLookup;
  feeder->current     = CurrentFeederPrimaryLookup;
  feeder->eof         = EofFeederPrimaryLookup;
  feeder->free        = FreeFeederPrimaryLookup;
  
  return feeder;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                        hash index
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief init hash lookup data feeder
////////////////////////////////////////////////////////////////////////////////

static void InitFeederHashLookup (TRI_data_feeder_t* feeder) {
  QL_optimize_range_t* range;
  TRI_data_feeder_hash_lookup_t* state;
  TRI_json_t* parameters;
  TRI_json_t* doc;
  TRI_string_buffer_t* buffer;
  size_t i;
  
  state = (TRI_data_feeder_hash_lookup_t*) feeder->_state;
  state->_isEmpty  = true;
  state->_context  = NULL;
  state->_position = 0;

  state->_hashElements = NULL;
  state->_index = TRI_IndexSimCollection((TRI_sim_collection_t*) feeder->_collection, 
                                         feeder->_indexId);
  if (!state->_index) {
    return;
  }
  assert(feeder->_ranges); 
  assert(feeder->_ranges->_length >= 1);
  
  range = (QL_optimize_range_t*) feeder->_ranges->_buffer[0];
  if (range->_valueType == RANGE_TYPE_FIELD) {
    // ref access
    feeder->_accessType = ACCESS_REF; 
    buffer = (TRI_string_buffer_t*) TRI_Allocate(sizeof(TRI_string_buffer_t));
    if (!buffer) {
      return;
    }

    TRI_AppendStringStringBuffer(buffer, "(function($) { return [");
    for (i = 0; i < feeder->_ranges->_length; i++) {
      range = (QL_optimize_range_t*) feeder->_ranges->_buffer[i];
      if (i > 0) {
        TRI_AppendCharStringBuffer(buffer, ',');
      }
      TRI_AppendStringStringBuffer(buffer, "$['");
      TRI_AppendStringStringBuffer(buffer, range->_refValue._collection);
      TRI_AppendStringStringBuffer(buffer, "'].");
      TRI_AppendStringStringBuffer(buffer, range->_refValue._field);
    }
    TRI_AppendStringStringBuffer(buffer, "] })");
    state->_context = TRI_CreateExecutionContext(buffer->_buffer);

    TRI_FreeStringBuffer(buffer);
    TRI_Free(buffer);

    if (!state->_context) {
      return;
    }
  }
  else {
    // const access
    feeder->_accessType = ACCESS_CONST;
     
    parameters = TRI_CreateListJson();
    if (!parameters) {
      return;
    }
    for (i = 0; i < feeder->_ranges->_length; i++) {
      range = (QL_optimize_range_t*) feeder->_ranges->_buffer[i];
      if (range->_valueType == RANGE_TYPE_STRING) {
        TRI_PushBack2ListJson(parameters, 
            TRI_CreateStringCopyJson(range->_minValue._stringValue));
      }
      else if (range->_valueType == RANGE_TYPE_DOUBLE) {
        TRI_PushBack2ListJson(parameters, 
            TRI_CreateNumberJson(range->_minValue._doubleValue));
      }
      else if (range->_valueType == RANGE_TYPE_JSON) {
        doc = TRI_JsonString(range->_minValue._stringValue);
        if (!doc) {
          TRI_FreeJson(parameters);
          return;
        }
        TRI_PushBackListJson(parameters, doc);
      }
    }
    state->_hashElements = TRI_LookupHashIndex(state->_index, parameters);
    TRI_FreeJson(parameters);
  }

  state->_isEmpty = false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief rewind hash lookup data feeder
////////////////////////////////////////////////////////////////////////////////

static void RewindFeederHashLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_hash_lookup_t* state;
  TRI_json_t* parameters;
   
  state = (TRI_data_feeder_hash_lookup_t*) feeder->_state;
  state->_position = 0;

  if (feeder->_accessType == ACCESS_REF) {
    if (state->_hashElements) {
      TRI_Free(state->_hashElements->_elements);
      TRI_Free(state->_hashElements);
    }
    state->_hashElements = NULL;

    if (!state->_context) {
      return;
    }
    parameters = TRI_CreateListJson();
    if (!parameters) {
      return;
    }
    TRI_DefineWhereExecutionContext(state->_context, 
                                    (TRI_join_t*) feeder->_join, 
                                    feeder->_level, 
                                    true);
    if (TRI_ExecuteRefExecutionContext (state->_context, parameters)) {
      state->_hashElements = TRI_LookupHashIndex(state->_index, parameters);
    }
 
    TRI_FreeJson(parameters);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get current item from hash lookup feeder
////////////////////////////////////////////////////////////////////////////////

static bool CurrentFeederHashLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_hash_lookup_t* state;
  TRI_doc_mptr_t* document;
  TRI_join_part_t* part;

  state = (TRI_data_feeder_hash_lookup_t*) feeder->_state;
  part = (TRI_join_part_t*) feeder->_part;

  if (state->_isEmpty || !state->_hashElements) {
    part->_singleDocument = NULL;
    return false;
  }

  while (state->_position < state->_hashElements->_numElements) {
    document = (TRI_doc_mptr_t*) ((state->_hashElements->_elements[state->_position++]).data);
    if (document && !document->_deletion) {
      part->_singleDocument = document;
      return true;
    }
  }

  part->_singleDocument = NULL;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief check if hash lookup data feeder is at eof
////////////////////////////////////////////////////////////////////////////////

static bool EofFeederHashLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_hash_lookup_t* state;
  
  state = (TRI_data_feeder_hash_lookup_t*) feeder->_state;
  if (state->_isEmpty || 
      !state->_hashElements || 
      state->_position >= state->_hashElements->_numElements) {
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free hash lookup data feeder
////////////////////////////////////////////////////////////////////////////////

static void FreeFeederHashLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_hash_lookup_t* state;

  state = (TRI_data_feeder_hash_lookup_t*) feeder->_state;
  if (state->_hashElements) {
    TRI_Free(state->_hashElements->_elements);
    TRI_Free(state->_hashElements);
  }

  if (state->_context) {
    TRI_FreeExecutionContext(state->_context);
  }

  if (state) {
    TRI_Free(state);
  }

  if (feeder->_ranges) {
    TRI_DestroyVectorPointer(feeder->_ranges);
    TRI_Free(feeder->_ranges);
  }

  if (feeder) {
    TRI_Free(feeder);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Create a new data feeder for hash lookups
////////////////////////////////////////////////////////////////////////////////

TRI_data_feeder_t* TRI_CreateDataFeederHashLookup (const TRI_doc_collection_t* collection,
                                                   TRI_join_t* join,
                                                   size_t level) {
  TRI_data_feeder_t* feeder;
  
  feeder = CreateDataFeeder(FEEDER_HASH_LOOKUP, collection, join, level);
  if (!feeder) {
    return NULL;
  }
  
  feeder->_state = (TRI_data_feeder_hash_lookup_t*) 
    TRI_Allocate(sizeof(TRI_data_feeder_hash_lookup_t));
  if (!feeder->_state) {
    TRI_Free(feeder);
    return NULL;
  }

  feeder->init        = InitFeederHashLookup; 
  feeder->rewind      = RewindFeederHashLookup;
  feeder->current     = CurrentFeederHashLookup;
  feeder->eof         = EofFeederHashLookup;
  feeder->free        = FreeFeederHashLookup;
  
  return feeder;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                         skiplists
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief free skiplist index elements
////////////////////////////////////////////////////////////////////////////////

static void FreeSkiplistElements (SkiplistIndexElements* elements) {
  TRI_Free(elements->_elements);
  TRI_Free(elements);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief init skiplist data feeder
////////////////////////////////////////////////////////////////////////////////

static void InitFeederSkiplistLookup (TRI_data_feeder_t* feeder) {
  QL_optimize_range_t* range;
  TRI_data_feeder_skiplist_lookup_t* state;
  TRI_json_t* parameters;
  TRI_json_t* doc;
  TRI_string_buffer_t* buffer;
  size_t i;
  
  state = (TRI_data_feeder_skiplist_lookup_t*) feeder->_state;
  state->_isEmpty  = true;
  state->_context  = NULL;
  state->_position = 0;

  state->_skiplistElements = NULL;
  state->_index = TRI_IndexSimCollection((TRI_sim_collection_t*) feeder->_collection, 
                                         feeder->_indexId);
  if (!state->_index) {
    return;
  }
  assert(feeder->_ranges); 
  assert(feeder->_ranges->_length >= 1);
  
  range = (QL_optimize_range_t*) feeder->_ranges->_buffer[0];
  if (range->_valueType == RANGE_TYPE_FIELD) {
    // ref access
    feeder->_accessType = ACCESS_REF; 
    buffer = (TRI_string_buffer_t*) TRI_Allocate(sizeof(TRI_string_buffer_t));
    if (!buffer) {
      return;
    }

    TRI_AppendStringStringBuffer(buffer, "(function($) { return [");
    for (i = 0; i < feeder->_ranges->_length; i++) {
      range = (QL_optimize_range_t*) feeder->_ranges->_buffer[i];
      if (i > 0) {
        TRI_AppendCharStringBuffer(buffer, ',');
      }
      TRI_AppendStringStringBuffer(buffer, "$['");
      TRI_AppendStringStringBuffer(buffer, range->_refValue._collection);
      TRI_AppendStringStringBuffer(buffer, "'].");
      TRI_AppendStringStringBuffer(buffer, range->_refValue._field);
    }
    TRI_AppendStringStringBuffer(buffer, "] })");
    state->_context = TRI_CreateExecutionContext(buffer->_buffer);

    TRI_FreeStringBuffer(buffer);
    TRI_Free(buffer);

    if (!state->_context) {
      return;
    }
  }
  else {
    // const access
    feeder->_accessType = ACCESS_CONST;
     
    parameters = TRI_CreateListJson();
    if (!parameters) {
      return;
    }
    for (i = 0; i < feeder->_ranges->_length; i++) {
      range = (QL_optimize_range_t*) feeder->_ranges->_buffer[i];
      if (range->_valueType == RANGE_TYPE_STRING) {
        TRI_PushBack2ListJson(parameters, 
            TRI_CreateStringCopyJson(range->_minValue._stringValue));
      }
      else if (range->_valueType == RANGE_TYPE_DOUBLE) {
        TRI_PushBack2ListJson(parameters, 
            TRI_CreateNumberJson(range->_minValue._doubleValue));
      }
      else if (range->_valueType == RANGE_TYPE_JSON) {
        doc = TRI_JsonString(range->_minValue._stringValue);
        if (!doc) {
          TRI_FreeJson(parameters);
          return;
        }
        TRI_PushBackListJson(parameters, doc);
      }
    }
    state->_skiplistElements = TRI_LookupSkiplistIndex(state->_index, parameters);
    TRI_FreeJson(parameters);
  }

  state->_isEmpty = false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief rewind skiplist data feeder
////////////////////////////////////////////////////////////////////////////////

static void RewindFeederSkiplistLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_skiplist_lookup_t* state;
  TRI_json_t* parameters;
   
  state = (TRI_data_feeder_skiplist_lookup_t*) feeder->_state;
  state->_position = 0;

  if (feeder->_accessType == ACCESS_REF) {
    if (state->_skiplistElements) {
      FreeSkiplistElements(state->_skiplistElements);
    }
    state->_skiplistElements = NULL;

    if (!state->_context) {
      return;
    }
    parameters = TRI_CreateListJson();
    if (!parameters) {
      return;
    }
    TRI_DefineWhereExecutionContext(state->_context, 
                                    (TRI_join_t*) feeder->_join, 
                                    feeder->_level, 
                                    true);
    if (TRI_ExecuteRefExecutionContext (state->_context, parameters)) {
      state->_skiplistElements = TRI_LookupSkiplistIndex(state->_index, parameters);
    }
 
    TRI_FreeJson(parameters);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get current item from skiplist feeder
////////////////////////////////////////////////////////////////////////////////

static bool CurrentFeederSkiplistLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_skiplist_lookup_t* state;
  TRI_doc_mptr_t* document;
  TRI_join_part_t* part;

  state = (TRI_data_feeder_skiplist_lookup_t*) feeder->_state;
  part = (TRI_join_part_t*) feeder->_part;

  if (state->_isEmpty || !state->_skiplistElements) {
    part->_singleDocument = NULL;
    return false;
  }

  while (state->_position < state->_skiplistElements->_numElements) {
    document = (TRI_doc_mptr_t*) ((state->_skiplistElements->_elements[state->_position++]).data);
    if (document && !document->_deletion) {
      part->_singleDocument = document;
      return true;
    }
  }

  part->_singleDocument = NULL;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief check if skiplist data feeder is at eof
////////////////////////////////////////////////////////////////////////////////

static bool EofFeederSkiplistLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_skiplist_lookup_t* state;
  
  state = (TRI_data_feeder_skiplist_lookup_t*) feeder->_state;
  if (state->_isEmpty || 
      !state->_skiplistElements || 
      state->_position >= state->_skiplistElements->_numElements) {
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free skiplist lookup data feeder
////////////////////////////////////////////////////////////////////////////////

static void FreeFeederSkiplistLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_skiplist_lookup_t* state;

  state = (TRI_data_feeder_skiplist_lookup_t*) feeder->_state;
  if (state->_skiplistElements) {
    FreeSkiplistElements(state->_skiplistElements);
  }

  if (state->_context) {
    TRI_FreeExecutionContext(state->_context);
  }

  if (state) {
    TRI_Free(state);
  }

  if (feeder->_ranges) {
    TRI_DestroyVectorPointer(feeder->_ranges);
    TRI_Free(feeder->_ranges);
  }

  if (feeder) {
    TRI_Free(feeder);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Create a new data feeder for skiplist lookups
////////////////////////////////////////////////////////////////////////////////

TRI_data_feeder_t* TRI_CreateDataFeederSkiplistLookup (const TRI_doc_collection_t* collection,
                                                       TRI_join_t* join,
                                                       size_t level) {
  TRI_data_feeder_t* feeder;

  feeder = CreateDataFeeder(FEEDER_SKIPLIST_LOOKUP, collection, join, level);
  if (!feeder) {
    return NULL;
  }
  
  feeder->_state = (TRI_data_feeder_skiplist_lookup_t*) 
    TRI_Allocate(sizeof(TRI_data_feeder_skiplist_lookup_t));
  if (!feeder->_state) {
    TRI_Free(feeder);
    return NULL;
  }

  feeder->init        = InitFeederSkiplistLookup; 
  feeder->rewind      = RewindFeederSkiplistLookup;
  feeder->current     = CurrentFeederSkiplistLookup;
  feeder->eof         = EofFeederSkiplistLookup;
  feeder->free        = FreeFeederSkiplistLookup;
  
  return feeder;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                         geo index
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief init geo index data feeder
////////////////////////////////////////////////////////////////////////////////

static void InitFeederGeoLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_geo_lookup_t* state;
  
  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;
  state->_isEmpty  = true;
  state->_position = 0;
  state->_coordinates = NULL;

  state->_index = TRI_IndexSimCollection((TRI_sim_collection_t*) feeder->_collection, 
                                         feeder->_indexId);
  if (!state->_index) {
    return;
  }
  if (!state->_restriction) {
    return;
  }
  
  state->_isEmpty = false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief rewind geo index data feeder
////////////////////////////////////////////////////////////////////////////////

static void RewindFeederGeoLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_geo_lookup_t* state;
   
  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;
  state->_position = 0;

  if (state->_restriction->_type == RESTRICT_WITHIN) {
    state->_coordinates = TRI_WithinGeoIndex(state->_index, 
                          state->_restriction->_lat, 
                          state->_restriction->_lon, 
                          state->_restriction->_arg._radius);
  }
  else {
    state->_coordinates = TRI_NearestGeoIndex(state->_index, 
                          state->_restriction->_lat, 
                          state->_restriction->_lon, 
                          state->_restriction->_arg._numDocuments);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief get current item from geo index feeder
////////////////////////////////////////////////////////////////////////////////

static bool CurrentFeederGeoLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_geo_lookup_t* state;
  TRI_doc_mptr_t* document;
  TRI_join_part_t* part;
  size_t position;

  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;
  part = (TRI_join_part_t*) feeder->_part;

  if (state->_isEmpty || !state->_coordinates) { 
    part->_singleDocument = NULL;
    part->_extraData._singleValue = NULL;
    return false;
  }

  while (state->_position < state->_coordinates->length) {
    position = state->_position++;
    document = (TRI_doc_mptr_t*) ((state->_coordinates->coordinates[position].data));
    if (document && !document->_deletion) {
      part->_singleDocument = document;
      // store extra data
      part->_extraData._singleValue = &state->_coordinates->distances[position];

      return true;
    }
  }

  part->_singleDocument = NULL;
  part->_extraData._singleValue = NULL;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief check if geo index data feeder is at eof
////////////////////////////////////////////////////////////////////////////////

static bool EofFeederGeoLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_geo_lookup_t* state;
  
  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;
  if (state->_isEmpty || 
      !state->_coordinates ||
      state->_position >= state->_coordinates->length) {
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief free geo index data feeder
////////////////////////////////////////////////////////////////////////////////

static void FreeFeederGeoLookup (TRI_data_feeder_t* feeder) {
  TRI_data_feeder_geo_lookup_t* state;

  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;
  if (state->_coordinates) {
    GeoIndex_CoordinatesFree(state->_coordinates);
  }

  if (state) {
    TRI_Free(state);
  }

  if (feeder) {
    TRI_Free(feeder);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Create a new data feeder for geo lookups
////////////////////////////////////////////////////////////////////////////////

TRI_data_feeder_t* TRI_CreateDataFeederGeoLookup (const TRI_doc_collection_t* collection,
                                                  TRI_join_t* join,
                                                  size_t level,
                                                  QL_ast_query_geo_restriction_t* restriction) {
  TRI_data_feeder_t* feeder;
  TRI_data_feeder_geo_lookup_t* state;
  
  feeder = CreateDataFeeder(FEEDER_GEO_LOOKUP, collection, join, level);
  if (!feeder) {
    return NULL;
  }
  
  feeder->_state = (TRI_data_feeder_geo_lookup_t*) 
    TRI_Allocate(sizeof(TRI_data_feeder_geo_lookup_t));

  state = (TRI_data_feeder_geo_lookup_t*) feeder->_state;

  if (!state) {
    TRI_Free(feeder);
    return NULL;
  }

  state->_restriction = restriction;

  feeder->init        = InitFeederGeoLookup; 
  feeder->rewind      = RewindFeederGeoLookup;
  feeder->current     = CurrentFeederGeoLookup;
  feeder->eof         = EofFeederGeoLookup;
  feeder->free        = FreeFeederGeoLookup;
  
  return feeder;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:

