#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include <vector>
#include "glm/glm.hpp"

struct Polygon {

  std::vector<size_t> vertex_indices;    ///< 0-based indices into vertex array
  std::vector<size_t> normal_indices;    ///< 0-based indices into normal array
  std::vector<size_t> texcoord_indices;  ///< 0-based indices into texcoord array

}; // struct Polygon

struct Polymesh {

  std::vector<glm::vec3> vertices;   ///< polygon vertex array
  std::vector<glm::vec3> normals;    ///< polygon normal array
  std::vector<glm::vec2> texcoords;  ///< texture coordinate array

  std::vector<Polygon> polygons;   ///< polygons

}; // struct Polymesh

void compute_triangle_normal(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& n);

class BezierPatch {
  public:

    BezierPatch();

    void add2mesh(Polymesh *mesh, int npt) const;

    void loadControlPoints(FILE *file);

    /**
     * The original control points of the Bezier patch
     */
    std::vector< std::vector<glm::vec3> > controlPoints;

    /**
     * Evaluates one step of the de Casteljau's algorithm using the given points and the scalar parameter t
     *
     * @param points    A vector of points in 3D
     * @param t         Scalar interpolation parameter
     * @return A vector containing intermediate points or the final interpolated vector
     */
    std::vector<glm::vec3> evaluateStep(std::vector<glm::vec3> const &points, float t) const;

    /**
     * Fully evaluates de Casteljau's algorithm for a vector of points at scalar parameter t
     *
     * @param points    A vector of points in 3D
     * @param t         Scalar interpolation parameter
     * @return Final interpolated vector
     */
    glm::vec3 evaluate1D(std::vector<glm::vec3> const &points, float t) const;

    /**
     * Evaluates the Bezier patch at parameter (u, v)
     *
     * @param u         Scalar interpolation parameter
     * @param v         Scalar interpolation parameter (along the other axis)
     * @return Final interpolated vector
     */
    glm::vec3 evaluate(float u, float v) const;

    void addTriangle(Polymesh *mesh, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &n) const;
};

#endif /* BEZIERPATCH_H */
