// Copyright (C) 2017 Alexander Hirsch.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at:
//  1zeeky@gmail.com


#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/static_assert.hpp"
#include "boost/core/lightweight_test.hpp"
#include "boost/type_traits/has_trivial_copy.hpp"
#include "boost/type_traits/has_trivial_destructor.hpp"
#include "boost/type_traits/has_trivial_move_constructor.hpp"

struct Foo
{
};

struct NDFoo
{
  NDFoo(const NDFoo&) = default;
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
  NDFoo(NDFoo&&) = default;
#endif
  ~NDFoo() { ; }
};

struct NCFoo
{
  NCFoo(const NCFoo&) { ; }
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
  NCFoo(NCFoo&&) = default;
#endif
  ~NCFoo() = default;
};

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
struct NMFoo
{
  NMFoo(const NMFoo&) = default;
  NMFoo(NMFoo&&) { ; }
  ~NMFoo() = default;
};
#endif

template<typename T>
void test()
{
  using optional = boost::optional<T>;
  BOOST_STATIC_ASSERT(boost::has_trivial_destructor<optional>::value == boost::has_trivial_destructor<T>::value);
  BOOST_STATIC_ASSERT(boost::has_trivial_copy<optional>::value == boost::has_trivial_copy<T>::value);
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
  BOOST_STATIC_ASSERT(boost::has_trivial_move_constructor<optional>::value == boost::has_trivial_move_constructor<T>::value);
#endif
}

int main()
{
  test<Foo>();
  test<NDFoo>();
  test<NCFoo>();
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
  test<NMFoo>();
#endif
}
