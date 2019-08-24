module rv_decode#(
    parameter XLEN = 32
)(
    input  logic [XLEN-1:0] insn_i,
    output logic [6:0] aluop
);

    always_comb begin
        casez (insn_i)
        32'b0000000??????????000?????0110011: aluop = 7'b0110011; /* add */
	default: aluop = 7'b0000000;
	endcase
    end
/*
    logic [XLEN-1:0] if_pc_r;

    always @(posedge clk_i) begin
        if (rst_i)
            if_pc_r <= 'h0;
        else
            if_pc_r <= if_next_pc_w;
    end

    wire [XLEN-1:0] if_next_pc_w = if_pc_r + 'd4;

    assign insn_addr_o = if_pc_r;
*/

endmodule
