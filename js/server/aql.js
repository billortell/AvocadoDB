////////////////////////////////////////////////////////////////////////////////
/// @brief Avocado Query Language
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
/// @author Dr. Frank Celler
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page FluentInterface Fluent Interface
///
/// The fluent interface for queries allows you to build simple queries
/// involving just one collection in a very natural way.
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      FLUENT QUERY
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief abstract fluent query constructor
////////////////////////////////////////////////////////////////////////////////

function AvocadoFluentQuery2 () {
  this._execution = null;

  this._skip = 0;
  this._limit = null;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief fluent query constructor for abstract representation
////////////////////////////////////////////////////////////////////////////////

function AvocadoFluentQueryAbstract (collection, query) {
  this._collection = collection;

  this._query = query;
  this._current = 0;
}

AvocadoFluentQueryAbstract.prototype = new AvocadoFluentQuery2();
AvocadoFluentQueryAbstract.prototype.constructor = AvocadoFluentQueryAbstract;

////////////////////////////////////////////////////////////////////////////////
/// @brief fluent query constructor for array representation
////////////////////////////////////////////////////////////////////////////////

function AvocadoFluentQueryArray (collection, documents) {
  this._collection = collection;

  this._documents = documents;
  this._current = 0;
}

AvocadoFluentQueryArray.prototype = new AvocadoFluentQuery2();
AvocadoFluentQueryArray.prototype.constructor = AvocadoFluentQueryArray;

////////////////////////////////////////////////////////////////////////////////
/// @brief fluent query constructor for internal representation
////////////////////////////////////////////////////////////////////////////////

function AvocadoFluentQueryInternal (collection) {
  this._collection = collection;

  this._query = null;

  this._select = null;
  this._joins = null;
  this._where = null;
}

AvocadoFluentQueryInternal.prototype = new AvocadoFluentQuery2();
AvocadoFluentQueryInternal.prototype.constructor = AvocadoFluentQueryInternal;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      FLUENT QUERY
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief limits an existing query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQuery2.prototype.limit = function (limit) {
  var copy;

  if (this._execution != null) {
    throw "query is already executing";
  }

  if (limit == 0) {
    return new AvocadoFluentQueryArray(this._collection, []);
  }

  copy = this.copyQuery();

  if (limit == null || copy._limit == 0) {
    return copy;
  }

  if (copy._limit == null) {
    copy._limit = limit;
  }
  else if (0 < limit) {
    if (0 < copy._limit && limit < copy._limit) {
      copy._limit = limit;
    }
    else if (copy._limit < 0) {
      copy = new AvocadoFluentQueryArray(copy._collection, copy.toArray());

      copy._limit = limit;
    }
  }
  else {
    if (copy._limit < 0 && copy._limit < limit) {
      copy._limit = limit;
    }
    else if (0 < copy._limit) {
      copy = new AvocadoFluentQueryArray(copy._collection, copy.toArray());

      copy._limit = limit;
    }
  }

  return copy;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief skips an existing query
///
/// @FUN{skip(@FA{number})}
///
/// Skips the first @FA{number} documents.
///
/// @verbinclude fluent31
///
/// The corresponding AQL queries would be:
///
/// @verbinclude fluent31-aql
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQuery2.prototype.skip = function (skip) {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @brief converts into an array
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQuery2.prototype.toArray = function () {
  var cursor;
  var result;

  if (this._execution != null) {
    throw "query is already executing";
  }

  result = [];

  while (this.hasNext()) {
    result.push(this.next());
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns all elements
///
/// @FUN{all()}
///
/// Selects all documents of a collection.
///
/// @verbinclude fluent23
///
/// The corresponding AQL query would be:
///
/// @verbinclude fluent23-aql
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQuery2.prototype.all = function () {
  if (this._execution != null) {
    throw "query is already executing";
  }

  return this.copyQuery();
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                           ABSTRACT REPRESENTATION
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief copies an abstract fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.copyQuery = function () {
  var copy;

  copy = new AvocadoFluentQueryAbstract(this._collection, this._query.copyQuery());

  copy._skip = this._skip;
  copy._limit = this._limit;

  return copy;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief executes an abstract fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.execute = function () {
  if (this._execution == null) {
    if (this._limit < 0) {
      throw "limit must be non-negative";
    }

    this._execution = true;

    if (0 < this._skip) {
      for (var i = 0;  i < this._skip && this._query.hasNext();  ++i) {
        this._query.useNext();
      }
    }

    this._current = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if the cursor is exhausted
///
/// @FUN{hasNext()}
///
/// The @FN{hasNext} operator returns @LIT{true}, if the cursor still has
/// documents.  In this case the next document can be accessed using the
/// @FN{next} operator, which will advance the cursor.
///
/// @verbinclude fluent3
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.hasNext = function () {
  this.execute();

  if (this._limit != null && this._limit <= this._current) {
    return false;
  }

  if (! this._query.hasNext()) {
    this._limit = 0;
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
///
/// @FUN{next()}
///
/// If the @FN{hasNext} operator returns @LIT{true}, if the cursor still has
/// documents.  In this case the next document can be accessed using the @FN{next}
/// operator, which will advance the cursor.
///
/// @verbinclude fluent28
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.next = function() {
  this.execute();

  if (this._limit != null) {
    if (this._limit <= this._current) {
      return false;
    }

    ++this._limit;
  }

  return this._query.next();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
///
/// @FUN{nextRef()}
///
/// If the @FN{hasNext} operator returns @LIT{true}, if the cursor still has
/// documents.  In this case the next document reference can be
/// accessed using the @FN{nextRef} operator, which will advance the
/// cursor.
///
/// @verbinclude fluent51
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.nextRef = function() {
  this.execute();

  if (this._limit != null) {
    if (this._limit <= this._current) {
      return false;
    }

    ++this._limit;
  }

  return this._query.nextRef();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief uses the next document
///
/// @FUN{useNext()}
///
/// If the @FN{hasNext} operator returns @LIT{true}, then the cursor still has
/// documents.  In this case the next document can be skipped using the
/// @FN{useNext} operator, which will advance the cursor and return @LIT{true}.
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryAbstract.prototype.useNext = function() {
  this.execute();

  if (this._limit != null) {
    if (this._limit <= this._current) {
      return;
    }

    ++this._limit;
  }

  this._query.useNext();
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                              ARRAY REPRESENTATION
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief copies an array fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.copyQuery = function() {
  var copy;

  copy = new AvocadoFluentQueryArray(this._collection, this._documents);

  copy._skip = this._skip;
  copy._limit = this._limit;

  return copy;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief executes an array fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.execute = function () {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if the cursor is exhausted
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.hasNext = function () {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.next = function() {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.nextRef = function() {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @brief uses the next document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryArray.prototype.useNext = function() {
  return "DELETE";
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                           INTERNAL REPRESENTATION
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief copies an internal fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.copyQuery = function() {
  var copy;

  copy = new AvocadoFluentQueryInternal(this._collection);

  copy._select = this._select;
  copy._joins = this._joins;
  copy._where = this._where;
  copy._skip = this._skip;
  copy._limit = this._limit;

  return copy;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief executes an internal fluent query
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.execute = function () {
  if (this._execution == null) {
    this._query = AQL_SELECT(this._select,
                             "$",
                             this._collection,
                             this._joins,
                             this._where,
                             this._skip,
                             this._limit);

    this._execution = this._query.execute();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if the cursor is exhausted
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.hasNext = function () {
  this.execute();

  return this._execution.hasNext();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.next = function () {
  this.execute();

  return this._execution.next();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the next result document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.nextRef = function () {
  this.execute();

  return this._execution.nextRef();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief uses the next document
////////////////////////////////////////////////////////////////////////////////

AvocadoFluentQueryInternal.prototype.useNext = function () {
  this.execute();

  return this._execution.useNext();
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                        COLLECTION
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AQL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief returns all elements
////////////////////////////////////////////////////////////////////////////////

AvocadoCollection.prototype.T_all = function () {
  return "DELETE";
}

AvocadoEdgesCollection.prototype.T_all = AvocadoCollection.prototype.T_all;

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up a document
////////////////////////////////////////////////////////////////////////////////

AvocadoCollection.prototype.T_document = function (id) {
  return "DOCUMENT";
}

AvocadoEdgesCollection.prototype.T_document = AvocadoCollection.prototype.T_document;

////////////////////////////////////////////////////////////////////////////////
/// @brief limits an existing query
////////////////////////////////////////////////////////////////////////////////

AvocadoCollection.prototype.T_limit = function (limit) {
  return "DELETE";
}

AvocadoEdgesCollection.prototype.T_limit = AvocadoCollection.prototype.T_limit;

////////////////////////////////////////////////////////////////////////////////
/// @brief skips an existing query
////////////////////////////////////////////////////////////////////////////////

AvocadoCollection.prototype.T_skip = function (skip) {
  return "DELETE";
}

AvocadoEdgesCollection.prototype.T_skip = AvocadoCollection.prototype.T_skip;

////////////////////////////////////////////////////////////////////////////////
/// @brief converts into an array
////////////////////////////////////////////////////////////////////////////////

AvocadoCollection.prototype.toArray = function () {
  return "DELETE";
}

AvocadoEdgesCollection.prototype.toArray = AvocadoCollection.prototype.toArray;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// @addtogroup\\|// --SECTION--\\|/// @page\\|/// @}\\)"
// End:
