struct A
{
  A(int) {}
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  A(A &&) {}
#endif
};

struct B
{
  operator A() { return A(1); }
  operator int() { return 0; }
};

int main()
{
  A t = B();
}