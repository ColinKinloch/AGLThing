#pragma once

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

#include <epoxy/gl.h>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec4 position;
  glm::vec3 normal;
  glm::vec4 colour;
};
struct Attribute {
  GLint id;
  std::string name;
};

class Renderer: public Gtk::GLArea {
public:
  Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  Renderer();

  static GLuint createShader(GLenum type, std::string uri);
  static GLuint createProgram(std::vector<GLuint> shaders);

  static void debug_callback(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

protected:
  GLuint* vao;
  GLuint* buffers;
  std::vector<Attribute> attributes;

  Glib::RefPtr< Gdk::GLContext > on_create_context();
  bool on_render(const Glib::RefPtr< Gdk::GLContext >&);
  void on_resize(int width, int height);

  void init_gl();

};
