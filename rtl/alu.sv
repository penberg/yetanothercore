// Arithmetic logical unit (ALU)

`include "constants.sv"

module alu #(
  parameter XLEN
) (
  input  [3:0]      alu_op,
  input  [XLEN-1:0] operand_a,
  input  [XLEN-1:0] operand_b,
  output [XLEN-1:0] result
);

  always @ (*) begin
    case (alu_op)
      `ALU_ADD: begin
        result = operand_a + operand_b;
      end
      `ALU_SUB: begin
        result = operand_a - operand_b;
      end
      `ALU_SLL: begin
        result = operand_a << operand_b[4:0];
      end
      `ALU_SLT: begin
        result = {/*XLEN*/31'b0, operand_a < operand_b};
      end
      `ALU_SLTU: begin
        result = {/*XLEN*/31'b0, $unsigned(operand_a) < $unsigned(operand_b)};
      end
      `ALU_XOR: begin
        result = operand_a ^ operand_b;
      end
      `ALU_SRL: begin
        result = operand_a >> operand_b[4:0];
      end
      `ALU_SRA: begin
        result = operand_a >>> operand_b[4:0];
      end
      `ALU_OR: begin
        result = operand_a | operand_b;
      end
      `ALU_AND: begin
        result = operand_a & operand_b;
      end
      default begin
        result = {XLEN{1'b0}};
      end
    endcase
  end
endmodule
