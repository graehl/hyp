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

    define span and related types for xmt library

    we use a different Span (even if integral types are same) in terms of
    StateId in Hypergraph library (and syntax based decoder)

*/

#ifndef SDL_TOKENS_HPP_20131022
#define SDL_TOKENS_HPP_20131022
#pragma once

#include <sdl/SharedPtr.hpp>
#include <sdl/Types.hpp>
#include <sdl/Position.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <utility>

namespace sdl {

typedef std::pair<Position, Position> TokenSpan;

inline Slice toSlice(std::string const& str, TokenSpan span) {
  Pchar data = arrayBegin(str);
  return Slice(data + span.first, data + span.second);
}

inline Slice toSlice(char const* data, TokenSpan span) {
  return Slice(data + span.first, data + span.second);
}

inline std::string substring(std::string const& str, TokenSpan span) {
  Pchar data = arrayBegin(str);
  return std::string(data + span.first, data + span.second);
}

inline std::string substring(char const* str, TokenSpan span) {
  return std::string(str + span.first, str + span.second);
}


/// for n tokens, n+1 positions (starting unicode char index). the last is string end (size). deletions of input chars must then be represented by empty token
template <class Token>
struct MonotoneAlignedToken {
  Token token;
  Position begin;
  TokenSpan span(Position end) const {
    return TokenSpan(begin, end);
  }
};

template <class Token>
struct AlignedToken : MonotoneAlignedToken<Token> {
  AlignedToken() {}
  AlignedToken(MonotoneAlignedToken<Token> const &token, Position end)
      : MonotoneAlignedToken<Token>(token)
      , end(end)
  {}
  Position end;
  TokenSpan span() const {
    return TokenSpan(this->begin, end);
  }
};

/// for n tokens, n+1 positions (starting unicode char index). the last is string end (size)
template <class Token>
struct MonotoneAlignedTokens : std::vector<MonotoneAlignedToken<Token> > {
  typedef std::vector<MonotoneAlignedToken<Token> > Base;
  typedef typename Base::const_iterator const_iterator;
  shared_ptr<std::string> pinput;

  Slice inputSlice(const_iterator i) const {
    return toSlice(*pinput, span(i));
  }

  std::string input(const_iterator i) const {
    return substring(*pinput, span(i));
  }

  TokenSpan span(const_iterator i) const {
    return i->span((i + 1)->begin);
  }

  AlignedToken<Token> aligned(const_iterator i) const {
    return AlignedToken<Token>(*i, (i + 1)->begin);
  }

  AlignedToken<Token> aligned(Position i) const { return aligned(this->begin() + i); }
  TokenSpan span(Position i) const { return span(this->begin() + i); }
  std::string input(Position i) const { return input(this->begin() + i); }
  Slice inputSlice(Position i) const { return inputSlice(this->begin() + i); }
};

namespace {
TokenSpan const kNullTokenSpan(kNullPosition, kNullPosition);
TokenSpan const kMinTokenSpan(kNullPosition, 0);
}

inline bool nullTokenSpan(TokenSpan const& span) {
  return span.first == kNullPosition;
}

inline void setNullTokenSpan(TokenSpan &span) {
  span.first = kNullPosition;
}

inline void setMinTokenSpan(TokenSpan &span) {
  span.first = kNullPosition;
  span.second = 0;
}

inline TokenSpan spanOrEmpty(TokenSpan span) {
  if (nullTokenSpan(span))
    span.first = span.second = (Position)0;
  return span;
}

inline Position len(TokenSpan span) {
  return span.second - span.first;
}

inline void growSpanRight(TokenSpan &span, Position i) {
  if (i >= span.second)
    span.second = i;
}

inline void growSpanLeft(TokenSpan &span, Position i) {
  if (i < span.first)
    span.first = i;
}

/**
   faster than span = TokenSpan(left, right),
   and simpler than span.left = left; span.right = right;
*/
inline void setSpan(TokenSpan &span, Position left, Position right) {
  span.first = left;
  span.second = right;
}

inline void growSpanRight(TokenSpan &span, TokenSpan const& cover) {
  growSpanRight(span, cover.second);
}

inline void growSpanLeft(TokenSpan &span, TokenSpan const& cover) {
  growSpanLeft(span, cover.first);
}

inline void growSpan(TokenSpan &span, Position i) {
  if (i < span.first)
    span.first = i;
  if (i >= span.second)
    span.second = i + 1;
}

inline void growSpan(TokenSpan &span, TokenSpan const& grow) {
  if (grow.first < span.first)
    span.first = grow.first;
  if (grow.second > span.second)
    span.second = grow.second;
}

inline void shift(TokenSpan &span, Position add) {
  span.first += add;
  span.second += add;
}

inline bool encloses(TokenSpan const& container, TokenSpan const& x) {
  return container.first <= x.first && container.second >= x.second;
}

inline bool contains(TokenSpan const& container, Position p) {
  return p >= container.first && p < container.second;
}

typedef std::vector<TokenSpan> TokenSpans;

struct PrintSpan : TokenSpan {
  PrintSpan(TokenSpan const& span)
      : TokenSpan(span)
  {}
  PrintSpan(Position l, Position r)
      : TokenSpan(l, r)
  {}
  friend inline std::ostream& operator<<(std::ostream &out, PrintSpan const& self) {
    self.print(out);
    return out;
  }
  void print(std::ostream &out) const {
    out << '[' << first << ',' << second << ')';
  }
};

/// the non-empty non-space spans in s[0..len)
inline void addTokenSpans(TokenSpans &spans, char const* s, Position len) {
  Position i = 0;
  for (;;) {
    if (i == len) return;
    if (s[i] != ' ') break;
    ++i;
  }
  TokenSpan span;
  span.first = span.second = i;
  for (;;) {
    assert(span.second < len && s[span.second] != ' ');
    if (++span.second == len) {
      spans.push_back(span);
      return;
    }
    if (s[span.second] == ' ') {
      spans.push_back(span);
      for (;;) {
        if (s[++span.second] != ' ') break;
        if (span.second == len) return;
      }
      span.first = span.second;
    }
  }
}

inline void addTokenSpans(TokenSpans &spans, std::string const& str) {
  Position len = (Position)str.size();
  if (len)
    addTokenSpans(spans, &str[0], len);
}

typedef std::vector<std::string> Tokens;

inline void spansToTokens(std::string const& str, TokenSpans const& spans, Tokens &tokens) {
  if (str.empty()) return;
  char const* s = &str[0];
  unsigned i = 0, n = spans.size();
  tokens.resize(n);
  for (; i < n; ++i) {
    TokenSpan const& span = spans[i];
    assert(span.first < str.size());
    assert(span.second <= str.size());
    tokens[i].assign(s + span.first, s + span.second);
  }

}


}

#endif
