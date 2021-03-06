////////////////////////////////////////////////////////////////////////////////
/// @brief basic memory management
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

#include "BasicsC/common.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Memory
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Init flag for deliberate out-of-memory cases
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_ENABLE_MEMFAIL
static bool Initialised = false;
#endif 

////////////////////////////////////////////////////////////////////////////////
/// @brief Probability for deliberate out-of-memory cases
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_ENABLE_MEMFAIL
static double MemFailProbability;
#endif 

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Memory
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief Enable deliberate out-of-memory cases for TRI_Allocate
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_ENABLE_MEMFAIL
void TRI_ActivateMemFailures (double probability) {
  srand(32452843 + time(NULL) * 49979687);
  MemFailProbability = probability;
  Initialised = true;
}
#endif 

////////////////////////////////////////////////////////////////////////////////
/// @brief Deactivate deliberate out-of-memory cases for TRI_Allocate
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_ENABLE_MEMFAIL
void TRI_DeactiveMemFailures (void) {
  Initialised = false;
  MemFailProbability = 0.0;
}
#endif 

////////////////////////////////////////////////////////////////////////////////
/// @brief basic memory management for allocate
////////////////////////////////////////////////////////////////////////////////

void* TRI_Allocate (uint64_t n) {
  char* m;

#ifdef TRI_ENABLE_MEMFAIL
  // if configured with --enable-memfail, we can make calls to malloc fail
  // deliberately. This makes testing memory bottlenecks easier.
  if (Initialised) {
    if (RAND_MAX * MemFailProbability >= rand ()) {
      errno = ENOMEM;
      return NULL;
    }
  }
#endif

  m = malloc((size_t) n);
  memset(m, 0, (size_t) n);

  return m;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief basic memory management for reallocate
////////////////////////////////////////////////////////////////////////////////

void* TRI_Reallocate (void* m, uint64_t n) {
  return realloc(m, n);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief basic memory management for deallocate
////////////////////////////////////////////////////////////////////////////////

void TRI_Free (void* m) {
  free(m);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
