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

#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/core/lightweight_test.hpp"

#if defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)

int main()
{
}

#else

struct NotDefaultConstructible
{
	NotDefaultConstructible() = delete;
};

void test_tc_base()
{
  boost::optional<NotDefaultConstructible> o;
  
  BOOST_TEST(boost::none == o);
}

int main()
{
  test_tc_base();
  return boost::report_errors();
}

#endif
