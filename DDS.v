module DDS (clock, reset, tuningWord, scale, sine_out,pwm_out);
input clock, reset;
input [31:0] tuningWord;
input [7:0]  scale;
wire [15:0] lookup;
reg [10:0] counter;
reg signed[31:0] buffer;


output reg pwm_out;
output wire [10:0] sine_out;
reg [31:0]	accumulator;

always@(posedge clock) begin
if(reset) begin
counter <=0;
pwm_out <=1;
accumulator <= 0;
end

else if(counter == 0) begin
pwm_out <= 1;
counter <= counter + 1;
accumulator <= accumulator + tuningWord;
end

else begin
if(counter > sine_out)
	pwm_out <=0;
counter <= counter + 1;
end

end


always@(*) begin
	buffer = (lookup * scale) >> 13;
end

// link the accumulator to the sine lookup table
sync_rom sineTable(clock, accumulator[31:24], lookup);

assign sine_out = buffer;//* scale) >> 8;




endmodule
