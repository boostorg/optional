struct A
{
  A(int) {}
  A(A &&) {}
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