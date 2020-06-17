// RISC-V instruction decoder

`include "constants.sv"

module insn_decode(
  input  [31:0] insn,
  output [3:0]  alu_op,
  output [4:0]  rd,
  output [4:0]  rs1,
  output [4:0]  rs2,
  output        rd_enable_write,
  output        immediate_select,
  output [31:0] immediate
);

  assign rd     = insn[11:7];
  assign rs1    = insn[19:15];
  assign rs2    = insn[24:20];

  logic [6:0]  opcode = insn[6:0];
  logic [2:0]  funct3 = insn[14:12];
  logic [6:0]  funct7 = insn[31:25];
  logic [19:0] imm    = insn[31:12];

  always @ (*) begin
    logic [9:0] funct10 = {funct7, funct3};
 
    case (opcode)
      7'b0110111: begin // LUI
        immediate = {{12{1'b0}}, imm};
	immediate_select = 1;
        rd_enable_write = 1;
      end
      7'b0110011: begin
        rd_enable_write = 1;
        case (funct10)
          10'b0000000000: begin
            alu_op = `ALU_ADD;
          end
          10'b0100000000: begin
            alu_op = `ALU_SUB;
          end
          10'b0000000001: begin
            alu_op = `ALU_SLL;
          end
          10'b0000000010: begin
            alu_op = `ALU_SLT;
          end
          10'b0000000011: begin
            alu_op = `ALU_SLTU;
          end
          10'b0000000100: begin
            alu_op = `ALU_XOR;
          end
          10'b0000000101: begin
            alu_op = `ALU_SRL;
          end
          10'b0100000101: begin
            alu_op = `ALU_SRA;
          end
          10'b0000000110: begin
            alu_op = `ALU_OR;
          end
          10'b0000000111: begin
            alu_op = `ALU_AND;
          end
          default: begin
            alu_op = 4'b0;
          end
        endcase
      end
      default: begin
        alu_op = 4'b0;
      end
    endcase
  end

endmodule
