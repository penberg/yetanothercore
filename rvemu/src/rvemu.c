/* RISC-V RVI32 software emulator */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Types for RISC-V machine. */
typedef uint64_t rv_imm_t;
typedef int64_t rv_simm_t;
typedef uint8_t rv_reg_t;
typedef uint64_t rv_value_t;
typedef int64_t rv_svalue_t;
typedef uint32_t rv_insn_t;
typedef uint32_t rv_addr_t;

/* RISC-V CPU core.  */
struct rv_cpu {
  rv_value_t PC;
  rv_value_t R[32];
  void *ram;
  size_t ram_size;
};

static void trace_exec_i(const char *op, rv_reg_t rd, rv_reg_t rs1,
                         rv_simm_t imm) {
  printf("trace: exec: %s x%d, x%d, %ld\n", op, rd, rs1, imm);
}

static void trace_exec(const char *op, rv_reg_t rd, rv_reg_t rs1,
                       rv_reg_t rs2) {
  printf("trace: exec: %s x%d, x%d, x%d\n", op, rd, rs1, rs2);
}

static uint8_t rv_insn_opcode(rv_insn_t insn) { return insn & 0b1111111; }

static uint32_t rv_insn_funct3(rv_insn_t insn) { return (insn >> 12) & 0b111; }

static uint32_t rv_insn_funct7(rv_insn_t insn) {
  return (insn >> 25) & 0b1111111;
}

static uint8_t rv_insn_rs2(rv_insn_t insn) { return (insn >> 20) & 0b11111; }

static uint8_t rv_insn_rs1(rv_insn_t insn) { return (insn >> 15) & 0b11111; }

static uint8_t rv_insn_rd(rv_insn_t insn) { return (insn >> 7) & 0b11111; }

static uint32_t rv_insn_i_imm(rv_insn_t insn) {
  return (insn >> 20) & 0b111111111111;
}

static uint32_t rv_insn_sb_imm(rv_insn_t insn) {
  return ((insn >> 25) & 0b11111) | ((insn >> 7) & 0b11111);
}

static uint32_t rv_cpu_read_mem32(struct rv_cpu *cpu, rv_addr_t addr) {
  assert(addr < cpu->ram_size);

  return *(uint32_t *)(cpu->ram + addr);
}

