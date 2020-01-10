// Copyright 2019 Weicheng Pei and Minghao Yang
#ifndef MINI_GEOMETRY_LINE_HPP_
#define MINI_GEOMETRY_LINE_HPP_

#include <stdexcept>

#include "mini/geometry/point.hpp"
#include "mini/geometry/vector.hpp"

namespace mini {
namespace geometry {

template <class Real, int kDim>
class Line {
 public:
  // Types:
  using Point = Point<Real, kDim>;
  // Constructors:
  Line(Point const& head, Point const& tail) : head_(&head), tail_(&tail) {}
  // Accessors:
  int CountVertices() const { return 2; }
  Point const& Head() const { return *head_; }
  Point const& Tail() const { return *tail_; }
  Point const& GetPoint(int i) const {
    switch (i)  {
    case 0:
      return Head();
    case 1:
      return Tail();
    default:
      throw std::out_of_range("A `Line` has two `Point`s.");
    }
  }
  // Geometric methods:
  Real Measure() const {
    auto v = Head() - Tail();
    return std::sqrt(v.Dot(v));
  }
  Point Center() const {
    auto center = Head();
    center += Tail();
    center *= 0.5;
    return center;
  }

 private:
  Point const* head_;
  Point const* tail_;
};

}  // namespace geometry
}  // namespace mini
#endif  // MINI_GEOMETRY_LINE_HPP_