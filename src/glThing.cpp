#include <epoxy/gl.h>
#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/glarea.h>

int main(int argc, char **argv) {
  auto app = Gtk::Application::create("GL Thing");
  auto win = new Gtk::ApplicationWindow(app);
  auto gl = new Gtk::GLArea();
  win->add(*gl);
  gl->show();
  app->run(*win, argc, argv);
  return EXIT_SUCCESS;
}
