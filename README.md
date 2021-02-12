# SoDa::Command a simple command line / option parser

 To get to doxygen generated documentation that is more detailed 
 than the following summary, go [here](https://kb1vc.github.io/SoDaCommand/)

 SoDa::Command is a class that allows the programmer to specify
 command line options (like --help, --out, --enable-deep-fry --set-sauce=Mephis)
 and parse the (argc, argv) input line.  There are other ways to do
 this.  BOOST::program_options is great. The posix getopt is not.
 
 What really motivated me to write SoDa::Command was a desire to
 eliminate boost dependencies from software that I've been developing.
 One could use the BOOST program_options facility. It is very flexible,
 a model of spectacular use of templates.  I am humbled everytime I
 look at it.   But carrying boost around is like bringing a piano on
 a picnic -- for some things it is the only tool to use, but for
 the most part it gets pretty heavy when you have to haul it home
 after all that fried chicken, potato salad, and lemonade. 
 

 If this looks a lot like boost::program_options, then that is no
 accident.  But I just need to get rid of this piano.

## Installing

Just like any other CMake project.  For instance, to install the
package in /usr/local ... From this directory

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

This will install the libraries in /usr/local/lib or lib64 as appropriate
and the include in /usr/local/include/SoDa/Command.hxx

It will also write doxygen output that starts at /usr/local/share/sodacommand/doc/html/index.html


## Testing and Using it all

Take a look at the CMakeLists.txt file and CommandExample.cxx in the example
directory.  If the installation has gone right, then you should be able to do this from this directory.

```
cd example
mkdir build
cd build
cmake ../
make
./CommandExample --help
./CommandExample --intarg 3 --boolarg 1 --presarg --strarg "Fred and Barney" --strlistarg "Wilma" --strlistarg "Betty"
```

Did that work?

