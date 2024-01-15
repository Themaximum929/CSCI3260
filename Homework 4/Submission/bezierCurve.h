#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "renderer.h"

class BezierCurve : public Renderer {
  public:

    explicit BezierCurve(int numControlPoints);

    void loadControlPoints(FILE *file);

    /**
     * Evaluates one step of the de Casteljau's algorithm using the given points and
     * the scalar parameter t (class member)
     *
     * @param points A vector of points in 2D
     * @return A vector containing intermediate points or the final interpolated vector
     */
    std::vector<glm::vec2> evaluateStep(std::vector<glm::vec2> const &points);

    void drawCurve();

    // inherited Renderer interface functions
    void render();

    void init(){}

    void resize(size_t w, size_t h) 
    {
        width = w;
        height = h;
    }

    std::string name() { return "Bezier Curve"; }

    std::string info() { return ""; }

    void key_event(char key) ;

    void cursor_event(float x, float y, unsigned char keys);

    void scroll_event(float offset_x, float offset_y);

  protected:

    /**
     * The original control points of the Bezier curve
     */
    std::vector<glm::vec2> controlPoints;

    /**
     * Value between 0 and 1 to evaluate the Bezier curve at
     */
    float t;

    int numControlPoints;

    size_t width, height;

    int selected = -1;

    int eval_level = 0;

    int drawingCurve = 0;

};

#endif /* BEZIERCURVE_H */
