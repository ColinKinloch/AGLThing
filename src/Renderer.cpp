#include "Renderer.hpp"

#include <iostream>
#include <chrono>

#include <giomm/file.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace std;

GLuint Renderer::createShader(GLenum type, string uri) {
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
GLuint Renderer::createProgram(vector<GLuint> shaders) {
  GLuint program = glCreateProgram();
  for(auto it = shaders.begin(); it != shaders.end(); ++it) glAttachShader(program, *it);
  glLinkProgram(program);
  return program;
}

Renderer::Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::GLArea(cobject) {
  startTime = chrono::system_clock::now();
}
Renderer::Renderer(): Gtk::GLArea() {
  startTime = chrono::system_clock::now();
}

Glib::RefPtr< Gdk::GLContext > Renderer::on_create_context() {
  Glib::RefPtr< Gdk::GLContext > gl;

  set_has_alpha(true);
  set_has_depth_buffer(true);

  try {
    gl = get_window()->create_gl_context();

    int maj, min;
    get_required_version(maj, min);
    gl->set_required_version (maj, min);
    gl->realize();
  } catch(Gdk::GLError& err) {
    cerr<<err.what()<<endl;
  }
  return gl;
}

void Renderer::on_realize() {
  set_required_version(3, 3);

  GLArea::on_realize();
  make_current();

  if(epoxy_has_gl_extension ("GL_ARB_debug_output")) {
    cout<<"Enabling Synchronous GL Debugging"<<endl;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
    0, nullptr, GL_TRUE);
  }

  glEnable(GL_ALPHA_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);

  vertexShader = createShader(GL_VERTEX_SHADER, "resource:///org/colinkinloch/glthing/shader/main.glslv");
  fragmentShader = createShader(GL_FRAGMENT_SHADER, "resource:///org/colinkinloch/glthing/shader/main.glslf");

  program = createProgram({vertexShader, fragmentShader});

  glUseProgram(program);

  attributes = {
    { "position", { -1, "position" } },
    { "normal", { -1, "normal" } },
    { "colour", { -1, "colour" } }
  };

  uniforms = {
    { "MODELVIEWINVERSETRANSPOSE", { -1, "normalMatrix" } },
    { "MODELVIEWPROJECTION", { -1, "modelViewProjectionMatrix" } }
  };

  for(auto&& attribute: attributes)
    attribute.second.id = glGetAttribLocation(program, attribute.second.name.c_str());
  for(auto&& uniform: uniforms)
    uniform.second.id = glGetUniformLocation(program, uniform.second.name.c_str());

  position = glm::vec3(0, 0, 0);
  orientation = glm::quat(0, 0, 0, 1);
  rotation = glm::vec3();

  unsigned char nVAO = 1;
  vao = new GLuint[nVAO];
  glGenVertexArrays(nVAO, vao);
  glBindVertexArray(vao[0]);

  unsigned char nBuffers = 2;
  buffers = new GLuint[nBuffers];
  vector<Vertex> vertices = {
    {{-1, -1, 0, 1}, {0, 0, 1}, {1, 0, 1, 1}},
    {{1, -1, 0, 1}, {0, 0, 1}, {1, 1, 0, 1}},
    {{0, 1, 0, 1}, {0, 0, 1}, {0, 1, 1, 1}}
  };
  vector<unsigned char> indices = {
    0, 1, 2
  };
  glGenBuffers(nBuffers, buffers);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned char), indices.data(), GL_STATIC_DRAW);

  for(auto attribute: attributes) glEnableVertexAttribArray(attribute.second.id);
  size_t size = sizeof(Vertex);
  glVertexAttribPointer(attributes["position"].id, 4, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, position));
  glVertexAttribPointer(attributes["normal"].id, 3, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, normal));
  glVertexAttribPointer(attributes["colour"].id, 4, GL_FLOAT, GL_FALSE, size, (void*) offsetof(struct Vertex, colour));
  for(auto attribute: attributes) glDisableVertexAttribArray(attribute.second.id);
}

void Renderer::on_unrealize() {
  cout<<"CleanGL"<<endl;
  make_current();
  glDeleteBuffers(2, buffers);
  glDeleteProgram(program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  Gtk::GLArea::on_unrealize();
}

bool Renderer::on_render(const Glib::RefPtr< Gdk::GLContext >& gl) {
  gl->make_current();

  chrono::system_clock::time_point currentTime = chrono::system_clock::now();
  chrono::system_clock::duration time = currentTime - previousTime;
  short t = chrono::duration_cast<chrono::milliseconds>(time).count();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  rotation.x += 0.004 * t;
  rotation.y += 0.00186 * t;
  orientation = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
  orientation = glm::rotate(orientation, rotation.y, glm::vec3(0, 1, 0));
  orientation = glm::rotate(orientation, rotation.z, glm::vec3(0, 0, 1));

  modelViewMatrix = glm::mat4_cast(orientation);
  modelViewMatrix = glm::translate(modelViewMatrix, position);
  modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(1, 1, 1));

  normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
  modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

  glUniformMatrix3fv(uniforms["MODELVIEWINVERSETRANSPOSE"].id, 1, GL_FALSE, glm::value_ptr(normalMatrix));
  glUniformMatrix4fv(uniforms["MODELVIEWPROJECTION"].id, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

  glBindVertexArray(vao[0]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

  for(auto attribute: attributes) glEnableVertexAttribArray(attribute.second.id);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, nullptr);
  for(auto attribute: attributes) glDisableVertexAttribArray(attribute.second.id);

  previousTime = currentTime;

  return true;
}
void Renderer::on_resize(int width, int height) {
  make_current();
  glViewport(0, 0, width, height);
  float s = 3;
  float r = (float)width / height;
  float w = s * r / 2;
  float h = s / 2;
  projectionMatrix = glm::ortho(-w, w, h, -h, -10.f, 10.f);
}
void Renderer::debug_callback(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
  {
    unsigned char l = 0;
    Glib::ustring sev;
    switch (severity) {
      case GL_DEBUG_SEVERITY_HIGH:
        l = 0;
        sev = "\x1b[31m☷";
        break;
      case GL_DEBUG_SEVERITY_MEDIUM:
        l = 1;
        sev = "\x1b[33m☶";
        break;
      case GL_DEBUG_SEVERITY_LOW:
        l = 2;
        sev = "\x1b[32m☵";
        break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        l = 3;
        sev = "\x1b[34m♨";
        break;
      default:
        l = 0;
        sev = "\x1b[36m🕴";
    }
    sev += "\x1b[0m";
    //Discard non issues
    if(l > 2) return;
    string s;
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
    string t;
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
    cout<<sev<<" GL "<<s<<' '<<t<<": "<<message<<endl;
  }
