
sources = files(
     'CalcppApp.cpp'
    , 'CalcppWin.cpp'
    , 'EvalContext.cpp'
    , 'OutputForm.cpp'
    , 'AngleUnit.cpp'
    , 'CalcTextView.cpp'
    , 'CalcTreeView.cpp'
    , 'CharDialog.cpp'
    , 'NumBaseDialog.cpp'
    , 'QuadDialog.cpp'
    , 'GaussDialog.cpp'
    , 'DateDialog.cpp'
    , 'UnitDialog.cpp'
    , 'PlotDialog.cpp'
    , 'PrefDialog.cpp'
    , 'ColorDialog.cpp'
    , 'FractDialog.cpp'
    , 'PrimeDialog.cpp'
    )

# create libraries so these can be used on test as well
lin_sources = files(
    'Matrix.cpp'
    , 'QuadraticEquation.cpp'
    , 'Fraction.cpp'
    , 'Primes.cpp'
)
linear_lib = static_library('linear_lib.a'
     , lin_sources
     , dependencies : deps
     , include_directories : incSrcLib)

calc_sources = files(
    'Unit.cpp'
    , 'NumDialog.cpp'
)
calc_lib = static_library('calc_lib.a'
    , calc_sources
    , dependencies : deps
    , include_directories : incSrcLib)
