
incSrcLibTest = include_directories(
                    '../src'
                  , '../srcLib'
                  , '..')   # for ...config.h

calc_test_src = files('calc_test.cpp')
calc_test = executable('calc_test'
    , calc_test_src
    , dependencies: deps
    , include_directories : incSrcLibTest
    , link_with: [expressions_lib, calc_lib])
test('calc_test', calc_test)

lin_test_src = files(
      'lin_test.cpp'
)
lin_test = executable('lin_test'
    , lin_test_src
    , dependencies: deps
    , include_directories : incSrcLibTest
    , link_with: [linear_lib])
test('lin_test', lin_test)

prime_test_src = files(
     'PrimeTest.cpp'
)
prime_test = executable('prime_test'
    , prime_test_src
    , include_directories : incSrcLibTest
    , link_with: [linear_lib]
)
test('prime_test', prime_test)