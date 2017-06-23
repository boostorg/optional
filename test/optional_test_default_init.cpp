// Copyright (C) 2017 Vinnie Falco
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at:
//  https://github.com/vinniefalco
//

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/core/lightweight_test.hpp"

#include "optional_test_common.cpp"

struct T
{
  T() = default;
  T(int){};
  char buf[1];
};

int main()
{
  try
  {
    boost::optional<T> t;
    t.emplace();
    t->buf[0] = 'x';
    BOOST_TEST(t->buf[0] == 'x');
    t.emplace(1);
    BOOST_TEST(t->buf[0] == 'x');
    t.emplace();
    BOOST_TEST(t->buf[0] == 'x');
  }
  catch ( ... )
  {
    BOOST_ERROR("Unexpected Exception caught!");
  }

  return boost::report_errors();
}
