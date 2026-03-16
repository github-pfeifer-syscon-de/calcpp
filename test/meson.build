
incSrcLibTest = include_directories(
                    '../src'
                  , '../srcLib'
                  , '..')   # for ...config.h

calc_test = executable('calc_test'
    , ['calc_test.cpp'
      , '../src/Unit.cpp'
      , '../src/NumDialog.cpp']
    , dependencies: deps
    , include_directories : incSrcLibTest
    , link_with: [expressions_lib])
test('calc_test', calc_test)

lin_test = executable('lin_test'
    , ['lin_test.cpp'
      , '../src/Matrix.cpp'
      , '../src/QuadraticEquation.cpp'
      , '../src/Fraction.cpp'
      ]
    , dependencies: deps
    , include_directories : incSrcLibTest
    , link_with: [expressions_lib])
test('lin_test', lin_test)

