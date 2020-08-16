all: emulator simulator

emulator:
	mkdir -p build/emulator && cd build/emulator && cmake ../../emulator
	make -C build/emulator

SIMULATOR_BUILD = build/simulator
SIMULATOR_PROGRAM = simulator

VERILATOR_FLAGS =
VERILATOR_FLAGS += -Wall
VERILATOR_FLAGS += -Irtl
VERILATOR_FLAGS += --cc --exe

simulator:
	mkdir -p $(SIMULATOR_BUILD)
	verilator_bin $(VERILATOR_FLAGS) rtl/riscv_core.sv ../../simulator/main.cpp -o $(SIMULATOR_PROGRAM) --Mdir $(SIMULATOR_BUILD)
	make -C $(SIMULATOR_BUILD) -j -f Vriscv_core.mk $(SIMULATOR_PROGRAM)

.PHONY: all emulator simulator
