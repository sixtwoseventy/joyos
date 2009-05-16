module Motor(clk, out, ctl, vel);
	input clk;
	output [1:0] out;
	reg pwmout;
	reg [7:0] pwmcount;
	input [7:0] vel;
	input [1:0] ctl;
	
	always @ (posedge clk)
	begin
		pwmcount <= pwmcount + 1;
		if (pwmcount==0)
			pwmout = 1;
		else
		if (pwmcount==vel)
			pwmout = 0;
	end
	
	
	wire fwd = (ctl==2'b01);	
	wire rev = (ctl==2'b10);	
//	wire brake = (ctl==2'b00);

	assign out[0] = fwd ? pwmout : 0;
	assign out[1] = rev ? pwmout : 0;
	
endmodule
