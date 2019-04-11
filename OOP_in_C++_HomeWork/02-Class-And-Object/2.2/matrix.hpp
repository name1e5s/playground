#ifndef MATRIX_CPP
#define MATRIX_CPP
#include <exception>
#include <iostream>

struct size_incompatible : public std::exception {
  const char *what() const noexcept override {
    return "matrices size incompatible!";
  }
};

class matrix {
public:
  matrix(int i = 4, int j = 5) : _i{i}, _j{j} {
    if (i <= 0 || j <= 0) {
      _value = nullptr;
      return;
    }
    _value = new double[i * j];
    std::cout << __FUNCTION__ << "(" << _i << "," << _j << ")"
              << " called." << std::endl;
  }
  matrix(const matrix &mat)
      : _i{mat._i}, _j{mat._j}, _value{(_i <= 0 || _j <= 0)
                                           ? nullptr
                                           : new double[_i * _j]} {
    std::copy(mat._value, mat._value + (_i * _j), _value);
    std::cout << __FUNCTION__ << " (copy) called." << std::endl;
  }
  ~matrix() {
    delete[] _value;
    std::cout << __FUNCTION__ << "(" << _i << "," << _j << ")"
              << " called." << std::endl;
  }
  // Operate overloads
  double operator()(int i, int j) const;
  double &operator()(int i, int j);

  matrix &operator=(const matrix &);
  matrix &operator+=(const matrix &);
  matrix &operator-=(const matrix &);

  // Inpu/Output
  void init() {
    for (int i = 0; i < _i * _j; i++)
      std::cin >> _value[i];
  }
  friend std::ostream &operator<<(std::ostream &os, const matrix &);

private:
  int _i, _j;
  double *_value;
};

matrix operator+(const matrix &, const matrix &);
matrix operator-(const matrix &, const matrix &);
#endif
