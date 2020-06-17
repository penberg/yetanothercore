module insn_fetch #(
    parameter XLEN
)(
    input             clock,
    input             reset,
    output [XLEN-1:0] insn_addr
);

reg [XLEN-1:0] pc;

always @(posedge clock) begin
    if (reset)
        pc <= 'h0;
    else
        pc <= pc + 'd4;
end

assign insn_addr = pc;

endmodule
