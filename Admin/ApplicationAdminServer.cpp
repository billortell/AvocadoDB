////////////////////////////////////////////////////////////////////////////////
/// @brief application simple user and session management feature
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
/// @author Copyright 2011-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "ApplicationAdminServer.h"

#include "build.h"

#include "Admin/RestAdminFeConfigurationHandler.h"
#include "Admin/RestAdminLogHandler.h"
#include "Admin/RestHandlerCreator.h"
#include "Admin/RestVersionHandler.h"
#include "Basics/ProgramOptionsDescription.h"
#include "HttpServer/HttpHandlerFactory.h"
#include "HttpServer/PathHandler.h"
#include "Logger/Logger.h"
#include "Rest/HttpResponse.h"

using namespace std;
using namespace triagens::basics;
using namespace triagens::rest;
using namespace triagens::admin;

// -----------------------------------------------------------------------------
// --SECTION--                                             static public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RestServer
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new feature
////////////////////////////////////////////////////////////////////////////////

ApplicationAdminServer* ApplicationAdminServer::create (ApplicationServer*) {
  return new ApplicationAdminServer();
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RestServer
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

ApplicationAdminServer::ApplicationAdminServer ()
  : _allowLogViewer(false),
    _allowAdminDirectory(false),
    _allowFeConfiguration(false),
    _allowVersion(false),
    _adminDirectory(),
    _pathOptions(0),
    _feConfiguration(),
    _name(),
    _version() {
  _pathOptions = new PathHandler::Options();

#ifndef TRI_ENABLE_RELATIVE
#ifdef _PKGDATADIR_
  _adminDirectory = string(_PKGDATADIR_) + "/html/admin";
#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

ApplicationAdminServer::~ApplicationAdminServer () {
  delete reinterpret_cast<PathHandler::Options*>(_pathOptions);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RestServer
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief add a log viewer
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowLogViewer () {
  _allowLogViewer = true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief allows for a webadmin directory
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowAdminDirectory () {
  _allowAdminDirectory = true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief allows for a webadmin directory
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowAdminDirectory (string const& adminDirectory) {
  _allowAdminDirectory = true;
  _adminDirectory = adminDirectory;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief allows for a front-end configuration
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowFeConfiguration () {
  _allowFeConfiguration = true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief allows for a version handler using the default version
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowVersion () {
  _allowVersion = true;
  _version = TRIAGENS_VERSION;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief allows for a version handler
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::allowVersion (string name, string version) {
  _allowVersion = true;
  _name = name;
  _version = version;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds the http handlers
///
/// Note that the server does not claim ownership of the factory.
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::addBasicHandlers (HttpHandlerFactory* factory) {
  if (_allowVersion) {
    pair<string, string>* data = new pair<string,string>(_name, _version);
    factory->addHandler("/version", RestHandlerCreator<RestVersionHandler>::createData<pair<string,string> const*>, (void*) data);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief adds the http handlers for administration
///
/// Note that the server does not claim ownership of the factory.
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::addHandlers (HttpHandlerFactory* factory, string const& prefix) {

  // .............................................................................
  // add log viewer
  // .............................................................................

  if (_allowLogViewer) {
    factory->addHandler("/admin/log", RestHandlerCreator<RestAdminLogHandler>::createNoData, 0);
  }

  // .............................................................................
  // add a web-admin directory
  // .............................................................................
  
  if (_allowAdminDirectory) {
    if (! _adminDirectory.empty()) {
      LOGGER_INFO << "using JavaScript front-end files stored at '" << _adminDirectory << "'";

      reinterpret_cast<PathHandler::Options*>(_pathOptions)->path = _adminDirectory;
      reinterpret_cast<PathHandler::Options*>(_pathOptions)->contentType = "text/plain";
      reinterpret_cast<PathHandler::Options*>(_pathOptions)->allowSymbolicLink = false;
      reinterpret_cast<PathHandler::Options*>(_pathOptions)->defaultFile = "index.html";
      
      factory->addPrefixHandler(prefix, RestHandlerCreator<PathHandler>::createData<PathHandler::Options*>, _pathOptions);
    }
  }
  
  // .............................................................................
  // add a front-end configuration
  // .............................................................................
  
  if (_allowFeConfiguration) {
    factory->addHandler(prefix + "/fe-configuration", RestHandlerCreator<RestAdminFeConfigurationHandler>::createData<char const*>, (void*) _feConfiguration.c_str());
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        ApplicationFeature methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup RestServer
/// @{
////////////////////////////////////////////////////////////////////////////////

void ApplicationAdminServer::setupOptions (map<string, basics::ProgramOptionsDescription>& options) {
  if (_allowAdminDirectory) {
    options[ApplicationServer::OPTIONS_SERVER + ":help-admin"]
      ("server.admin-directory", &_adminDirectory, "directory containing the ADMIN front-end")
    ;
  }

  if (_allowFeConfiguration) {
    options[ApplicationServer::OPTIONS_SERVER + ":help-admin"]
      ("server.fe-configuration", &_feConfiguration, "file to store the front-end preferences")
    ;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
