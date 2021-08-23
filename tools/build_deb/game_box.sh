#!/bin/bash

SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)

export QT_HOME=$SHELL_FOLDER
export QT_PLUGIN_PATH=$QT_HOME/plugins
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$QT_HOME/lib" 
./game_box

