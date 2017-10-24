// Copyright (C) 2003, 2008 Fernando Luis Cacciola Carballal.
// Copyright (C) 2014 - 2016 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
// Revisions:
// 27 Apr 2008 (improved swap) Fernando Cacciola, Niels Dekker, Thorsten Ottosen
// 05 May 2014 (Added move semantics) Andrzej Krzemienski
//
#ifndef BOOST_OPTIONAL_OPTIONAL_FLC_19NOV2002_HPP
#define BOOST_OPTIONAL_OPTIONAL_FLC_19NOV2002_HPP

#include <new>
#include <iosfwd>

#include <boost/assert.hpp>
#include <boost/core/addressof.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/core/explicit_operator_bool.hpp>
#include <boost/core/swap.hpp>
#include <boost/optional/bad_optional_access.hpp>
#include <boost/static_assert.hpp>
#include <boost/throw_exception.hpp>
#include <boost/type.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/has_nothrow_constructor.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/has_trivial_move_constructor.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_constructible.hpp>
#include <boost/type_traits/is_lvalue_reference.hpp>
#include <boost/type_traits/is_nothrow_move_assignable.hpp>
#include <boost/type_traits/is_nothrow_move_constructible.hpp>
#include <boost/type_traits/is_rvalue_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/move/utility.hpp>
#include <boost/none.hpp>
#include <boost/utility/compare_pointees.hpp>

#include <boost/optional/optional_fwd.hpp>
#include <boost/optional/detail/optional_config.hpp>
#include <boost/optional/detail/optional_factory_support.hpp>
#include <boost/optional/detail/optional_aligned_storage.hpp>

#ifdef BOOST_OPTIONAL_CONFIG_USE_OLD_IMPLEMENTATION_OF_OPTIONAL
#include <boost/optional/detail/old_optional_implementation.hpp>
#else
namespace boost {
  
namespace optional_ns {
  
// a tag for in-place initialization of contained value
struct in_place_init_t
{
  struct init_tag{};
  explicit in_place_init_t(init_tag){}
};            
const in_place_init_t in_place_init ((in_place_init_t::init_tag()));

// a tag for conditional in-place initialization of contained value
struct in_place_init_if_t
{
  struct init_tag{};
  explicit in_place_init_if_t(init_tag){}
};
const in_place_init_if_t in_place_init_if ((in_place_init_if_t::init_tag()));
  
} // namespace optional_ns

using optional_ns::in_place_init_t;
using optional_ns::in_place_init;
using optional_ns::in_place_init_if_t;
using optional_ns::in_place_init_if;

namespace optional_detail {

struct optional_tag {} ;

template<class T, bool trivially_destructible>
class optional_destroyer ;

template<class T>
class optional_base ;

template<class T>
class optional_storage : optional_tag
{
    friend class optional_destroyer<T, false> ;
    friend class optional_destroyer<T, true> ;
    friend class optional_base<T> ;

  private :
    typedef aligned_storage<T> storage_type ;
    typedef optional_base<T> this_type ;

  protected :
    typedef T value_type ;

    typedef T &       reference_type ;
    typedef T const&  reference_const_type ;
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    typedef T &&  rval_reference_type ;
    typedef T &&  reference_type_of_temporary_wrapper ;
#endif
    typedef T *         pointer_type ;
    typedef T const*    pointer_const_type ;
    typedef T const&    argument_type ;

  public:
    // Returns a pointer to the value if this is initialized, otherwise,
    // returns NULL.
    // No-throw
    pointer_const_type get_ptr() const { return m_initialized ? get_ptr_impl() : 0 ; }
    pointer_type       get_ptr()       { return m_initialized ? get_ptr_impl() : 0 ; }

    bool is_initialized() const { return m_initialized ; }

  protected :
    optional_storage()
      :
      m_initialized( false ) {}

    void construct ( argument_type val )
     {
       ::new (m_storage.address()) value_type(val) ;
       m_initialized = true ;
     }
     
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    void construct ( rval_reference_type val )
     {
       ::new (m_storage.address()) value_type( boost::move(val) ) ;
       m_initialized = true ;
     }
#endif

#if (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) && (!defined BOOST_NO_CXX11_VARIADIC_TEMPLATES)
    // Constructs in-place
    // upon exception *this is always uninitialized
    template<class... Args>
    void construct ( in_place_init_t, Args&&... args )
    {
      ::new (m_storage.address()) value_type( boost::forward<Args>(args)... ) ;
      m_initialized = true ;
    }
#elif (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES)
    template<class Arg>
    void construct ( in_place_init_t, Arg&& arg )
     {
       ::new (m_storage.address()) value_type( boost::forward<Arg>(arg) );
       m_initialized = true ;
     }
     
    void construct ( in_place_init_t )
     {
       ::new (m_storage.address()) value_type();
       m_initialized = true ;
     }
#else
     
    template<class Arg>
    void construct ( in_place_init_t, const Arg& arg )
     {
       ::new (m_storage.address()) value_type( arg );
       m_initialized = true ;
     }
     
    template<class Arg>
    void construct ( in_place_init_t, Arg& arg )
     {
       ::new (m_storage.address()) value_type( arg );
       m_initialized = true ;
     }
     
    void construct ( in_place_init_t )
     {
       ::new (m_storage.address()) value_type();
       m_initialized = true ;
     }
#endif

#ifndef BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Constructs in-place using the given factory
    template<class Expr>
    void construct ( Expr&& factory, in_place_factory_base const* )
     {
       boost_optional_detail::construct<value_type>(factory, m_storage.address());
       m_initialized = true ;
     }

