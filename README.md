[![Build Status](https://travis-ci.org/avasyukov/gcm-3d.svg?branch=master)](https://travis-ci.org/avasyukov/gcm-3d)

## About

gcm-3d is a library implementing grid-characteristic method to model mechanical problems of deformable rigid body.

## License

The code is licensed under GPLv3 license.

## Build

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> -DCMAKE_BUILD_TYPE=<RELEASE || DEBUG> ..
make
```

Parameters CMAKE_INSTALL_PREFIX and CMAKE_BUILD_TYPE are optional.
CMAKE_INSTALL_PREFIX defaults to /usr, CMAKE_BUILD_TYPE defaults to DEBUG.
