module Encoder(clk, enc, count);
	input clk;
	input enc;
	reg enc_clean, enc_new;
	reg [7:0] debounce;
	output [15:0] count;
	reg [15:0] count;

	always @ (posedge clk)
     if (enc != enc_new) begin enc_new <= enc; debounce <= 0; end
     else if (debounce == 127) enc_clean <= enc_new;
     else debounce <= debounce+1;

	always @ (posedge enc_clean)
		count <= count + 1;
	
endmodule
