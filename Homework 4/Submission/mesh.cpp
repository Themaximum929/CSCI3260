#include "GL/glew.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <numeric>
#include "mesh.h"

void Mesh::init()
{
    face_color = glm::vec3(0.5, 0.5, 0.9);
    edge_color = glm::vec3(0.5, 0.5, 0.5);
    vertex_color = glm::vec3(0.0, 0.0, 0.0);
    edge_width = 1.0; 
    vertex_radius = 10.0;

    smoothShading = false;
    shadingMode = false;

    left_down = false;
    right_down = false;
    middle_down = false;

    hfov = 50;
    vfov = 35;
    nclip = 0.1;
    fclip = 100;

    camera_angles = glm::vec3(-M_PI_4/2, M_PI_4, 0);

    nBezierPt = 8;

    glEnable(GL_DEPTH_TEST);

    // Red diffuse light.//.4
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_ambient[] = {.2, .2, .2, 1.0};
    // Infinite light location.
    // Note: Viewer is in the positive z direction,
    // so we make that component significant.
    GLfloat light_position[] = {.2, 0.5, 1.0, 0.0};
    /* Enable a single OpenGL light. */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    // Lighting needs to be explicitly enabled.
    glEnable(GL_LIGHTING);
}

void Mesh::loadBezierMeshFile(std::string filename)
{
    FILE* file = fopen(filename.c_str(), "r");
    int numPatches;
    fscanf(file, "%d", &numPatches);

    patches.resize(numPatches);
    for(int i=0; i<numPatches; i++)
        patches[i].loadControlPoints(file);
    fclose(file);
}

void Mesh::evaluateMesh()
{
    if(pmesh.vertices.size()>0 && !nBezierPt_changed)
        return;

    pmesh.vertices.clear();
    pmesh.polygons.clear();
    pmesh.normals.clear();
    for(int i=0; i<patches.size(); i++)
        patches[i].add2mesh(&pmesh, nBezierPt);

    nBezierPt_changed = false;

    if(!camera_setup)
    {
        setup_camera();
        camera_setup=true;
    }
}

void Mesh::getBounds(const Polymesh& mesh, glm::vec3& low, glm::vec3& high, glm::vec3& center)
{
    float maxV = std::numeric_limits<float>::max();
    low.x = maxV; high.x = -maxV;
    low.y = maxV; high.y = -maxV;
    low.z = maxV; high.z = -maxV;
    center = glm::vec3(0.);

    for(int i=0; i<mesh.vertices.size(); i++)
    {
        glm::vec3 v=mesh.vertices[i];
        low.x = std::min(low.x, v.x);
        low.y = std::min(low.y, v.y);
        low.z = std::min(low.z, v.z);

        high.x = std::max(high.x, v.x); 
        high.y = std::max(high.y, v.y);
        high.z = std::max(high.z, v.z); 

        center += v;
    }
    center /= mesh.vertices.size();
}

void Mesh::reset_camera()
{
    view_distance = canonical_view_distance*1.3;
    camera_angles = glm::vec3(-M_PI_4/2, M_PI_4, 0);
}


void Mesh::setup_camera()
{
    glm::vec3 low, high, center;
    getBounds(pmesh, low, high, center);

    canonical_view_distance = glm::length(high-low)*1.01;
    min_view_distance = canonical_view_distance/10.0;
    view_distance = canonical_view_distance*1.3;
    max_view_distance = canonical_view_distance*20.0;

    view_focus = center;
}

void Mesh::resize(size_t w, size_t h)
{
    screen_w = w;
    screen_h = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(vfov, 
        1.0*screen_w/screen_h,
        nclip,
        fclip);
}

void Mesh::update_camera()
{
    // setup camera fov and clipping
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    resize(view[2], view[3]);

    // control the camera to look at the mesh
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glm::vec3 cp;
    cp.x = view_focus.x + view_distance*sin(camera_angles.x)*cos(camera_angles.y*.99);
    cp.y = view_focus.y + view_distance*cos(camera_angles.x)*cos(camera_angles.y*.99);
    cp.z = view_focus.z + view_distance*sin(camera_angles.y);

    glm::vec3 up(0,0,1.0);
    gluLookAt(cp.x, cp.y, cp.z, view_focus.x, view_focus.y, view_focus.z, up.x, up.y, up.z);
}

