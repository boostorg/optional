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

#if defined BOOST_NO_CXX11_NOEXCEPT
BOOST_STATIC_ASSERT_MSG(false, "absent noexcept");
#endif

int main()
{
  return 0;
}


