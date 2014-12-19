// Copyright 2014 SDL plc
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \file

    for swapping objects of the same concrete type where just swapping their
    bytes will work.  will at least work on plain old data. memcpy is usually a
    fast intrinsic optimized by the compiler
*/

#ifndef GRAEHL_SHARED__SWAP_POD
#define GRAEHL_SHARED__SWAP_POD
#pragma once

namespace graehl {

#include <algorithm>  // not used, but people who use this will want to bring std::swap in anyway
#include <cstring>

template <class T>
inline void swap_pod(T& a, T& b) {
  using namespace std;
  unsigned const s = sizeof(T);
  char tmp[s];
  void* pt = (void*)tmp;
  void* pa = (void*)&a;
  void* pb = (void*)&b;
  memcpy(pt, pa, s);
  memcpy(pa, pb, s);
  memcpy(pb, pt, s);
}


}

#endif
