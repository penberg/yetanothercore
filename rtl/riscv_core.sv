module riscv_core #(
    parameter XLEN = 32
)(
    input             clock,
    input             reset,
    input  [XLEN-1:0] insn_data_bus,
    output [XLEN-1:0] insn_addr_bus
);

  insn_fetch #(
      .XLEN(XLEN))
  fetch(
      .clock(clock),
      .reset(reset),
      .insn_addr(insn_addr_bus)
  );

  wire [6:0] opcode;
  wire [4:0] rd;
  wire [2:0] funct3;
  wire [4:0] rs1;
  wire [4:0] rs2;
  wire [6:0] funct7;
  wire [19:0] imm;

  insn_decode decode(
      .insn(insn_data_bus),
      .opcode(opcode),
      .rd(rd),
      .funct3(funct3),
      .rs1(rs1),
      .rs2(rs2),
      .funct7(funct7),
      .imm(imm)
  );

  wire rd_enable_write;
  wire [XLEN-1:0] rd_data;
  wire [XLEN-1:0] rs1_data;
  wire [XLEN-1:0] rs2_data;

  regfile #(
      .XLEN(XLEN))
  regfile(
    .clock(clock),
    /* FIXME: .reset(reset), */
    .rs1_addr(rs1),
    .rs2_addr(rs2),
    .rd_addr(rd),
    .rd_enable_write(rd_enable_write),
    .rd_data(rd_data),
    .rs1_data(rs1_data),
    .rs2_data(rs2_data)
  );

  insn_exec #(
      .XLEN(XLEN))
  exec(
    .opcode(opcode),
    .funct3(funct3),
    .rs1(rs1_data),
    .rs2(rs2_data),
    .funct7(funct7),
    .imm(imm),
    .rd_enable_write(rd_enable_write),
    .rd(rd_data)
  );

endmodule
