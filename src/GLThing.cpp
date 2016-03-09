#include <epoxy/gl.h>

#include <iostream>
#include <functional>

#include <glibmm/main.h>

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

#include "json.hpp"

#include "GLThing-resources.h"

#include "GLThing.hpp"
#include "Renderer.hpp"

#include <cstdint>

using namespace std;
using namespace nlohmann;

struct glTFb {
  json scene;
  map<string, uint8_t*> buffers;
};

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

  builder->get_widget_derived("gl-area", area);

  Glib::signal_timeout().connect([this]() -> bool {
    area->queue_render();
    return true;
  }, (double)1000. / 60.);

  window->set_icon_name("application-x-executable");

  this->add_window(*window);

  add_action("about", bind(&GLThing::show_about, this));
  add_action("quit", bind(&GLThing::on_quit, this));

  /*auto area = new Renderer();
  area->set_visible(true);

  window->add(*area);*/
  window->show();

  auto stream = Gio::File::create_for_uri("resource:///org/colinkinloch/glthing/model/duck.glb")->read();

  char* magic = new char[4];
  uint8_t* body;
  stream->read(magic, 4);
  if(string(magic, 4) != "glTF") throw "File not glTF";

  uint32_t version;
  stream->read(&version, 4);
  switch(version) {
    case 1:
      break;
    default:
      cerr<<"Formats other than glTF v1.0 are not supported."<<endl;
  }

  uint32_t length;
  uint32_t sceneLength;
  stream->read(&length, 4);
  stream->read(&sceneLength, 4);

  uint32_t sceneFormat;
  stream->read(&sceneFormat, 4);
  switch(sceneFormat) {
    case 0:
      break;
    default:
      throw "Only JSON scenes are supported";
  }
  char* sceneData = new char [sceneLength];
  stream->read(sceneData, sceneLength);

  glTFb gltf;
  gltf.scene = json::parse(sceneData);

  size_t bodyLength = length - stream->tell();
  body = new uint8_t[bodyLength];
  stream->read(body, bodyLength);

  gltf.buffers.insert({"binary_glTF", body});
  {
    json b = gltf.scene["buffers"];
    for(json::iterator it = b.begin(); it != b.end(); ++it) {
      if(it.key() == "binary_glTF") continue;
      cout<<it.key()<<endl;
    }
  }
  {
    json bv = gltf.scene["bufferViews"];
    for(json::iterator it = bv.begin(); it != bv.end(); ++it) {
      cout<<it.key()<<endl;
    }
  }
}

void GLThing::show_about() {
  aboutDialog->show();
}

void GLThing::on_quit() {
  quit();
}
