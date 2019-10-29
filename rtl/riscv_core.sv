module riscv_core #(
    parameter XLEN = 32
)(
    input             clock,
    input             reset,
    input  [XLEN-1:0] insn_data_bus,
    output [XLEN-1:0] insn_addr_bus
);

  riscv_insn_fetch #(
      .XLEN(XLEN))
  fetch(
      .clock(clock),
      .reset(reset),
      .insn_addr(insn_addr_bus)
  );

  /* verilator lint_off UNUSED */
  logic [6:0] opcode;
  /* verilator lint_on UNUSED */
  logic [4:0] rd;
  /* verilator lint_off UNUSED */
  logic [2:0] funct3;
  /* verilator lint_on UNUSED */
  logic [4:0] rs1;
  logic [4:0] rs2;
  /* verilator lint_off UNUSED */
  logic [6:0] funct7;
  /* verilator lint_on UNUSED */

  riscv_insn_decode decode(
      .insn(insn_data_bus),
      .opcode(opcode),
      .rd(rd),
      .funct3(funct3),
      .rs1(rs1),
      .rs2(rs2),
      .funct7(funct7)
  );
 
  /* verilator lint_off UNDRIVEN */
  logic [XLEN-1:0] rd_data;
  /* verilator lint_on UNDRIVEN */
  logic [XLEN-1:0] rs1_data;
  logic [XLEN-1:0] rs2_data;

  riscv_register_file #(
      .XLEN(XLEN))
  regfile(
    .clock(clock),
    /* FIXME: .reset(reset), */
    .rs1_addr(rs1),
    .rs2_addr(rs2),
    .rd_addr(rd),
    .rd_enable_write(0), /* FIXME */
    .rd_data(rd_data),
    .rs1_data(rs1_data),
    .rs2_data(rs2_data)
  );

  riscv_insn_exec #(
      .XLEN(XLEN))
  exec(
    .clock(clock),
    .opcode(opcode),
    .rd(rd_data),
    .funct3(funct3),
    .rs1(rs1_data),
    .rs2(rs2_data),
    .funct7(funct7)
  );

endmodule