static void rv_cpu_exec(struct rv_cpu *cpu) {
  rv_insn_t insn = rv_cpu_read_mem32(cpu, cpu->PC);
  printf("trace: decode: pc=%08x, insn=%08x opcode=%02x\n", (uint32_t)cpu->PC,
         insn, rv_insn_opcode(insn));
  uint8_t opcode = rv_insn_opcode(insn);
  switch (opcode) {
  case 0b0010011: {
    uint32_t funct3 = rv_insn_funct3(insn);
    uint8_t rd = rv_insn_rd(insn);
    uint8_t rs1 = rv_insn_rs1(insn);
    uint32_t imm = rv_insn_i_imm(insn);
    switch (funct3) {
    case 0b000: {
      trace_exec_i("addi", rd, rs1, imm);
      cpu->R[rd] = (rv_svalue_t)cpu->R[rs1] + imm;
      break;
    }
    case 0b010: {
      trace_exec_i("slti", rd, rs1, imm);
      cpu->R[rd] = (rv_svalue_t)cpu->R[rs1] < imm;
      break;
    }
    case 0b011: {
      trace_exec_i("sltiu", rd, rs1, imm);
      cpu->R[rd] = cpu->R[rs1] < imm;
      break;
    }
    case 0b100: {
      trace_exec_i("xori", rd, rs1, imm);
      cpu->R[rd] = cpu->R[rs1] ^ imm;
      break;
    }
    case 0b110: {
      trace_exec_i("ori", rd, rs1, imm);
      cpu->R[rd] = cpu->R[rs1] | imm;
      break;
    }
    case 0b111: {
      trace_exec_i("andi", rd, rs1, imm);
      cpu->R[rd] = cpu->R[rs1] & imm;
      break;
    }
    case 0b001: {
      trace_exec_i("slli", rd, rs1, imm);
      cpu->R[rd] = cpu->R[rs1] << imm;
      break;
    }
    case 0b101: {
      bool srai = (imm >> 7) & 0b01000;
      if (srai) {
        trace_exec_i("srai", rd, rs1, imm);
        cpu->R[rd] = (rv_svalue_t)cpu->R[rs1] >> imm;
      } else {
        trace_exec_i("srli", rd, rs1, imm);
        cpu->R[rd] = cpu->R[rs1] >> imm;
      }
      break;
    }
    default:
      assert(0);
    }
    break;
  }
  case 0b1100011: {
    uint32_t funct3 = rv_insn_funct3(insn);
    uint8_t rs1 = rv_insn_rs1(insn);
    uint8_t rs2 = rv_insn_rs2(insn);
    uint32_t imm = rv_insn_sb_imm(insn);
    switch (funct3) {
    case 0x0: { /* beq */
      if (cpu->R[rs1] == cpu->R[rs2]) {
        cpu->PC += imm;
        return;
      }
      break;
    }
    case 0x1: { /* bne */
      if (cpu->R[rs1] != cpu->R[rs2]) {
        cpu->PC += imm;
        return;
      }
      break;
    }
    case 0x4: { /* blt */
      if (cpu->R[rs1] < cpu->R[rs2]) {
        cpu->PC += imm;
        return;
      }
      break;
    }
    case 0x5: { /* bge */
      if (cpu->R[rs1] >= cpu->R[rs2]) {
        cpu->PC += imm;
        return;
      }
      break;
    }
    default:
      assert(0);
    }
    break;
  }
  case 0b0110011: {
    uint32_t funct10 = (rv_insn_funct7(insn) << 3) | rv_insn_funct3(insn);
    uint8_t rd = rv_insn_rd(insn);
    uint8_t rs1 = rv_insn_rs1(insn);
    uint8_t rs2 = rv_insn_rs2(insn);
    switch (funct10) {
    case 0b0000000000: {
      trace_exec("add", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] + cpu->R[rs2];
      break;
    }
    case 0b0100000000: {
      trace_exec("sub", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] - cpu->R[rs2];
      break;
    }
    case 0b0000000001: {
      trace_exec("sll", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] << cpu->R[rs2];
      break;
    }
    case 0b0000000010: {
      trace_exec("slt", rd, rs1, rs2);
      cpu->R[rd] = (rv_svalue_t)cpu->R[rs1] < (rv_svalue_t)cpu->R[rs2];
      break;
    }
    case 0b0000000011: {
      trace_exec("sltu", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] < cpu->R[rs2];
      break;
    }
    case 0b0000000100: {
      trace_exec("xor", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] ^ cpu->R[rs2];
      break;
    }
    case 0b0000000101: {
      trace_exec("srl", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] >> cpu->R[rs2];
      break;
    }
    case 0b0100000101: {
      trace_exec("sra", rd, rs1, rs2);
      cpu->R[rd] = (rv_svalue_t)cpu->R[rs1] >> cpu->R[rs2];
      break;
    }
    case 0b0000000110: {
      trace_exec("or", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] | cpu->R[rs2];
      break;
    }
    case 0b0000000111: {
      trace_exec("and", rd, rs1, rs2);
      cpu->R[rd] = cpu->R[rs1] & cpu->R[rs2];
      break;
    }
    default:
      assert(0);
    }
    break;
  }
  default:
    assert(0);
  }
  cpu->PC += 4;
}

void rv_cpu_run(struct rv_cpu *cpu) {
  for (;;) {
    rv_cpu_exec(cpu);
  }
}

static const char *program;

static void die(const char *fmt, ...) {
  va_list ap;

  fprintf(stderr, "%s: error: ", program);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, ": %s\n", strerror(errno));
  exit(1);
}

int main(int argc, char *argv[]) {
  program = basename(argv[0]);
  if (argc != 2) {
    fprintf(stderr, "usage: %s [image]\n", program);
    exit(1);
  }
  const char *image_filename = argv[1];
  int fd = open(image_filename, 0);
  if (fd < 0) {
    die("open %s", image_filename);
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    die("fstat");
  }
  void *ram_mmap = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ram_mmap == MAP_FAILED) {
    die("mmap");
  }
  struct rv_cpu cpu = {
      .ram = ram_mmap,
      .ram_size = st.st_size,
  };
  rv_cpu_run(&cpu);
  if (munmap(ram_mmap, st.st_size) < 0) {
    die("munmap");
  }
  if (close(fd) < 0) {
    die("close");
  }
  return 0;
}
