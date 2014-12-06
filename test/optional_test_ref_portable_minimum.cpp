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
#include "boost/core/enable_if.hpp"
#include "boost/core/lightweight_test.hpp"

using boost::optional;
using boost::none;

// testable classes
struct ScopeGuard // no copy/move ctor/assign
{
  int val_;
  explicit ScopeGuard(int v) : val_(v) {}
  int& val() { return val_; }
  const int& val() const { return val_; }
  
private:
  ScopeGuard(ScopeGuard const&);
  void operator=(ScopeGuard const&);
};

struct Abstract
{
  virtual int& val() = 0;
  virtual const int& val() const = 0;
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
  const int& val() const { return val_; }
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

template <typename T>
struct has_arrow
{
  static const bool value = true;
};

template <>
struct has_arrow<int>
{
  static const bool value = false;
};

int& val(int& i) { return i; }
int& val(Abstract& a) { return a.val(); }
int& val(ScopeGuard& g) { return g.val(); }

bool operator==(const Abstract& l, const Abstract& r) { return l.val() == r.val(); }
bool operator==(const ScopeGuard& l, const ScopeGuard& r) { return l.val() == r.val(); }

bool operator<(const Abstract& l, const Abstract& r) { return l.val() < r.val(); }
bool operator<(const ScopeGuard& l, const ScopeGuard& r) { return l.val() < r.val(); }
// end testable classes

template <typename T>
typename boost::enable_if< has_arrow<T> >::type
test_arrow()
{
  typename concrete_type_of<T>::type v(2);
  optional<T&> o(v);
  BOOST_TEST(o);
  BOOST_TEST_EQ(o->val(), 2);
  
  o->val() = 3;
  BOOST_TEST(o);
  BOOST_TEST_EQ(o->val(), 3);
  BOOST_TEST_EQ(v.val(), 3);
}

template <typename T>
typename boost::disable_if< has_arrow<T> >::type
test_arrow()
{
}

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
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 2);
   
  val(v) = 9;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 9);
  BOOST_TEST_EQ(val(v), 9);
  
  val(*o) = 7;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 7);
  BOOST_TEST_EQ(val(v), 7);
}

template <typename T>
void test_clearing_the_value()
{
  typename concrete_type_of<T>::type v(2);
  optional<T&> o1(v), o2(v);
  
  BOOST_TEST(o1);
  BOOST_TEST(o1 != none);
  BOOST_TEST(o2);
  BOOST_TEST(o2 != none);
  
  o1 = none;
  BOOST_TEST(!o1);
  BOOST_TEST(o1 == none);
  BOOST_TEST(o2);
  BOOST_TEST(o2 != none);
  BOOST_TEST_EQ(val(*o2), 2);
  BOOST_TEST(boost::addressof(*o2) == boost::addressof(v));
  BOOST_TEST_EQ(val(v), 2);
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
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 9);
  BOOST_TEST_EQ(val(v), 9);
  
  val(*o) = 7;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 7);
  BOOST_TEST_EQ(val(v), 7);
}

template <typename T>
void test_rebinding_assignment_semantics()
{
  typename concrete_type_of<T>::type v(2), w(7);
  optional<T&> o(v);
  
  BOOST_TEST(o);
  BOOST_TEST(boost::addressof(*o) == boost::addressof(v));
  BOOST_TEST_EQ(val(*o), val(v));
  BOOST_TEST_EQ(val(*o), 2);
  
  o = optional<T&>(w);
  BOOST_TEST_EQ(val(v), 2);
  
  BOOST_TEST(o);
  BOOST_TEST(boost::addressof(*o) != boost::addressof(v));
  BOOST_TEST_NE(val(*o), val(v));
  BOOST_TEST_NE(val(*o), 2);
  
  BOOST_TEST(boost::addressof(*o) == boost::addressof(w));
  BOOST_TEST_EQ(val(*o), val(w));
  BOOST_TEST_EQ(val(*o), 7);
  
  val(*o) = 8;
  BOOST_TEST(boost::addressof(*o) == boost::addressof(w));
  BOOST_TEST_EQ(val(*o), val(w));
  BOOST_TEST_EQ(val(*o), 8);
  BOOST_TEST_EQ(val(w), 8);
  BOOST_TEST_EQ(val(v), 2);
}

