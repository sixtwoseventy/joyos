module Servo(clk, out, pos, enable);
	input clk;
	output out;
	reg out;
	input [9:0] pos;
    input enable;

	// fpga4fun
	parameter ClkDiv = 31;
	parameter PulseCountSize = 11;

	reg [6:0] ClkCount;
	reg [PulseCountSize:0] PulseCount;
	reg ClkTick;

	always @(posedge clk) begin
		ClkTick <= (ClkCount==ClkDiv-2);

		if(ClkTick) 
			ClkCount <= 0; 
		else 
			ClkCount <= ClkCount + 1;

		if(ClkTick) 
			PulseCount <= PulseCount + 1;
	
		out = enable ? (PulseCount < {2'b00, pos}) : 0;
    end
	// fpga4fun

endmodule
