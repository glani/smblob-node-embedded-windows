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

    