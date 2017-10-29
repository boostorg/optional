// Copyright 2017 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0.
//
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>

#if defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)

int main()
{
}

#else

#include <boost/optional.hpp>

template<class T1, class T2> struct pair
{
    T1 first;
    T2 second;

    pair(): first(), second()
    {
    }
};

struct A
{
    A() = delete;
};

int main()
{
    boost::optional< pair<A, int> > opt, opt2;
    opt = opt2;
}

#endif
