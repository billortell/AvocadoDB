////////////////////////////////////////////////////////////////////////////////
/// @brief task for http communication
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
/// @author Achim Brandt
/// @author Copyright 2009-2011, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_HTTP_SERVER_HTTP_COMM_TASK_H
#define TRIAGENS_FYN_HTTP_SERVER_HTTP_COMM_TASK_H 1

#include <Basics/Common.h>

#include "Scheduler/ListenTask.h"
#include "HttpServer/HttpHandlerFactory.h"
#include "GeneralServer/GeneralCommTask.h"

namespace triagens {
  namespace rest {
    class HttpHandlerFactory;
    class HttpServerImpl;

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief task for http communication
    ////////////////////////////////////////////////////////////////////////////////

    class HttpCommTask : public GeneralCommTask<HttpServerImpl, HttpHandlerFactory> {
      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief constructors
        ////////////////////////////////////////////////////////////////////////////////

        HttpCommTask (HttpServerImpl* server, socket_t fd, ConnectionInfo const&);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief destructors
        ////////////////////////////////////////////////////////////////////////////////

        ~HttpCommTask ();

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        bool processRead ();

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        void addResponse (HttpResponse*);
    };
  }
}

#endif
