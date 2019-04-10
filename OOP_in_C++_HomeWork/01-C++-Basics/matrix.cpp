#include "matrix.hpp"
#include <stdexcept>

double &matrix::operator()(int i, int j) {
  if ((i < 0 || j < 0) || (i >= _i || j > _j)) {
    throw std::out_of_range("Term requested out of range!");
  }
  return _value[i * _j + j];
}

double matrix::operator()(int i, int j) const {
  if ((i < 0 || j < 0) || (i >= _i || j > _j)) {
    throw std::out_of_range("Term requested out of range!");
  }
  return _value[i * _j + j];
}

matrix &matrix::operator=(const matrix &mat) {
  matrix tmp(mat);
  std::swap(_i, tmp._i);
  std::swap(_j, tmp._j);
  std::swap(_value, tmp._value);
  return *this;
}

matrix &matrix::operator+=(const matrix &mat) {
  if (mat._i != _i || mat._j != mat._j) {
    throw size_incompatible();
  }
  for (int i = 0; i < _i * _j; i++) {
    _value[i] += mat._value[i];
  }
  return *this;
}

matrix &matrix::operator-=(const matrix &mat) {
  if (mat._i != _i || mat._j != mat._j) {
    throw size_incompatible();
  }
  for (int i = 0; i < _i * _j; i++) {
    _value[i] -= mat._value[i];
  }
  return *this;
}

std::ostream &operator<<(std::ostream &os, const matrix &mat) {
  for (int i = 0; i < mat._i; i++) {
    os << (i == 0 ? "[ " : "  ");
    for (int j = 0; j < mat._j; j++) {
      os << mat._value[i * mat._j + j] << " ";
    }
    os << (i == mat._i - 1 ? "]" : "\n");
  }
  return os;
}

matrix operator+(const matrix &a, const matrix &b) {
  matrix tmp = matrix(a);
  tmp += b;
  return tmp;
}

matrix operator-(const matrix &a, const matrix &b) {
  matrix tmp = matrix(a);
  tmp -= b;
  return tmp;
}