    // Constructs in-place using the given typed factory
    template<class Expr>
    void construct ( Expr&& factory, typed_in_place_factory_base const* )
     {
       factory.apply(m_storage.address()) ;
       m_initialized = true ;
     }
#else
    // Constructs in-place using the given factory
    template<class Expr>
    void construct ( Expr const& factory, in_place_factory_base const* )
     {
       boost_optional_detail::construct<value_type>(factory, m_storage.address());
       m_initialized = true ;
     }

    // Constructs in-place using the given typed factory
    template<class Expr>
    void construct ( Expr const& factory, typed_in_place_factory_base const* )
     {
       factory.apply(m_storage.address()) ;
       m_initialized = true ;
     }
#endif

#endif

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Constructs using any expression implicitly convertible to the single argument
    // of a one-argument T constructor.
    // Converting constructions of optional<T> from optional<U> uses this function with
    // 'Expr' being of type 'U' and relying on a converting constructor of T from U.
    template<class Expr>
    void construct ( Expr&& expr, void const* )
    {
      new (m_storage.address()) value_type(boost::forward<Expr>(expr)) ;
      m_initialized = true ;
    }
#else
    // Constructs using any expression implicitly convertible to the single argument
    // of a one-argument T constructor.
    // Converting constructions of optional<T> from optional<U> uses this function with
    // 'Expr' being of type 'U' and relying on a converting constructor of T from U.
    template<class Expr>
    void construct ( Expr const& expr, void const* )
     {
       new (m_storage.address()) value_type(expr) ;
       m_initialized = true ;
     }
#endif

#ifdef BOOST_OPTIONAL_WEAK_OVERLOAD_RESOLUTION
    // BCB5.64 (and probably lower versions) workaround.
    //   The in-place factories are supported by means of catch-all constructors
    //   and assignment operators (the functions are parameterized in terms of
    //   an arbitrary 'Expr' type)
    //   This compiler incorrectly resolves the overload set and sinks optional<T> and optional<U>
    //   to the 'Expr'-taking functions even though explicit overloads are present for them.
    //   Thus, the following overload is needed to properly handle the case when the 'lhs'
    //   is another optional.
    //
    // For VC<=70 compilers this workaround dosen't work becasue the comnpiler issues and error
    // instead of choosing the wrong overload
    //
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Notice that 'Expr' will be optional<T> or optional<U> (but not optional_base<..>)
    template<class Expr>
    void construct ( Expr&& expr, optional_tag const* )
     {
       if ( expr.is_initialized() )
       {
         // An exception can be thrown here.
         // It it happens, THIS will be left uninitialized.
         new (m_storage.address()) value_type(boost::move(expr.get())) ;
         m_initialized = true ;
       }
     }
#else
    // Notice that 'Expr' will be optional<T> or optional<U> (but not optional_base<..>)
    template<class Expr>
    void construct ( Expr const& expr, optional_tag const* )
     {
       if ( expr.is_initialized() )
       {
         // An exception can be thrown here.
         // It it happens, THIS will be left uninitialized.
         new (m_storage.address()) value_type(expr.get()) ;
         m_initialized = true ;
       }
     }
#endif
#endif // defined BOOST_OPTIONAL_WEAK_OVERLOAD_RESOLUTION

    void assign_value ( argument_type val ) { get_impl() = val; }
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    void assign_value ( rval_reference_type val ) { get_impl() = static_cast<rval_reference_type>(val); }
#endif

    reference_const_type get_impl() const { return m_storage.ref() ; }
    reference_type       get_impl()       { return m_storage.ref() ; }

    pointer_const_type get_ptr_impl() const { return m_storage.ptr_ref(); }
    pointer_type       get_ptr_impl()       { return m_storage.ptr_ref(); }

  private :
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1600)
    void destroy_impl ( ) { m_storage.ptr_ref()->~T() ; m_initialized = false ; }
#else
    void destroy_impl ( ) { m_storage.ref().T::~T() ; m_initialized = false ; }
#endif

    bool m_initialized ;
    storage_type m_storage ;
};

template<class T, bool trivially_copyable = boost::has_trivial_copy<T>::value>
class optional_copier : public optional_storage<T>
{
    typedef optional_storage<T> storage_base ;

  protected :
    optional_copier() {}
    optional_copier ( optional_copier const& rhs )
    {
      if ( rhs.is_initialized() )
        storage_base::construct(rhs.get_impl());
    }

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_copier ( optional_copier && ) = default;
#else
    optional_copier ( optional_copier && rhs ) : storage_base( boost::move(rhs) ) {}
#endif
#endif
};

#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
template<class T>
class optional_copier<T, true> : public optional_storage<T>
{
  protected :
    optional_copier() {}
    optional_copier ( optional_copier const& ) = default;
    optional_copier ( optional_copier && ) = default;
};
#endif

template<class T, bool trivially_movable = boost::has_trivial_move_constructor<T>::value>
class optional_mover : public optional_copier<T>
{
    typedef optional_storage<T> storage_base ;

  protected :
    optional_mover() {}
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_mover ( optional_mover const& ) = default;
#else
    optional_mover ( optional_mover const& rhs ) : base( rhs ) {}
#endif

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Creates a deep move of another optional<T>
    // Can throw if T::T(T&&) does
    optional_mover ( optional_mover&& rhs )
      BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value)
    {
      if ( rhs.is_initialized() )
        storage_base::construct( boost::move(rhs.get_impl()) );
    }
#endif
};

#if (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) && (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
template<class T>
class optional_mover<T, true> : public optional_copier<T>
{
  protected :
    optional_mover() {}
    optional_mover ( optional_mover const& ) = default;
    optional_mover ( optional_mover && ) = default;
};
#endif

template<class T, bool trivially_destructible = boost::has_trivial_destructor<T>::value>
class optional_destroyer : public optional_mover<T>
{
    typedef optional_mover<T> base ;
    typedef optional_storage<T> storage_base ;

