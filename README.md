# calcpp
A desktop calculator

![calcpp](calcpp.png "calcpp")

A simple lightweight desktop calculator
(this time based on C++11 to try some it's features.
As far i coud see they are useful,
and as i'am just getting use to it not try to over featurize).
Write down your calculation e.g.:
<pre>
3 + 4 * 5
</pre>
and press [control] and [enter] to evaluate it.

Adjustable output format: decimal, scientific, hexadecimal and octal.
Input for decimal (with local support) and hexadecimal (with 0x...) numbers.
Octal numbers e.g. 0123 are only parsed as such when the output format is octal.
Adjustable angle unit: radian, degree, gon.

The following operators are supported:
<pre>
+,- add, subtract
*,/,% multiply, divide, modulo
^ or ** power
|,&,!,<<,>> logical or, and, xor, shift left, shift right
</pre>

The following functions are supported:
<pre>
* sqrt, square root
* cbrt, cubic root
* log,ln, logarithm on base e
* log2, logarithm on base 2
* log10,lg, logarithm on base 10
* exp, e ^ x
* sin, sinus
* cos, cosinus
* tan, tangens
* asin, arcus sinus
* acos, arcus cosinus
* atan, arcus tangens
* abs, absolut value
* fac, factorial (usually writen as n!)
</pre>

Usage of variables e.g.
<pre>
a = 3 + 4 * 5
b = a + 7
</pre>
the variable table can be edited so its easy to change a value by clicking it.
Or rename a variable by clicking on its name.
To remove a variable you can change its name to an empty string.

Unicode support for variable/constant names
e.g. π (if you read this without unicode support small greek letter pi)

Automatic save/restore of edit buffer and window position.

A character list allows picking symbols by choosing a unicode page and
click on the character.

A simple calendar view is included.

A simple option to convert units is included.
Sorry i'm still not too deep into non SI units,
please check your results, suggestions are welcome.
In INSTALL_DIR/share/calcpp/unit.js can be found if
it is copied to ~/.local/share/calcpp/unit.js
you can modify it or add other units.

As a prerequisite genericImg is required, you have to install
it first.

To build on (Lin)ux use :
(the "out of tree" compile method is preferred as some files are optionally
loaded from build-dirs and too many variations result in unclear errors):
<pre>
meson setup build -Dprefix=/usr
cd build
meson compile
</pre>
For a first test `./calcpp` should be sufficient.
If the application was once installed there is the likelihood, 
of conflicts between the installed configuration schema
and a new version, so for further testing use `meson install` 
to avoid this.

Build for windows was integrated at least for msys2 use (choose your shell consistently):
<pre>
meson setup build -Dprefix=${MINGW_PREFIX}
cd build
meson compile
</pre>

### i18n

To update i18n data use:
<pre>
meson compile calcpp-pot
meson compile calcpp-update-po
</pre>