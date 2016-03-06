#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/aboutdialog.h>

class GLThing: public Gtk::Application {
public:
  static Glib::RefPtr<GLThing> create() {
    return Glib::RefPtr<GLThing>(new GLThing);
  }
protected:
  Gtk::ApplicationWindow* window;
  Gtk::AboutDialog* aboutDialog;

  void on_startup();

  void show_about();

  void on_quit();
private:
  GLThing();
};
