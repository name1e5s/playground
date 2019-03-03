#include "shapes.hpp"

int main(int argc, const char **argv) {
  cout << "Rectangle:" << endl;
  Rectangle rectangle(1, 10);
  cout << "Area of rectangle is: " << rectangle.area() << endl;
  cout << "Square:" << endl;
  Square square(10);
  cout << "Area of square is: " << square.area() << endl;
  cout << "Circle:" << endl;
  Circle circle(10);
  cout << "Area of circle is: " << circle.area() << endl;
}
