module Quadrature(clk, enc_a, enc_b, count);
    input clk;
    input enc_a;
    input enc_b;
    output count;
    
    reg signed [15:0] count;

    wire [1:0] cur_state;
    reg [1:0] last_state;

    // Debounce encoder inputs
    Debouncer dbc_a(clk, enc_a, cur_state[0]);
    Debouncer dbc_b(clk, enc_b, cur_state[1]);


    always @ (posedge clk) begin
        case ({last_state, cur_state})
            4'b0001: count <= count + 1;
            4'b0111: count <= count + 1;
            4'b1110: count <= count + 1;
            4'b1000: count <= count + 1;

            4'b0010: count <= count - 1;
            4'b1011: count <= count - 1;
            4'b1101: count <= count - 1;
            4'b0100: count <= count - 1;
        endcase
        last_state <= cur_state;
    end
endmodule