  protected :
    optional_destroyer() {}
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_destroyer ( optional_destroyer const& ) = default;
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    optional_destroyer ( optional_destroyer && ) = default;
#endif
#else
    optional_destroyer ( optional_destroyer const& rhs ) : base( rhs ) {}
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    optional_destroyer ( optional_destroyer && ) : base( boost::move(rhs) ) {}
#endif
#endif

    // No-throw (assuming T::~T() doesn't)
    ~optional_destroyer() { destroy() ; }

    void destroy ( )
    {
      if ( storage_base::m_initialized )
        storage_base::destroy_impl() ;
    }
};

template<class T>
class optional_destroyer<T, true> : public optional_mover<T>
{
    typedef optional_storage<T> storage_base ;

  protected :
    optional_destroyer() {}
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_destroyer ( optional_destroyer const& ) = default;
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    optional_destroyer ( optional_destroyer && ) = default;
#endif
#else
    optional_destroyer ( optional_destroyer const& rhs ) : base( rhs ) {}
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    optional_destroyer ( optional_destroyer && ) : base( boost::move(rhs) ) {}
#endif
#endif

    void destroy ( ) { storage_base::m_initialized = false; }
};

template<class T>
class optional_base : public optional_destroyer<T>
{
    typedef optional_destroyer<T> base ;
    typedef optional_storage<T> storage_base ;

  protected:
    typedef typename base::value_type value_type ;

    typedef T &       reference_type ;
    typedef T const&  reference_const_type ;
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    typedef T &&  rval_reference_type ;
    typedef T &&  reference_type_of_temporary_wrapper ;
#endif
    typedef T *         pointer_type ;
    typedef T const*    pointer_const_type ;
    typedef T const&    argument_type ;

    // Creates an optional<T> uninitialized.
    // No-throw
    optional_base() {}

    // Creates an optional<T> uninitialized.
    // No-throw
    optional_base ( none_t ) {}

    // Creates an optional<T> initialized with 'val'.
    // Can throw if T::T(T const&) does
    optional_base ( argument_type val )
    {
        storage_base::construct(val);
    }

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // move-construct an optional<T> initialized from an rvalue-ref to 'val'.
    // Can throw if T::T(T&&) does
    optional_base ( rval_reference_type val )
    {
        storage_base::construct( boost::move(val) );
    }
#endif

    // Creates an optional<T> initialized with 'val' IFF cond is true, otherwise creates an uninitialzed optional<T>.
    // Can throw if T::T(T const&) does
    optional_base ( bool cond, argument_type val )
    {
      if ( cond )
        storage_base::construct(val);
    }

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Creates an optional<T> initialized with 'move(val)' IFF cond is true, otherwise creates an uninitialzed optional<T>.
    // Can throw if T::T(T &&) does
    optional_base ( bool cond, rval_reference_type val )
    {
      if ( cond )
        storage_base::construct(boost::move(val));
    }
#endif

    // Creates a deep copy of another optional<T>
    // Can throw if T::T(T const&) does
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_base ( optional_base const& ) = default;
#else
    optional_base ( optional_base const& rhs )
      :
      base( rhs ) {}
#endif

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
#if (!defined BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && (!defined BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    optional_base ( optional_base && ) = default;
#else
    optional_base ( optional_base && rhs )
      :
      base( boost::move(rhs) ) {}
#endif
#endif

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES

    template<class Expr, class PtrExpr>
    explicit optional_base ( Expr&& expr, PtrExpr const* tag )
    {
        storage_base::construct(boost::forward<Expr>(expr),tag);
    }

#else
    // This is used for both converting and in-place constructions.
    // Derived classes use the 'tag' to select the appropriate
    // implementation (the correct 'construct()' overload)
    template<class Expr>
    explicit optional_base ( Expr const& expr, Expr const* tag )
    {
        storage_base::construct(expr,tag);
    }

#endif

    // Assigns from another optional<T> (deep-copies the rhs value)
    void assign ( optional_base const& rhs )
    {
      if (storage_base::is_initialized())
      {
        if ( rhs.is_initialized() )
             storage_base::assign_value(rhs.get_impl());
        else base::destroy();
      }
      else
      {
        if ( rhs.is_initialized() )
          storage_base::construct(rhs.get_impl());
      }
    }
    
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Assigns from another optional<T> (deep-moves the rhs value)
    void assign ( optional_base&& rhs )
    {
      if (storage_base::is_initialized())
      {
        if ( rhs.is_initialized() )
             storage_base::assign_value( boost::move(rhs.get_impl()) );
        else base::destroy();
      }
      else
      {
        if ( rhs.is_initialized() )
          storage_base::construct(boost::move(rhs.get_impl()));
      }
    }
#endif 

    // Assigns from another _convertible_ optional<U> (deep-copies the rhs value)
    template<class U>
    void assign ( optional<U> const& rhs )
    {
      if (storage_base::is_initialized())
      {
        if ( rhs.is_initialized() )
#ifndef BOOST_OPTIONAL_CONFIG_RESTORE_ASSIGNMENT_OF_NONCONVERTIBLE_TYPES
          storage_base::assign_value( rhs.get() );
#else
          storage_base::assign_value( static_cast<value_type>(rhs.get()) );
#endif
          
        else base::destroy();
      }
      else
      {
        if ( rhs.is_initialized() )
#ifndef BOOST_OPTIONAL_CONFIG_RESTORE_ASSIGNMENT_OF_NONCONVERTIBLE_TYPES
          storage_base::construct(rhs.get());
#else
        storage_base::construct(static_cast<value_type>(rhs.get()));
#endif
      }
    }

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // move-assigns from another _convertible_ optional<U> (deep-moves from the rhs value)
    template<class U>
    void assign ( optional<U>&& rhs )
    {
      typedef BOOST_DEDUCED_TYPENAME optional<U>::rval_reference_type ref_type;
      if (storage_base::is_initialized())
      {
        if ( rhs.is_initialized() )
             storage_base::assign_value( static_cast<ref_type>(rhs.get()) );
        else base::destroy();
      }
      else
      {
        if ( rhs.is_initialized() )
          storage_base::construct(static_cast<ref_type>(rhs.get()));
      }
    }
#endif
    
