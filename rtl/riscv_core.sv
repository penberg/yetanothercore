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

  logic [3:0] alu_op;

  logic [4:0] rd;
  logic [4:0] rs1;
  logic [4:0] rs2;
  logic [XLEN-1:0] alu_result;
  logic immediate_select;
  logic [XLEN-1:0] immediate;

  insn_decode decode(
      .insn(insn_data_bus),
      .alu_op(alu_op),
      .rd(rd),
      .rs1(rs1),
      .rs2(rs2),
      .rd_enable_write(rd_enable_write),
      .immediate_select(immediate_select),
      .immediate(immediate)
  );

  wire rd_enable_write;
  wire [XLEN-1:0] rd_data;
  wire [XLEN-1:0] rs1_data;
  wire [XLEN-1:0] rs2_data;

  always @ (*) begin
    if (immediate_select == 0)
      rd_data = alu_result;
    else
      rd_data = immediate;
  end

  alu #(
      .XLEN(XLEN))
  alu(
    .alu_op(alu_op),
    .operand_a(rs1_data),
    .operand_b(rs2_data),
    .result(alu_result)
  );

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

endmodule
