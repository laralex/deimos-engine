## Deimos Engine
### Supported platforms
* Ubuntu 20.04
To install dependencies and git submodules, run
```
make install_linux
```
### Build
Install `clang` to compile

* Compile everything in Release (Debug) mode and run hotloader (will hot-reload engine or app dynamic library when they're recompiled)
```
make run
# make run DEBUG=y
```

* Forcefully recompile application library
```
make app 
# make app DEBUG=y
```

* Forcefully recompile engine library
```
make dei 
# make dei DEBUG=y
```
