#include "Renderer.hpp"

#include <epoxy/gl.h>
#include <giomm/file.h>
#include <iostream>

using namespace std;

GLuint Renderer::createShader(GLenum type, std::string uri) {
  GLuint shader = glCreateShader(type);

  char* shaderSource;
  size_t shaderSourceSize;
  Gio::File::create_for_uri(uri)->load_contents(shaderSource, shaderSourceSize);

  const GLchar* shadStr[] = { shaderSource };
  const GLint shadStrSize[] = { shaderSourceSize };
  glShaderSource(shader, 1, shadStr, shadStrSize);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    GLchar errorLog[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
    cout<<errorLog<<endl;
    glDeleteShader(shader);
  }

  return shader;
}
GLuint Renderer::createProgram(std::vector<GLuint> shaders) {
  GLuint program = glCreateProgram();
  for(auto it = shaders.begin(); it != shaders.end(); ++it) glAttachShader(program, *it);
  glLinkProgram(program);
  return program;
}

Renderer::Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::GLArea(cobject) {}
Renderer::Renderer(): Gtk::GLArea() {}

void Renderer::init_gl() {
  glClearColor(0.0, 0.0, 0.2, 1.0);
  glClearDepth(1.0);

  GLuint vertexShader = createShader(GL_VERTEX_SHADER, "resource:///org/colinkinloch/glthing/shader/main.glslv");
  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, "resource:///org/colinkinloch/glthing/shader/main.glslf");

  GLuint program = createProgram({vertexShader, fragmentShader});

  glUseProgram(program);

  unsigned char nVAO = 1;
  vao = new GLuint[nVAO];
  glGenVertexArrays(nVAO, vao);
  glBindVertexArray(vao[0]);

  attributes = {
    { -1, "position" },
    { -1, "normal" },
    { -1, "colour" }
  };

  for(auto it = attributes.begin(); it != attributes.end(); ++it) it->id = glGetAttribLocation(program, it->name.c_str());

  unsigned char nBuffers = 2;
  buffers = new GLuint[nBuffers];
  std::vector<Vertex> vertices = {
    {{-.5, 0, 0, 1}, {1, 0, 1}, {1, 0, 1, 1}},
    {{.5, 0, 0, 1}, {1, 1, 0}, {1, 1, 0, 1}},
    {{0, 1, 0, 1}, {0, 1, 1}, {0, 1, 1, 1}}
  };
  std::vector<unsigned char> indices = {
    1, 2, 3
  };
  glGenBuffers(nBuffers, buffers);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned char), indices.data(), GL_STATIC_DRAW);
}

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
      glDebugMessageCallback(debug_callback, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
      0, nullptr, GL_TRUE);
    }

    init_gl();
  } catch(Gdk::GLError& err) {
    cerr<<err.what()<<endl;
  }
  return gl;
}
bool Renderer::on_render(const Glib::RefPtr< Gdk::GLContext >& gl) {
  gl->make_current();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

  size_t size = sizeof(Vertex);
  glBindVertexArray(vao[0]);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, normal));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, colour));

  for(auto it = attributes.begin(); it != attributes.end(); ++it) glEnableVertexAttribArray(it->id);
  glDrawElements(GL_TRIANGLES, 1, GL_UNSIGNED_BYTE, nullptr);
  for(auto it = attributes.begin(); it != attributes.end(); ++it) glDisableVertexAttribArray(it->id);

  return true;
}
void Renderer::on_resize(int width, int height) {
  this->make_current();
  glViewport(0, 0, width, height);
}
void Renderer::debug_callback(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
  {
    unsigned char l = 0;
    switch (severity) {
      case GL_DEBUG_SEVERITY_HIGH:
        l = 0;
        break;
      case GL_DEBUG_SEVERITY_MEDIUM:
        l = 1;
        break;
      case GL_DEBUG_SEVERITY_LOW:
        l = 2;
        break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        l = 3;
        break;
      default:
        l = 0;
    }
    //Discard non issues
    if(l > 2) return;
    std::string s;
    switch(source) {
      case GL_DEBUG_SOURCE_API:
        s = "API";
        break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        s = "Window System";
        break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        s = "Shader Compiler";
        break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        s = "Third Party";
        break;
      case GL_DEBUG_SOURCE_APPLICATION:
        s = "Application";
        break;
      case GL_DEBUG_SOURCE_OTHER:
        s = "Other";
        break;
      default:
        s = "Unknown";
    }
    std::string t;
    switch(type) {
      case GL_DEBUG_TYPE_ERROR:
        t = "Error";
        break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        t = "Deprecated Behavior";
        break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        t = "Undefined Behavior";
        break;
      case GL_DEBUG_TYPE_PORTABILITY:
        t = "Portability";
        break;
      case GL_DEBUG_TYPE_PERFORMANCE:
        t = "Performance";
        break;
      case GL_DEBUG_TYPE_MARKER:
        t = "Marker";
        break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
        t = "Push Group";
        break;
      case GL_DEBUG_TYPE_POP_GROUP:
        t = "Pop Group";
        break;
      case GL_DEBUG_TYPE_OTHER:
        t = "Other";
        break;
      default:
        t = "Unknown";
    }
    cout<<"GL "<<s<<' '<<t<<": "<<message<<endl;
  }
