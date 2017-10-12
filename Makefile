all: release
#.PHONY: release

release:
	mkdir -p build	
	cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_IMG=ON -DBUILD_SFC=ON -DBUILD_TEST=ON .. && make -j4 VERBOSE=1
	
debug:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_IMG=ON -DBUILD_SFC=ON -DBUILD_TEST=ON .. && make -j4 VERBOSE=1

install:
	cd build && sudo make install