    // Assigns from a T (deep-copies the rhs value)
    void assign ( argument_type val )
    {
      if (storage_base::is_initialized())
           storage_base::assign_value(val);
      else storage_base::construct(val);
    }
    
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Assigns from a T (deep-moves the rhs value)
    void assign ( rval_reference_type val )
    {
      if (storage_base::is_initialized())
           storage_base::assign_value( boost::move(val) );
      else storage_base::construct( boost::move(val) );
    }
#endif

    // Assigns from "none", destroying the current value, if any, leaving this UNINITIALIZED
    // No-throw (assuming T::~T() doesn't)
    void assign ( none_t ) BOOST_NOEXCEPT { base::destroy(); }

#ifndef BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    template<class Expr, class ExprPtr>
    void assign_expr ( Expr&& expr, ExprPtr const* tag )
    {
      if (storage_base::is_initialized())
        assign_expr_to_initialized(boost::forward<Expr>(expr),tag);
      else storage_base::construct(boost::forward<Expr>(expr),tag);
    }
#else
    template<class Expr>
    void assign_expr ( Expr const& expr, Expr const* tag )
    {
      if (storage_base::is_initialized())
        assign_expr_to_initialized(expr,tag);
      else storage_base::construct(expr,tag);
    }
#endif

#endif

  public :

    // **DEPPRECATED** Destroys the current value, if any, leaving this UNINITIALIZED
    // No-throw (assuming T::~T() doesn't)
    void reset() BOOST_NOEXCEPT { base::destroy(); }

    // **DEPPRECATED** Replaces the current value -if any- with 'val'
    void reset ( argument_type val ) { assign(val); }

  protected:
#if (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) && (!defined BOOST_NO_CXX11_VARIADIC_TEMPLATES)

    template<class... Args>
    void emplace_assign ( Args&&... args )
    {
      base::destroy();
      storage_base::construct(in_place_init, boost::forward<Args>(args)...);
    }
     
    template<class... Args>
    explicit optional_base ( in_place_init_t, Args&&... args )
    {
        storage_base::construct(in_place_init, boost::forward<Args>(args)...);
    }
    
    template<class... Args>
    explicit optional_base ( in_place_init_if_t, bool cond, Args&&... args )
    {
      if ( cond )
        storage_base::construct(in_place_init, boost::forward<Args>(args)...);
    }
#elif (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES)
     
    template<class Arg>
    void emplace_assign ( Arg&& arg )
     {
       base::destroy();
       storage_base::construct(in_place_init, boost::forward<Arg>(arg)) ;
     }
     
    void emplace_assign ()
     {
       base::destroy();
       storage_base::construct(in_place_init) ;
     }
     
    template<class Arg>
    explicit optional_base ( in_place_init_t, Arg&& arg )
      :
      m_initialized(false)
    {
        storage_base::construct(in_place_init, boost::forward<Arg>(arg));
    }
    
    explicit optional_base ( in_place_init_t )
      :
      m_initialized(false)
    {
        storage_base::construct(in_place_init);
    }
    
    template<class Arg>
    explicit optional_base ( in_place_init_if_t, bool cond, Arg&& arg )
      :
      m_initialized(false)
    {
      if ( cond )
        storage_base::construct(in_place_init, boost::forward<Arg>(arg));
    }
    
    explicit optional_base ( in_place_init_if_t, bool cond )
      :
      m_initialized(false)
    {
      if ( cond )
        storage_base::construct(in_place_init);
    }
#else

    template<class Arg>
    void emplace_assign ( const Arg& arg )
    {
      base::destroy();
      storage_base::construct(in_place_init, arg);
    }
     
    template<class Arg>
    void emplace_assign ( Arg& arg )
    {
      base::destroy();
      storage_base::construct(in_place_init, arg);
    }
     
    void emplace_assign ()
    {
      base::destroy();
      storage_base::construct(in_place_init);
    }
    
    template<class Arg>
    explicit optional_base ( in_place_init_t, const Arg& arg )
    {
        storage_base::construct(in_place_init, arg);
    }

    template<class Arg>
    explicit optional_base ( in_place_init_t, Arg& arg )
    {
        storage_base::construct(in_place_init, arg);
    }
    
    explicit optional_base ( in_place_init_t )
    {
        storage_base::construct(in_place_init);
    }
    
    template<class Arg>
    explicit optional_base ( in_place_init_if_t, bool cond, const Arg& arg )
    {
      if ( cond )
        storage_base::construct(in_place_init, arg);
    }
    
    template<class Arg>
    explicit optional_base ( in_place_init_if_t, bool cond, Arg& arg )
    {
      if ( cond )
        storage_base::construct(in_place_init, arg);
    } 
    
    explicit optional_base ( in_place_init_if_t, bool cond )
    {
      if ( cond )
        storage_base::construct(in_place_init);
    }
#endif

#ifndef BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    template<class Expr>
    void assign_expr_to_initialized ( Expr&& factory, in_place_factory_base const* tag )
     {
       base::destroy();
       storage_base::construct(factory,tag);
     }

