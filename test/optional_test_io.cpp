// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#include<stdexcept>
#include<string>
#include<sstream>

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/optional/optional.hpp"
#include "boost/optional/optional_io.hpp"
#include "boost/none.hpp"

#include "boost/test/minimal.hpp"

#ifdef ENABLE_TRACE
#define TRACE(msg) std::cout << msg << std::endl ;
#else
#define TRACE(msg)
#endif

namespace boost {

void assertion_failed (char const * expr, char const * func, char const * file, long )
{
  using std::string ;
  string msg =  string("Boost assertion failure for \"")
               + string(expr)
               + string("\" at file \"")
               + string(file)
               + string("\" function \"")
               + string(func)
               + string("\"") ;

  TRACE(msg);

  throw std::logic_error(msg);
}

}

using namespace std ;
using namespace boost ;

template<class Opt>
void test2( Opt o, Opt buff )
{
  stringstream s ;

  const int markv = 123 ;
  int mark = 0 ;
  
  s << o << " " << markv ;
  s >> buff >> mark ;

  BOOST_CHECK( buff == o ) ;
  BOOST_CHECK( mark == markv ) ;
}


template<class T>
void test( T v, T w )
{
  test2( make_optional(v), optional<T>  ());
  test2( make_optional(v), make_optional(w));
  test2( optional<T>  () , optional<T>  ());
  test2( optional<T>  () , make_optional(w));
}


template <class T>
void subtest_tag_none_reversibility_with_optional(optional<T> ov)
{
  stringstream s;
  s << boost::none;
  s >> ov;
  BOOST_CHECK(!ov);
}

template <class T>
void subtest_tag_none_equivalence_with_optional()
{
  stringstream s, r;
  optional<T> ov;
  s << boost::none;
  r << ov;
  BOOST_CHECK(s.str() == r.str());
}

template <class T>
void test_tag_none(T v)
{
  subtest_tag_none_reversibility_with_optional(optional<T>(v));
  subtest_tag_none_reversibility_with_optional(optional<T>());
  subtest_tag_none_equivalence_with_optional<T>();
}


int test_main( int, char* [] )
{
  try
  {
    test(1,2);
    test(string("hello"),string("buffer"));
    test_tag_none(10);
    test_tag_none(string("text"));
  }
  catch ( ... )
  {
    BOOST_ERROR("Unexpected Exception caught!");
  }

  return 0;
}

