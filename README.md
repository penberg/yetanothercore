# RISC-V CPU core

This project aims to implement a simple RISC-V core, with the intent to learn more about RISC-V architecture, and digital logic design with SystemVerilog.
The current plan is to implement an in-order, single issue CPU core with classic five stage RISC pipeline, which supports the RVI32 instruction set.

## Building from Sources

To build everything, type:

```
make
```

## Softwate Emulator and Simulator

### Building Images 

The software emulator and simulator expect animage to be a flat binary. You can generate one with:

```
riscv64-linux-gnu-as tests/add.S -o tests/add.o \
  && riscv64-linux-gnu-objcopy -O binary tests/add.o tests/add.bin
```

### Running Images

To run the generated image with *emulator*, type:

```
./build/rvemu/rvemu tests/add.bin
```

To run the generated image with *simulator*, type:

```
./build/rvsim/rvsim tests/add.bin
```