template <typename T>
void test_equality()
{
  typename concrete_type_of<T>::type v1(1), v2(2), v2_(2), v3(3);
  optional<T&> o1(v1), o2(v2), o2_(v2_), o3(v3), o3_(v3), oN, oN_;
  // o2 and o2_ point to different objects; o3 and o3_ point to the same object
  
  BOOST_TEST(oN  == oN);
  BOOST_TEST(oN  == oN_);
  BOOST_TEST(oN_ == oN);
  BOOST_TEST(o1  == o1);
  BOOST_TEST(o2  == o2);
  BOOST_TEST(o2  == o2_);
  BOOST_TEST(o2_ == o2);
  BOOST_TEST(o3  == o3);
  BOOST_TEST(o3  == o3_);
  BOOST_TEST(!(oN == o1));
  BOOST_TEST(!(o1 == oN));
  BOOST_TEST(!(o2 == o1));
  BOOST_TEST(!(o2 == oN));
  
  BOOST_TEST(!(oN  != oN));
  BOOST_TEST(!(oN  != oN_));
  BOOST_TEST(!(oN_ != oN));
  BOOST_TEST(!(o1  != o1));
  BOOST_TEST(!(o2  != o2));
  BOOST_TEST(!(o2  != o2_));
  BOOST_TEST(!(o2_ != o2));
  BOOST_TEST(!(o3  != o3));
  BOOST_TEST(!(o3  != o3_));
  BOOST_TEST( (oN  != o1));
  BOOST_TEST( (o1  != oN));
  BOOST_TEST( (o2  != o1));
  BOOST_TEST( (o2  != oN));
}

