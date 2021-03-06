////////////////////////////////////////////////////////////////////////////////
/// @brief json shaper used to compute the shape of an json object
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

#ifndef TRIAGENS_SHAPED_JSON_JSON_SHAPER_H
#define TRIAGENS_SHAPED_JSON_JSON_SHAPER_H 1

#include "BasicsC/common.h"

#include "BasicsC/json.h"
#include "ShapedJson/shaped-json.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       JSON SHAPER
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Json
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief json shaper
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_shaper_s {
  TRI_shape_aid_t (*findAttributeName) (struct TRI_shaper_s*, char const*);
  char const* (*lookupAttributeId) (struct TRI_shaper_s*, TRI_shape_aid_t);
  TRI_shape_t const* (*findShape) (struct TRI_shaper_s*, TRI_shape_t*);
  TRI_shape_t const* (*lookupShapeId) (struct TRI_shaper_s*, TRI_shape_sid_t);

  TRI_shape_path_t const* (*lookupAttributePathByPid) (struct TRI_shaper_s*, TRI_shape_pid_t);
  TRI_shape_pid_t (*findAttributePathByName) (struct TRI_shaper_s*, char const*);

  TRI_associative_synced_t _attributePathsByName;
  TRI_associative_synced_t _attributePathsByPid;

  TRI_shape_pid_t _nextPid;
  TRI_mutex_t _attributePathLock;

  TRI_shape_sid_t _sidNull;
  TRI_shape_sid_t _sidBoolean;
  TRI_shape_sid_t _sidNumber;
  TRI_shape_sid_t _sidShortString;
  TRI_shape_sid_t _sidLongString;
  TRI_shape_sid_t _sidList;
}
TRI_shaper_t;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      ARRAY SHAPER
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Json
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a simple, array-based shaper
////////////////////////////////////////////////////////////////////////////////

TRI_shaper_t* TRI_CreateArrayShaper (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array-based shaper, but does not free the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyArrayShaper (TRI_shaper_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an array-based shaper and frees the pointer
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeArrayShaper (TRI_shaper_t*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                            SHAPER
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Json
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates the attribute path
////////////////////////////////////////////////////////////////////////////////

char const* TRI_AttributeNameShapePid (TRI_shaper_t*, TRI_shape_pid_t);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               protected functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Json
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialises the shaper
////////////////////////////////////////////////////////////////////////////////

void TRI_InitShaper (TRI_shaper_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys the shaper
////////////////////////////////////////////////////////////////////////////////

void TRI_DestroyShaper (TRI_shaper_t* shaper);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates or finds the basic types
////////////////////////////////////////////////////////////////////////////////

bool TRI_InsertBasicTypesShaper (TRI_shaper_t*);

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
