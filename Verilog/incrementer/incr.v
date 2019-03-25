`ifndef INCR
`define INCR


module reg_incr(
	input clk,
	input rst,
	input [7:0] in,
	output [7:0] out
);

reg [7:0] reg_out;
always @(posedge clk) begin
	if(rst)
		reg_out <= 0;
	else
		reg_out <= in + 1;
end

reg [7:0] tmp;
always @(*) begin
	tmp = reg_out;
end

assign out = tmp;
endmodule

`endif
