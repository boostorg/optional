struct A
{
  A(int) {}
};

struct B
{
  operator int() { return 0; }
};

int main()
{
  A t = B();
}