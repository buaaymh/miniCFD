// Copyright 2019 Weicheng Pei and Minghao Yang

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "mini/mesh/data.hpp"
#include "mini/mesh/dim2.hpp"
#include "mini/mesh/vtk.hpp"
#include "mini/data/path.hpp"  // defines TEST_DATA_DIR

namespace mini {
namespace mesh {

class VtkReaderTest : public ::testing::Test {
 protected:
  using Mesh = Mesh<double>;
  using Cell = Mesh::Cell;
  VtkReader<Mesh> reader;
  const std::string test_data_dir_{TEST_DATA_DIR};
};
TEST_F(VtkReaderTest, ReadFromFile) {
  EXPECT_TRUE(reader.ReadFromFile(test_data_dir_ + "tiny.vtk"));
  EXPECT_TRUE(reader.ReadFromFile(test_data_dir_ + "tiny.vtu"));
}
TEST_F(VtkReaderTest, GetMesh) {
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFromFile(test_data_dir_ + "tiny" + suffix);
    auto mesh = reader.GetMesh();
    ASSERT_TRUE(mesh);
    EXPECT_EQ(mesh->CountNodes(), 6);
    EXPECT_EQ(mesh->CountWalls(), 8);
    EXPECT_EQ(mesh->CountCells(), 3);
    // sum of each face's area
    double area = 0.0;
    auto visitor = [&area](const Cell& d) { area += d.Measure(); };
    mesh->ForEachCell(visitor);
    EXPECT_EQ(area, 2.0);
  }
}
TEST_F(VtkReaderTest, MediumMesh) {
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFromFile(test_data_dir_ + "medium" + suffix);
    auto mesh = reader.GetMesh();
    ASSERT_TRUE(mesh);
    EXPECT_EQ(mesh->CountNodes(), 920);
    auto n_lines = (918*3 + 400*4 + 12*10) / 2;
    EXPECT_EQ(mesh->CountWalls(), n_lines);
    EXPECT_EQ(mesh->CountCells(), 918/* Triangles */ + 400/* Rectangles */);
    // sum of each face's area
    double area = 0.0;
    auto visitor = [&area](const Cell& d) { area += d.Measure(); };
    mesh->ForEachCell(visitor);
    EXPECT_NEAR(area, 8.0, 1e-6);
  }
}

class VtkWriterTest : public ::testing::Test {
 public:
  static const char* mesh_name;
 protected:
  const std::string test_data_dir_{TEST_DATA_DIR};
};
const char* VtkWriterTest::mesh_name;
TEST_F(VtkWriterTest, TinyMesh) {
  auto reader = VtkReader<Mesh<double>>();
  auto writer = VtkWriter<Mesh<double>>();
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFromFile(test_data_dir_ + "tiny" + suffix);
    auto mesh_old = reader.GetMesh();
    ASSERT_TRUE(mesh_old);
    // Write the mesh just read:
    writer.SetMesh(mesh_old.get());
    auto filename = std::string("tiny") + suffix;
    ASSERT_TRUE(writer.WriteToFile(filename));
    // Read the mesh just written:
    reader.ReadFromFile(filename);
    auto mesh_new = reader.GetMesh();
    // Check consistency:
    EXPECT_EQ(mesh_old->CountNodes(),
              mesh_new->CountNodes());
    EXPECT_EQ(mesh_old->CountWalls(),
              mesh_new->CountWalls());
    EXPECT_EQ(mesh_old->CountCells(),
              mesh_new->CountCells());
  }
}
TEST_F(VtkWriterTest, MeshWithData) {
  using NodeData = Data<double, 2/* dims */, 2/* scalars */, 2/* vectors */>;
  using EdgeData = Empty;
  using CellData = NodeData;
  using Mesh = Mesh<double, NodeData, EdgeData, CellData>;
  auto reader = VtkReader<Mesh>();
  auto writer = VtkWriter<Mesh>();
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFromFile(test_data_dir_ + mesh_name + suffix);
    auto mesh_old = reader.GetMesh();
    ASSERT_TRUE(mesh_old);
    // Create some data on it:
    Mesh::Node::scalar_names.at(0) = "X + Y";
    Mesh::Node::scalar_names.at(1) = "X - Y";
    Mesh::Node::vector_names.at(0) = "(X, Y)";
    Mesh::Node::vector_names.at(1) = "(-X, -Y)";
    mesh_old->ForEachNode([](Mesh::Node& node) {
      auto x = node.X();
      auto y = node.Y();
      node.data.scalars.at(0) = x + y;
      node.data.scalars.at(1) = x - y;
      node.data.vectors.at(0) = {x, y};
      node.data.vectors.at(1) = {-x, -y};
    });
    Mesh::Cell::scalar_names.at(0) = "X + Y";
    Mesh::Cell::scalar_names.at(1) = "X - Y";
    Mesh::Cell::vector_names.at(0) = "(X, Y)";
    Mesh::Cell::vector_names.at(1) = "(-X, -Y)";
    mesh_old->ForEachCell([](Mesh::Cell& cell) {
      auto center = cell.Center();
      auto y = center.Y();
      auto x = center.X();
      cell.data.scalars.at(0) = x + y;
      cell.data.scalars.at(1) = x - y;
      cell.data.vectors.at(0) = {x, y};
      cell.data.vectors.at(1) = {-x, -y};
    });
    // Write the mesh just read:
    writer.SetMesh(mesh_old.get());
    auto filename = std::string(mesh_name) + "_with_data" + suffix;
    ASSERT_TRUE(writer.WriteToFile(filename));
    // Read the mesh just written:
    reader.ReadFromFile(filename);
    auto mesh_new = reader.GetMesh();
    ASSERT_TRUE(mesh_new);
    // Check consistency:
    EXPECT_EQ(mesh_old->CountNodes(),
              mesh_new->CountNodes());
    EXPECT_EQ(mesh_old->CountWalls(),
              mesh_new->CountWalls());
    EXPECT_EQ(mesh_old->CountCells(),
              mesh_new->CountCells());
  }
}

}  // namespace mesh
}  // namespace mini

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  if (argc == 1) {
    mini::mesh::VtkWriterTest::mesh_name = "tiny";
  } else {
    mini::mesh::VtkWriterTest::mesh_name = argv[1];
  }
  return RUN_ALL_TESTS();
}
