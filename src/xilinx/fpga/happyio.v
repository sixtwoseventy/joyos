`include "Motor.v"
`include "Servo.v"
`include "Encoder.v"
`include "Debouncer.v"
`include "Quadrature.v"
`include "Pwm.v"

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
	inout  [7:0] Digital;
	output [5:0] Servo;
	output ramce;

	
	// tri-state digital IO
	reg [7:0] digitalPinMode = 8'h00;
	wire [7:0] digitalOutput;
	reg [7:0] digitalPwm [7:0];

	assign Digital[0] = digitalPinMode[0] ? digitalOutput[0] : 1'bz;
	assign Digital[1] = digitalPinMode[1] ? digitalOutput[1] : 1'bz;
	assign Digital[2] = digitalPinMode[2] ? digitalOutput[2] : 1'bz;
	assign Digital[3] = digitalPinMode[3] ? digitalOutput[3] : 1'bz;
	assign Digital[4] = digitalPinMode[4] ? digitalOutput[4] : 1'bz;
	assign Digital[5] = digitalPinMode[5] ? digitalOutput[5] : 1'bz;
	assign Digital[6] = digitalPinMode[6] ? digitalOutput[6] : 1'bz;
	assign Digital[7] = digitalPinMode[7] ? digitalOutput[7] : 1'bz;

	//reg ramce;

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
    reg srv0_e;
    reg srv1_e;
    reg srv2_e;
    reg srv3_e;
    reg srv4_e;
    reg srv5_e;

	reg [7:0] tempLo;
	reg [7:0] tempHi;

	// bidirectional data bus
	 assign ad = (addr[15] | nRD ) ? 8'hzz : dataOut ;
	//assign ad = 8'hzz;
	
	// latch the lower 8 bits of address
	assign data = ad;
	always @(negedge ale) begin
			addr[7:0] = ad[7:0];
			addr[15:8] = a[7:0];
	end

	assign aout[7:0]  = addr[7:0];

	// assign ram ce	
	assign ramce = ~addr[15];
	//always @ (addr[15] or nRD or nWR) begin
	//	ramce = ~addr[15];
	//end 

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
			16'h11FF:	dataOut = 7;
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
			16'h1121: 	begin
							srv0 = {data[1:0],tempLo}; 
							srv0_e = data[7];
						end
			16'h1122:	tempLo = data;
			16'h1123: 	begin
							srv1 = {data[1:0],tempLo}; 
							srv1_e = data[7];
						end
			16'h1124:	tempLo = data;
			16'h1125: 	begin
							srv2 = {data[1:0],tempLo}; 
							srv2_e = data[7];
						end
			16'h1126:	tempLo = data;
			16'h1127: 	begin
							srv3 = {data[1:0],tempLo}; 
							srv3_e = data[7];
						end
			16'h1128:	tempLo = data;
			16'h1129: 	begin
							srv4 = {data[1:0],tempLo}; 
							srv4_e = data[7];
						end
			16'h112A:	tempLo = data;
			16'h112B: 	begin
							srv5 = {data[1:0],tempLo}; 
							srv5_e = data[7];
						end


			// Digital I/O mode
			16'h1130:	digitalPinMode = data;

			// Digital Output
			16'h1131:	digitalPwm[0] = data;
			16'h1132:	digitalPwm[1] = data;
			16'h1133:	digitalPwm[2] = data;
			16'h1134:	digitalPwm[3] = data;
			16'h1135:	digitalPwm[4] = data;
			16'h1136:	digitalPwm[5] = data;
			16'h1137:	digitalPwm[6] = data;
			16'h1138:	digitalPwm[7] = data;
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
    `ifndef QUADRATURE
	Encoder encoder0(clk,Enc[0],enc0);
	Encoder encoder1(clk,Enc[1],enc1);
	Encoder encoder2(clk,Enc[2],enc2);
	Encoder encoder3(clk,Enc[3],enc3);
    `else
	Quadrature quad0(clk,Enc[0],Enc[1], enc0);
	Quadrature quad1(clk,Enc[2],Enc[3], enc1);
    `endif

	// servo drivers
	Servo servo0(clk,Servo[0],srv0, srv0_e);
	Servo servo1(clk,Servo[1],srv1, srv1_e);
	Servo servo2(clk,Servo[2],srv2, srv2_e);
	Servo servo3(clk,Servo[3],srv3, srv3_e);
	Servo servo4(clk,Servo[4],srv4, srv4_e);
	Servo servo5(clk,Servo[5],srv5, srv5_e);

	// digital IO
	Pwm pwm0(clk, digitalOutput[0], digitalPwm[0]);
	Pwm pwm1(clk, digitalOutput[1], digitalPwm[1]);
	Pwm pwm2(clk, digitalOutput[2], digitalPwm[2]);
	Pwm pwm3(clk, digitalOutput[3], digitalPwm[3]);
	Pwm pwm4(clk, digitalOutput[4], digitalPwm[4]);
	Pwm pwm5(clk, digitalOutput[5], digitalPwm[5]);
	Pwm pwm6(clk, digitalOutput[6], digitalPwm[6]);
	Pwm pwm7(clk, digitalOutput[7], digitalPwm[7]);

endmodule
