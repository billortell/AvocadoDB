////////////////////////////////////////////////////////////////////////////////
/// @brief index
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
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
/// @author Copyright 2011, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_DURHAM_VOC_BASE_INDEX_H
#define TRIAGENS_DURHAM_VOC_BASE_INDEX_H 1

#include "VocBase/vocbase.h"

#include "BasicsC/json.h"
#include "ShapedJson/shaped-json.h"
#include "GeoIndex/GeoIndex.h"
#include "HashIndex/hashindex.h"
#include "SkipLists/skiplistIndex.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

struct TRI_collection_s;
struct TRI_doc_mptr_s;
struct TRI_shaped_json_s;
struct TRI_sim_collection_s;

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief index indetifier
////////////////////////////////////////////////////////////////////////////////

typedef TRI_voc_tick_t TRI_idx_iid_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief index type
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_IDX_TYPE_PRIMARY_INDEX,
  TRI_IDX_TYPE_GEO_INDEX,
  TRI_IDX_TYPE_HASH_INDEX,
  TRI_IDX_TYPE_SKIPLIST_INDEX
}
TRI_idx_type_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief geo index variants
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  INDEX_GEO_NONE = 0,
  INDEX_GEO_INDIVIDUAL_LAT_LON,
  INDEX_GEO_COMBINED_LAT_LON,
  INDEX_GEO_COMBINED_LON_LAT
}
TRI_index_geo_variant_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief index definition struct as used by the query optimizer
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_index_definition_s {
  TRI_idx_iid_t _iid;
  TRI_idx_type_e _type;
  TRI_vector_string_t* _fields;
  bool _isUnique;
  TRI_index_geo_variant_e _geoVariant;
}
TRI_index_definition_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief index base class
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_index_s {
  TRI_idx_iid_t _iid;
  TRI_idx_type_e _type;
  struct TRI_doc_collection_s* _collection;

  bool (*insert) (struct TRI_index_s*, struct TRI_doc_mptr_s const*);
  bool (*remove) (struct TRI_index_s*, struct TRI_doc_mptr_s const*);
  bool (*update) (struct TRI_index_s*, struct TRI_doc_mptr_s const*, struct TRI_shaped_json_s const*);
  TRI_json_t* (*json) (struct TRI_index_s*, struct TRI_doc_collection_s*);
}
TRI_index_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief geo index
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_geo_index_s {
  TRI_index_t base;

  GeoIndex* _geoIndex;

  TRI_shape_pid_t _location;
  TRI_shape_pid_t _latitude;
  TRI_shape_pid_t _longitude;

  bool _geoJson;
}
TRI_geo_index_t;


////////////////////////////////////////////////////////////////////////////////
/// @brief hash index
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_hash_index_s {
  TRI_index_t base;

  HashIndex* _hashIndex;    // effectively the associative array
  TRI_vector_t* _shapeList; // a list of shape pid which identifies the fields of the index
  bool _unique;
} TRI_hash_index_t;


////////////////////////////////////////////////////////////////////////////////
/// @brief skiplist index
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_skiplist_index_s {
  TRI_index_t base;

  SkiplistIndex* _skiplistIndex;    // effectively the skiplist
  TRI_vector_t* _shapeList; // a list of shape pid which identifies the fields of the index
  bool _unique;
} TRI_skiplist_index_t;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                             INDEX
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief removes an index file
////////////////////////////////////////////////////////////////////////////////

bool TRI_RemoveIndex (struct TRI_doc_collection_s* collection, TRI_index_t* idx);

////////////////////////////////////////////////////////////////////////////////
/// @brief saves an index
////////////////////////////////////////////////////////////////////////////////

bool TRI_SaveIndex (struct TRI_doc_collection_s*, TRI_index_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up an index identifier
////////////////////////////////////////////////////////////////////////////////

TRI_index_t* TRI_LookupIndex (struct TRI_doc_collection_s*, TRI_idx_iid_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief free an existing index definition
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeIndexDefinition (TRI_index_definition_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief free an existing index definitions vector
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeIndexDefinitions (TRI_vector_pointer_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief gets the definitions of all index files for a collection
////////////////////////////////////////////////////////////////////////////////

TRI_vector_pointer_t* TRI_GetCollectionIndexes (const TRI_vocbase_t* vocbase,
                                                const char* collectionName);

////////////////////////////////////////////////////////////////////////////////
/// @brief gets the names of all index files for a collection
////////////////////////////////////////////////////////////////////////////////

TRI_vector_string_t TRI_GetCollectionIndexFiles (const TRI_vocbase_t* vocbase,
                                                 const char* collectionName);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                         GEO INDEX
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a geo-index for lists
///
/// If geoJson is true, than the coordinates should be in the order described
/// in http://geojson.org/geojson-spec.html#positions, which is longitude
/// first and latitude second.
////////////////////////////////////////////////////////////////////////////////

TRI_index_t* TRI_CreateGeoIndex (struct TRI_doc_collection_s*,
                                 TRI_shape_pid_t,
                                 bool geoJson);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a geo-index for arrays
////////////////////////////////////////////////////////////////////////////////

TRI_index_t* TRI_CreateGeoIndex2 (struct TRI_doc_collection_s*,
                                  TRI_shape_pid_t,
                                  TRI_shape_pid_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief frees the memory allocated, but does not free the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_DestoryGeoIndex (TRI_index_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief frees the memory allocated and frees the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeGeoIndex (TRI_index_t*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up all points within a given radius
////////////////////////////////////////////////////////////////////////////////

GeoCoordinates* TRI_WithinGeoIndex (TRI_index_t*,
                                    double lat,
                                    double lon,
                                    double radius);

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up the nearest points
////////////////////////////////////////////////////////////////////////////////

GeoCoordinates* TRI_NearestGeoIndex (TRI_index_t*,
                                     double lat,
                                     double lon,
                                     size_t count);

                                     
                                     
////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                        HASH INDEX
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////


HashIndexElements* TRI_LookupHashIndex (TRI_index_t*, TRI_json_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a hash-index
////////////////////////////////////////////////////////////////////////////////

TRI_index_t* TRI_CreateHashIndex (struct TRI_doc_collection_s*,
                                  TRI_vector_t* shapeList,
                                  bool unique);

                                  
                                  
////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    SKIPLIST INDEX
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////


SkiplistIndexElements* TRI_LookupSkiplistIndex (TRI_index_t*, TRI_json_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a hash-index
////////////////////////////////////////////////////////////////////////////////

TRI_index_t* TRI_CreateSkiplistIndex (struct TRI_doc_collection_s*,
                                      TRI_vector_t* shapeList,
                                      bool unique);

                                  
////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
