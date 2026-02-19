

# also make config available here
configure_file (output : meson.project_name() + '_config.h'
    , configuration : conf)
incDirs = include_directories('../srcLib','../src')

calc_test = executable('calc_test'
    , 'calc_test.cpp', '../src/Unit.cpp', '../src/NumDialog.cpp'
    , dependencies: deps
    , include_directories : incDirs
    , link_with: [expressions_lib ])
test('calc_test', calc_test)

lin_test = executable('lin_test'
    , 'lin_test.cpp', '../src/Matrix.cpp', '../src/QuadraticEquation.cpp'
    , dependencies: deps
    , include_directories : incDirs
    , link_with: [expressions_lib ])
test('lin_test', lin_test)

