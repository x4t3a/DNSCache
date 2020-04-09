#!/usr/bin/env bash

runUnderValgrindIfExists() {
	if hash valgrind 2>/dev/null; then
		printf "######### Running under valgrind... ######\n\n" &&
		valgrind ./build/examples/example_dns_cache
	else
		printf "######### Running w/o valgrind... ########\n\n" &&
		./build/examples/example_dns_cache
	fi
}

printf "\n######### Setting up build env... ########\n\n" && ## error chaining
rm -rf build                                              &&
mkdir build                                               &&
cd build                                                  &&
cmake .. -DBUILD_EXAMPLES=ON                              &&
printf "\n######### Building... ####################\n\n" &&
make                                                      &&
cd ..                                                     &&
printf "\n######### Running example app... #########\n"   &&
runUnderValgrindIfExists
