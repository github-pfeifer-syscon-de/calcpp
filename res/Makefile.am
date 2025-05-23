## build resources
##   the autoreconf complains about the $(shell) construct, fix see configure.ac  -Wno-portability.
##   But this seems the best way to keep this portable

PKGCONFIG = pkg-config
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schemas gio-2.0)

CALCPP_RESOURCE_XML=calcpp.gresources.xml
gsettings_SCHEMAS=de.pfeifer_syscon.calcpp.gschema.xml
RESOURCE_C=../src/resources.c
GSCHEMA_COMPILED=gschemas.compiled
CALCPP_RESOURCES=$(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies $(CALCPP_RESOURCE_XML))

all:$(RESOURCE_C) $(GSCHEMA_COMPILED)

$(RESOURCE_C): $(CALCPP_RESOURCE_XML) $(CALCPP_RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$(RESOURCE_C) --sourcedir=. --generate-source $(CALCPP_RESOURCE_XML)

# The desktop files
desktopdir = $(datadir)/applications
desktop_DATA=calcpp.desktop

# application icon
icon_basename=calcpp
pixmapdir=$(datadir)/icons/hicolor/64x64/apps
pixmap_DATA=$(icon_basename).png
appicondir=$(datadir)/icons/hicolor/scalable/apps
appicon_DATA=$(icon_basename).svg

# include source for build
EXTRA_DIST = $(gsettings_SCHEMAS) $(CALCPP_RESOURCE_XML) $(CALCPP_RESOURCES) $(desktop_in_files) $(pixmap_DATA) $(appicon_DATA)

@GSETTINGS_RULES@

# compile schema locally to allow run from source (without root permission)
$(GSCHEMA_COMPILED): $(gsettings_SCHEMAS)
	$(GLIB_COMPILE_SCHEMAS) .

clean-local:
	-rm -rf $(RESOURCE_C)
	-rm -rf $(GSCHEMA_COMPILED)
