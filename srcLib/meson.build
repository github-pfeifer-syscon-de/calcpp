

configure_file(output : meson.project_name() + '_config.h'
               , configuration : conf)

lib_sources = files(
  'NumberFormat.cpp'
  ,'Token.cpp'
  ,'Function.cpp'
  ,'BaseEval.cpp'
  ,'Syntax.cpp')

expressions_lib = static_library('expressions.a'
    , lib_sources
    , dependencies: deps
    , gnu_symbol_visibility : 'default')         # visibility was hidden, but selected visible for compatibility
