module Servo(clk, out, pos);
	input clk;
	output out;
	reg out;
	input [9:0] pos;

	// fpga4fun
	parameter ClkDiv = 31;
	parameter PulseCountSize = 11;

	reg [6:0] ClkCount;
	reg [PulseCountSize:0] PulseCount;
	reg ClkTick;

	always @(posedge clk) 
		ClkTick <= (ClkCount==ClkDiv-2);

	always @(posedge clk) 
		if(ClkTick) 
			ClkCount <= 0; 
		else 
			ClkCount <= ClkCount + 1;

	always @(posedge clk) 
		if(ClkTick) 
			PulseCount <= PulseCount + 1;
	
	always @(posedge clk) 
		out = (PulseCount < {2'b00, pos});
	// fpga4fun

endmodule
