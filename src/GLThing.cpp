#include <epoxy/gl.h>

#include <iostream>
#include <functional>

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

#include "GLThing-resources.h"

#include "GLThing.hpp"
#include "Renderer.hpp"

using namespace std;

GLThing::GLThing(): Gtk::Application("org.colinkinloch.GLThing") {
  //this->set_resource_base_path("/org/colinkinloch/glthing");
}

void GLThing::on_startup() {
  Gtk::Application::on_startup();

  auto builder = Gtk::Builder::create_from_resource("/org/colinkinloch/glthing/ui/glthing.ui");
  builder->set_application(Glib::RefPtr<Gtk::Application>(this));
  builder->get_widget("main-window", window);
  Renderer* area;
  builder->get_widget_derived("gl-area", area);

  //window = new Gtk::ApplicationWindow(Glib::RefPtr<Gtk::Application>(this));
  //window = new Gtk::ApplicationWindow();
  //window->set_icon_name("preferences-desktop-wallpaper-symbolic");

  this->add_window(*window);

  /*auto area = new Renderer();
  area->set_visible(true);

  window->add(*area);*/
  window->show();
}
