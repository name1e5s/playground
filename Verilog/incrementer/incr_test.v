`timescale 1ns/1ns
`include "incr.v"

module top;

logic clk = 1;
always #1 clk = ~clk;

logic rst = 1;
logic [7:0]  in;
logic [7:0] out;

reg_incr r(
	.clk (clk),
	.rst (rst),
	.in  (in),
	.out (out)
);

initial begin
	$dumpfile("incr_sim.vcd");
	$dumpvars;

	#20;
	rst = 1'b0;

	in = 8'h00;
	
	#10;
	if(out != 8'h01) begin
		$display("ERROR");
		$finish;
	end
	in = 8'h13;
	
	#10;
 	if ( out != 8'h14 ) begin
	$display( "ERROR: out, expected = %x, actual = %x", 8'h14, out );
	$finish;
	end

	in = 8'h27;
	#10;
 	if ( out != 8'h28 ) begin
 		$display( "ERROR: out, expected = %x, actual = %x", 8'h28, out );
 		$finish;
 	end

	$display( "*** PASSED ***" );
	$finish;
end
endmodule
