#ifndef SHAPES
#define SHAPES
#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;

struct NotImplemented : public std::logic_error {
  NotImplemented() : std::logic_error("Function Not Implemented Yet!") {}
};

class Shape {
public:
  Shape() { cout << __FUNCTION__ << " called." << endl; }
  ~Shape() { cout << __FUNCTION__ << " called." << endl; }
  virtual double area() { throw NotImplemented(); }
};

class Rectangle : public Shape {
public:
  Rectangle(double w = 0, double h = 0) : Shape(), width(w), height(h) {
    cout << __FUNCTION__ << " called." << endl;
  }
  ~Rectangle() { cout << __FUNCTION__ << " called." << endl; }
  double area() override { return width * height; }

protected:
  double width, height;
};

class Square : public Rectangle {
public:
  Square(double l = 0) : Rectangle(l, l) {
    cout << __FUNCTION__ << " called." << endl;
  }
  ~Square() { cout << __FUNCTION__ << " called." << endl; }
};

class Circle : public Shape {
public:
  Circle(double r = 0) : Shape(), radius(r) {
    cout << __FUNCTION__ << " called." << endl;
  }

  ~Circle() { cout << __FUNCTION__ << " called." << endl; }
  double area() override {
#define _USE_MATH_DEFINES
    return M_PI * radius * radius;
#undef _USE_MATH_DEFINES
  }

private:
  double radius;
};
#endif
