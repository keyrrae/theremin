module widthDet(vin, reset, clk, out);

input vin;
input clk;
input reset;
output [12:0] out;
reg [12:0] out;

reg [12:0] width;
reg [6:0] counter;

reg state;
reg next_state;

parameter IDLE = 0;
parameter COUNT = 1;


always @(posedge clk) begin
    if(reset) begin
        state <= IDLE;
        counter <= 1;
        width <=0;
        out <= 300;
	next_state <= IDLE;
    end

    else if(counter == 99) begin
        counter <= 0;

        case(state)
            IDLE: begin
                if (vin == 1'b1) begin
                    state <= COUNT;
                    width = 1;
                end
	    end
            COUNT: begin
                if(vin == 1'b1)
                    width <= width + 1;
                else begin
                    state <= IDLE;
                    out <= width;
                end
	    end
            default: begin
                state <= IDLE;
                counter <= 1;
                width <= 0;
                out <= 300;
            end
        endcase
        
    end
    else begin
	counter <= counter + 1;
end
	

end

endmodule
