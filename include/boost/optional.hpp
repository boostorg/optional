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
    : public moveable< optional<T> >
{
    typedef aligned_storage<
           sizeof(T)
         , alignment_of<T>::value
         > storage_t;

    bool empty_;
    storage_t storage_;

public: // structors
    ~optional()
    {
        clear();
    }

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

    optional(move_source<optional> source)
        : empty_(source.get().empty_)
    {
        if (!empty_)
        {
            optional& operand = source.get();
            new(storage.address()) T( move(operand.get()) );
        }
    }

    optional(const T& operand)
        : empty_(false)
    {
        new(storage_.address()) T(operand);
    }

    optional(move_source<T> source)
        : empty_(false)
    {
        T& operand = source.get();
        new(storage_.address()) T( move(operand) );
    }

public: // modifiers
    optional& operator=(const optional& rhs)
    {
        // If rhs is empty...
        if (rhs.empty_)
        {
            // ...then simply clear *this and leave:
            clear();
            return *this;
        }

        // Otherwise, assign rhs's content to *this:
        return (*this = rhs.get());
    }

    optional& operator=(move_source<optional> source)
    {
        // If rhs is empty...
        if (rhs.empty_)
        {
            // ...then simply clear *this and leave:
            clear();
            return *this;
        }

        optional& rhs = source.get();

        // Otherwise, move rhs's content to *this:
        return (*this = move(rhs.get()));
    }

    optional& operator=(const T& rhs)
    {
        // If *this is empty...
        if (empty_)
        {
            // ...then copy rhs to *this's storage:
            new(storage_.address()) T(rhs);
            empty_ = false;
        }
        else
        {
            // ...otherwise, assign rhs to *this's content:
            get() = rhs;
        }

        return *this;
    }

    optional& operator=(move_source<T> source)
    {
        T& rhs = source.get();

        // If *this is empty...
        if (empty_)
        {
            // ...then move-construct rhs to *this's storage:
            new(storage_.address()) T( move(rhs) );
            empty_ = false;
        }
        else
        {
            // ...otherwise, move-assign rhs to *this's content:
            get() = move(rhs);
        }

        return *this;
    }

    void swap(optional& operand)
    {
        // Move *this into temporary storage...
        optional temp( move(get()) );

        // ...move operand into *this...
        *this = move(operand);

        // ...and move temporary into operand:
        operand = move(temp);
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
        return *static_cast<T*>(storage_.address());
    }

    const T& get() const
    {
        return *static_cast<const T*>(storage_.address());
    }
};

} // namespace boost

#endif // BOOST_OPTIONAL_HPP
