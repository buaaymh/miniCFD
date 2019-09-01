// Copyright 2019 Weicheng Pei and Minghao Yang

#ifndef MINI_ELEMENT_DIM0_HPP_
#define MINI_ELEMENT_DIM0_HPP_

#include <cstddef>
#include <initializer_list>
#include <utility>

#include "mini/geometry/dim0.hpp"

namespace mini {
namespace element {

template <class Real, int kDim>
class Node : public geometry::Point<Real, kDim> {
 public:
  // Types:
  using Id = std::size_t;
  // Constructors:
  template<class... XYZ>
  Node(Id i, XYZ&&... xyz)
      : i_(i), geometry::Point<Real, kDim>{std::forward<XYZ>(xyz)...} {}
  Node(Id i, std::initializer_list<Real> xyz)
      : i_(i), geometry::Point<Real, kDim>(xyz) {}
  Node(std::initializer_list<Real> xyz) : Node{DefaultId(), xyz} {}
  // Accessors:
  Id I() const { return i_; }
  static Id DefaultId() { return -1; }
 private:
  Id i_;
};

}  // namespace element
}  // namespace mini

#endif  // MINI_ELEMENT_DIM0_HPP_
