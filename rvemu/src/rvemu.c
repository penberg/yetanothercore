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

/* RISC-V CPU core.  */
struct rv_cpu {
  rv_value_t PC;
  rv_value_t R[32];
};

static void trace_exec_i(const char *op, rv_reg_t rd, rv_reg_t rs1,
                         rv_simm_t imm) {
  printf("trace: exec: %s x%d, x%d, %ld\n", op, rd, rs1, imm);
}

static void trace_exec(const char *op, rv_reg_t rd, rv_reg_t rs1,
                       rv_reg_t rs2) {
  printf("trace: exec: %s x%d, x%d, x%d\n", op, rd, rs1, rs2);
}

static void rv_op_addi(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1,
                       rv_simm_t imm) {
  trace_exec_i("addi", rd, rs1, imm);
  R[rd] = (rv_svalue_t)R[rs1] + imm;
}

static void rv_op_slti(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1,
                       rv_simm_t imm) {
  trace_exec_i("slti", rd, rs1, imm);
  R[rd] = (rv_svalue_t)R[rs1] < imm;
}

static void rv_op_sltiu(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1,
                        rv_imm_t imm) {
  trace_exec_i("sltiu", rd, rs1, imm);
  R[rd] = R[rs1] < imm;
}

static void rv_op_xori(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("xori", rd, rs1, imm);
  R[rd] = R[rs1] ^ imm;
}

static void rv_op_ori(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("ori", rd, rs1, imm);
  R[rd] = R[rs1] | imm;
}

static void rv_op_andi(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("andi", rd, rs1, imm);
  R[rd] = R[rs1] & imm;
}

static void rv_op_slli(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("slli", rd, rs1, imm);
  R[rd] = R[rs1] << imm;
}

static void rv_op_srli(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("srli", rd, rs1, imm);
  R[rd] = R[rs1] >> imm;
}

static void rv_op_srai(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_imm_t imm) {
  trace_exec_i("srai", rd, rs1, imm);
  R[rd] = (rv_svalue_t)R[rs1] >> imm;
}

static void rv_op_add(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("add", rd, rs1, rs2);
  R[rd] = R[rs1] + R[rs2];
}

static void rv_op_sub(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("sub", rd, rs1, rs2);
  R[rd] = R[rs1] - R[rs2];
}

static void rv_op_sll(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("sll", rd, rs1, rs2);
  R[rd] = R[rs1] << R[rs2];
}

static void rv_op_slt(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("slt", rd, rs1, rs2);
  R[rd] = (rv_svalue_t)R[rs1] < (rv_svalue_t)R[rs2];
}

static void rv_op_sltu(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("sltu", rd, rs1, rs2);
  R[rd] = R[rs1] < R[rs2];
}

static void rv_op_xor(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("xor", rd, rs1, rs2);
  R[rd] = R[rs1] ^ R[rs2];
}

static void rv_op_srl(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("srl", rd, rs1, rs2);
  R[rd] = R[rs1] >> R[rs2];
}

static void rv_op_sra(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("sra", rd, rs1, rs2);
  R[rd] = (rv_svalue_t)R[rs1] >> R[rs2];
}

static void rv_op_or(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("or", rd, rs1, rs2);
  R[rd] = R[rs1] | R[rs2];
}

static void rv_op_and(rv_value_t *R, rv_reg_t rd, rv_reg_t rs1, rv_reg_t rs2) {
  trace_exec("and", rd, rs1, rs2);
  R[rd] = R[rs1] & R[rs2];
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

static void rv_cpu_exec(struct rv_cpu *cpu, rv_insn_t insn) {
  printf("trace: decode: insn=%08x opcode=%02x\n", insn, rv_insn_opcode(insn));
  uint8_t opcode = rv_insn_opcode(insn);
  switch (opcode) {
  case 0b0010011: {
    uint32_t funct3 = rv_insn_funct3(insn);
    uint8_t rd = rv_insn_rd(insn);
    uint8_t rs1 = rv_insn_rs1(insn);
    uint32_t imm = rv_insn_i_imm(insn);
    switch (funct3) {
    case 0b000: {
      rv_op_addi(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b010: {
      rv_op_slti(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b011: {
      rv_op_sltiu(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b100: {
      rv_op_xori(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b110: {
      rv_op_ori(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b111: {
      rv_op_andi(cpu->R, rd, rs1, imm);
      break;
    }
    case 0b001: {
      rv_op_slli(cpu->R, rd, rs1, imm & 0b11111);
      break;
    }
    case 0b101: {
      bool srai = (imm >> 7) & 0b01000;
      if (srai) {
        rv_op_srai(cpu->R, rd, rs1, imm & 0b11111);
      } else {
        rv_op_srli(cpu->R, rd, rs1, imm & 0b11111);
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
      rv_op_add(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0100000000: {
      rv_op_sub(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000001: {
      rv_op_sll(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000010: {
      rv_op_slt(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000011: {
      rv_op_sltu(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000100: {
      rv_op_xor(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000101: {
      rv_op_srl(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0100000101: {
      rv_op_sra(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000110: {
      rv_op_or(cpu->R, rd, rs1, rs2);
      break;
    }
    case 0b0000000111: {
      rv_op_and(cpu->R, rd, rs1, rs2);
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
}

void rv_cpu_run(struct rv_cpu *cpu, void *ram) {
  rv_insn_t *insn = ram + cpu->PC;
  rv_cpu_exec(cpu, *insn);
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
  struct rv_cpu cpu = {};
  rv_cpu_run(&cpu, ram_mmap);
  if (munmap(ram_mmap, st.st_size) < 0) {
    die("munmap");
  }
  if (close(fd) < 0) {
    die("close");
  }
  return 0;
}
