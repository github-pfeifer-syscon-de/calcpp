
#
# see https://mesonbuild.com/Gnome-module.html
gnome = import('gnome')

app_resources = gnome.compile_resources(
    'resources', meson.project_name() + '.gresources.xml'
    , source_dir: meson.current_source_dir()
)

# icons + desktop
dataDir = join_paths(get_option('prefix'), get_option('datadir'))
iconDir = join_paths(dataDir, 'icons/hicolor')
install_data(
      meson.project_name() + '.png'
    , install_dir: join_paths(iconDir, '64x64/apps'))
install_data(
      meson.project_name() + '.svg'
    , install_dir: join_paths(iconDir, 'scalable/apps'))
install_data(
      meson.project_name() + '.desktop'
    , install_dir: join_paths(dataDir, 'applications'))

# Schema
schemaSrc = 'de.pfeifer_syscon.calcpp.gschema.xml'
install_data(
     schemaSrc 
    , install_dir: join_paths(dataDir, 'glib-2.0/schemas'))
gnome.post_install( glib_compile_schemas: true )
# ask pkg-config to find compile schema
pkg_config = find_program('pkg-config', required: true)
r = run_command( pkg_config, '--variable=glib_compile_schemas', 'gio-2.0', capture: true, check: true)
glib_compile_schemas = find_program(r.stdout().strip(), required: true)
# to allow testrun & checking schema compile build it
#   this is build too often but better than only on install...
#   violates the out of tree convention, but to reduce the variations when searching.
custom_target(
    build_by_default   : true
    , command          : [glib_compile_schemas, '--strict', meson.current_source_dir()]
    , input            : schemaSrc
    , output           : 'geschemas.compiled'
    )

# Doc-files
docDir= join_paths(dataDir, 'doc', meson.project_name())
install_data(
      ['../README', '../README_de']
    , install_dir: docDir)
install_data(
      ['unit.js']
    , install_dir: join_paths(dataDir, meson.project_name()))
