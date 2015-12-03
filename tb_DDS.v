`timescale 1ns/1ns
module tb_DDS;

reg[31:0] dfreq;
reg[31:0] clkfreq;
reg clk;
reg reset;
reg [31:0] tuningWord;
wire unsigned [10:0] sine_out;
wire pwm_out;
integer unsigned index;
reg [7:0] scale;

DDS sine(.clock(clk), .reset(reset), .tuningWord(tuningWord), .scale(scale), .sine_out(sine_out), .pwm_out(pwm_out));

initial begin
		clk=1'b0;
		reset=1'b0;
		dfreq = 100;
	scale = 10;
		clkfreq = 100000;
index = 0;
		
		tuningWord = 8796115; 
/// clkfreq;
		#5 reset = 1'b1;
		#20 reset = 1'b0;
	end
	
	//Toggle the clocks
	always begin
		#5
		clk  = ~clk;
	end

always @(posedge clk)
index <= index + 1;

endmodule
