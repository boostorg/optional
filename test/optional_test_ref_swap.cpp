// Copyright (C) 2016 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the maintainer at: akrzemi1@gmail.com


#include "boost/optional/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <string>
#include "boost/core/addressof.hpp"
#include "boost/core/lightweight_test.hpp"

using boost::optional;

struct Iface
{
  virtual void fun() = 0;
  protected: ~Iface() {}
};

struct Impl : Iface
{
  void fun() {}
};

template <typename T>
void test_swap_empty_with_empty()
{
  boost::optional<T&> o1, o2;
  swap(o1, o2);
  
  BOOST_TEST(!o1);
  BOOST_TEST(!o2);
}

template <typename T, typename U>
void test_swap_value_with_empty(U v1)
{
  boost::optional<T&> o1 (v1), o2;
  swap(o1, o2);
  
  BOOST_TEST(!o1);
  BOOST_TEST(o2);
  BOOST_TEST(boost::addressof(*o2) == boost::addressof(v1));
}

template <typename T, typename U>
void test_swap_empty_with_value(U v2)
{
  boost::optional<T&> o1, o2(v2);
  swap(o1, o2);

  BOOST_TEST(o1);
  BOOST_TEST(boost::addressof(*o1) == boost::addressof(v2));
  BOOST_TEST(!o2);
}

template <typename T, typename U>
void test_swap_value_with_value(U v1, U v2)
{
  boost::optional<T&> o1(v1), o2(v2);
  swap(o1, o2);

  BOOST_TEST(o1);
  BOOST_TEST(o2);
  BOOST_TEST(boost::addressof(*o1) == boost::addressof(v2));
  BOOST_TEST(boost::addressof(*o2) == boost::addressof(v1));
}

int main()
{
  std::string s1 ("AAA"), s2 ("BB");
  int i1 = 1, i2 = 2;
  Impl p1, p2;
  
  test_swap_empty_with_empty<std::string>(      );
  test_swap_value_with_empty<std::string>(s1    );
  test_swap_empty_with_value<std::string>(    s2);
  test_swap_value_with_value<std::string>(s1, s2);

  test_swap_empty_with_empty<int>(      );
  test_swap_value_with_empty<int>(i1    );
  test_swap_empty_with_value<int>(    i2);
  test_swap_value_with_value<int>(i1, i2);
  
  test_swap_empty_with_empty<Iface>(      );
  test_swap_value_with_empty<Iface>(p1    );
  test_swap_empty_with_value<Iface>(    p2);
  test_swap_value_with_value<Iface>(p1, p2);
  
  test_swap_empty_with_empty<const Iface>(      );
  test_swap_value_with_empty<const Iface>(p1    );
  test_swap_empty_with_value<const Iface>(    p2);
  test_swap_value_with_value<const Iface>(p1, p2);
  
  return boost::report_errors();
}
