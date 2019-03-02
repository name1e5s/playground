#ifndef CIRCLE
#define CIRCLE
#include <cmath>
#include <iostream>
class circle {
private:
  double x, y, r;

public:
  void init(void) {
    std::cout << "Input x,y and radius:" << std::endl;
    std::cin >> x >> y >> r;
  };
  circle(double x = 0, double y = 0, double r = 0) : x(x), y(y), r(r) {
    std::cout << "constructed." << std::endl;
  }
  ~circle() { std::cout << "deconstructed." << std::endl; }

  bool isCross(circle &maru) {
    double d =
        std::sqrt((x - maru.x) * (x - maru.x) + (y - maru.y) * (y - maru.y));
    return (std::abs(r - maru.r) < d) && (d < r + maru.r);
  }
};
#endif
