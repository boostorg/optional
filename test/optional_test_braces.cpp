// Copyright (C) 2016 Jürgen Hunold
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.


#include "boost/optional/optional.hpp"

#include <iostream>

#include "boost/core/lightweight_test.hpp"

using boost::optional;


struct Init
{
    Init(int,double);
};

#ifndef BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX


template <typename T>
void test_brace_init()
{
  optional<T> o = {1, 1.1};
  BOOST_TEST(o);
}

template <typename T>
void test_brace_assign()
{
  optional<T> o;
  o = {1, 1.1};
  BOOST_TEST(o);
}
#endif

int main()
{
#ifndef BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
  test_brace_init<Init>();
  test_brace_assign<Init>();
#endif

  return boost::report_errors();
}
