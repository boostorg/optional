#include <boost/type_traits/has_nothrow_constructor.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

namespace some_namespace
{
  class base_class
  {
  public:
    base_class & operator=(const base_class &){ throw int(); }
    virtual ~base_class() {}
  };

  class class_without_default_ctor : public base_class
  {
  public:
    char data;
    explicit class_without_default_ctor(char arg) : data(arg) {}
  };
}

int main()
{
  BOOST_TEST_TRAIT_FALSE((boost::has_nothrow_default_constructor<some_namespace::class_without_default_ctor>));
  return boost::report_errors();
}
