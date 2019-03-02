#include "circle.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char **argv) {
  circle a, b;
  a.init();
  b.init();
  if (a.isCross(b))
    cout << "Crossed." << endl;
  else
    cout << "Not Crossed." << endl;
  return 0;
}
