`timescale 1ns/1ns

module tb_width;

reg vin, reset, clk;
wire [12:0] out;

widthDet DUT(.vin(vin), .reset(reset), .clk(clk), .out(out));

initial begin
    clk = 0;
    reset = 0
    vin = 0;
    #5 reset = 1;
    #30 reset = 0;

    #500 vin = 1;
    #300000 vin = 0;

    #500000 vin = 1;
    #2500000 vin = 0;

    #500000 vin = 1;
    #5000000 vin = 0;

end

always #5 clk = ~clk;


endmodule
