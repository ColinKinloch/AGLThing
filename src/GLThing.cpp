#include <epoxy/gl.h>

#include <iostream>
#include <functional>

#include <glibmm/main.h>

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

#include "GLThing-resources.h"

#include "GLThing.hpp"
#include "Renderer.hpp"

using namespace std;

GLThing::GLThing(): Gtk::Application("org.colinkinloch.GLThing") {
  set_resource_base_path("/org/colinkinloch/glthing");
}

void GLThing::on_startup() {
  Gtk::Application::on_startup();

  auto builder = Gtk::Builder::create();
  builder->set_application(Glib::RefPtr<Gtk::Application>(this));
  builder->add_from_resource("/org/colinkinloch/glthing/ui/main.ui");
  builder->add_from_resource("/org/colinkinloch/glthing/ui/about.ui");
  builder->get_widget("main-window", window);
  builder->get_widget("about-dialog", aboutDialog);

  aboutDialog->set_transient_for(*window);

  Renderer* area;
  builder->get_widget_derived("gl-area", area);

  Glib::signal_timeout().connect([area]() -> bool {
    area->queue_render();
    return true;
  }, (double)1000. / 60.);
  //window = new Gtk::ApplicationWindow(Glib::RefPtr<Gtk::Application>(this));
  //window = new Gtk::ApplicationWindow();
  window->set_icon_name("application-x-executable");

  this->add_window(*window);

  add_action("about", bind(&GLThing::show_about, this));
  add_action("quit", bind(&GLThing::on_quit, this));

  /*auto area = new Renderer();
  area->set_visible(true);

  window->add(*area);*/
  window->show();
}

void GLThing::show_about() {
  aboutDialog->show();
}

void GLThing::on_quit() {
  quit();
}