    // Constructs in-place using the given typed factory
    template<class Expr>
    void assign_expr_to_initialized ( Expr&& factory, typed_in_place_factory_base const* tag )
     {
       base::destroy();
       storage_base::construct(factory,tag);
     }
#else
    template<class Expr>
    void assign_expr_to_initialized ( Expr const& factory, in_place_factory_base const* tag )
     {
       base::destroy();
       storage_base::construct(factory,tag);
     }

    // Constructs in-place using the given typed factory
    template<class Expr>
    void assign_expr_to_initialized ( Expr const& factory, typed_in_place_factory_base const* tag )
     {
       base::destroy();
       storage_base::construct(factory,tag);
     }
#endif

#endif

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Assigns using a form any expression implicitly convertible to the single argument
    // of a T's assignment operator.
    // Converting assignments of optional<T> from optional<U> uses this function with
    // 'Expr' being of type 'U' and relying on a converting assignment of T from U.
    template<class Expr>
    void assign_expr_to_initialized ( Expr&& expr, void const* )
    {
      storage_base::assign_value( boost::forward<Expr>(expr) );
    }
#else
    // Assigns using a form any expression implicitly convertible to the single argument
    // of a T's assignment operator.
    // Converting assignments of optional<T> from optional<U> uses this function with
    // 'Expr' being of type 'U' and relying on a converting assignment of T from U.
    template<class Expr>
    void assign_expr_to_initialized ( Expr const& expr, void const* )
     {
       storage_base::assign_value(expr);
     }
#endif
} ;

// definition of metafunciton is_optional_val_init_candidate
template <typename U>
struct is_optional_related
  : boost::conditional< boost::is_base_of<optional_detail::optional_tag, BOOST_DEDUCED_TYPENAME boost::decay<U>::type>::value
                     || boost::is_same<BOOST_DEDUCED_TYPENAME boost::decay<U>::type, none_t>::value
                     || boost::is_same<BOOST_DEDUCED_TYPENAME boost::decay<U>::type, in_place_init_t>::value
                     || boost::is_same<BOOST_DEDUCED_TYPENAME boost::decay<U>::type, in_place_init_if_t>::value,
    boost::true_type, boost::false_type>::type
{};

#if !defined(BOOST_OPTIONAL_DETAIL_NO_IS_CONSTRUCTIBLE_TRAIT)
  
template <typename T, typename U>
struct is_convertible_to_T_or_factory
  : boost::conditional< boost::is_base_of<boost::in_place_factory_base, BOOST_DEDUCED_TYPENAME boost::decay<U>::type>::value
                     || boost::is_base_of<boost::typed_in_place_factory_base, BOOST_DEDUCED_TYPENAME boost::decay<U>::type>::value
                     || (boost::is_constructible<T, U&&>::value && !boost::is_same<T, BOOST_DEDUCED_TYPENAME boost::decay<U>::type>::value)
                      , boost::true_type, boost::false_type>::type
{};

template <typename T, typename U>
struct is_optional_constructible : boost::is_constructible<T, U>
{};

#else

template <typename, typename>
struct is_convertible_to_T_or_factory : boost::true_type
{};

template <typename T, typename U>
struct is_optional_constructible : boost::true_type
{};

#endif // is_convertible condition

template <typename T, typename U>
struct is_optional_val_init_candidate
  : boost::conditional< !is_optional_related<U>::value && is_convertible_to_T_or_factory<T, U>::value
                      , boost::true_type, boost::false_type>::type
{};
    
} // namespace optional_detail

template<class T>
class optional : public optional_detail::optional_base<T>
{
    typedef optional_detail::optional_base<T> base ;

  public :

    typedef optional<T> this_type ;

    typedef BOOST_DEDUCED_TYPENAME base::value_type           value_type ;
    typedef BOOST_DEDUCED_TYPENAME base::reference_type       reference_type ;
    typedef BOOST_DEDUCED_TYPENAME base::reference_const_type reference_const_type ;
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    typedef BOOST_DEDUCED_TYPENAME base::rval_reference_type  rval_reference_type ;
    typedef BOOST_DEDUCED_TYPENAME base::reference_type_of_temporary_wrapper reference_type_of_temporary_wrapper ;
#endif
    typedef BOOST_DEDUCED_TYPENAME base::pointer_type         pointer_type ;
    typedef BOOST_DEDUCED_TYPENAME base::pointer_const_type   pointer_const_type ;
    typedef BOOST_DEDUCED_TYPENAME base::argument_type        argument_type ;

    // Creates an optional<T> uninitialized.
    // No-throw
    optional() BOOST_NOEXCEPT : base() {}

    // Creates an optional<T> uninitialized.
    // No-throw
    optional( none_t none_ ) BOOST_NOEXCEPT : base(none_) {}

    // Creates an optional<T> initialized with 'val'.
    // Can throw if T::T(T const&) does
    optional ( argument_type val ) : base(val) {}

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Creates an optional<T> initialized with 'move(val)'.
    // Can throw if T::T(T &&) does
    optional ( rval_reference_type val ) : base( boost::forward<T>(val) ) 
      {}
#endif

    // Creates an optional<T> initialized with 'val' IFF cond is true, otherwise creates an uninitialized optional.
    // Can throw if T::T(T const&) does
    optional ( bool cond, argument_type val ) : base(cond,val) {}

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    /// Creates an optional<T> initialized with 'val' IFF cond is true, otherwise creates an uninitialized optional.
    // Can throw if T::T(T &&) does
    optional ( bool cond, rval_reference_type val ) : base( cond, boost::forward<T>(val) ) 
      {}
#endif

    // NOTE: MSVC needs templated versions first

