all: rvemu rvsim

rvemu:
	mkdir -p build/rvemu && cd build/rvemu && cmake ../../rvemu
	make -C build/rvemu

RVSIM_BUILD = build/rvsim
RVSIM_PROGRAM = rvsim

rvsim:
	mkdir -p $(RVSIM_BUILD)
	verilator_bin -Wall -Irtl --cc rtl/rv_core.sv --exe ../../rvsim/rvsim.cpp -o $(RVSIM_PROGRAM) --Mdir $(RVSIM_BUILD)
	make -C $(RVSIM_BUILD) -j -f Vrv_core.mk $(RVSIM_PROGRAM)

.PHONY: all rvemu rvsim
