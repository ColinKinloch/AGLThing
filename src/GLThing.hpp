#include <gtkmm/application.h>
#include <gtkmm/settings.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/aboutdialog.h>

#include <gtkmm/filechooserdialog.h>

#include "Renderer.hpp"

class GLThing: public Gtk::Application {
public:
  static Glib::RefPtr<GLThing> create() {
    return Glib::RefPtr<GLThing>(new GLThing);
  }
protected:
  Gtk::ApplicationWindow* window;
  Renderer* area;
  Gtk::AboutDialog* aboutDialog;

  Glib::RefPtr<Gtk::Settings> uiSettings;

  std::map<guint, glTFb> gltfs;

  void on_startup();

  void open_gltf(Glib::RefPtr<Gio::File> file);

  void show_open();
  void show_about();

  void on_quit();
private:
  GLThing();
};
