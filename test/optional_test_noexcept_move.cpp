// Copyright (C) 2014 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at:
//  akrzemi1@gmail.com

#include "boost/static_assert.hpp"
#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


using boost::optional;


#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
#ifndef BOOST_NO_NOEXCEPT

// these 4 classes have different noexcept signatures in move operations
struct NothrowBoth {
  NothrowBoth(NothrowBoth&&) BOOST_NOEXCEPT_IF(true) {};
  void operator=(NothrowBoth&&) BOOST_NOEXCEPT_IF(true) {};
};
struct NothrowCtor {
  NothrowCtor(NothrowCtor&&) BOOST_NOEXCEPT_IF(true) {};
  void operator=(NothrowCtor&&) BOOST_NOEXCEPT_IF(false) {};
};
struct NothrowAssign {
  NothrowAssign(NothrowAssign&&) BOOST_NOEXCEPT_IF(false) {};
  void operator=(NothrowAssign&&) BOOST_NOEXCEPT_IF(true) {};
};
struct NothrowNone {
  NothrowNone(NothrowNone&&) BOOST_NOEXCEPT_IF(false) {};
  void operator=(NothrowNone&&) BOOST_NOEXCEPT_IF(false) {};
};

void test_noexcept_as_defined() // this is a compile-time test
{
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_constructible<NothrowBoth>::value);
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_assignable<NothrowBoth>::value);
  
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_constructible<NothrowCtor>::value);
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_assignable<NothrowCtor>::value);
  
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_constructible<NothrowAssign>::value);
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_assignable<NothrowAssign>::value);
  
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_constructible<NothrowNone>::value);
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_assignable<NothrowNone>::value);
}

void test_noexcept_on_optional() // this is a compile-time test
{
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_constructible<optional<NothrowBoth> >::value);
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_assignable<optional<NothrowBoth> >::value);
  BOOST_STATIC_ASSERT(BOOST_NOEXCEPT_EXPR(optional<NothrowBoth>()));
    
  BOOST_STATIC_ASSERT(::boost::is_nothrow_move_constructible<optional<NothrowCtor> >::value);
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_assignable<optional<NothrowCtor> >::value);
  BOOST_STATIC_ASSERT(BOOST_NOEXCEPT_EXPR(optional<NothrowCtor>()));
    
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_constructible<optional<NothrowAssign> >::value);
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_assignable<optional<NothrowAssign> >::value);
  BOOST_STATIC_ASSERT(BOOST_NOEXCEPT_EXPR(optional<NothrowAssign>()));
    
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_constructible<optional<NothrowNone> >::value);
  BOOST_STATIC_ASSERT(!::boost::is_nothrow_move_assignable<optional<NothrowNone> >::value);
  BOOST_STATIC_ASSERT(BOOST_NOEXCEPT_EXPR(optional<NothrowNone>()));
}

#endif // !defned BOOST_NO_NOEXCEPT
#endif // !defined BOOST_NO_CXX11_RVALUE_REFERENCES

int main()
{
  return 0;
}


