# smblob-node-embedded-windows


### Daemon

To run locally and be able to connect to consumer in the DEBUG mode use following arguments:

    --log 5 --debug 1 --pipe-name /tmp/uv-test-sock --logFile $HOME/sources/smblob-node-embedded-windows/cmake-build-debug/log-smblob-node-embedded-windows-daemon.log


## Some interesting things

### Linux

To be able to send key event to window via `xdotool` embedded window should be active:

    xdotool search --name "New Tab" windowactivate --sync %1 key F11 windowactivate $(xdotool getactivewindow)
