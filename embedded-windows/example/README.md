# Build the example

## Linux
Download wxWidgets

    cd wxWidgets-3.2.1/
    mkdir gtk-build
    cd gtk-build
    ../configure  --with-gtk=3 --disable-shared --enable-unicode --prefix=${HOME}/wx/wxw
    make -j4
    make install

    
Instead of:

    cmake -DwxWidgets_ROOT_DIR:PATH=${HOME}/wx/wxw ...
    export wxWidgets_ROOT_DIR:PATH=${HOME}/wx/wxw

use (according to FindwxWidgets.cmake documentation):

    cmake -DwxWidgets_CONFIG_EXECUTABLE:PATH=${HOME}/wx/wxw/bin/wx-config ...
    
or alternatively

    sudo apt-get install libwxgtk3.2unofficial-dev


## Macos
Download wxWidgets

    cd wxWidgets-3.2.1/
    mkdir -p cocoa-build
    cd cocoa-build/
    ../configure --disable-shared --enable-unicode --enable-universal_binary=x86_64,arm64  --enable-macosx-arch=arm64 --with-osx_cocoa --with-macosx-version-min=11 --prefix $HOME/wx/wxw 
    make -j6
    make install

If you miss `wxrc` try to rebuild it

    cd utils/wxrc
    g++ -o wxrc wxrc.cpp `wx-config --cxxflags --libs base,xml`

Using with cmake:

    cmake -DwxWidgets_CONFIG_EXECUTABLE:PATH=${HOME}/wx/wxw/bin/wx-config ...
