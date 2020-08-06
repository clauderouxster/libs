# Libsound

# Windows

To compile libsound, you need to install:

libao:          from _https://xiph.org/ao/_
libmpg123: from _https://www.mpg123.de/api_
libsndfile: from _http://www.mega-nerd.com/libsndfile_

## include files

The following include files should be copied in: : _include/windows/ao_

Please place the following files in it (from the different libraries above)

```
ao.h
ao_private.h
mpg123.h
os_types.h
os_types.h.in
plugin.h
sndfile.h
sndfile.hh
```

## Compiling

You need to install mingw: _http://www.mingw.org_ to compile the different versions.

Then you will be required the following libraries:

```
libgcc_s_sjlj-1.dll
libltdl-7.dll
libstdc++-6.dll
libwinpthread-1.dll
```
Any other linux kernel on Windows should be Ok.


## Mac OS

You need to compile the different versions: libao, libmpg123 and libsndfile for Mac OS.

Compile the static libraries: ".a" and put the following libs in: _libs/macs_:
```
libao.a
libmpg123.a
libsndfile.a
```

## include files

The following include files should be copied in: _include/ao_

```
ao.h
ao_private.h
mpg123.h
os_types.h
os_types.h.in
plugin.h
sndfile.h
sndfile.hh
```

## Linux

Usually, these libraries are already installed. However, you can install them yourself with _apt-get_ or _yum_ according to your platform.
Install the development versions.

```
ao.h
ao_private.h
mpg123.h
os_types.h
os_types.h.in
plugin.h
sndfile.h
sndfile.hh
```


# Compiling on Linux and Mac OS

If you want to compile these libraries on your machine, you need to launch _install.py_ with the option: _-withsound_.

```
python install.py -withsound
```

Then in the folder: _libsound_, run: _make sound_