    // Creates a deep copy of another convertible optional<U>
    // Requires a valid conversion from U to T.
    // Can throw if T::T(U const&) does
    template<class U>
    explicit optional ( optional<U> const& rhs
#ifndef BOOST_OPTIONAL_DETAIL_NO_SFINAE_FRIENDLY_CONSTRUCTORS
                        ,typename boost::enable_if< optional_detail::is_optional_constructible<T, U const&> >::type* = 0
#endif
                      )
      :
      base()
    {
      if ( rhs.is_initialized() )
        this->construct(rhs.get());
    }
    
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Creates a deep move of another convertible optional<U>
    // Requires a valid conversion from U to T.
    // Can throw if T::T(U&&) does
    template<class U>
    explicit optional ( optional<U> && rhs
#ifndef BOOST_OPTIONAL_DETAIL_NO_SFINAE_FRIENDLY_CONSTRUCTORS
                        ,typename boost::enable_if< optional_detail::is_optional_constructible<T, U> >::type* = 0
#endif
                      )
      :
      base()
    {
      if ( rhs.is_initialized() )
        this->construct( boost::move(rhs.get()) );
    }
#endif

#ifndef BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT
    // Creates an optional<T> with an expression which can be either
    //  (a) An instance of InPlaceFactory (i.e. in_place(a,b,...,n);
    //  (b) An instance of TypedInPlaceFactory ( i.e. in_place<T>(a,b,...,n);
    //  (c) Any expression implicitly convertible to the single type
    //      of a one-argument T's constructor.
    //  (d*) Weak compilers (BCB) might also resolved Expr as optional<T> and optional<U>
    //       even though explicit overloads are present for these.
    // Depending on the above some T ctor is called.
    // Can throw if the resolved T ctor throws.
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES


  template<class Expr>
  explicit optional ( Expr&& expr, 
                      BOOST_DEDUCED_TYPENAME boost::enable_if< optional_detail::is_optional_val_init_candidate<T, Expr> >::type* = 0 
  ) 
    : base(boost::forward<Expr>(expr),boost::addressof(expr)) 
    {}

#else
    template<class Expr>
    explicit optional ( Expr const& expr ) : base(expr,boost::addressof(expr)) {}
#endif // !defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
#endif // !defined BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT

    // Creates a deep copy of another optional<T>
    // Can throw if T::T(T const&) does
#ifndef BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
    optional ( optional const& rhs ) = default;
#else
    optional ( optional const& rhs ) : base( static_cast<base const&>(rhs) ) {}
#endif

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Creates a deep move of another optional<T>
    // Can throw if T::T(T&&) does
#ifndef BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
    optional ( optional && rhs )
      BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value)
    = default;
#else
    // Creates a deep move of another optional<T>
    // Can throw if T::T(T&&) does
    optional ( optional && rhs )
      BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value)
      : base( boost::move(rhs) )
    {}
#endif
#endif

#if BOOST_WORKAROUND(_MSC_VER, <= 1600)
    //  On old MSVC compilers the implicitly declared dtor is not called
    ~optional() {}
#endif


#if !defined(BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT) && !defined(BOOST_OPTIONAL_WEAK_OVERLOAD_RESOLUTION)
    // Assigns from an expression. See corresponding constructor.
    // Basic Guarantee: If the resolved T ctor throws, this is left UNINITIALIZED
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES

    template<class Expr>
    BOOST_DEDUCED_TYPENAME boost::enable_if<optional_detail::is_optional_val_init_candidate<T, Expr>, optional&>::type 
    operator= ( Expr&& expr )
      {
        this->assign_expr(boost::forward<Expr>(expr),boost::addressof(expr));
        return *this ;
      }

#else
    template<class Expr>
    optional& operator= ( Expr const& expr )
      {
        this->assign_expr(expr,boost::addressof(expr));
        return *this ;
      }
#endif // !defined  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
#endif // !defined(BOOST_OPTIONAL_NO_INPLACE_FACTORY_SUPPORT) && !defined(BOOST_OPTIONAL_WEAK_OVERLOAD_RESOLUTION)

    // Copy-assigns from another convertible optional<U> (converts && deep-copies the rhs value)
    // Requires a valid conversion from U to T.
    // Basic Guarantee: If T::T( U const& ) throws, this is left UNINITIALIZED
    template<class U>
    optional& operator= ( optional<U> const& rhs )
      {
        this->assign(rhs);
        return *this ;
      }
      
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Move-assigns from another convertible optional<U> (converts && deep-moves the rhs value)
    // Requires a valid conversion from U to T.
    // Basic Guarantee: If T::T( U && ) throws, this is left UNINITIALIZED
    template<class U>
    optional& operator= ( optional<U> && rhs )
      {
        this->assign(boost::move(rhs));
        return *this ;
      }
#endif

    // Assigns from another optional<T> (deep-copies the rhs value)
    // Basic Guarantee: If T::T( T const& ) throws, this is left UNINITIALIZED
    //  (NOTE: On BCB, this operator is not actually called and left is left UNMODIFIED in case of a throw)
    optional& operator= ( optional const& rhs )
      {
        this->assign( static_cast<base const&>(rhs) ) ;
        return *this ;
      }

#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Assigns from another optional<T> (deep-moves the rhs value)
    optional& operator= ( optional && rhs ) 
      BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value && ::boost::is_nothrow_move_assignable<T>::value)
      {
        this->assign( static_cast<base &&>(rhs) ) ;
        return *this ;
      }
#endif

#ifndef BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX

    // Assigns from a T (deep-moves/copies the rhs value)
    template <typename T_>
    BOOST_DEDUCED_TYPENAME boost::enable_if<boost::is_same<T, BOOST_DEDUCED_TYPENAME boost::decay<T_>::type>, optional&>::type
    operator= ( T_&& val )
      {
        this->assign( boost::forward<T_>(val) ) ;
        return *this ;
      }
      
