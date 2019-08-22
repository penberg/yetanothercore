module rv_fetch#(
    parameter XLEN = 32
)(
    input clk_i,
    input rst_i,

    output [XLEN-1:0] insn_addr_o
);

reg [XLEN-1:0] if_pc_r;

always @(posedge clk_i) begin
    if (rst_i)
        if_pc_r <= 'h0;
    else
        if_pc_r <= if_next_pc_w;
end

wire [XLEN-1:0] if_next_pc_w = if_pc_r + 'd4;

assign insn_addr_o = if_pc_r;

endmodule
