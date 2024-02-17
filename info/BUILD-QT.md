# Build Qt as static libraries

## Prerequirements


### Linux

https://doc.qt.io/qt-6/linux-requirements.html


        sudo apt-get install libfontconfig1-dev \
        libfreetype6-dev\
        libx11-dev\
        libx11-xcb-dev\
        libxext-dev\
        libxfixes-dev\
        libxi-dev\
        libxrender-dev\
        libxcb1-dev\
        '^libxcb.*-dev'\
        libx11-xcb-dev\
        libxrender-dev\
        libxi-dev\
        libxkbcommon-dev\ libxkbcommon-x11-dev
        libglu1-mesa-dev\
        libxcb-glx0-dev\
        libxcb-keysyms1-dev\
        libxcb-image0-dev\
        libxcb-shm0-dev\
        libxcb-icccm4-dev\
        libxcb-sync-dev\
        libxcb-xfixes0-dev\
        libxcb-shape0-dev\
        libxcb-randr0-dev\
        libxcb-render-util0-dev\
        libxcb-util-dev\
        libxcb-xinerama0-dev\
        libxcb-xkb-dev\
        libxkbcommon-dev\
        libxkbcommon-x11-dev

Version `6.2.4` [Download 6.2.4](https://download.qt.io/official_releases/qt/6.2/6.2.4/single/)

File qt-everywhere-src-6.2.4.tar.xz

    tar xvf qt-everywhere-src-6.2.4.tar.xz

For 6.4.2 use qt-everywhere-src-6.4.2.tar.xz

    tar xvf qt-everywhere-src-6.4.2.tar.xz


Go to the folder where QT sources unpacked **qt-everywhere-src-6.2.4**


Configure Qt
    
        ./configure -static -release -opensource -confirm-license -prefix ${PWD}/qt-642 -skip qtwebglplugin -skip qtwebview -skip qtwebengine -skip webengine -make libs -nomake tools -nomake examples -nomake tests

        cmake --install .


For example

        -DCMAKE_PREFIX_PATH=<path_to_qt>/qt-everywhere-src-6.4.2/qt-642

or

        -DCMAKE_PREFIX_PATH=$HOME/qt-sources/qt-everywhere-src-6.4.2/qt-642

