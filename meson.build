project('calcpp', 'c', 'cpp'
    , default_options: ['c_std=c11'
                       , 'cpp_std=c++20'
                       , 'buildtype=debugoptimized'
                       , 'warning_level=3']
    , license : 'GPL-3.0'
    , meson_version: '>=1.3.0'
    , version: '0.1')

fs = import('fs')

pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), meson.project_name())
pkglocaledir = join_paths(get_option('prefix'), get_option('datadir'), 'locale')
pkgsrcdir = fs.relative_to( meson.global_source_root(), meson.current_build_dir()) + '/src'
add_project_arguments('-DPACKAGE_DATA_DIR="' + pkgdatadir + '"', language: ['cpp','c'])
add_project_arguments('-DPACKAGE="' + meson.project_name() + '"', language: ['cpp','c'])
# this is to recreate the conventions from autotools
add_project_arguments('-DPACKAGE_SRC_DIR="' + pkgsrcdir + '"', language: ['cpp','c'])
add_project_arguments('-DGETTEXT_PACKAGE="' + meson.project_name() + '"', language: ['c'])
add_project_arguments('-DPACKAGE_LOCALE_DIR="' + pkglocaledir + '"', language: ['cpp','c'])


disable_flags = [   # limit the compiler noisiness
    '-Wno-unused-parameter'
    ,'-Wno-overlength-strings'
]
cc = meson.get_compiler('c')
foreach cflag : disable_flags
    if cc.has_argument(cflag)
        add_project_arguments(cflag, language: ['c', 'cpp'])
    endif
endforeach

conf = configuration_data()
conf.set_quoted('CALCPP_VERSION', meson.project_version())     # Surround the version in quotes to make it a C string
conf_file = configure_file(output : meson.project_name() + '_config.h'
               , configuration : conf)

incSrcLib = include_directories('srcLib'
                               , fs.parent(conf_file))

unistring_deps    = declare_dependency(dependencies : cc.find_library('unistring', required: true))

gtkmm3_deps     = dependency('gtkmm-3.0')
glibmm2_deps    = dependency('glibmm-2.4 giomm-2.4')
thread_deps     = dependency('threads')
genericimg_deps = dependency('genericimg', version :'>= 0.4.8')
deps = [gtkmm3_deps
    , glibmm2_deps
    , thread_deps
    , genericimg_deps
    , unistring_deps
    ]
subdir('po')
subdir('srcLib')
subdir('src')
subdir('test')
subdir('res')
sources += app_resources
sources += win_res_file

project_targt = executable(meson.project_name()
    , sources
    , dependencies: deps
    , install: true
    , include_directories : incSrcLib
    , link_with: expressions_lib
    , win_subsystem: 'windows'  # disable this to see console output for windows
    )
