#include "boost/optional.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits.hpp"

using namespace boost;

typedef optional<char>        oc;
typedef optional<int>         oi;
typedef optional<int&>        of;
typedef optional<std::string> os;

BOOST_STATIC_ASSERT( sizeof(oc) <= 2*sizeof(char) );
BOOST_STATIC_ASSERT( sizeof(oi) <= 2*sizeof(int)  );

BOOST_STATIC_ASSERT( !has_trivial_default_constructor<oi>::value );  //never true for optional
BOOST_STATIC_ASSERT( !has_trivial_default_constructor<os>::value );

BOOST_STATIC_ASSERT(  has_trivial_destructor<oi>::value );  //should be true where has_trivial_destructor<T>
BOOST_STATIC_ASSERT(  has_trivial_destructor<of>::value );
BOOST_STATIC_ASSERT( !has_trivial_destructor<os>::value );

BOOST_STATIC_ASSERT(  has_trivial_copy<oi>::value );  //should be true where has_trivial_copy<T>
BOOST_STATIC_ASSERT(  has_trivial_copy<of>::value );
BOOST_STATIC_ASSERT( !has_trivial_copy<os>::value );

BOOST_STATIC_ASSERT(  has_trivial_assign<oi>::value );  //should be true where has_trivial_assign<T>
BOOST_STATIC_ASSERT(  has_trivial_assign<of>::value );
BOOST_STATIC_ASSERT( !has_trivial_assign<os>::value );

BOOST_STATIC_ASSERT( oi::dtor_optimized::value );
BOOST_STATIC_ASSERT( oi::copy_optimized::value );


BOOST_STATIC_ASSERT( of::dtor_optimized::value );
BOOST_STATIC_ASSERT( of::copy_optimized::value );



/*
 * The code generated below can be looked at with objdump to see if it is suboptimal
 */


void assign_optional_int(oi& x,oi& y){
	x=y;
}

void reset_optional_int(oi& x){
	x=boost::none;
}



