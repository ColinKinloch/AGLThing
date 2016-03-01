bin_PROGRAMS += glThing

glThing_resource_deps = \
	$(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies \
		--sourcedir $(srcdir)/src/res \
		--sourcedir $(builddir)/src/res \
		$(builddir)/src/res/glthing.gresource.xml)

glThing_resource_c_deps = \
	$(builddir)/src/res/glthing.gresource.xml \
	$(glThing_resource_deps)

src/GLThing-resources.h:
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/res" \
		--sourcedir="$(builddir)/src/res" \
		--generate-header "$(builddir)/src/res/glthing.gresource.xml"

src/GLThing-resources.c: $(glThing_resource_c_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) \
		--target="$@" \
		--sourcedir="$(srcdir)/src/res" \
		--sourcedir="$(builddir)/src/res" \
		--generate-source "$(builddir)/src/res/glthing.gresource.xml"

glThing_LDADD = \
	$(GTK_LIBS) \
	$(GL_LIBS)
glThing_CFLAGS = \
	$(GTK_CFLAGS) \
	$(GL_CFLAGS)
glThing_CPPFLAGS = \
	$(glThing_CFLAGS) \
	$(GTK_CPPFLAGS) \
	$(GL_CPPFLAGS)
glThing_SOURCES = \
	src/main.cpp \
	src/GLThing.cpp \
	src/GLThing.hpp \
	src/Renderer.cpp \
	src/Renderer.hpp \
	src/GLThing-resources.c \
	src/GLThing-resources.h

BUILT_SOURCES += \
	src/GLThing-resources.c \
	src/GLThing-resources.h
