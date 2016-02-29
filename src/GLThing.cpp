#include <epoxy/gl.h>

#include <iostream>
#include <functional>

#include <gtkmm/glarea.h>

#include "GLThing.hpp"

using namespace std;

GLThing::GLThing(): Gtk::Application("org.colinkinloch.GLThing") {}

void GLThing::on_startup() {
  Gtk::Application::on_startup();
  window = new Gtk::ApplicationWindow(Glib::RefPtr<Gtk::Application>(this));

  auto area = new Gtk::GLArea();
  area->set_visible(true);

  /*area->signal_create_context().connect([area]() -> Glib::RefPtr< Gdk::GLContext > {
    Glib::RefPtr< Gdk::GLContext > gl = area->get_context();
    gl->make_current();
    //area->make_current();
    glClearColor(1.0, 1.0, 0.0, 1.0);
    return gl;
  });*/

  area->signal_resize().connect([area](int width, int height){
    area->make_current();
    glViewport(0, 0, width, height);
  });

  area->signal_render().connect([](const Glib::RefPtr< Gdk::GLContext >& gl) -> bool {
    gl->make_current();
    glClear(GL_COLOR_BUFFER_BIT);
    return true;
  });

  window->add(*area);
  window->show();

  area->make_current();
  glClearColor(0.0, 0.0, 0.2, 1.0);
  glClearDepth(1.0);
}
