//-----------------------------------------------------------------------------
// boost libs/optional/optional_test.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2002
// Eric Friedman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/optional.hpp"

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>
#include "boost/cstdlib.hpp"

boost::optional<double> f(const boost::optional<bool>& return_flag, int arg)
{
    if (!return_flag.empty() && return_flag.get() == true)
        return double(arg);

    return boost::optional<double>();
}

//////////////////////////////////////////////////////////////////////////
// function test_main
//
int test_main( int, char *[] )
{
    boost::optional<double> result;
    BOOST_TEST((
          result.empty()
        ));
    
    result = f(true, 3);
    BOOST_TEST((
          !result.empty() && result.get() == 3
        ));

    result = f(false, 3);
    BOOST_TEST((
          result.empty()
        ));

    boost::optional<bool> empty_flag;
    result = f(empty_flag, 3);
    BOOST_TEST((
          result.empty()
        ));

    return boost::exit_success;
}
