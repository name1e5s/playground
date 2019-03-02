#include "matrix.hpp"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Input A1:" << std::endl;
  matrix a1;
  std::cout << "Input A2:" << std::endl;
  matrix a2;
  std::cout << "A1 is\n" << a1 << "\n";
  std::cout << "A2 is\n" << a2 << "\n";
  std::cout << "A1 + A2 is\n" << a1 + a2 << "\n";
  std::cout << "A1 - A2 is\n" << a1 - a2 << "\n";
  return 0;
}
