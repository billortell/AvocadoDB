////////////////////////////////////////////////////////////////////////////////
/// @brief force symbols into programm, initialise globals
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

#include "init.h"

#include "BasicsC/hashes.h"
#include "BasicsC/logging.h"
#include "BasicsC/random.h"
#include "BasicsC/socket-utils.h"

#include "build.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Configuration
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialise function
////////////////////////////////////////////////////////////////////////////////

void TRI_InitialiseC () {
  TRI_InitialiseError();
  TRI_InitialiseLogging(true);
  TRI_InitialiseHashes();
  TRI_InitialiseRandom();
  TRI_InitialiseSockets();

  LOG_TRACE("%s", "$Revision: BASICS-C " TRIAGENS_VERSION " (c) triAGENS GmbH $");

#ifdef TRI_NCURSES_VERSION
  LOG_TRACE("%s", "$Revision: NCURSES " TRI_NCURSES_VERSION " $");
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief shutdown function
////////////////////////////////////////////////////////////////////////////////

void TRI_ShutdownC () {
  TRI_ShutdownSockets();
  TRI_ShutdownRandom();
  TRI_ShutdownHashes();
  TRI_ShutdownLogging();
  TRI_ShutdownError();
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