template <typename T>
void test_order()
{
  typename concrete_type_of<T>::type v1(1), v2(2), v2_(2), v3(3);
  optional<T&> o1(v1), o2(v2), o2_(v2_), o3(v3), o3_(v3), oN, oN_;
  // o2 and o2_ point to different objects; o3 and o3_ point to the same object
  
  BOOST_TEST(!(oN  < oN));
  BOOST_TEST(!(oN  < oN_));
  BOOST_TEST(!(oN_ < oN));
  BOOST_TEST(!(o1  < o1));
  BOOST_TEST(!(o2  < o2));
  BOOST_TEST(!(o2  < o2_));
  BOOST_TEST(!(o2_ < o2));
  BOOST_TEST(!(o3  < o3));
  BOOST_TEST(!(o3  < o3_));
  
  BOOST_TEST( (oN  <= oN));
  BOOST_TEST( (oN  <= oN_));
  BOOST_TEST( (oN_ <= oN));
  BOOST_TEST( (o1  <= o1));
  BOOST_TEST( (o2  <= o2));
  BOOST_TEST( (o2  <= o2_));
  BOOST_TEST( (o2_ <= o2));
  BOOST_TEST( (o3  <= o3));
  BOOST_TEST( (o3  <= o3_));
  
  BOOST_TEST(!(oN  > oN));
  BOOST_TEST(!(oN  > oN_));
  BOOST_TEST(!(oN_ > oN));
  BOOST_TEST(!(o1  > o1));
  BOOST_TEST(!(o2  > o2));
  BOOST_TEST(!(o2  > o2_));
  BOOST_TEST(!(o2_ > o2));
  BOOST_TEST(!(o3  > o3));
  BOOST_TEST(!(o3  > o3_));
  
  BOOST_TEST( (oN  >= oN));
  BOOST_TEST( (oN  >= oN_));
  BOOST_TEST( (oN_ >= oN));
  BOOST_TEST( (o1  >= o1));
  BOOST_TEST( (o2  >= o2));
  BOOST_TEST( (o2  >= o2_));
  BOOST_TEST( (o2_ >= o2));
  BOOST_TEST( (o3  >= o3));
  BOOST_TEST( (o3  >= o3_));
  
  BOOST_TEST( (oN  < o1));
  BOOST_TEST( (oN_ < o1));
  BOOST_TEST( (oN  < o2));
  BOOST_TEST( (oN_ < o2));
  BOOST_TEST( (oN  < o2_));
  BOOST_TEST( (oN_ < o2_));
  BOOST_TEST( (oN  < o3));
  BOOST_TEST( (oN_ < o3));
  BOOST_TEST( (oN  < o3_));
  BOOST_TEST( (oN_ < o3_));
  BOOST_TEST( (o1  < o2));
  BOOST_TEST( (o1  < o2_));
  BOOST_TEST( (o1  < o3));
  BOOST_TEST( (o1  < o3_));
  BOOST_TEST( (o2  < o3));
  BOOST_TEST( (o2_ < o3));
  BOOST_TEST( (o2  < o3_));
  BOOST_TEST( (o2_ < o3_));
  
  BOOST_TEST( (oN  <= o1));
  BOOST_TEST( (oN_ <= o1));
  BOOST_TEST( (oN  <= o2));
  BOOST_TEST( (oN_ <= o2));
  BOOST_TEST( (oN  <= o2_));
  BOOST_TEST( (oN_ <= o2_));
  BOOST_TEST( (oN  <= o3));
  BOOST_TEST( (oN_ <= o3));
  BOOST_TEST( (oN  <= o3_));
  BOOST_TEST( (oN_ <= o3_));
  BOOST_TEST( (o1  <= o2));
  BOOST_TEST( (o1  <= o2_));
  BOOST_TEST( (o1  <= o3));
  BOOST_TEST( (o1  <= o3_));
  BOOST_TEST( (o2  <= o3));
  BOOST_TEST( (o2_ <= o3));
  BOOST_TEST( (o2  <= o3_));
  BOOST_TEST( (o2_ <= o3_));
  
  BOOST_TEST(!(oN  > o1));
  BOOST_TEST(!(oN_ > o1));
  BOOST_TEST(!(oN  > o2));
  BOOST_TEST(!(oN_ > o2));
  BOOST_TEST(!(oN  > o2_));
  BOOST_TEST(!(oN_ > o2_));
  BOOST_TEST(!(oN  > o3));
  BOOST_TEST(!(oN_ > o3));
  BOOST_TEST(!(oN  > o3_));
  BOOST_TEST(!(oN_ > o3_));
  BOOST_TEST(!(o1  > o2));
  BOOST_TEST(!(o1  > o2_));
  BOOST_TEST(!(o1  > o3));
  BOOST_TEST(!(o1  > o3_));
  BOOST_TEST(!(o2  > o3));
  BOOST_TEST(!(o2_ > o3));
  BOOST_TEST(!(o2  > o3_));
  BOOST_TEST(!(o2_ > o3_));
  
  BOOST_TEST(!(oN  >= o1));
  BOOST_TEST(!(oN_ >= o1));
  BOOST_TEST(!(oN  >= o2));
  BOOST_TEST(!(oN_ >= o2));
  BOOST_TEST(!(oN  >= o2_));
  BOOST_TEST(!(oN_ >= o2_));
  BOOST_TEST(!(oN  >= o3));
  BOOST_TEST(!(oN_ >= o3));
  BOOST_TEST(!(oN  >= o3_));
  BOOST_TEST(!(oN_ >= o3_));
  BOOST_TEST(!(o1  >= o2));
  BOOST_TEST(!(o1  >= o2_));
  BOOST_TEST(!(o1  >= o3));
  BOOST_TEST(!(o1  >= o3_));
  BOOST_TEST(!(o2  >= o3));
  BOOST_TEST(!(o2_ >= o3));
  BOOST_TEST(!(o2  >= o3_));
  BOOST_TEST(!(o2_ >= o3_));
  
  BOOST_TEST(!(o1  < oN));
  BOOST_TEST(!(o1  < oN_));
  BOOST_TEST(!(o2  < oN));
  BOOST_TEST(!(o2  < oN_));
  BOOST_TEST(!(o2_ < oN));
  BOOST_TEST(!(o2_ < oN_));
  BOOST_TEST(!(o3  < oN));
  BOOST_TEST(!(o3  < oN_));
  BOOST_TEST(!(o3_ < oN));
  BOOST_TEST(!(o3_ < oN_));
  BOOST_TEST(!(o2  < oN));
  BOOST_TEST(!(o2_ < oN_));
  BOOST_TEST(!(o3  < oN));
  BOOST_TEST(!(o3_ < oN_));
  BOOST_TEST(!(o3  < oN));
  BOOST_TEST(!(o3  < oN_));
  BOOST_TEST(!(o3_ < oN));
  BOOST_TEST(!(o3_ < oN_));
}

template <typename T>
void test_swap()
{
  typename concrete_type_of<T>::type v1(1), v2(2);
  optional<T&> o1(v1), o1_(v1), o2(v2), o2_(v2), oN, oN_;
  
  // swap(o1, o1); DOESN'T WORK
}

template <typename T>
void test_optional_ref()
{
  test_not_containing_value_for<T>();
  test_direct_init_for<T>();
  test_copy_assignment_for<T>();
  test_rebinding_assignment_semantics<T>();
  test_clearing_the_value<T>();
  test_arrow<T>();
  test_equality<T>();
  test_order<T>();
  test_swap<T>();
}

int main()
{
  test_optional_ref<int>();
  test_optional_ref<ScopeGuard>();
  //test_optional_ref<Abstract>();
  
  return boost::report_errors();
}
