#!/bin/bash

THIS_FOLDER=`pwd`
echo "$THIS_FOLDER"

cd bin
LD_LIBRARY_PATH="$THIS_FOLDER/libs:LD_LIBRARY_PATH" ./CatapultGame