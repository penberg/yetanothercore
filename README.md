# Yet another CPU core

This is a single-cycle RISC-V core written in SystemVerilog for educational purposes.

## Building from sources

To build everything, type:

```
make
```

## Softwate emulator and simulator

### Building images 

The software emulator and simulator expect animage to be a flat binary. You can generate one with:

```
riscv64-linux-gnu-as tests/alu.S -o tests/alu.o \
  && riscv64-linux-gnu-objcopy -O binary tests/alu.o tests/alu.bin
```

### Running images

To run the generated image with *emulator*, type:

```
./build/rvemu/rvemu tests/alu.bin
```

To run the generated image with *simulator*, type:

```
./build/rvsim/rvsim tests/alu.bin
```

## Other RISC-V cores

https://github.com/tilk/riscv-simple-sv
