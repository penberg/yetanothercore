module rv_core #(
    parameter XLEN = 32
)(
    input clk_i,
    input rst_i,

    output [XLEN-1:0] insn_addr_o,
    input [XLEN-1:0] insn_data_i
);

rv_fetch #(
    .XLEN(XLEN))
u_fetch(
    .clk_i(clk_i),
    .rst_i(rst_i),
    .insn_addr_o(insn_addr_o)
);

always @(posedge clk_i) begin
  $display("insn=", insn_data_i);
end

endmodule
