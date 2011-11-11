module Pwm(clk, out, val);
	input clk;
	output reg out;
	reg [7:0] pwmcount = 0;
	input [7:0] val;
	
	always @ (posedge clk)
	begin
		pwmcount <= pwmcount + 1;
		if (pwmcount==0)
			out <= 1;
		if (pwmcount==val)
			out <= 0;
	end
endmodule