#else

    // Assigns from a T (deep-copies the rhs value)
    // Basic Guarantee: If T::( T const& ) throws, this is left UNINITIALIZED
    optional& operator= ( argument_type val )
      {
        this->assign( val ) ;
        return *this ;
      }
      
#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    // Assigns from a T (deep-moves the rhs value)
    optional& operator= ( rval_reference_type val )
      {
        this->assign( boost::move(val) ) ;
        return *this ;
      }
#endif
      
#endif // BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
      
    // Assigns from a "none"
    // Which destroys the current value, if any, leaving this UNINITIALIZED
    // No-throw (assuming T::~T() doesn't)
    optional& operator= ( none_t none_ ) BOOST_NOEXCEPT
      {
        this->assign( none_ ) ;
        return *this ;
      }
      
#if (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) && (!defined BOOST_NO_CXX11_VARIADIC_TEMPLATES)
    // Constructs in-place
    // upon exception *this is always uninitialized
    template<class... Args>
    void emplace ( Args&&... args )
    {
      this->emplace_assign( boost::forward<Args>(args)... );
    }
     
    template<class... Args>
    explicit optional ( in_place_init_t, Args&&... args )
    : base( in_place_init, boost::forward<Args>(args)... )
    {}
    
    template<class... Args>
    explicit optional ( in_place_init_if_t, bool cond, Args&&... args )
    : base( in_place_init_if, cond, boost::forward<Args>(args)... )
    {}
    
#elif (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES)
    template<class Arg>
    void emplace ( Arg&& arg )
     {
       this->emplace_assign( boost::forward<Arg>(arg) );
     }
     
    void emplace ()
     {
       this->emplace_assign();
     }
     
    template<class Args>
    explicit optional ( in_place_init_t, Args&& args )
    : base( in_place_init, boost::forward<Args>(args) )
    {}

    explicit optional ( in_place_init_t )
    : base( in_place_init )
    {}
    
    template<class Args>
    explicit optional ( in_place_init_if_t, bool cond, Args&& args )
    : base( in_place_init_if, cond, boost::forward<Args>(args) )
    {}
    
    explicit optional ( in_place_init_if_t, bool cond )
    : base( in_place_init_if, cond )
    {}
#else
    template<class Arg>
    void emplace ( const Arg& arg )
     {
       this->emplace_assign( arg );
     }
     
    template<class Arg>
    void emplace ( Arg& arg )
     {
       this->emplace_assign( arg );
     }
     
    void emplace ()
     {
       this->emplace_assign();
     }
     
    template<class Arg>
    explicit optional ( in_place_init_t, const Arg& arg )
    : base( in_place_init, arg )
    {}
    
    template<class Arg>
    explicit optional ( in_place_init_t, Arg& arg )
    : base( in_place_init, arg )
    {}

    explicit optional ( in_place_init_t )
    : base( in_place_init )
    {}
    
    template<class Arg>
    explicit optional ( in_place_init_if_t, bool cond, const Arg& arg )
    : base( in_place_init_if, cond, arg )
    {}
    
    template<class Arg>
    explicit optional ( in_place_init_if_t, bool cond, Arg& arg )
    : base( in_place_init_if, cond, arg )
    {} 
    
    explicit optional ( in_place_init_if_t, bool cond )
    : base( in_place_init_if, cond )
    {}
#endif

    void swap( optional & arg )
      BOOST_NOEXCEPT_IF(::boost::is_nothrow_move_constructible<T>::value && ::boost::is_nothrow_move_assignable<T>::value)
      {
        // allow for Koenig lookup
        boost::swap(*this, arg);
      }


    // Returns a reference to the value if this is initialized, otherwise,
    // the behaviour is UNDEFINED
    // No-throw
    reference_const_type get() const { BOOST_ASSERT(this->is_initialized()) ; return this->get_impl(); }
    reference_type       get()       { BOOST_ASSERT(this->is_initialized()) ; return this->get_impl(); }

    // Returns a copy of the value if this is initialized, 'v' otherwise
    reference_const_type get_value_or ( reference_const_type v ) const { return this->is_initialized() ? get() : v ; }
    reference_type       get_value_or ( reference_type       v )       { return this->is_initialized() ? get() : v ; }

    // Returns a pointer to the value if this is initialized, otherwise,
    // the behaviour is UNDEFINED
    // No-throw
    pointer_const_type operator->() const { BOOST_ASSERT(this->is_initialized()) ; return this->get_ptr_impl() ; }
    pointer_type       operator->()       { BOOST_ASSERT(this->is_initialized()) ; return this->get_ptr_impl() ; }

    // Returns a reference to the value if this is initialized, otherwise,
    // the behaviour is UNDEFINED
    // No-throw
#if (!defined BOOST_NO_CXX11_REF_QUALIFIERS) && (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) 
    reference_const_type operator *() const& { return this->get() ; }
    reference_type       operator *() &      { return this->get() ; }
    reference_type_of_temporary_wrapper operator *() && { return boost::move(this->get()) ; }
#else
    reference_const_type operator *() const { return this->get() ; }
    reference_type       operator *()       { return this->get() ; }
#endif // !defined BOOST_NO_CXX11_REF_QUALIFIERS

#if (!defined BOOST_NO_CXX11_REF_QUALIFIERS) && (!defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES) 
    reference_const_type value() const&
      { 
        if (this->is_initialized())
          return this->get() ;
        else
          throw_exception(bad_optional_access());
      }
      
    reference_type value() &
      { 
        if (this->is_initialized())
          return this->get() ;
        else
          throw_exception(bad_optional_access());
      }
      
    reference_type_of_temporary_wrapper value() &&
      { 
        if (this->is_initialized())
          return boost::move(this->get()) ;
        else
          throw_exception(bad_optional_access());
      }

