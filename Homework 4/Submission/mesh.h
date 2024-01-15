#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "glm/glm.hpp"

#include "bezierPatch.h"
#include "renderer.h"

class Mesh : public Renderer {
public:
  
  void init() ; 
  void render() ;
  void resize(size_t w, size_t h) ;
  std::string name() { return "Mesh"; }
  std::string info() { return ""; }

  void loadBezierMeshFile(std::string filename);

  void evaluateMesh();

  void setup_camera();
  void drawFaces();
  void drawEdges();
  void drawVertices();

  void key_event(char key) ;

  void cursor_event(float x, float y, unsigned char keys) ;

  void scroll_event(float offset_x, float offset_y) ;

  void mouse_button_event(int button, int event) ;

  void getBounds(const Polymesh& mesh, glm::vec3& low, glm::vec3& high, glm::vec3& center);

  void reset_camera();

  void update_camera();

private:
  std::vector<BezierPatch> patches;
  Polymesh pmesh;

  float view_distance, min_view_distance, max_view_distance, canonical_view_distance;
  glm::vec3 camera_angles;
  glm::vec3 view_focus;

  size_t screen_w, screen_h;
  float vfov, hfov;
  float nclip, fclip;
  bool smoothShading, shadingMode;

  bool left_down, right_down, middle_down;
  float mouse_x, mouse_y;

  glm::vec3 face_color, edge_color, vertex_color;
  float edge_width, vertex_radius;

  bool camera_setup = false;
  bool nBezierPt_changed = false;
  int nBezierPt; // number of pts to evaluate of each Bezier Patch. default (4x4)
}; // class Mesh
#endif // MESH_H
