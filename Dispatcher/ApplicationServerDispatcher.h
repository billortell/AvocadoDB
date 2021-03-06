////////////////////////////////////////////////////////////////////////////////
/// @brief application server with dispatcher
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
/// @author Copyright 2009-2011, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_REST_APPLICATION_SERVER_DISPATCHER_H
#define TRIAGENS_FYN_REST_APPLICATION_SERVER_DISPATCHER_H 1

#include "ApplicationServer/ApplicationServer.h"

namespace triagens {
  namespace rest {
    class Dispatcher;

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief application server with dispatcher
    ////////////////////////////////////////////////////////////////////////////////

    class ApplicationServerDispatcher : virtual public ApplicationServer {
      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief constructs a new skeleton
        ////////////////////////////////////////////////////////////////////////////////

        static ApplicationServerDispatcher* create (string const& description, string const& version);

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief returns the dispatcher
        ////////////////////////////////////////////////////////////////////////////////

        virtual Dispatcher* dispatcher () const = 0;

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief builds the dispatcher
        ////////////////////////////////////////////////////////////////////////////////

        virtual void buildDispatcher () = 0;

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief builds the dispatcher reporter
        ////////////////////////////////////////////////////////////////////////////////

        virtual void buildDispatcherReporter () = 0;

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief builds the dispatcher queue
        ////////////////////////////////////////////////////////////////////////////////

        virtual void buildStandardQueue (size_t nrThreads) = 0;

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief builds the named dispatcher queue
        ////////////////////////////////////////////////////////////////////////////////

        virtual void buildNamedQueue (string const& name, size_t nrThreads) = 0;
    };
  }
}

#endif


