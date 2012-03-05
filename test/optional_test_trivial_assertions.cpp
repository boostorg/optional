#include "boost/optional.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits.hpp"

using namespace boost;

typedef optional<char>        oc;
typedef optional<int>         oi;
typedef optional<std::string> os;

BOOST_STATIC_ASSERT( sizeof(oc) <= 2*sizeof(char) );
BOOST_STATIC_ASSERT( sizeof(oi) <= 2*sizeof(int)  );

BOOST_STATIC_ASSERT( !has_trivial_default_constructor<oi>::value );  //never true for optional
BOOST_STATIC_ASSERT( !has_trivial_default_constructor<os>::value );

BOOST_STATIC_ASSERT(  has_trivial_destructor<oi>::value );  //should be true where has_trivial_destructor<T>
BOOST_STATIC_ASSERT( !has_trivial_destructor<os>::value );


/*
has_trivial_assign
has_trivial_constructor
has_trivial_copy
has_trivial_copy_constructor
has_trivial_default_constructor
has_trivial_destructor
 */


