// Copyright 2019 Weicheng Pei and Minghao Yang
#include "mini/element/line.hpp"

#include <vector>

#include "gtest/gtest.h"

namespace mini {
namespace element {

class LineTest : public ::testing::Test {
 protected:
  using Real = double;
  using Line = Line<Real, 2>;
  using Point = Line::Point;
  Point head{1, {0.3, 0.0}}, tail{2, {0.0, 0.4}};
};
TEST_F(LineTest, ConstructorWithId) {
  // Test Line(Id, Point const&, Point const&):
  auto i = Line::Id{0};
  auto line = Line(i, head, tail);
  EXPECT_EQ(line.I(), i);
  EXPECT_EQ(line.Head(), head);
  EXPECT_EQ(line.Tail(), tail);
  EXPECT_EQ(line.Head().I(), head.I());
  EXPECT_EQ(line.Tail().I(), tail.I());
}
TEST_F(LineTest, ConstructorWithoutId) {
  // Test Line(Point const&, Point const&):
  auto line = Line(head, tail);
  EXPECT_EQ(line.I(), Line::DefaultId());
  EXPECT_EQ(line.Head(), head);
  EXPECT_EQ(line.Tail(), tail);
  EXPECT_EQ(line.Head().I(), head.I());
  EXPECT_EQ(line.Tail().I(), tail.I());
}
TEST_F(LineTest, MeshMethods) {
  auto line = Line(head, tail);
  EXPECT_EQ(line.Measure(), 0.5);
  auto center = line.Center();
  EXPECT_EQ(center.X() * 2, head.X() + tail.X());
  EXPECT_EQ(center.Y() * 2, head.Y() + tail.Y());
  auto integrand = [](const auto& point) { return 3.14; };
  EXPECT_EQ(line.Integrate(integrand), line.Measure() * 3.14);
}

}  // namespace element
}  // namespace mini

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}