#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>

class GLThing: public Gtk::Application {
public:
  static Glib::RefPtr<GLThing> create() {
    return Glib::RefPtr<GLThing>(new GLThing);
  }
protected:
  Gtk::ApplicationWindow* window;

  void on_startup();
private:
  GLThing();
};
