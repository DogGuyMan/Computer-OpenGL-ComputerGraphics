freeglut 3.0.0-1.mp for MSVC

This package contains freeglut import libraries, headers, and Windows DLLs.
These allow 32 and 64 bit GLUT applications to be compiled on Windows using
Microsoft Visual C++.

For more information on freeglut, visit http://freeglut.sourceforge.net/.


Installation

Create a folder on your PC which is readable by all users, for example
�C:\Program Files\Common Files\MSVC\freeglut\� on a typical Windows system. Copy
the �lib\� and �include\� folders from this zip archive to that location.

The appropriate freeglut DLL can either be placed in the same folder as your
application, or can be installed in a system-wide folder which appears in your
%PATH% environment variable. Be careful not to mix the 32 bit DLL up with the 64
bit DLL, as they are not interchangeable.


Compiling 32 bit Applications

To create a 32 bit freeglut application, create a new Win32 C++ project in MSVC.
From the �Win32 Application Wizard�, choose a �Windows application�, check the
�Empty project� box, and submit.

You�ll now need to configure the compiler and linker settings. Open up the
project properties, and select �All Configurations� (this is necessary to ensure
our changes are applied for both debug and release builds). Open up the
�general� section under �C/C++�, and configure the �include\� folder you created
above as an �Additional Include Directory�. If you have more than one GLUT
package which contains a �glut.h� file, it�s important to ensure that the
freeglut include folder appears above all other GLUT include folders.

Now open up the �general� section under �Linker�, and configure the �lib\�
folder you created above as an �Additional Library Directory�. A freeglut
application depends on the import libraries �freeglut.lib� and �opengl32.lib�,
which can be configured under the �Input� section. However, it shouldn�t be
necessary to explicitly state these dependencies, since the freeglut headers
handle this for you. Now open the �Advanced� section, and enter �mainCRTStartup�
as the �Entry Point� for your application. This is necessary because GLUT
applications use �main� as the application entry point, not �WinMain��without it
you�ll get an undefined reference when you try to link your application.

That�s all of your project properties configured, so you can now add source
files to your project and build the application. If you want your application to
be compatible with GLUT, you should �#includeGL/freeglut.hut.h>�. If you want to use
freeglut specific extensions, you should �#include <GL/freeglut.h>� instead.

Don�t forget to either include the freeglut DLL when distributing applications,
or provide your users with some method of obtaining it if they don�t already
have it!


Compiling 64 bit Applications

Building 64 bit applications is almost identical to building 32 bit applications.
When you use the configuration manager to add the x64 platform, it�s easiest to
copy the settings from the Win32 platform. If you do so, it�s then only necessary
to change the �Additional Library Directories� configuration so that it
references the directory containing the 64 bit import library rather
than the 32 bit one.


Problems?

If you have problems using this package (compiler / linker errors etc.), please
check that you have followed all of the steps in this readme file correctly.
Almost all of the problems which are reported with these packages are due to
missing a step or not doing it correctly, for example trying to build a 32 bit
app against the 64 bit import library. If you have followed all of the steps
correctly but your application still fails to build, try building a very simple
but functional program (the example at
http://www.transmissionzero.co.uk/computing/using-glut-with-mingw/ works fine
with MSVC). A lot of people try to build very complex applications after
installing these packages, and often the error is with the application code or
other library dependencies rather than freeglut.

If you still can�t get it working after trying to compile a simple application,
then please get in touch via http://www.transmissionzero.co.uk/contact/,
providing as much detail as you can. Please don�t complain to the freeglut guys
unless you�re sure it�s a freeglut bug, and have reproduced the issue after
compiling freeglut from the latest SVN version�if that�s still the case, I�m
sure they would appreciate a bug report or a patch.


Changelog

2015�07�22: Release 3.0.0-2.mp

  � Modified the freeglut_std.h file so that it doesn�t try to link against the
    freeglutd.lib import library.

2015�03�15: Release 3.0.0-1.mp

  � First 3.0.0 MSVC release. I�ve built the package using Visual Studio 2013,
    and the only change I�ve made is to the DLL version resource�I�ve changed
    the description so that my MinGW and MSVC builds are distinguishable from
    each other (and other builds) using Windows Explorer.


Transmission Zero
2015�07�22

http://www.transmissionzero.co.uk/
