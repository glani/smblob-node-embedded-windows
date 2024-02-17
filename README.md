# smblob-node-embedded-windows


### Daemon

To run locally and be able to connect to consumer in the DEBUG mode use following arguments:

    --log 5 --debug 1 --pipe-name /tmp/uv-test-sock --logFile $HOME/sources/smblob-node-embedded-windows/cmake-build-debug/log-smblob-node-embedded-windows-daemon.log


## Some interesting things

### Linux

To be able to send key event to window via `xdotool` embedded window should be active:

    xdotool search --name "New Tab" windowactivate --sync %1 key F11 windowactivate $(xdotool getactivewindow)
    xdotool --sync 37748752 key F11 windowactivate $(xdotool getactivewindow)
    xdotool key F11 --window 37748752 windowactivate 
    xdotool windowactivate 37748752  

    xdotool key F11 --window 37748752
    xdotool search --name "Qt 6.2.4"

xdotool search --name "Qt 6.2.4" windowactivate && xdotool windowactivate 37748752 && xdotool key --delay 100 --window 37748752 F11
xdotool key --delay 100 F11 --window 37748752

    xdotool windowfocus 37748752
    xdotool windowfocus 50331944

python3 -c 'print(int("0x05e0003d", 16))'

## Useful links
https://github.com/gusnan/devilspie2
https://gist.github.com/cat-in-136/96ee8e96e81e0cc763d085ed697fe193
https://unix.stackexchange.com/questions/103356/remove-title-bar-of-another-program
https://stackoverflow.com/questions/30032416/xcb-not-receiving-motion-notify-events-on-all-windows

https://stackoverflow.com/questions/57896007/detect-window-focus-changes-with-xcb

https://coral.googlesource.com/weston-imx/+/refs/tags/3.0.0-3/xwayland/window-manager.c

### Close windows XCB

    https://marc.info/?l=freedesktop-xcb&m=129381953404497


## Macos

https://github.com/msolo/simbl/blob/master/SIMBL%20Agent/SIMBLAgent.m

https://github.com/dfct/TrueFramelessWindow