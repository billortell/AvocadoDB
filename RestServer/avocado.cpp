////////////////////////////////////////////////////////////////////////////////
/// @brief AvocadoDB server
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
/// @author Copyright 2010-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "Basics/Common.h"

#include "RestServer/AvocadoServer.h"
#include "ResultGenerator/Initialise.h"

using namespace triagens;
using namespace triagens::basics;
using namespace triagens::rest;
using namespace triagens::avocado;

// -----------------------------------------------------------------------------
// --SECTION--                                                       USER MANUAL
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @page UserManual AvocadoDB User Manual
///
/// <ol>
///  <li>@ref Basics
///   <ol>
///    <li>@ref StartStop
///    </li>
///    <li>AvocadoScript
///     <ol>
///      <li>@ref SimpleQueries
///      </li>
///     </ol>
///    </li>
///    <li>@ref AQL
///     <ol>
///      <li>@ref Optimizer
///      </li>
///      <li>@ref IndexUsage
///      </li>
///     </ol>
///    </li>
///    <li>@ref AvocadoScript
///     <ol>
///      <li>@ref GeoCoordinates
///      </li>
///      <li>@ref Pagination
///      </li>
///     </ol>
///    </li>
///    <li>Vertices, Edges, and Graphs
///     <ol>
///      <li>@ref Graphs
///      </li>
///      <li>@ref JSModuleGraph
///      </li>
///     </ol>
///    </li>
///   </ol>
///  </li>
///  <li>Client Communication
///   <ol>
///    <li>@ref HttpInterface
///     <ol>
///      <li>@ref RestDocument
///      </li>
///     </ol>
///    </li>
///   </ol>
///  </li>
///  <li>Advanced Topics
///   <ol>
///    <li>Actions
///     <ol>
///      <li>@ref Actions
///      </li>
///      <li>@ref DefineAction
///      </li>
///     </ol>
///    </li>
///    <li>@ref HttpInterface
///     <ol>
///      <li>@ref RestSystem
///      </li>
///     </ol>
///    </li>
///    <li>@ref jsUnity
///    </li>
///   </ol>
///  </li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page Basics Basics
///
/// The AvocadoDB groups documents into collections. Each collection can be
/// accessed using queries. For simple queries involving just one collection,
/// you can use a fluent interface from within JavaScript code, see @ref
/// AvocadoScript. This interface allows you to select documents from one
/// collection based on simple search criteria. The @ref HttpInterface
/// lets you create, modify, or delete a single document via HTTP. For 
/// more complex queries, you can use the Avocado Query Language, which is 
/// an extension of SQL resp. UNQL.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page StartStopTOC
///
/// <ol>
///  <li>@ref StartStopHttp "Starting the HTTP Server"</li>
///  <li>@ref StartStopDebug "Starting the Debug Shell"</li>
///  <li>@ref StartStopOptions "Frequently Used Options"</li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page StartStop Starting the AvocadoDB
///
/// The AvocadoDB has two mode of operation: as server, where it will answer to
/// HTTP requests, see @ref HttpInterface, and a debug shell, where you can
/// access the database directly. Using the debug shell allows you to issue all
/// commands normally available in actions and transactions, see @ref
/// AvocadoScript.
///
/// You should never start more than one server for the same database,
/// independent from the mode of operation.
///
/// <hr>
/// @copydoc StartStopTOC
/// <hr>
///
/// @section StartStopHttp Starting the HTTP Server
///
/// The following command starts the AvocadoDB in server mode. You will be able
/// to access the server using HTTP request on port 8529. See below for a list
/// of frequently used options, see @ref CommandLine "here" for a complete list.
///
/// @verbinclude start2
///
/// @section StartStopDebug Starting the Debug Shell
///
/// The following command starts a debug shell. See below for a list of
/// frequently used options, see @ref CommandLine "here" for a complete list.
///
/// @verbinclude start1
///
/// @section StartStopOptions Frequently Used Options
///
/// The following command-line options are frequently used. For a full
/// list of options see @ref CommandLine "here".
///
/// @CMDOPT{@CA{database-directory}}
///
/// Uses the @CA{database-directory} as base directory. There is an alternative
/// version available for use in configuration files, see @ref
/// CommandLineAvocado "here".
///
/// @copydetails triagens::rest::ApplicationServerImpl::options
///
/// @CMDOPT{--log @CA{level}}
///
/// Allows the user to choose the level of information which is logged by the
/// server. The @CA{level} is specified as a string and can be one of the
/// following values: fatal, error, warning, info, debug, trace.  For more
/// information see @ref CommandLineLogging "here".
///
/// @copydetails triagens::avocado::AvocadoServer::_httpPort
///
/// @CMDOPT{--shell}
///
/// Opens a debug shell instead of starting the HTTP server.
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               INSTALLATION MANUAL
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @page InstallManual AvocadoDB Installation Manual
///
/// <ol>
///  <li>@ref Installing
///  <li>
///  <li>@ref Compiling
///  </li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  REFERENCE MANUAL
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @page RefManual AvocadoDB Reference Manual
///
/// <ol>
///  <li>@ref JSModules
///   <ol>
///    <li>@ref JSModuleConsole
///    </li>
///    <li>@ref JSModuleFs
///    </li>
///    <li>@ref JSModuleGraph
///    </li>
///    <li>@ref JSModuleInternal
///    </li>
///   </ol>
///  <li>@ref CommandLine
///   <ol>
///    <li>@ref CommandLineAvocado
///    </li>
///    <li>@ref CommandLineScheduler
///    </li>
///    <li>@ref CommandLineLogging
///    </li>
///    <li>@ref CommandLineRandom
///    </li>
///   </ol>
///  </li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page CommandLineTOC
///
/// <ol>
///  <li>configuration</li>
///  <li>daemon</li>
///  <li>gid</li>
///  <li>help</li>
///  <li>pid-file</li>
///  <li>uid</li>
///  <li>version</li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page CommandLine Command-Line Options
///
/// <hr>
/// @copydoc CommandLineTOC
/// <hr>
///
/// @section GeneralOptions General Options
///
/// @copydetails triagens::rest::ApplicationServerImpl::initFile
///
/// @CMDOPT{--daemon}
///
/// Runs the server as a daemon (as a background process). This parameter can
/// only be set if the pid (process id) file is specified. That is, unless a
/// value to the parameter pid-file is given, then the server will report an
/// error and exit.
///
/// @copydetails triagens::rest::ApplicationServerImpl::gid
///
/// @copydetails triagens::rest::ApplicationServerImpl::options
///
/// @copydetails triagens::rest::AnyServer::_pidFile
///
/// @CMDOPT{--show-io-backends}
///
/// If this option is specified, then the server will list available backends
/// and exit. This option is useful only when used in conjunction with the
/// option scheduler.backend. An integer is returned (which is platform
/// dependent) which indicates available backends on your platform. See libev
/// for further details and for the meaning of the integer returned. This
/// describes the allowed integers for @CODE{scheduler.backend}, see
/// @ref CommandLineScheduler "here" for details.
///
/// @CMDOPT{--supervisor}
///
/// Executes the server in supervisor mode. In the event that the server
/// unexpectedly terminates due to an internal error, the supervisor will
/// automatically restart the server. Setting this flag automatically implies
/// that the server will run as a daemon. Note that, as with the daemon flag,
/// this flag requires that the pid-file parameter will set.
///
/// @copydetails triagens::rest::ApplicationServerImpl::uid
///
/// @copydetails triagens::rest::ApplicationServerImpl::version
///
/// <hr>
/// Next steps:
///
///  - @ref CommandLineAvocado "Command-Line Options for the AvocadoDB"
///  - @ref CommandLineScheduler "Command-Line Options for Communication"
///  - @ref CommandLineLogging "Command-Line Options for Logging"
///  - @ref CommandLineRandom "Command-Line Options for Random Numbers"
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page CommandLineAvocadoTOC
///
/// <ol>
///  <li>database.directory</li>
///  <li>server.admin-port</li>
///  <li>server.http-port</li>
/// </ol>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @page CommandLineAvocado Command-Line Options for the AvocadoDB
///
/// <hr>
/// @copydoc CommandLineAvocadoTOC
/// <hr>
///
/// @copydetails triagens::avocado::AvocadoServer::_databasePath
///
/// @copydetails triagens::avocado::AvocadoServer::_adminPort
///
/// @copydetails triagens::avocado::AvocadoServer::_httpPort
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup AvocadoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an application server
////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[]) {
  TRIAGENS_RESULT_GENERATOR_INITIALISE;
  TRI_InitialiseVocBase();
  TRI_InitialiseQueryErrors();

  // create and start a AvocadoDB server
  AvocadoServer server(argc, argv);

  int res = server.start();

  // shutdown
  TRI_ShutdownVocBase();
  TRIAGENS_RESULT_GENERATOR_SHUTDOWN;

  return res;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
