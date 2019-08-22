all:
	mkdir -p build/rvemu && cd build/rvemu && cmake ../../rvemu
	make -C build/rvemu
.PHONY: all
