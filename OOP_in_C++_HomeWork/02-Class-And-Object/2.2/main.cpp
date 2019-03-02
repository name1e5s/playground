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
  matrix *pA1 = new matrix(1, 2);
  matrix *pA2 = new matrix(1, 2);
  matrix *pA3 = new matrix(2, 33); // Doesn't matter.
  pA1->init();
  pA2->init();
  *pA3 = *pA1 + *pA2;
  std::cout << "pA2 is: \n" << *pA3 << "\n";
  delete pA1;
  delete pA2;
  delete pA3;
  return 0;
}