#else 
    reference_const_type value() const
      { 
        if (this->is_initialized())
          return this->get() ;
        else
          throw_exception(bad_optional_access());
      }
      
    reference_type value()
      { 
        if (this->is_initialized())
          return this->get() ;
        else
          throw_exception(bad_optional_access());
      }
#endif


#ifndef BOOST_NO_CXX11_REF_QUALIFIERS
    template <class U>
    value_type value_or ( U&& v ) const&
      { 
        if (this->is_initialized())
          return get();
        else
          return boost::forward<U>(v);
      }
    
    template <class U>
    value_type value_or ( U&& v ) && 
      { 
        if (this->is_initialized())
          return boost::move(get());
        else
          return boost::forward<U>(v);
      }
#elif !defined BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
    template <class U>
    value_type value_or ( U&& v ) const 
      {
        if (this->is_initialized())
          return get();
        else
          return boost::forward<U>(v);
      }
#else
    template <class U>
    value_type value_or ( U const& v ) const 
      { 
        if (this->is_initialized())
          return get();
        else
          return v;
      }
      
    template <class U>
    value_type value_or ( U& v ) const 
      { 
        if (this->is_initialized())
          return get();
        else
          return v;
      }
#endif


#ifndef BOOST_NO_CXX11_REF_QUALIFIERS
    template <typename F>
    value_type value_or_eval ( F f ) const&
      {
        if (this->is_initialized())
          return get();
        else
          return f();
      }
      
    template <typename F>
    value_type value_or_eval ( F f ) &&
      {
        if (this->is_initialized())
          return boost::move(get());
        else
          return f();
      }
#else
    template <typename F>
    value_type value_or_eval ( F f ) const
      {
        if (this->is_initialized())
          return get();
        else
          return f();
      }
#endif
      
    bool operator!() const BOOST_NOEXCEPT { return !this->is_initialized() ; }
    
    BOOST_EXPLICIT_OPERATOR_BOOL_NOEXCEPT()
} ;

} // namespace boost

#endif // BOOST_OPTIONAL_CONFIG_USE_OLD_IMPLEMENTATION_OF_OPTIONAL

namespace boost {
  
#ifndef  BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
template<class T>
class optional<T&&>
{
  BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Optional rvalue references are illegal.");
} ;
#endif

} // namespace boost

#ifndef BOOST_OPTIONAL_CONFIG_DONT_SPECIALIZE_OPTIONAL_REFS
# include <boost/optional/detail/optional_reference_spec.hpp>
#endif

namespace boost {

#ifndef BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES

template<class T>
inline
optional<BOOST_DEDUCED_TYPENAME boost::decay<T>::type> make_optional ( T && v  )
{
  return optional<BOOST_DEDUCED_TYPENAME boost::decay<T>::type>(boost::forward<T>(v));
}

// Returns optional<T>(cond,v)
template<class T>
inline
optional<BOOST_DEDUCED_TYPENAME boost::decay<T>::type> make_optional ( bool cond, T && v )
{
  return optional<BOOST_DEDUCED_TYPENAME boost::decay<T>::type>(cond,boost::forward<T>(v));
}

#else

// Returns optional<T>(v)
template<class T>
inline
optional<T> make_optional ( T const& v  )
{
  return optional<T>(v);
}

// Returns optional<T>(cond,v)
template<class T>
inline
optional<T> make_optional ( bool cond, T const& v )
{
  return optional<T>(cond,v);
}

#endif // BOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES

// Returns a reference to the value if this is initialized, otherwise, the behaviour is UNDEFINED.
// No-throw
template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::reference_const_type
get ( optional<T> const& opt )
{
  return opt.get() ;
}

template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::reference_type
get ( optional<T>& opt )
{
  return opt.get() ;
}

// Returns a pointer to the value if this is initialized, otherwise, returns NULL.
// No-throw
template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::pointer_const_type
get ( optional<T> const* opt )
{
  return opt->get_ptr() ;
}

template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::pointer_type
get ( optional<T>* opt )
{
  return opt->get_ptr() ;
}

// Returns a reference to the value if this is initialized, otherwise, the behaviour is UNDEFINED.
// No-throw
template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::reference_const_type
get_optional_value_or ( optional<T> const& opt, BOOST_DEDUCED_TYPENAME optional<T>::reference_const_type v )
{
  return opt.get_value_or(v) ;
}

template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::reference_type
get_optional_value_or ( optional<T>& opt, BOOST_DEDUCED_TYPENAME optional<T>::reference_type v )
{
  return opt.get_value_or(v) ;
}

// Returns a pointer to the value if this is initialized, otherwise, returns NULL.
// No-throw
template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::pointer_const_type
get_pointer ( optional<T> const& opt )
{
  return opt.get_ptr() ;
}

template<class T>
inline
BOOST_DEDUCED_TYPENAME optional<T>::pointer_type
get_pointer ( optional<T>& opt )
{
  return opt.get_ptr() ;
}

} // namespace boost

namespace boost {
  
// The following declaration prevents a bug where operator safe-bool is used upon streaming optional object if you forget the IO header.
template<class CharType, class CharTrait>
std::basic_ostream<CharType, CharTrait>&
operator<<(std::basic_ostream<CharType, CharTrait>& os, optional_detail::optional_tag const&)
{
  BOOST_STATIC_ASSERT_MSG(sizeof(CharType) == 0, "If you want to output boost::optional, include header <boost/optional/optional_io.hpp>");
  return os;  
}

} // namespace boost

#include <boost/optional/detail/optional_relops.hpp>
#include <boost/optional/detail/optional_swap.hpp>

#endif // header guard
