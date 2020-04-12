#!/usr/bin/env bash

printf "\n######### Setting up build env... ########\n\n" && ## error chaining
#rm -rf build-xcode                                        &&
#mkdir build-xcode                                         &&
cd build-xcode                                            &&
cmake .. -GXcode -DBUILD_EXAMPLES=ON                      &&
cd ..
