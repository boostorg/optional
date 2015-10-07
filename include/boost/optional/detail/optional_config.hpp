// Copyright (C) 2003, 2008 Fernando Luis Cacciola Carballal.
// Copyright (C) 2015 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  akrzemi1@gmail.com

#ifndef BOOST_OPTIONAL_DETAIL_OPTIONAL_CONFIG_AJK_28JAN2015_HPP
#define BOOST_OPTIONAL_DETAIL_OPTIONAL_CONFIG_AJK_28JAN2015_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#if (defined BOOST_NO_CXX11_RVALUE_REFERENCES) || (defined BOOST_OPTIONAL_CONFIG_NO_RVALUE_REFERENCES)
# define BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
#endif

#if BOOST_WORKAROUND(BOOST_INTEL_CXX_VERSION,<=700)
// AFAICT only Intel 7 correctly resolves the overload set
// that includes the in-place factory taking functions,
// so for the other icc versions, in-place factory support
// is disabled
# define BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT
#endif

#if BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
// BCB (5.5.1) cannot parse the nested template struct in an inplace factory.
# define BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT
#endif

#if !defined(BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT) \
    && defined BOOST_BCB_PARTIAL_SPECIALIZATION_BUG
// BCB (up to 5.64) has the following bug:
//   If there is a member function/operator template of the form
//     template<class Expr> mfunc( Expr expr ) ;
//   some calls are resolved to this even if there are other better matches.
//   The effect of this bug is that calls to converting ctors and assignments
//   are incorrectly sink to this general catch-all member function template as shown above.
# define BOOST_OPTIONAL_WEAK_OVERLOAD_RESOLUTION
#endif

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
// GCC since 3.3 has may_alias attribute that helps to alleviate optimizer issues with
// regard to violation of the strict aliasing rules. The optional< T > storage type is marked
// with this attribute in order to let the compiler know that it will alias objects of type T
// and silence compilation warnings.
# define BOOST_OPTIONAL_DETAIL_USE_ATTRIBUTE_MAY_ALIAS
#endif

#endif // header guard