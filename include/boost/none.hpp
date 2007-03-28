// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional/ for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#ifndef BOOST_NONE_17SEP2003_HPP
#define BOOST_NONE_17SEP2003_HPP

#include <boost/detail/workaround.hpp>

namespace boost {

#if BOOST_WORKAROUND(__BORLANDC__, <= 0x564)

namespace detail { struct none_helper{}; }

typedef int detail::none_helper::*none_t ;

none_t const none = ((none_t)0) ;

#else

enum none_t {none = 0};

#endif

} // namespace boost

#endif

