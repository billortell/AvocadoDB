////////////////////////////////////////////////////////////////////////////////
/// @brief threads
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

#ifndef TRIAGENS_BASICS_C_THREADS_H
#define TRIAGENS_BASICS_C_THREADS_H 1

#include "BasicsC/common.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief posix threads
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_HAVE_POSIX_THREADS
#include "BasicsC/threads-posix.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief windows threads
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_HAVE_WIN32_THREADS
#include "BasicsC/threads-win32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                            THREAD
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Threading
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief initialises a thread
////////////////////////////////////////////////////////////////////////////////

void TRI_InitThread (TRI_thread_t*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Threading
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the current process identifier
////////////////////////////////////////////////////////////////////////////////

TRI_pid_t TRI_CurrentProcessId (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the current thread process identifier
////////////////////////////////////////////////////////////////////////////////

TRI_tpid_t TRI_CurrentThreadProcessId (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the current thread identifier
////////////////////////////////////////////////////////////////////////////////

TRI_tid_t TRI_CurrentThreadId (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief starts a thread
////////////////////////////////////////////////////////////////////////////////

bool TRI_StartThread (TRI_thread_t* thread, void (*starter)(void*), void* data);

////////////////////////////////////////////////////////////////////////////////
/// @brief trys to stops a thread
////////////////////////////////////////////////////////////////////////////////

void TRI_StopThread (TRI_thread_t* thread);

////////////////////////////////////////////////////////////////////////////////
/// @brief detachs a thread
////////////////////////////////////////////////////////////////////////////////

void TRI_DeatchThread (TRI_thread_t* thread);

////////////////////////////////////////////////////////////////////////////////
/// @brief waits for a thread to finish
////////////////////////////////////////////////////////////////////////////////

void TRI_JoinThread (TRI_thread_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief sends a signal to the thread
////////////////////////////////////////////////////////////////////////////////

bool TRI_SignalThread (TRI_thread_t* thread, int signal);

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if we are the thread
////////////////////////////////////////////////////////////////////////////////

bool TRI_IsSelfThread (TRI_thread_t* thread);

////////////////////////////////////////////////////////////////////////////////
/// @brief allow cancellation
////////////////////////////////////////////////////////////////////////////////

void TRI_AllowCancelation (void);

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