void Mesh::drawFaces()
{
    glEnable(GL_LIGHTING);

    // These guys prevent z fighting / prevents the faces from bleeding into the edge lines and points.
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset( 1.0, 1.0 );

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glColor3f(face_color.x, face_color.y, face_color.z);
    for(int i=0; i<pmesh.polygons.size(); i++)
    {
        std::vector<size_t> idx = pmesh.polygons[i].vertex_indices;
        glBegin(GL_POLYGON);
        for(int j=0; j<idx.size(); j++)
        {
            glm::vec3 v = pmesh.vertices[idx[j]];
            glVertex3fv(&v.x);

            if(smoothShading)
            {
                glm::vec3 n = pmesh.normals[idx[j]];
                glNormal3fv(&n.x);
            }
        }
        glEnd();
    }
    glDisable(GL_LIGHTING);
    glUseProgram(0);
}

void Mesh::render()
{
    evaluateMesh(); 
    update_camera();

    drawFaces();

    if(!shadingMode)
    {
        drawEdges();
        drawVertices();
    }
    glFlush();
}

void Mesh::drawEdges()
{
    glColor3f(edge_color.x, edge_color.y, edge_color.z);
    glLineWidth(edge_width);
    for(int i=0; i<pmesh.polygons.size(); i++)
    {
        std::vector<size_t> idx = pmesh.polygons[i].vertex_indices;
        glBegin(GL_LINE_LOOP);
        for(int j=0; j<idx.size(); j++)
        {
            glm::vec3 v = pmesh.vertices[idx[j]];
            glVertex3fv(&v.x);
        }
        glEnd();
    }
    glEnd();
}

void Mesh::drawVertices()
{
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_POINTS);
    glColor3f(vertex_color.x, vertex_color.y, vertex_color.z);
    glPointSize(vertex_radius);
    for(int i=0; i<pmesh.vertices.size(); i++)
    {
        glm::vec3 v = pmesh.vertices[i];
        glVertex3fv(&v.x);
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);
}

void Mesh::key_event(char key)
{
    switch(key) {
        case ' ':
            reset_camera();
            break;
        case 'w':
        case 'W':
            shadingMode = !shadingMode;
            break;
        case 'a':
        case 'A':
            nBezierPt += 2;
            nBezierPt = glm::min(nBezierPt, 16);
            nBezierPt = glm::max(nBezierPt, 4);
            nBezierPt_changed = true;
            break;
        case 'd':
        case 'D':
            nBezierPt -= 2;
            nBezierPt = glm::min(nBezierPt, 16);
            nBezierPt = glm::max(nBezierPt, 4);
            nBezierPt_changed = true;
            break;
        case 'q':
        case 'Q':
            smoothShading = !smoothShading;
            break;
        default:
            break;
    }
}

void Mesh::cursor_event(float x, float y, unsigned char keys)
{
    // mouse dragged if any mouse button is held down
    if(left_down || middle_down || right_down)
    {
        // rotate the camera
        float dx = (x-mouse_x);
        float dy = (y-mouse_y);

        float& cx = camera_angles.x;
        float& cy = camera_angles.y;

        cx += dx*2*M_PI/screen_w;
        cy += dy*  M_PI/screen_h;

        cx = cx>=0 ? glm::min(cx, float(cx-2*M_PI)) : (cx+2*M_PI);
        cy = glm::min(cy, float(M_PI_2));
        cy = glm::max(cy, -float(M_PI_2));
    }
    mouse_x = x;
    mouse_y = y;
}

void Mesh::scroll_event(float offset_x, float offset_y)
{
    if(offset_y > 0 )
    {
        view_distance -= offset_y*(view_distance/4);
    }
    if(offset_y < 0 )
    {
        view_distance -= offset_y*(view_distance/4);
    }

    // Ensure appropriate bounds
    view_distance = std::max(min_view_distance, view_distance);
    view_distance = std::min(max_view_distance, view_distance);
}

void Mesh::mouse_button_event(int button, int event)
{
    switch(event) {
      case MOUSE_BUTTON_RELEASE:
        {
          switch( button )
          {
            case MOUSE_BUTTON_LEFT:
              left_down = false;
              break;
            case MOUSE_BUTTON_RIGHT:
              right_down = false;
              break;
            case MOUSE_BUTTON_MIDDLE:
              middle_down = false;
              break;
          }
          break;
        }
      case MOUSE_BUTTON_PRESS:
        {
          switch( button )
          {
            case MOUSE_BUTTON_LEFT:
              left_down = true;
              break;
            case MOUSE_BUTTON_RIGHT:
              right_down = true;
              break;
            case MOUSE_BUTTON_MIDDLE:
              middle_down = true;
              break;
          }
          break;
        }
    }
}