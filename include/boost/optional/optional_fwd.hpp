// Copyright (C) 2003, 2008 Fernando Luis Cacciola Carballal.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
// Revisions:
// 10 May 2008 (added swap related forward declaration) Niels Dekker
// 17 Apr 2014 (added noexcept) Andrzej Krzemienski 
//
#ifndef BOOST_OPTIONAL_OPTIONAL_FWD_FLC_19NOV2002_HPP
#define BOOST_OPTIONAL_OPTIONAL_FWD_FLC_19NOV2002_HPP
#include <boost/config.hpp>
#include <boost/type_traits/is_nothrow_move_assignable.hpp>
#include <boost/type_traits/is_nothrow_move_constructible.hpp>


namespace boost {

template<class T> class optional ;

template<class T> void swap ( optional<T>& , optional<T>& )
    BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value && ::boost::is_nothrow_move_assignable<T>::value);

template<class T> struct optional_swap_should_use_default_constructor ;

} // namespace boost

#endif

