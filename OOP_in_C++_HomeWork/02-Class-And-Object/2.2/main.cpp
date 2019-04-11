#include "matrix.hpp"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Input A1:" << std::endl;
  matrix a1;
  a1.init();
  std::cout << "Input A2:" << std::endl;
  matrix a2;
  a2.init();
  std::cout << "A1 is\n" << a1 << "\n";
  std::cout << "A2 is\n" << a2 << "\n";
  std::cout << "A1 + A2 is\n" << a1 + a2 << "\n";
  std::cout << "A1 - A2 is\n" << a1 - a2 << "\n";
  auto pA1 = new matrix(1, 2);
  auto pA2 = new matrix(1, 2);
  auto pA3 = new matrix(2, 33); // Doesn't matter.

  std::cout << "Input pA1:" << std::endl;
  pA1->init();
  std::cout << "Input pA2:" << std::endl;
  pA2->init();
  *pA3 = *pA1 + *pA2;
  std::cout << "pA3 is: \n" << *pA3 << "\n";
  delete pA1;
  delete pA2;
  delete pA3;
  return 0;
}
