# Build Qt as static libraries
Version `6.2.4` [Download 6.2.4](https://download.qt.io/official_releases/qt/6.2/6.2.4/single/)
File qt-everywhere-src-6.2.4.tar.xz

    tar xvf qt-everywhere-src-6.2.4.tar.xz

Go to the folder **qt-everywhere-src-6.2.4.tar**

## Linux

```
./configure -static -release -platform "linux-g++-64" -opensource -confirm-license -prefix $PWD/qtbase -qt-zlib -qt-libpng -qt-libjpeg -qt-freetype \
-no-opengl -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcharts -skip qtconnectivity -skip qtdatavis3d \
-skip qtmqtt -skip qtdeclarative -skip qtopcua -skip qtdoc -skip qtgamepad -skip qtlocation -skip qtlottie -skip qtmacextras -skip qtmultimedia \
-skip qtnetworkauth -skip qtscxml -skip qtpurchasing -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtquicktimeline -skip qtvirtualkeyboard \
-skip qtremoteobjects -skip qtscript -skip qtsensors -skip qtspeech -skip qtsvg -skip qtwayland -skip qtwebglplugin \
-skip qtwebview -skip qtwebengine -skip webengine -make libs -nomake tools -nomake examples -nomake tests
```

