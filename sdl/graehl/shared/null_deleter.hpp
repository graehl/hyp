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

    shared_ptr helper for nondeleting references (where the refcount is
    meaningless, but you pay for it anyway to simplify - everything can be a
    shared_ptr)
*/

#ifndef GRAEHL__SHARED__NULL_DELETER_HPP
#define GRAEHL__SHARED__NULL_DELETER_HPP
#pragma once

#include <boost/shared_ptr.hpp>

namespace graehl {

struct null_deleter {
  void operator()(void const*) const {}
};

template <class V>
boost::shared_ptr<V> no_delete(V &v) {
  return boost::shared_ptr<V>(&v, null_deleter());
}

template <class V>
boost::shared_ptr<V> no_delete(V *v) {
  return boost::shared_ptr<V>(v, null_deleter());
}


}

#endif
