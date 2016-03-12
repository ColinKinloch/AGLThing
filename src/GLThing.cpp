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
using namespace nlohmann;

GLThing::GLThing(): Gtk::Application("org.colinkinloch.GLThing") {
  set_resource_base_path("/org/colinkinloch/glthing");
}

void GLThing::on_startup() {
  Gtk::Application::on_startup();

  uiSettings = Gtk::Settings::get_default();
  uiSettings->property_gtk_application_prefer_dark_theme() = true;

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

  add_action("open", bind(&GLThing::show_open, this));

  add_action("about", bind(&GLThing::show_about, this));
  add_action("quit", bind(&GLThing::on_quit, this));

  /*auto area = new Renderer();
  area->set_visible(true);

  window->add(*area);*/
  window->show();

  open_gltf(Gio::File::create_for_uri("resource:///org/colinkinloch/glthing/model/duck.glb"));
}

void GLThing::open_gltf(Glib::RefPtr<Gio::File> file) {
  cout<<file->get_uri()<<endl;
  glTFb gltf;
  if(gltfs.count(file->hash())>0) {
    gltf = gltfs[file->hash()];
  } else {
    gltf.file = file;
    auto stream = gltf.file->read();

    std::string type = gltf.file->query_info()->get_content_type();
    if(type == "application/octet-stream") {
      cout<<"glb"<<endl;

      {
        char m;
        for(char c: string("glTF")) {
          stream->read(&m, 1);
          if(c != m) throw "File not glTF";
        }
      }

      {
        uint32_t version;
        stream->read(&version, 4);
        switch(version) {
          case 1:
            break;
          default:
            cerr<<"Formats other than glTF v1.0 are not supported."<<endl;
        }
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

      gltf.scene = json::parse(string(sceneData, sceneLength));

      {
        size_t bodyLength = length - stream->tell();
        uint8_t* body = new uint8_t[bodyLength];
        stream->read(body, bodyLength);
        gltf.buffers.insert({"binary_glTF", vector<uint8_t>(*body, bodyLength)});
      }
    } else if (type == "text/plain" || type == "model/gltf+json") {
      cout<<"json"<<endl;
      char* scene;
      size_t sceneLength;
      gltf.file->load_contents(scene, sceneLength);
      gltf.scene = json::parse(string(scene, sceneLength));
    } else {
      cout<<"Unknown type: "<<type<<endl;
    }

    {
      json b = gltf.scene["buffers"];
      for(json::iterator it = b.begin(); it != b.end(); ++it) {
        if(it.key() == "binary_glTF") continue;
        cout<<it.key()<<':'<<it.value()["uri"]<<endl;
        auto f = gltf.file->get_parent()->resolve_relative_path(it.value()["uri"]);
        uint8_t* body;
        size_t length;
        f->load_contents((char*&)body, length);
        gltf.buffers.insert({it.key(), vector<uint8_t>(*body, length)});
      }
    }
    gltfs.insert({file->hash(), gltf});
  }
  area->set_gltf(gltf);
}

void GLThing::show_open() {
  Gtk::FileChooserDialog dialog(*window, "Open GLTF file.", Gtk::FILE_CHOOSER_ACTION_OPEN);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);

  int res = dialog.run();
  Glib::RefPtr<Gio::File> file;
  switch(res) {
    case Gtk::RESPONSE_OK:
      open_gltf(dialog.get_file());
      break;
    case Gtk::RESPONSE_CANCEL:
      break;
    default:
      cerr<<"Unexpected dialog response!"<<endl;
  }
}

void GLThing::show_about() {
  aboutDialog->show();
}

void GLThing::on_quit() {
  quit();
}
