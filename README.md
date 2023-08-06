# VolTrayke

[![CI]](https://github.com/qtilities/voltrayke/actions/workflows/build.yml)

## Overview

An audio volume system tray widget,
inspired by [PNMixer] and LXQt Panel' volume plugin.

## Dependencies

Runtime:

- Qt5/6 base
- ALSA
- Pulseaudio

Build:

- CMake
- Qt Linguist Tools
- [Qtilitools] CMake modules
- Git (optional, to pull latest VCS checkouts)

## Build

`CMAKE_BUILD_TYPE` is usually set to `Release`, though `None` might be a valid [alternative].<br>
`CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.<br>
Using `sudo make install` is discouraged, instead use the system package manager where possible.

```bash
cmake -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr -W no-dev
cmake --build build --verbose
DESTDIR="$(pwd)/package" cmake --install build
```


[alternative]: https://wiki.archlinux.org/title/CMake_package_guidelines#Fixing_the_automatic_optimization_flag_override
[CI]:          https://github.com/qtilities/sqeleton/actions/workflows/build.yml/badge.svg
[PNMixer]:     https://github.com/nicklan/pnmixer/
[Qtilitools]:  https://github.com/qtilities/qtilitools/
