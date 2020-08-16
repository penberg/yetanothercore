// RISC-V instruction execution unit.

module insn_exec #(
  parameter XLEN
) (
  input logic  [6:0]      opcode,
  input logic  [2:0]      funct3,
  input logic  [6:0]      funct7,
  input logic  [19:0]     imm,
  input logic  [XLEN-1:0] rs1,
  input logic  [XLEN-1:0] rs2,
  output logic            rd_enable_write,
  output logic [XLEN-1:0] rd
);

  always @ (*) begin
    logic [9:0] funct10 = {funct7, funct3};

    //$display("opcode=", opcode);
    //$display("funct3=", funct3);
    //$display("funct7=", funct7);
    //$display("funct10=", funct10);
 
    case (opcode)
      7'b0110111: begin
        $display("lui %h", imm);
        rd = {{12{1'b0}}, imm};
        rd_enable_write = 1;
      end
      7'b0110011: begin
        rd_enable_write = 1;
        case (funct10)
          10'b0000000000: begin
            $display("add %h, %h", rs1, rs2);
            rd = rs1 + rs2;
          end
          10'b0100000000: begin
            $display("sub %h, %h", rs1, rs2);
            rd = rs1 - rs2;
          end
          10'b0000000001:
            $display("sll");
          10'b0000000010:
            $display("slt");
          10'b0000000011:
            $display("sltu");
          10'b0000000100: begin
            $display("xor %h, %h", rs1, rs2);
            rd = rs1 ^ rs2;
          end
          10'b0000000101:
            $display("srl");
          10'b0100000101:
            $display("sra");
          10'b0000000110: begin
            $display("or %h, %h", rs1, rs2);
            rd = rs1 | rs2;
          end
          10'b0000000111: begin
            $display("and %h, %h", rs1, rs2);
            rd = rs1 & rs2;
          end
          default begin
            // FIXME
          end
        endcase
      end
      default begin
          // FIXME
      end
    endcase
  end
endmodule
