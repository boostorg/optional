//-----------------------------------------------------------------------------
// boost optional.hpp header file
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

#ifndef BOOST_OPTIONAL_HPP
#define BOOST_OPTIONAL_HPP

#include <new> // for placement-new

#include "boost/aligned_storage.hpp"
#include "boost/move.hpp"

namespace boost {

template <typename T>
class optional
{
    typedef aligned_storage<
           sizeof(T)
         , alignment_of<T>::value
         > storage_t;

    bool empty_;
    storage_t storage_;

public: // structors
    optional()
        : empty_(true)
    {
    }

    optional(const optional& operand)
        : empty_(operand.empty_)
    {
        if (!empty_)
            new(storage_.address()) T(operand.get());
    }

    optional(const T& operand)
        : empty_(false)
    {
        new(storage_.address()) T(operand);
    }

    ~optional()
    {
        clear();
    }

private:
    void assign_impl(
          const optional& operand
        , mpl::true_c// is_moveable
        )
    {
        // If operand is not empty...
        if (!operand.empty_)
        {
            // ...then attempt to copy operand's content...
            T temp(operand);

            // ...and commit copy to *this using nothrow operations:
            clear();
            move(storage_.address(), temp);
        }
        else
        {
            // ...otherwise, just clear *this:
            clear();
        }
    }

    void assign_impl(
          const optional& operand
        , mpl::false_c// is_moveable
		)
    {
        // Clear *this...
        clear();

        // ...and if operand is not empty...
        if (!operand.empty_)
        {
            // ...then attempt to copy operand's content to *this's storage...
            new(storage_.address()) T(operand.get());

            // ...and signal success:
            empty_ = false;
        }
    }

public: // modifiers
    optional& operator=(const optional& operand)
    {
        clear();

        if (!operand.empty_)
        {
            new(storage_.address()) T(operand.get());
            empty_ = false;
        }

        return *this;
    }

    optional& operator=(const T& operand)
    {
        clear();
        new(storage_.address()) T(operand.get());

        return *this;
    }

    optional& swap(optional& operand)
    {
        // Create a temporary, empty storage...
        optional temp;

        // ...and if operand is not empty...
        if (!operand.empty_)
        {
            // ...then move/copy operand's content into temp's (empty) storage...
            move_or_copy(temp.storage_.address(), operand.get());

            // ...flag temp as no longer empty...
            temp.empty_ = false;

            // ...and clear operand:
            operand.clear();
        }

        // Now if *this is not empty...
        if (!empty_)
        {
            // ...then move/copy *this's content into operand's (now-empty) storage...
            move_or_copy(operand.storage_.address(), get());

            // ...flag operand as no longer empty...
            operand.empty_ = false;

            // ...and clear *this:
            clear();
        }

        // Finally if temp is not empty...
        if (!temp.empty_)
        {
            // ...then move/copy temp's content into *this's (now-empty) storage:
            move_or_copy(storage_.address(), temp.get());

            // ...and flag *this as no longer empty:
            empty_ = false;
        }

        return *this;
    }

    void clear()
    {
        if (!empty_)
        {
            get().~T();
            empty_ = true;
        }
    }

public: // queries
    bool empty() const
    {
        return empty_;
    }

    T& get()
    {
        return *reinterpret_cast<T*>(storage_.address());
    }

    const T& get() const
    {
        return *reinterpret_cast<const T*>(storage_.address());
    }
};

template <typename T>
struct move_traits< optional<T> >
{
    static void move(void* dest, optional<T>& src)
    {
        BOOST_STATIC_ASSERT(is_moveable<T>::value);

        optional<T>* p = new(dest) optional<T>;

        if (!src.empty())
        {
            move(p->storage_.address(), src.get());
            p->empty_ = false;
        }
    }
};

} // namespace boost

/*

//////////////////////////////////////////////////////////////////////////
// Implementation below leverages boost::variant to stay simple. Use of  
// boost::variant, however, unnecessarily increases compile times, so the
// implementation above is used instead.
//

#include "boost/config.hpp"
#include "boost/variant.hpp"
#include "boost/extract.hpp"
#include "boost/move_fwd.hpp"

#include "boost/mpl/bool_c.hpp"
#include "boost/type_traits/is_POD.hpp"
#include "boost/type_traits/is_empty.hpp"

namespace boost {

namespace detail {
namespace optional {

struct empty_tag { };

} // namespace optional
} // namespace detail

template <>
struct is_POD< detail::optional::empty_tag >
{
    typedef mpl::bool_c<true> type;
	BOOST_STATIC_CONSTANT(bool, value = type::value);
};

template <>
struct is_empty< detail::optional::empty_tag >
{
    typedef mpl::bool_c<true> type;
	BOOST_STATIC_CONSTANT(bool, value = type::value);
};

template <typename T>
class optional
{
    typedef variant<
          detail::optional::empty_tag
        , T
        > value_type;

    value_type value_;

public: // structors
    optional()
        : value_()
    {
    }

    optional(const optional& operand)
        : value_(operand.value_)
    {
    }

    explicit optional(const T& operand)
        : value_(operand)
    {
    }

    ~optional()
    {
    }

public: // modifiers
    optional& operator=(const optional& operand)
    {
        value_ = operand.value_;
    }

    optional& operator=(const T& operand)
    {
        value_ = operand;
    }

    optional& swap(optional& operand)
    {
        value_.swap(operand.value_);
        return *this;
    }

    void clear()
    {
        value_ = detail::optional::empty_tag();
    }

public: // queries
    bool empty() const
    {
        return value_.which() == 0;
    }

    T& get()
    {
        return extract<T>(value_);
    }

    const T& get() const
    {
        return extract<T>(value_);
    }
};

template <typename T>
struct move_traits< optional<T> >
{
    static void move(void* dest, optional<T>& src)
    {
        BOOST_STATIC_ASSERT(is_moveable<T>::value);

        optional<T>* p = new(dest) optional<T>;
        p->swap(src);
    }
};

} // namespace boost
*/

#endif // BOOST_OPTIONAL_HPP
