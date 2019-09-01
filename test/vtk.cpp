// Copyright 2019 Weicheng Pei and Minghao Yang

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "mini/mesh/dim2.hpp"
#include "mini/mesh/vtk.hpp"
#include "data.hpp"  // defines TEST_DATA_DIR

namespace mini {
namespace mesh {

class VtkReaderTest : public ::testing::Test {
 protected:
  using Mesh = Mesh<double>;
  using Domain = Mesh::Domain;
  VtkReader<Mesh> reader;
  const std::string test_data_dir_{TEST_DATA_DIR};
};
TEST_F(VtkReaderTest, ReadFile) {
  EXPECT_TRUE(reader.ReadFile(test_data_dir_ + "tiny.vtk"));
  EXPECT_TRUE(reader.ReadFile(test_data_dir_ + "tiny.vtu"));
}
TEST_F(VtkReaderTest, GetMesh) {
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFile(test_data_dir_ + "tiny" + suffix);
    auto mesh = reader.GetMesh();
    ASSERT_TRUE(mesh);
    EXPECT_EQ(mesh->CountNodes(), 6);
    EXPECT_EQ(mesh->CountBoundaries(), 8);
    EXPECT_EQ(mesh->CountDomains(), 3);
    // sum of each face's area
    double area = 0.0;
    auto visitor = [&area](const Domain& d) { area += d.Measure(); };
    mesh->ForEachDomain(visitor);
    EXPECT_EQ(area, 2.0);
  }
}
TEST_F(VtkReaderTest, MediumMesh) {
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFile(test_data_dir_ + "medium" + suffix);
    auto mesh = reader.GetMesh();
    ASSERT_TRUE(mesh);
    EXPECT_EQ(mesh->CountNodes(), 920);
    auto n_lines = (918*3 + 400*4 + 12*10) / 2;
    EXPECT_EQ(mesh->CountBoundaries(), n_lines);
    EXPECT_EQ(mesh->CountDomains(), 918/* Triangles */ + 400/* Rectangles */);
    // sum of each face's area
    double area = 0.0;
    auto visitor = [&area](const Domain& d) { area += d.Measure(); };
    mesh->ForEachDomain(visitor);
    EXPECT_NEAR(area, 8.0, 1e-6);
  }
}

class VtkWriterTest : public ::testing::Test {
 protected:
  using Mesh = Mesh<double>;
  using Domain = Mesh::Domain;
  VtkReader<Mesh> reader;
  VtkWriter<Mesh> writer;
  const std::string test_data_dir_{TEST_DATA_DIR};
};
TEST_F(VtkWriterTest, TinyMesh) {
  for (auto suffix : {".vtk", ".vtu"}) {
    reader.ReadFile(test_data_dir_ + "tiny" + suffix);
    auto mesh_old = reader.GetMesh();
    ASSERT_TRUE(mesh_old);
    // Write the mesh just read:
    writer.SetMesh(mesh_old.get());
    auto filename = std::string("tiny") + suffix;
    ASSERT_TRUE(writer.WriteFile(filename));
    // Read the mesh just written:
    reader.ReadFile(filename);
    auto mesh_new = reader.GetMesh();
    // Check consistency:
    EXPECT_EQ(mesh_old->CountNodes(),
              mesh_new->CountNodes());
    EXPECT_EQ(mesh_old->CountBoundaries(),
              mesh_new->CountBoundaries());
    EXPECT_EQ(mesh_old->CountDomains(),
              mesh_new->CountDomains());
  }
}

}  // namespace mesh
}  // namespace mini

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
