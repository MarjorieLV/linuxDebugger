#include <iostream>
int foo (int a, int b)
{
    return a+b;
}
int main ()
{
    int toto = 1;
    int titi = 99;
    int tata = foo(toto, titi);
    int a = 2;
    int b = 4;
    int c = a * b;
    std::cerr << "c : " << c << "\n";
    std::cerr << "tata = " << tata << "\n";
    std::cerr << "H E L L O    W O R L D\n";
  return 0;
}
