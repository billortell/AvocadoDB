////////////////////////////////////////////////////////////////////////////////
/// @brief join executor
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

#ifndef TRIAGENS_DURHAM_VOC_BASE_JOIN_EXECUTE_H
#define TRIAGENS_DURHAM_VOC_BASE_JOIN_EXECUTE_H 1

#include "VocBase/select-result.h"
#include "VocBase/join.h"
#include "VocBase/data-feeder.h"
#include "QL/optimize.h"
#include "QL/ast-query.h"
#include "V8/v8-c-utils.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Create a new select result from a join definition
////////////////////////////////////////////////////////////////////////////////

TRI_select_result_t* TRI_JoinSelectResult (const TRI_vocbase_t*, 
                                           TRI_select_join_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief Execute joins
////////////////////////////////////////////////////////////////////////////////

void TRI_ExecuteJoins (TRI_select_result_t*,
                       TRI_select_join_t*, 
                       TRI_qry_where_t*,
                       TRI_rc_context_t*,
                       const TRI_voc_size_t,
                       const TRI_voc_ssize_t);

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

