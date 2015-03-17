# Compiling the project and its dependencies #

To compile this project, you will need to setup your system. Currently we are using C++ with Visual Studio 2010.

# Details #

Steps:
  1. Install visual studio 2010
  1. Download and compile the boost libraries (I am using version 1.45.0)
    * download boost sources and extract
    * open the visual studio 2010 command line tool and cd to boost root
    * run the 'bootstrap' command to build BJAM
    * run
```
bjam variant=debug,release threading=multi link=static
```
    * set include and library paths in the visual studio 2010 property pages
  1. Download and compile the OpenCV 2.1 library (2.2 has a bug on win7 with web-cam!)
    * Download CMake 2.8
    * Download threading building blocks (I am using TBB 3.0 Update 4)
    * Download OpenCV 2.1 sources
    * Open CMake-gui and set the **source code path** to OpenCV root, and **where to build binaries** to OpenCV root/build
    * run configure (select visual studio 2010 as environment, use native compilers)
    * select with TBB, run configure again
    * search for TBB\_PATH\_NOT\_FOUND, click on that line and enter the path manually
    * press configure again and then click generate
    * go to the OpenCV root/build and open OpenCV.sln, inside the .sln build the debug and release versions.
    * if you get errors concerning TBB, add the include path again in the property pages
  1. Download and compile Qt (I am using 4.7.1, we downloaded the vs2008 windows installer package)
    * open the visual studio 2010 command prompt
    * cd to the Qt root directory and run configure --help. Add some changes if you like
    * make sure you have opengl enabled, use the -mp flag to use multiple cores during compilation, we build the static library variant. this is the build configure command I used:
```
configure -platform win32-msvc2010 -static -debug-and-release -mp -no-qt3support -no-webkit -qt-sql-sqlite -plugin-sql-sqlite -fast
```
    * when configure finishes, run nmake (this takes 1 to 2 hours)
    * download and install the QT visual studio plugin (i am using 1.1.7)
    * configure visual studio 2010 to use the default qt version
  1. (optional) Install AnkhSVN to access subversion trough Visual Studio
  1. Download the code in the trunk, set the correct include paths and libraries needed (cv210.lib, highgui210.lib, cxcore210.lib) or debug versions (cv210d.lib, highgui210d.lib, cxcore210d.lib). Boost uses auto-linking and QT configures the project trough the vs plugin tool, so no configuration needed for those libraries.
  1. compile
  1. place the dll files in the Release folder next to the executable (cv210.dll, highgui210.dll, cxcore210.dll, tbb.dll) or debug versions in the Debug folder (cv210d.dll, highgui210d.dll, cxcore210d.dll, tbb\_debug.dll)
  1. run the executable and enjoy!

# Video Tutorials #
For compiling Boost, you can also watch this video tutorial: [Compiling and configuring Boost for Visual Studio 2010](http://www.youtube.com/watch?v=5AmwIwedTCM)

For compiling OpenCV, you can also watch this video tutorial:
[Compiling and configuring the OpenCV library for use with Visual Studio 2010](http://www.youtube.com/watch?v=XeBhwbRoKvk)