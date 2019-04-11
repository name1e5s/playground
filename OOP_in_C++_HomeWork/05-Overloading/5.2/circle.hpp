#ifndef CIRCLE
#define CIRCLE
#include <cmath>
#include <iostream>
class Point {
public:
  double x, y;
  Point(double x = 0, double y = 0) : x(x), y(y) {
    std::cout << __FUNCTION__ << " called." << std::endl;
  }
  ~Point() { std::cout << __FUNCTION__ << " called." << std::endl; }
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

class Circle {
private:
  double r;

public:
  Point p;
  void init() {
    std::cout << "Input x,y and radius:" << std::endl;
    std::cin >> p.x >> p.y >> r;
  };
  Circle(double x = 0, double y = 0, double r = 0) : r(r), p(x, y) {
    std::cout << __FUNCTION__ << " called." << std::endl;
  }
  ~Circle() { std::cout << __FUNCTION__ << " called." << std::endl; }

  bool isCross(const Circle &maru) {
    double d = p.distance(maru.p);
    return (std::abs(r - maru.r) < d) && (d < r + maru.r);
  }
};
#endif
