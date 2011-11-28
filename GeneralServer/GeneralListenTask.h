////////////////////////////////////////////////////////////////////////////////
/// @brief tasks used to establish connections
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

#ifndef FYN_GENERALSERVER_GENERAL_LISTEN_TASK_H
#define FYN_GENERALSERVER_GENERAL_LISTEN_TASK_H 1

#include <Basics/Common.h>

#include <Rest/ListenTask.h>

namespace triagens {
  namespace rest {

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief task used to establish connections
    ////////////////////////////////////////////////////////////////////////////////

    template<typename S>
    class GeneralListenTask : public ListenTask {
      GeneralListenTask (GeneralListenTask const&);
      GeneralListenTask& operator= (GeneralListenTask const&);

      public:

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief listen to given address and port
        ////////////////////////////////////////////////////////////////////////////////

        GeneralListenTask (S* server, string const& address, int port, bool reuseAddress)
          : Task("GeneralListenTask"), ListenTask(address, port, reuseAddress), server(server) {
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief listen to given port
        ////////////////////////////////////////////////////////////////////////////////

        GeneralListenTask (S* server, int port, bool reuseAddress)
          : Task("GeneralListenTask"), ListenTask(port, reuseAddress), server(server) {
        }

      protected:

        ////////////////////////////////////////////////////////////////////////////////
        /// {@inheritDoc}
        ////////////////////////////////////////////////////////////////////////////////

        bool handleConnected (socket_t socket, ConnectionInfo const& info) {
          ConnectionInfo newInfo = info;
          server->handleConnected(socket, newInfo);
          return true;
        }

      private:
        S* server;
    };
  }
}

#endif