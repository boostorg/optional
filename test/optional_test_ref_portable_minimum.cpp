// Copyright (C) 2014 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at: akrzemi1@gmail.com


#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/core/addressof.hpp"
#include "boost/core/lightweight_test.hpp"
#include "boost/none.hpp"

using boost::optional;
using boost::none;

// testable classes
struct ScopeGuard // no copy/move ctor/assign
{
  int val;
  explicit ScopeGuard(int v) : val(v) {}
  
private:
  ScopeGuard(ScopeGuard const&);
  void operator=(ScopeGuard const&);
};

struct Abstract
{
  virtual int& val() = 0;
  virtual ~Abstract() {}
  Abstract(){}
private:
  Abstract(Abstract const&);
  void operator=(Abstract const&);
};

struct Impl : Abstract
{
  int val_;
  Impl(int v) : val_(v) {}
  int& val() { return val_; }
};

template <typename T>
struct concrete_type_of
{
  typedef T type;
};

template <>
struct concrete_type_of<Abstract>
{
  typedef Impl type;
};

int& val(int& i) { return i; }
int& val(Abstract& a) { return a.val(); }
int& val(ScopeGuard& g) { return g.val; }

template <typename T>
void test_not_containing_value_for()
{
  optional<T&> o1;
  optional<T&> o2 = none;
  optional<T&> o3 = o1;
  
  BOOST_TEST(!o1);
  BOOST_TEST(!o2);
  BOOST_TEST(!o3);
  
  BOOST_TEST(o1 == none);
  BOOST_TEST(o2 == none);
  BOOST_TEST(o3 == none);
}

template <typename T>
void test_direct_init_for()
{
  typename concrete_type_of<T>::type v(2);
  optional<T&> o(v);
  
  BOOST_TEST(o);
  BOOST_TEST(o != none);
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
   
  val(v) = 9;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
  BOOST_TEST(val(*o) == 9);
  
  val(*o) = 7;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
  BOOST_TEST(val(*o) == 7);
}

template <typename T>
void test_copy_assignment_for()
{
  typename concrete_type_of<T>::type v(2);
  optional<T&> o;
  o = optional<T&>(v);
  
  BOOST_TEST(o);
  BOOST_TEST(o != none);
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
  BOOST_TEST(val(*o) == 2);
  
  val(v) = 9;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
  BOOST_TEST(val(*o) == 9);
  
  val(*o) = 7;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST(val(*o) == val(v));
  BOOST_TEST(val(*o) == 7);
}


template <typename T>
void test_optional_ref()
{
  test_not_containing_value_for<T>();
  test_direct_init_for<T>();
  test_copy_assignment_for<T>();
}

int main()
{
  test_optional_ref<int>();
  test_optional_ref<ScopeGuard>();
  test_optional_ref<Abstract>();
  
  return boost::report_errors();
}
