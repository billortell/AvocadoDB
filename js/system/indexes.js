////////////////////////////////////////////////////////////////////////////////
/// @brief system actions for indexes
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

var defineHttpSystemAction = require("avocado").defineHttpSystemAction;

// -----------------------------------------------------------------------------
// --SECTION--                                            administration actions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ActionsAdmin
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief returns information about all indexes of a collection
///
/// @REST{GET /_system/collection/indexes?collection=@FA{identifier}}
///
/// Returns information about all indexes of a collection of the database.
////////////////////////////////////////////////////////////////////////////////

defineHttpSystemAction("collection/indexes",
  function (req, res) {
    try {
      result = {};
      result.name = req.collection._name;
      result.id = req.collection._id;
      result.indexes = req.collection.getIndexes();

      actionResult(req, res, 200, result);
    }
    catch (err) {
      actionError(req, res, err);
    }
  },
  {
    parameters : {
      collection : "collection-identifier"
    }
  }
);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @page\\|/// @}\\)"
// End:
