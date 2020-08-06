# libs

These two directories contain the code to create the GUI and SOUND libraries for TAMGU. Install these two libraries in the *tamgu* directory.
The main git repository for *Tamgu* is: https://github.com/naver/tamgu.

## Linux Specifics

To compile these libraries you need to have the following libraries on your system:

fltk
libao, libsnd and libmpg123

These libraries are usually either installed or can be installed with *yum* or *apt-get*, according to your package installer and your Linux system.

## Windows

The Windows Makefiles are also available. To compile these libraries on Windows, you need to enrich your main Makefile (tamguraw.sln or tamgu.sln) with the following Makefiles that are present in *libgui* and *libsound*:

* libgui.vcxproj
* libsound.vcxproj

The libraries and include files are all provided within these directories

## Mac OS and Linux

First move the directories to the main directory *tamgu*

Launch install.py with the options: *-withgui* and *-withsound*.


**DISCLAIMER**

These libraries are provided as is and are disjoined from the GitHub archive. 