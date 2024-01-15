#include "GL/glew.h"
#include "viewer.h"

#include <stdio.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>


using namespace std;
using namespace chrono;

#define DEFAULT_W 960
#define DEFAULT_H 640

// framecount & related timeers
int Viewer::framecount;
time_point<system_clock> Viewer::sys_last;
time_point<system_clock> Viewer::sys_curr;

// draw toggles
bool Viewer::showInfo = true;

// window properties
GLFWwindow* Viewer::window;
size_t Viewer::buffer_w;
size_t Viewer::buffer_h;

// user space renderer
Renderer* Viewer::renderer;

Viewer::Viewer() {
}

Viewer::~Viewer() {

  glfwDestroyWindow(window);
  glfwTerminate();

  // free resources
  delete renderer;
}


void Viewer::init() {

  // initialize glfw
  glfwSetErrorCallback( err_callback );
  if( !glfwInit() ) {
    std::cerr<<"Error: could not initialize GLFW!"<<std::endl;
    exit( 1 );
  }

  // create window
  string title = renderer ? renderer->name() : " ";
  window = glfwCreateWindow( DEFAULT_W, DEFAULT_H, title.c_str(), NULL, NULL );
  if (!window) {
    std::cerr<<"Error: could not create window!"<<std::endl;
    glfwTerminate();
    exit( 1 );
  }

  // set context
  glfwMakeContextCurrent( window );
  glfwSwapInterval(1);

  // framebuffer event callbacks
  glfwSetFramebufferSizeCallback( window, resize_callback );

  // key event callbacks
  glfwSetKeyCallback( window, key_callback );

  // cursor event callbacks
  glfwSetCursorPosCallback( window, cursor_callback );

  // wheel event callbacks
  glfwSetScrollCallback(window, scroll_callback);

  // mouse button callbacks
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // initialize glew
  if (glewInit() != GLEW_OK) {
    std::cerr<<"Error: could not initialize GLEW!"<<std::endl;
    glfwTerminate();
    exit( 1 );
  }

  // enable alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // resize components to current window size, get DPI
  glfwGetFramebufferSize(window, (int*) &buffer_w, (int*) &buffer_h );

  // initialize renderer if already set
  if (renderer){
    renderer->init();
  }
  // resize elements to current size
  resize_callback(window, buffer_w, buffer_h);
}

void Viewer::start() {
  // start timer
  sys_last = system_clock::now();

  // run update loop
  while( !glfwWindowShouldClose( window ) ) {
    update();
  }
}

void Viewer::set_renderer(Renderer *renderer) {
  this->renderer = renderer;
}

void Viewer::update() {

  // clear frame
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // run user renderer
  if (renderer) {
    renderer->render();
  }

  // draw info
  if( showInfo ) {
    drawInfo();
  }

  // swap buffers
  glfwSwapBuffers(window);

  // poll events
  glfwPollEvents();
}


void Viewer::drawInfo() {

  // compute timers - fps is update every second
  sys_curr = system_clock::now();
  double elapsed = ((duration<double>) (sys_curr - sys_last)).count();
  if (elapsed >= 1.0f) {

    // reset timer and counter
    framecount = 0;
    sys_last = sys_curr;

  } else {

    // increment framecount
    framecount++;

  }

  // udpate renderer OSD
  // This is done on every update and it shouldn't be!
  // The viewer should only update when the renderer needs to
  // update the info text.
  if (renderer) {
    string renderer_info = renderer->info();
  } else {
    string renderer_info = "No input renderer";
  }
}

void Viewer::err_callback( int error, const char* description ) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

void Viewer::key_callback( GLFWwindow* window,
                           int key, int scancode, int action, int mods ) {
  if( action == GLFW_PRESS ) {
    if( key == GLFW_KEY_ESCAPE ) {
      glfwSetWindowShouldClose( window, true );
    } else if( key == GLFW_KEY_GRAVE_ACCENT ) {
      showInfo = !showInfo;
    } else {
      renderer->key_event(key);
    }
  }
}

void Viewer::resize_callback( GLFWwindow* window, int width, int height ) {
  // get framebuffer size
  int w, h;
  glfwGetFramebufferSize(window, &w, &h );

  // update buffer size
  buffer_w = w; buffer_h = h;
  glViewport( 0, 0, buffer_w, buffer_h );

  // resize render if there is a user space renderer
  // use this line to work on mac retina
  //if (renderer) renderer->resize( buffer_w/2, buffer_h/2 );

  // use this line to work on normal screen
  if (renderer) renderer->resize( buffer_w, buffer_h);
}


void Viewer::cursor_callback( GLFWwindow* window, double xpos, double ypos ) {
  // get keydown bitmask
  unsigned char keys;
  keys  |= (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS);
  keys <<= 1;
  keys  |= (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
  keys <<= 1;
  keys  |= (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS);

  bool a = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS;
  bool b = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
  bool c = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS;

  float cursor_x = xpos;
  float cursor_y = ypos;
  renderer->cursor_event(cursor_x, cursor_y, keys);
}

void Viewer::scroll_callback( GLFWwindow* window, double xoffset, double yoffset) {

  renderer->scroll_event(xoffset, yoffset);

}

void Viewer::mouse_button_callback( GLFWwindow* window, int button, int action, int mods ) {

  renderer->mouse_button_event( button, action );

}
