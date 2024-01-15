#include "bezierPatch.h"
#include <unordered_map>
#include <unordered_set>

BezierPatch::BezierPatch()
{
  controlPoints = std::vector< std::vector<glm::vec3> >(4);
  for (int i = 0; i < 4; i++)
  {
    controlPoints[i] = std::vector<glm::vec3>(4);
  }
}

void BezierPatch::add2mesh(Polymesh* mesh, int npt) const
{
  int nu = npt;
  int nv = npt;
  for (int i = 0; i < nu; i++)
  {
    for (int j = 0; j < nv; j++)
    {
      glm::vec3 v0, v1, v2, v3;

      float ui, uii, vj, vjj;
      ui = i/float(nu);
      uii = (i+1)/float(nu);
      vj = j/float(nv);
      vjj = (j+1)/float(nv);

      v0 = evaluate(ui, vj);
      v1 = evaluate(uii, vj);
      v2 = evaluate(uii, vjj);
      v3 = evaluate(ui, vjj);
      
      glm::vec3 n1, n2;
      compute_triangle_normal(v1, v2, v0, n1);
      compute_triangle_normal(v2, v3, v0, n2);

      addTriangle(mesh, v1, v2, v0, n1);
      addTriangle(mesh, v2, v3, v0, n2);
    }
  }
}


void BezierPatch::addTriangle(Polymesh* mesh, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& n) const
{
  size_t base = mesh->vertices.size();
  mesh->vertices.push_back(v0);
  mesh->vertices.push_back(v1);
  mesh->vertices.push_back(v2);
  
  // calculate per-vertex normal here
  mesh->normals.push_back(n);
  mesh->normals.push_back(n);
  mesh->normals.push_back(n);

  Polygon poly;
  poly.vertex_indices.push_back(base);
  poly.vertex_indices.push_back(base+1);
  poly.vertex_indices.push_back(base+2);
  mesh->polygons.push_back(poly);
}


void BezierPatch::loadControlPoints(FILE* file)
{
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
    {
      fscanf(file, "%f %f %f", &controlPoints[i][j].x, &controlPoints[i][j].y,
      &controlPoints[i][j].z);
    }
  }
}