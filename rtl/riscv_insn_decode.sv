// RISC-V instruction decoder

module riscv_insn_decode(
  input  [31:0] insn,
  output [6:0]  opcode,
  output [4:0]  rd,
  output [2:0]  funct3,
  output [4:0]  rs1,
  output [4:0]  rs2,
  output [6:0]  funct7,
  output [19:0] imm
);

  assign opcode = insn[6:0];
  assign rd     = insn[11:7];
  assign funct3 = insn[14:12];
  assign rs1    = insn[19:15];
  assign rs2    = insn[24:20];
  assign funct7 = insn[31:25];
  assign imm    = insn[31:12];

endmodule
