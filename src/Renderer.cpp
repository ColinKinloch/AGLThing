#include "Renderer.hpp"

#include <epoxy/gl.h>
#include <iostream>

#include <glm/glm.hpp>

using namespace std;

struct Vertex {
  glm::vec3 coord;
  glm::vec3 colour;
};

Renderer::Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::GLArea(cobject) {}
Renderer::Renderer(): Gtk::GLArea() {}

Glib::RefPtr< Gdk::GLContext > Renderer::on_create_context() {
  Glib::RefPtr< Gdk::GLContext > gl;
  try {
    gl = this->get_window()->create_gl_context();

    gl->set_required_version(3, 3);
    gl->set_debug_enabled(true);
    gl->realize();
    gl->make_current();

    int maj = 0;
    int min = 0;
    gl->get_version(maj, min);
    cout<<"Version:"<<maj<<':'<<min<<endl;

    if(epoxy_has_gl_extension ("GL_ARB_debug_output")) {
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback([](
        GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar *message, const void *userParam
      ){
        if(severity <= GL_DEBUG_SEVERITY_MEDIUM) cout<<"GL Error:"<<message<<endl;
      }, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
      0, nullptr, GL_TRUE);
    }

    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClearDepth(1.0);

    GLuint buffer;
    std::vector<Vertex> vertices = {
      {{-.5, .0, .0}, {1, 0, 1}},
      {{.5, .0, .0}, {1, 1, 0}},
      {{.0, 1., .0}, {0, 1, 1}}
    };
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    cout<<"buf:"<<buffer<<endl;
  } catch(Gdk::GLError& err) {
    cerr<<err.what()<<endl;
  }
  return gl;
}
bool Renderer::on_render(const Glib::RefPtr< Gdk::GLContext >& gl) {
  gl->make_current();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  return true;
}
void Renderer::on_resize(int width, int height) {
  this->make_current();
  glViewport(0, 0, width, height);
}
