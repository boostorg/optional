// Copyright (C) 2014, Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  akrzemi1@gmail.com
//
#ifndef BOOST_BAD_OPTIONAL_ACCESS_22MAY2014_HPP
#define BOOST_BAD_OPTIONAL_ACCESS_22MAY2014_HPP

#include <stdexcept>

namespace boost {

class bad_optional_access : public std::logic_error
{
public:
  explicit bad_optional_access(const std::string& what_arg) : std::logic_error(what_arg) {}
  explicit bad_optional_access(const char* what_arg) : std::logic_error(what_arg) {}
};

} // namespace boost

#endif
