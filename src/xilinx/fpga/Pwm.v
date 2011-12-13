module Pwm(clk, out, val);
	input clk;
	output reg out;
	reg [7:0] pwmcount = 0;
	input [7:0] val;
	
	always @ (posedge clk) begin
		pwmcount <= pwmcount + 1;
        if (val == 0) begin
            out <= 0;
        end else if (val == 255) begin
            out <= 1;
        end else if (pwmcount==0) begin
			out <= 1;
		end else if (pwmcount==val) begin
			out <= 0;
        end
	end
endmodule
