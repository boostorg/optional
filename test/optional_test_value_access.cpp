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
//
// Revisions:
//
#include<iostream>
#include<stdexcept>
#include<string>

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/bind/apply.hpp" // Included just to test proper interaction with boost::apply<> as reported by Daniel Wallin
#include "boost/mpl/bool.hpp"
#include "boost/mpl/bool_fwd.hpp"  // For mpl::true_ and mpl::false_

#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/none.hpp"

#include "boost/test/minimal.hpp"

#include "optional_test_common.cpp"

struct IntWrapper
{
  int _i;
  IntWrapper(int i) : _i(i) {}
  bool operator==(IntWrapper const& rhs) const { return _i == rhs._i; }
};

template <typename T>
void test_function_value_for()
{
  optional<T> o0;
  optional<T> o1(1);
  const optional<T> oC(2);
  
  try
  {
    T& v = o1.value();
    BOOST_CHECK(v == 1);
  }
  catch(...)
  {
    BOOST_CHECK(false);
  }
  
  try
  {
    T const& v = oC.value();
    BOOST_CHECK(v == 2);
  }
  catch(...)
  {
    BOOST_CHECK(false);
  }
  
  try
  {
    T& v = o0.value();
    BOOST_CHECK(false);
    unused_variable(v);
  }
  catch(boost::bad_optional_access const&)
  {
  }
  catch(...)
  {
    BOOST_CHECK(false);
  }
}

void test_function_value()
{
    test_function_value_for<int>();
    test_function_value_for<float>();
    test_function_value_for<IntWrapper>();
}

int test_main( int, char* [] )
{
  try
  {
    test_function_value();

  }
  catch ( ... )
  {
    BOOST_ERROR("Unexpected Exception caught!");
  }

  return 0;
}


