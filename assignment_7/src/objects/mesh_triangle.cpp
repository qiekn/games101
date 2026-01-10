#include "objects/mesh_triangle.h"
#include "utils/obj_loader.h"

Vector3f MeshTriangle::EvalDiffuseColor(const Vector2f& st) const {
  float scale = 5;
  float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
  return Lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231), pattern);
}

Intersection MeshTriangle::GetIntersection(Ray ray) {
  Intersection intersec;

  if (bvh) {
    intersec = bvh->Intersect(ray);
  }

  return intersec;
}

void MeshTriangle::Sample(Intersection& pos, float& pdf) {
  bvh->Sample(pos, pdf);
  pos.emit = m->GetEmission();
}

void MeshTriangle::GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                                        Vector3f& N, Vector2f& st) const {
  const Vector3f& v0 = vertices[vertex_index[index * 3]];
  const Vector3f& v1 = vertices[vertex_index[index * 3 + 1]];
  const Vector3f& v2 = vertices[vertex_index[index * 3 + 2]];
  Vector3f e0 = Normalize(v1 - v0);
  Vector3f e1 = Normalize(v2 - v1);
  N = Normalize(CrossProduct(e0, e1));
  const Vector2f& st0 = st_coordinates[vertex_index[index * 3]];
  const Vector2f& st1 = st_coordinates[vertex_index[index * 3 + 1]];
  const Vector2f& st2 = st_coordinates[vertex_index[index * 3 + 2]];
  st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
}

bool MeshTriangle::Intersect(const Ray& ray, float& tnear, uint32_t& index) const {
  bool intersect = false;
  for (uint32_t k = 0; k < num_triangles; ++k) {
    const Vector3f& v0 = vertices[vertex_index[k * 3]];
    const Vector3f& v1 = vertices[vertex_index[k * 3 + 1]];
    const Vector3f& v2 = vertices[vertex_index[k * 3 + 2]];
    float t, u, v;
    if (RayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t, u, v) && t < tnear) {
      tnear = t;
      index = k;
      intersect |= true;
    }
  }

  return intersect;
}

MeshTriangle::MeshTriangle(const std::string& filename, Material* mt) {
  objl::Loader loader;
  loader.LoadFile(filename);
  area = 0;
  m = mt;
  assert(loader.LoadedMeshes.size() == 1);
  auto mesh = loader.LoadedMeshes[0];

  Vector3f min_vert = Vector3f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(),
                               std::numeric_limits<float>::infinity()};
  Vector3f max_vert = Vector3f{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
                               -std::numeric_limits<float>::infinity()};
  for (int i = 0; i < mesh.Vertices.size(); i += 3) {
    std::array<Vector3f, 3> face_vertices;

    for (int j = 0; j < 3; j++) {
      auto vert =
          Vector3f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z);
      face_vertices[j] = vert;

      min_vert = Vector3f(std::min(min_vert.x, vert.x), std::min(min_vert.y, vert.y), std::min(min_vert.z, vert.z));
      max_vert = Vector3f(std::max(max_vert.x, vert.x), std::max(max_vert.y, vert.y), std::max(max_vert.z, vert.z));
    }

    triangles.emplace_back(face_vertices[0], face_vertices[1], face_vertices[2], mt);
  }

  bounding_box = Bounds3(min_vert, max_vert);

  std::vector<Object*> ptrs;
  for (auto& tri : triangles) {
    ptrs.push_back(&tri);
    area += tri.area;
  }
  bvh = new BVHAccel(ptrs);
}
