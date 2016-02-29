#include "config.h"

#include "GLThing.hpp"

int main(int argc, char **argv) {
  Glib::RefPtr<GLThing> app = GLThing::create();
  app->run(argc, argv);
  return EXIT_SUCCESS;
}
