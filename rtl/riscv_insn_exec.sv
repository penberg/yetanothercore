// RISC-V instruction execution unit.

module riscv_insn_exec #(
  parameter XLEN
) (
  input             clock,
  input  [6:0]      opcode,
  input  [2:0]      funct3,
  input  [6:0]      funct7,
  input  [XLEN-1:0] rs1,
  input  [XLEN-1:0] rs2,
  output [XLEN-1:0] rd
);

  always @ (posedge clock) begin
    logic [9:0] funct10 = {funct7, funct3};

    //$display("opcode=", opcode);
    //$display("funct3=", funct3);
    //$display("funct7=", funct7);
    //$display("funct10=", funct10);
 
    case (opcode)
      7'b0110111:
        $display("lui");
      7'b0110011:
        case (funct10)
          10'b0000000000: begin
            $display("add");
            rd <= rs1 + rs2;
          end
          10'b0100000000: begin
            $display("sub");
            rd <= rs1 - rs2;
          end
          10'b0000000001:
            $display("sll");
          10'b0000000010:
            $display("slt");
          10'b0000000011:
            $display("sltu");
          10'b0000000100: begin
            $display("xor");
            rd <= rs1 ^ rs2;
          end
          10'b0000000101:
            $display("srl");
          10'b0100000101:
            $display("sra");
          10'b0000000110: begin
            $display("or");
            rd <= rs1 | rs2;
          end
          10'b0000000111: begin
            $display("and");
            rd <= rs1 & rs2;
          end
          default begin
            // FIXME
          end
        endcase
      default begin
          // FIXME
      end
    endcase
  end
endmodule
