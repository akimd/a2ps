// This module checks for BDVL deasserting when BEPVL is asserted.

module epvl_chkr(clk, bpvl, bepvl, bdvl, stop);
    input clk;
    input bpvl;
    input bepvl;
    input bdvl;
    input stop;         // $stop (or $finish if BATCH) on error

`ifdef JUST_COUNT_PACKETS
    // Do nothing
`else
    integer status;
`endif // ifdef JUST_COUNT_PACKETS

   always @(posedge clk) begin
      if (!bpvl && bepvl && bdvl) begin
         $display("%%E-%M, %0t BDVL cannot be deasserted with BEPVL",
                  $time);
         if (stop) begin
            #1000
`ifdef JUST_CHECK_COUNTS
            // Do nothing
`else
            status = $check_counts;
`endif // !ifdef JUST_CHECK_COUNTS
            $display("%%I-%M, %0t Stopping simulation on error!", $time);
`ifdef BATCH
            $finish;
`else
            $stop;
`endif // !ifdef BATCH
         end // if (backplane.stop_on_error)

      end // if (!bpvl && bepvl && bdvl)
   end // always @ (posedge clk)

endmodule // epvl_chkr
