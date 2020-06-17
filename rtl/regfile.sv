// RISC-V register file.

/* verilator lint_off UNUSED */

module regfile #(
  parameter XLEN
) (
  input           clock,           // The clock signal.
  input  [4:0]    rs1_addr,        // The "rs1" register to read.
  input  [4:0]    rs2_addr,        // The "rs2" register to read.
  input  [4:0]    rd_addr,         // The "rd" register to write to.
  input           rd_enable_write, // Enable writing to the "rd" register.
  input  [XLEN-1:0] rd_data,       // The data to write to the "rd" register.
  output [XLEN-1:0] rs1_data,      // The data read from the "rs1" register.
  output [XLEN-1:0] rs2_data       // The data read from the "rs2" register.
);

  // The 32 registers defined by the RISC-V instruction set. 
  logic [XLEN-1:0] registers[0:31];

  // The "x0" register is always zero.
  initial registers[0] = '0;

  // Read the "rs1" and "rs2" registers:
  assign rs1_data = registers[rs1_addr];
  assign rs2_data = registers[rs2_addr];

  // Write to "rd" register only if writes are enabled and it is not the
  // "x0" register, which is hard-wired to zero. 
  always_ff @(posedge clock) begin
    if (rd_enable_write & (rd_addr != 5'b0)) begin
      registers[rd_addr] <= rd_data;
      $display("register writeback: %h -> %d", rd_data, rd_addr);
    end
  end

endmodule
