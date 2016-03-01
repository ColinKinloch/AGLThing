#pragma once

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

class Renderer: public Gtk::GLArea {
public:
  Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  Renderer();
protected:
  Glib::RefPtr< Gdk::GLContext > on_create_context();
  bool on_render(const Glib::RefPtr< Gdk::GLContext >&);
  void on_resize(int width, int height);

};
