#ifndef CIRCLE
#define CIRCLE
#include <cmath>
#include <iostream>
class Point {
public:
  double x, y;
  Point(double x = 0, double y = 0) : x(x), y(y) {
    std::cout << "Point::Point called." << std::endl;
  }
  ~Point() { std::cout << "Point destroyed." << std::endl; }
  double distance(const Point &p) {
    return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
  }
};

class circle {
private:
  Point p;
  double r;

public:
  void init(void) {
    std::cout << "Input x,y and radius:" << std::endl;
    std::cin >> p.x >> p.y >> r;
  };
  circle(double x = 0, double y = 0, double r = 0) : p(x, y), r(r) {
    std::cout << "Circle::Circle called.." << std::endl;
  }
  ~circle() { std::cout << "Circle destroyed." << std::endl; }

  bool isCross(const circle &maru) {
    double d = p.distance(maru.p);
    return (std::abs(r - maru.r) < d) && (d < r + maru.r);
  }
};
#endif
