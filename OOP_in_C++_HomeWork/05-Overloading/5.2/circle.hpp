#ifndef CIRCLE
#define CIRCLE
#include <cmath>
#include <iostream>
class Point {
public:
  double x, y;
  Point(double x = 0, double y = 0) : x(x), y(y) {
    std::cout << "Circle::Circle() called." << std::endl;
  }
  ~Point() { std::cout << "Circle destroied." << std::endl; }
  double distance(const Point &p) {
    return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
  };
  // Overloading
  Point &operator++() {
    x++;
    return *this;
  }
  Point &operator--() {
    x--;
    return *this;
  }
  Point operator++(int n) {
    Point tmp = *this;
    y++;
    return tmp;
  }
  Point operator--(int n) {
    Point tmp = *this;
    y--;
    return tmp;
  }
};

class circle {
private:
  double r;

public:
  Point p;
  void init(void) {
    std::cout << "Input x,y and radius:" << std::endl;
    std::cin >> p.x >> p.y >> r;
  };
  circle(double x = 0, double y = 0, double r = 0) : p(x, y), r(r) {
    std::cout << "constructed." << std::endl;
  }
  ~circle() { std::cout << "deconstructed." << std::endl; }

  bool isCross(const circle &maru) {
    double d = p.distance(maru.p);
    return (std::abs(r - maru.r) < d) && (d < r + maru.r);
  }
};
#endif
