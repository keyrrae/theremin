module widthDet(vin, reset, clk, out);

input vin;
output [12:0] out;

reg [12:0] width

reg state;
reg next_state;

parameter IDLE = 0;
parameter COUNT = 1;


always @(posedge clk) begin
    if(reset) begin
        state <= IDLE;
        counter <= 0;
        width <=0;
        out <= 300;
    end

    else if(counter == 100) begin
        counter <= 0;

        case(state)
            IDLE: begin
                if (vin == 1'b1) begin
                    next_state <= count;
                    width = 1;
                end
            COUNT: begin
                if(vin == 1'b1)
                    width <= width + 1;
                else begin
                    next_state <= IDLE;
                    out <= width;
                end

            default: begin
                next_state <= IDLE;
                counter <= 0;
                width <= 0;
                out <= 300;
            end
        endcase
        state <= next_state;
    end


end

endmodule
