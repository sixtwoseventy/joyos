`include "Motor.v"
`include "Servo.v"
`include "Encoder.v"

module happyio(clk, ad, a, aout, ale, nRD, nWR, mot0, mot1, mot2, mot3, mot4, mot5, Servo, Enc, Digital, ramce);
	// clock
	input clk;
	// AVR XMEM interface
	input  [7:0] a;
	inout  [7:0] ad;
	input ale;
	input nRD, nWR;
	// address [0..7] output
	output [7:0] aout;
	// IO Pins
	output [1:0] mot0;
	output [1:0] mot1;
	output [1:0] mot2;
	output [1:0] mot3;
	output [1:0] mot4;
	output [1:0] mot5;
	input  [3:0] Enc;
	input  [7:0] Digital;
	output [5:0] Servo;
	output ramce;

	// internal
	reg    [15:0] addr;
	wire	 [7:0] aout;
	wire   [7:0] data;
	reg    [7:0] dataOut;

	// registers (motors)
	reg [1:0] ma1_ctl;
	reg [7:0] ma1_vel;
	reg [1:0] ma2_ctl;
	reg [7:0] ma2_vel;
	reg [1:0] mb1_ctl;
	reg [7:0] mb1_vel;
	reg [1:0] mb2_ctl;
	reg [7:0] mb2_vel;
	reg [1:0] mc1_ctl;
	reg [7:0] mc1_vel;
	reg [1:0] mc2_ctl;
	reg [7:0] mc2_vel;
	
	// registers (encoders)
	wire [15:0] enc0;
	wire [15:0] enc1;
	wire [15:0] enc2;
	wire [15:0] enc3;
	
	// registers (servos)
	reg [9:0] srv0;
	reg [9:0] srv1;
	reg [9:0] srv2;
	reg [9:0] srv3;
	reg [9:0] srv4;
	reg [9:0] srv5;

	reg [7:0] tempLo;
	reg [7:0] tempHi;

	// bidirectional data bus
	assign ad = nRD ? 8'hzz : dataOut;
	
	// latch the lower 8 bits of address
	assign data = ad;
	always @(negedge ale) begin
		addr[7:0] = ad[7:0];
		addr[15:8] = a[7:0];
	end

	assign aout[7:0]  = addr[7:0];

	// assign ram ce	
	always @ (addr[15] or nRD or nWR) begin
		if (nRD & nWR) begin
			ramce = 1;
		end
		if (ramce) begin
			ramce = ~addr[15]; 	
		end
	end 

	// read control
	always @ (negedge nRD)
	begin
		case (addr)
			// 0x1100 - 0x110B : motors
			16'h1100:	dataOut[1:0] = ma1_ctl;
			16'h1101:	dataOut = ma1_vel;
			16'h1102:	dataOut[1:0] = ma2_ctl;
			16'h1103:	dataOut = ma2_vel;
			16'h1104:	dataOut[1:0] = mb1_ctl;
			16'h1105:	dataOut = mb1_vel;
			16'h1106:	dataOut[1:0] = mb2_ctl;
			16'h1107:	dataOut = mb2_vel;
			16'h1108:	dataOut[1:0] = mc1_ctl;
			16'h1109:	dataOut = mc1_vel;
			16'h110A:	dataOut[1:0] = mc2_ctl;
			16'h110B:	dataOut = mc2_vel;
			// 0x110C - 0x1113 : encoders
			16'h110C:	{tempHi, dataOut} = enc0;
			16'h110D:	dataOut = tempHi;
			16'h110E:	{tempHi, dataOut} = enc1;
			16'h110F:	dataOut = tempHi;
			16'h1110:	{tempHi, dataOut} = enc2;
			16'h1111:	dataOut = tempHi;
			16'h1112:	{tempHi, dataOut} = enc3;
			16'h1113:	dataOut = tempHi;
			// 0x1120 - 0x112B : servos
			/*
			16'h1118:	dataOut = srv0;
			16'h1119:	dataOut = srv1;
			16'h111A:	dataOut = srv2;
			16'h111B:	dataOut = srv3;
			16'h111C:	dataOut = srv4;
			16'h111D:	dataOut = srv5;
			*/
			// 0x11 : digital in
			16'h111E:	dataOut = Digital;
			// 0x11FE : major version
			16'h11FE:	dataOut = 0;
			// 0x11FF : minor version
			16'h11FF:	dataOut = 6;
		endcase
	end
	
	// write control
	always @ (negedge nWR or posedge clk)
	begin 
		if (!nWR)
		case (addr)
			// 0x1100 - 0x110B : motors
			16'h1100:	ma1_ctl = data;
			16'h1101:	ma1_vel = data;
			16'h1102:	ma2_ctl = data;
			16'h1103:	ma2_vel = data;
			16'h1104:	mb1_ctl = data;
			16'h1105:	mb1_vel = data;
			16'h1106:	mb2_ctl = data;
			16'h1107:	mb2_vel = data;
			16'h1108:	mc1_ctl = data;
			16'h1109:	mc1_vel = data;
			16'h110A:	mc2_ctl = data;
			16'h110B:	mc2_vel = data;
			// 0x110C - 0x1113 : encoders
			// ...
			// 0x1120 - 0x112B : servos
			16'h1120:	tempLo = data;
			16'h1121: srv0 = {data[1:0],tempLo};
			16'h1122:	tempLo = data;
			16'h1123: srv1 = {data[1:0],tempLo};
			16'h1124:	tempLo = data;
			16'h1125: srv2 = {data[1:0],tempLo};
			16'h1126:	tempLo = data;
			16'h1127: srv3 = {data[1:0],tempLo};
			16'h1128:	tempLo = data;
			16'h1129: srv4 = {data[1:0],tempLo};
			16'h112A:	tempLo = data;
			16'h112B: srv5 = {data[1:0],tempLo};
			// ...
		endcase
	end
	

	// motor drivers
	Motor motor0(clk,mot0,ma2_ctl,ma2_vel);
	Motor motor1(clk,mot1,ma1_ctl,ma1_vel);
	Motor motor2(clk,mot2,mb2_ctl,mb2_vel);
	Motor motor3(clk,mot3,mb1_ctl,mb1_vel);
	Motor motor4(clk,mot4,mc2_ctl,mc2_vel);
	Motor motor5(clk,mot5,mc1_ctl,mc1_vel);

	// encoder drivers
	Encoder encoder0(clk,Enc[0],enc0);
	Encoder encoder1(clk,Enc[1],enc1);
	Encoder encoder2(clk,Enc[2],enc2);
	Encoder encoder3(clk,Enc[3],enc3);

	// servo drivers
	Servo servo0(clk,Servo[0],srv0);
	Servo servo1(clk,Servo[1],srv1);
	Servo servo2(clk,Servo[2],srv2);
	Servo servo3(clk,Servo[3],srv3);
	Servo servo4(clk,Servo[4],srv4);
	Servo servo5(clk,Servo[5],srv5);

endmodule
