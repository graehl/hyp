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

    'WARNING: '-prefixed warning messages.
*/

#ifndef GRAEHL_SHARED__WARN_HPP
#define GRAEHL_SHARED__WARN_HPP
#pragma once

#include <iostream>
#include <boost/function.hpp>
#include <graehl/shared/noreturn.hpp>
#include <stdexcept>

namespace graehl {


typedef boost::function<void(std::string const&)> string_consumer;

struct warn_consumer  // a string_consumer
    {
  std::ostream* o;
  std::string prefix;
  bool enabled;
  warn_consumer(std::ostream& o = std::cerr, std::string const& prefix = "WARNING: ", bool enabled = true)
      : o(&o), prefix(prefix), enabled(enabled) {}
  warn_consumer(warn_consumer const& o) : o(o.o), prefix(o.prefix), enabled(o.enabled) {}
  void operator()(std::string const& msg) const {
    if (o && enabled) *o << prefix << msg << '\n';
  }
};

struct ignore {
  ignore() {}
  template <class V>
  void operator()(V const&) const {}
};

// append_string_builder_newline is also a string_consumer

namespace {
warn_consumer const cerr_warnings;
ignore const ignore_warnings;
string_consumer const default_warn_consumer = cerr_warnings;
}

template <class Msg>
void warn(Msg const& msg, string_consumer msgto = default_warn_consumer) {
  msgto(to_string(msg));
}

template <class Msg>
inline void warn_and_throw(Msg const& msg, string_consumer msgto = default_warn_consumer) NORETURN;

template <class Msg>
inline void warn_and_throw(Msg const& msg, string_consumer msgto) {
  msgto(to_string(msg));
  throw(std::runtime_error(msg));
}

template <class Pre, class Msg>
void warnp(Pre const& pre, Msg const& msg, string_consumer msgto = default_warn_consumer) {
  msgto("(" + to_string(pre) + ") " + to_string(msg));
}


}

#endif
