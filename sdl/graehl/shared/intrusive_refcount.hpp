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

   usage:

   namespace my {

     struct mine : private boost::instrusive_refcount<mine> {
       typedef mine self_type;
       friend void intrusive_ptr_add_ref(self_type *p) { p->add_ref();  }
       friend void intrusive_ptr_release(self_type *p) { p->release(p); }
     };

          //   (a non-virtual dtor requires the CRTP <mine> template)

     boost::intrusive_ptr<mine> p(new mine());
   }

*/

#ifndef GRAEHL__SHARED__INTRUSIVE_REFCOUNT_HPP
#define GRAEHL__SHARED__INTRUSIVE_REFCOUNT_HPP
#pragma once

#include <graehl/shared/alloc_new_delete.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/detail/atomic_count.hpp>
#include <boost/utility/enable_if.hpp>
#include <cassert>

namespace graehl {

using boost::detail::atomic_count;

/**
   U is a user allocator e.g. boost::default_user_allocator_new_delete - but
   make sure to get objects' memory using U::malloc() if you change the default

   for use in boost::intrusive_ptr, it's required that InitCount be UniqueCount
   - 1, because newly created intrusive_ptr cause add_ref to happen.
*/
template <class T, class R = atomic_count, class U = alloc_new_delete, long UniqueCount = 1>
struct intrusive_refcount {
  typedef void is_refcounted_enable;  // for is_refcounted
  typedef T intrusive_type;
  typedef U user_allocator;
  typedef T pointed_type;

  // for copy-on-write - may not give the thread-safety you expect, though (but if it's update+copy at same
  // time, there's a race no matter what)
  bool unique() const { return refcount == UniqueCount; }

  // please don't call these directly - only let boost::intrusive_ptr<T> do it:
  inline void add_ref() const { ++refcount; }
  inline void release(T const* tc) const {
    T* t = const_cast<T*>(tc);
    if (!--refcount) {
      t->~T();
      U::free((char*)t);
    }
  }
  inline void release() const { release(derivedPtr()); }

  typedef T self_type;

  template <class A0>
  static T* construct(A0 const& a0) {
    T* r = (T*)U::malloc(sizeof(T));
    return new (&r) T(a0);
  }

  inline T const* derivedPtr() const { return static_cast<T const*>(this); }

  intrusive_refcount() : refcount(0) {}
  intrusive_refcount(intrusive_refcount const& o) : refcount(0) {}
  ~intrusive_refcount() { assert(refcount == 0); }

 private:
  mutable R refcount;
};

template <typename UserAlloc, typename element_type>
element_type* construct() {

  element_type* const ret = (element_type*)UserAlloc::malloc(sizeof(element_type));
  if (!ret) return ret;
  try {
    new (ret) element_type();
  } catch (...) {
    UserAlloc::free((char*)ret);
    throw;
  }
  return ret;
}

template <typename UserAlloc, typename element_type>
element_type* construct_copy(element_type const& copy_me) {
  element_type* const ret = (element_type*)UserAlloc::malloc(sizeof(element_type));
  if (!ret) return ret;
  try {
    new (ret) element_type(copy_me);
  } catch (...) {
    UserAlloc::free((char*)ret);
    throw;
  }
  return ret;
}

template <class T>
struct intrusive_traits;

template <class T>
struct intrusive_traits {
  typedef typename T::user_allocator user_allocator;
  // TODO: thread_safe constant? only true for atomic_count?
};

/// this trait means you have add_ref() and release(p) members and is used to prevent making a refcount around
/// a refcount
template <class T, class Enable = void>
struct is_refcounted {
  enum { value = 0 };
};

template <class T>
struct is_refcounted<T, typename T::is_refcounted_enable> {
  enum { value = 1 };
};
}

namespace boost {
template <class T>
inline typename boost::enable_if<graehl::is_refcounted<T> >::type intrusive_ptr_add_ref(T const* p) {
  p->add_ref();
}

template <class T>
inline typename boost::enable_if<graehl::is_refcounted<T> >::type intrusive_ptr_release(T const* p) {
  p->release(p);
}
}

// clang requires we declare the above before using intrusive_ptr ...

#include <boost/intrusive_ptr.hpp>

namespace graehl {

template <class T, class Enable = void>
struct shared_ptr_maybe_intrusive {
  typedef boost::shared_ptr<T> type;
};

template <class T>
struct shared_ptr_maybe_intrusive<T, typename boost::enable_if<is_refcounted<T> >::type> {
  typedef boost::intrusive_ptr<T> type;
};

template <class T>
// typename boost::enable_if<is_refcountend<T>,T>::type
inline T* intrusive_clone(
    T const& x)  // result has refcount of 0 - must delete yourself or use to build an intrusive_ptr
{
  return construct_copy<typename intrusive_traits<T>::user_allocator, T>(x);
}

template <class T>
// enable_if ...
typename boost::intrusive_ptr<T> intrusive_copy_on_write(boost::intrusive_ptr<T> const& p) {
  assert(p);
  return p->unique() ? p : intrusive_clone(*p);
}

template <class T>
// enable_if ...
void intrusive_make_unique(boost::intrusive_ptr<T>& p) {
  assert(p);
  if (!p->unique()) p.reset(intrusive_clone(*p));
}

template <class T>
// typename boost::enable_if<typename intrusive_traits<T>::user_allocator>::type
void intrusive_make_valid_unique(boost::intrusive_ptr<T>& p) {
  if (!p)
    p.reset(construct<typename intrusive_traits<T>::user_allocator, T>());
  else if (!p->unique())
    p.reset(intrusive_clone(*p));
}

template <class T>
struct intrusive_deleter {
  void operator()(T* p) {
    if (p) intrusive_ptr_release(p);
  }
};

template <class T>
boost::shared_ptr<T> shared_from_intrusive(T* p) {
  if (p) intrusive_ptr_add_ref(p);
  return boost::shared_ptr<T>(p, intrusive_deleter<T>());
}


}

#endif
