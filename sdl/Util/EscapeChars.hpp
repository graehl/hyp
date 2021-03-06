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

    escape quote symbols on interior of string literal.
*/

#ifndef LWUTIL_QI_KARMA_SYMBOLS_HPP
#define LWUTIL_QI_KARMA_SYMBOLS_HPP
#pragma once

#include <utility>
#include <vector>
#include <sdl/graehl/shared/escape3.hpp>

namespace sdl {

using graehl::Escape3;
enum { kLimitLogChars = 100 };

template <class A, class B>
Escape3 escapeLimited(A a, B b) {
  return Escape3(a, b, kLimitLogChars);
}

std::string const kNullDbtStr("[NULL]");

template <class Dbt>
Escape3 escapeLimited(Dbt* dbt) {
  return dbt ? Escape3(dbt->get_data(), dbt->get_size(), kLimitLogChars)
             : Escape3(kNullDbtStr.data(), kNullDbtStr.size());
}

template <class Dbt>
Escape3 escapeLimitedLen(Dbt* dbt) {
  return dbt ? Escape3(dbt->get_data(), dbt->get_size(), kLimitLogChars, true)
             : Escape3(kNullDbtStr.data(), kNullDbtStr.size(), kLimitLogChars, true);
}

namespace Util {

struct EscapeChars {
  typedef std::pair<char, char const*> Esc;
  typedef std::vector<Esc> Escs;
  Escs escs;
  EscapeChars& add() { return *this; }
  EscapeChars& add(char c, char const* t) {
    escs.push_back(Esc(c, t));
    return *this;
  }
  EscapeChars& operator()(char c, char const* t) { return add(c, t); }
  template <class QiSym>
  void toQi(QiSym& qi) const {
    for (Escs::const_iterator i = escs.begin(), e = escs.end(); i != e; ++i) qi.add(i->second, i->first);
  }
  template <class KarmaSym>
  void toKarma(KarmaSym& karma) const {
    for (Escs::const_iterator i = escs.begin(), e = escs.end(); i != e; ++i) karma.add(i->first, i->second);
  }
};


}}

#endif
