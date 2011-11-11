module Debouncer (clk, dirty, clean);
    input clk;
    input dirty;
    output reg clean;

    reg [6:0] debounce;
    reg last_dirty;

    always @ (posedge clk) begin
        if (dirty != last_dirty) begin 
            last_dirty <= dirty; 
            debounce <= 0; 
        end else if (debounce == 127)  begin
            clean <= last_dirty;
        end else begin
            debounce <= debounce+1;
        end
    end
endmodule

