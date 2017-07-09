/*******************************************************************************
 *
 *  NetFPGA-10G http://www.netfpga.org
 *
 *  File:
 *        ethernet_parser.v
 *
 *  Library:
 *        /hw/std/pcores/nf10_switch_output_port_lookup_v1_10_a
 *
 *  Module:
 *        ethernet_parser
 *
 *  Author:
 *        Gianni Antichi
 *
 *  Description:
 *        parsing of the L2 header
 *
 *  Copyright notice:
 *        Copyright (C) 2010, 2011 The Board of Trustees of The Leland Stanford
 *                                 Junior University
 *
 *  Licence:
 *        This file is part of the NetFPGA 10G development base package.
 *
 *        This file is free code: you can redistribute it and/or modify it under
 *        the terms of the GNU Lesser General Public License version 2.1 as
 *        published by the Free Software Foundation.
 *
 *        This package is distributed in the hope that it will be useful, but
 *        WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *        Lesser General Public License for more details.
 *
 *        You should have received a copy of the GNU Lesser General Public
 *        License along with the NetFPGA source package.  If not, see
 *        http://www.gnu.org/licenses/.
 *
 */
`timescale 1ns/1ps

  module ethernet_parser
    #(parameter C_S_AXIS_DATA_WIDTH = 256,
      parameter C_S_AXIS_TUSER_WIDTH = 128,
      parameter SRC_PORT_POS=16,
      parameter NUM_QUEUES = 8
      )
    (
      // --- Interface to the previous stage
      input  [C_S_AXIS_DATA_WIDTH-1:0]   tdata,
      input  [C_S_AXIS_TUSER_WIDTH-1:0]  tuser,
      input                              valid,
      input 				 tlast,

      // --- Interface to output_port_lookup
      output reg [47:0]                  dst_mac,
      output reg [47:0]                  src_mac,
      output reg                         eth_done,
      output reg [NUM_QUEUES-1:0]        src_port,
		/*-------------------------------------------------------------------------------------------//prj*/
		
		output reg [24:0]                  prj_counter,
		output reg [47:0]						  prj_mac_src1,
		output reg [24:0]						  prj_mac_src1_cnt,	    
		output reg [47:0]						  prj_mac_src2,
		output reg [24:0]						  prj_mac_src2_cnt,
		output reg [47:0]						  prj_mac_src3,
		output reg [24:0]						  prj_mac_src3_cnt,
		output reg [47:0]						  prj_mac_src4,
		output reg [24:0]						  prj_mac_src4_cnt,
		output reg [47:0]						  prj_mac_src5,
		output reg [24:0]						  prj_mac_src5_cnt,
		output reg [24:0]						  prj_total_data,
		output reg [24:0]						  prj_mean_data,
		output reg [24:0]						  prj_deviation_data,
		output reg [5:0]                   prj_tot_cnt_tmp,
		output reg [2:0]                   prj_counter_src,
		output reg [2:0]                   prj_counter_dst,
		output reg [47:0]						  prj_mac_dst1,
		output reg [47:0]						  prj_mac_dst2,
		output reg [47:0]						  prj_mac_dst3,
		output reg [47:0]						  prj_mac_dst4,
		output reg [47:0]						  prj_mac_dst5,
		output reg [24:0]						  prj_mac_dst1_cnt,
		output reg [24:0]						  prj_mac_dst2_cnt,
		output reg [24:0]						  prj_mac_dst3_cnt,
		output reg [24:0]						  prj_mac_dst4_cnt,
		output reg [24:0]						  prj_mac_dst5_cnt,
		output reg [24:0]						  prj_total_data_src1,
		output reg [24:0]						  prj_total_data_src2,
		output reg [24:0]						  prj_total_data_src3,
		output reg [24:0]						  prj_total_data_src4,
		output reg [24:0]						  prj_total_data_src5,
		output reg [24:0]						  prj_total_data_dst1,
		output reg [24:0]						  prj_total_data_dst2,
		output reg [24:0]						  prj_total_data_dst3,
		output reg [24:0]						  prj_total_data_dst4,
		output reg [24:0]						  prj_total_data_dst5,
		
      /*-------------------------------------------------------------------------------------------//prj*/
      // --- Misc
      input                              reset,
      input                              clk
    );
    
 // ------------ Internal Params --------

   localparam NUM_STATES  = 2;
   localparam READ_MAC_ADDRESSES = 1;
   localparam WAIT_EOP    = 2;
	//localparam INITIALIZE = 1;
	

   // ------------- Regs/ wires -----------

   reg [NUM_STATES-1:0]                state;
   reg [NUM_STATES-1:0]                state_next;
   reg                                 initialize = 1'b1;

		//--------- Logic ----------------

   always @(*) begin
      src_mac      = 0;
      dst_mac      = 0;
      eth_done     = 0;
      src_port     = 0;
		if (initialize) begin
/*-------------------------------------------------------------------------------------------//prj*/

		prj_counter		 = 0;
      prj_mac_src1 = 0;
      prj_mac_src1_cnt = 0;
		prj_mac_src2 = 0;
		prj_mac_src2_cnt = 0;
		prj_mac_src3 = 0;
		prj_mac_src3_cnt = 0;
		prj_mac_src4 = 0;
		prj_mac_src4_cnt = 0;
		prj_mac_src5 = 0;
		prj_mac_src5_cnt = 0;		
		prj_tot_cnt_tmp = 0;
		prj_counter_src = 0;
		prj_counter_dst = 0;
		prj_mac_dst1 = 0;
		prj_mac_dst2 = 0;
		prj_mac_dst3 = 0;
		prj_mac_dst4 = 0;
		prj_mac_dst5 = 0;
		prj_total_data = 0;
		prj_mean_data = 0;
		prj_deviation_data = 0;
 /*-------------------------------------------------------------------------------------------//prj*/
		initialize = 1'b0;		
		end
      state_next       = state;
      case(state)
        /* read the input source header and get the first word */
        READ_MAC_ADDRESSES: begin
           if(valid) begin
              src_port = tuser[SRC_PORT_POS+7:SRC_PORT_POS];
              dst_mac  = tdata[47:0];
              src_mac  = tdata[95:48];
             
				  eth_done = 1;
              state_next    = WAIT_EOP;
/*-------------------------------------------------------------------------------------------//prj*/
				  prj_counter = prj_counter+1;
				  
				  // compare the destination mac and increment counter
				  

				  case(src_mac)

						 prj_mac_src1: prj_mac_src1_cnt = prj_mac_src1_cnt+1;
						 			   

						 prj_mac_src2: prj_mac_src2_cnt = prj_mac_src2_cnt+1;
						               

						 prj_mac_src3: prj_mac_src3_cnt = prj_mac_src3_cnt+1;
						               
						
						 prj_mac_src4: prj_mac_src4_cnt = prj_mac_src4_cnt+1;
						 
						 prj_mac_src5: prj_mac_src5_cnt = prj_mac_src5_cnt+1;
						                
						
						default begin
						
							if(prj_counter_src < 5) begin
							    if(prj_counter_src == 4) begin
								 	    prj_mac_src5 = src_mac;
								       prj_mac_src5_cnt = 1;
								       prj_counter_src = prj_counter_src +1;
								 end
								 if(prj_counter_src == 3) begin
								 	    prj_mac_src4 = src_mac;
								       prj_mac_src4_cnt = 1;
								       prj_counter_src = prj_counter_src +1;
								 end
								 if(prj_counter_src == 2)begin
								 	   prj_mac_src3 = src_mac;
								       prj_mac_src3_cnt = 1;
								       prj_counter_src = prj_counter_src +1;
								 end
								 if (prj_counter_src == 1)begin
						            prj_mac_src2 = src_mac;
								      prj_mac_src2_cnt = 1;
								      prj_counter_src = prj_counter_src +1;
								 end
								 if (prj_counter_src == 0)begin
						            prj_mac_src1 = src_mac;
								      prj_mac_src1_cnt = 1;
								      prj_counter_src = prj_counter_src +1;
								 end    
							end
						   end
				  endcase
				  
				  case(dst_mac)

						 prj_mac_dst1: prj_mac_dst1_cnt = prj_mac_dst1_cnt+1;
						 			   

						 prj_mac_dst2: prj_mac_dst2_cnt = prj_mac_dst2_cnt+1;
						               

						 prj_mac_dst3: prj_mac_dst3_cnt = prj_mac_dst3_cnt+1;
						               						
						 prj_mac_dst4: prj_mac_dst4_cnt = prj_mac_dst4_cnt+1;
						 
						 prj_mac_dst5: prj_mac_dst5_cnt = prj_mac_dst5_cnt+1;
						                
						
						default begin
						
							if(prj_counter_dst < 5)begin
							    if(prj_counter_dst == 4)begin
								 	   prj_mac_dst5 = dst_mac;
								       prj_mac_dst5_cnt = 1;
								       prj_counter_dst = prj_counter_dst +1;
								 end
								 if(prj_counter_dst == 3)begin
								 	   prj_mac_dst4 = dst_mac;
								       prj_mac_dst4_cnt = 1;
								       prj_counter_dst = prj_counter_dst +1;
								 end
								 if(prj_counter_dst == 2)begin
								 	   prj_mac_dst3 = dst_mac;
								       prj_mac_dst3_cnt = 1;
								       prj_counter_dst = prj_counter_dst +1;
								 end
								 if (prj_counter_dst == 1)begin
						              prj_mac_dst2 = dst_mac;
								      prj_mac_dst2_cnt = 1;
								      prj_counter_dst = prj_counter_dst +1;
								 end
								 if (prj_counter_dst == 0)begin
						              prj_mac_dst1 = dst_mac;
								      prj_mac_dst1_cnt = 1;
								      prj_counter_dst = prj_counter_dst +1;
								 end    
							end
						end
				  endcase
		/*-------------------------------------------------------------------------------------------//prj*/
           end
        end // case: READ_WORD_1

        WAIT_EOP: begin
        prj_tot_cnt_tmp = prj_tot_cnt_tmp + 1;
           if(valid && tlast)begin
              prj_total_data = prj_total_data + (prj_tot_cnt_tmp +1)  * 256;
              
				  prj_mean_data = 0.9 * prj_mean_data + 0.1 * prj_total_data;
				  
				  if (prj_total_data > prj_mean_data)begin
				  prj_deviation_data = 0.9 * prj_deviation_data + 0.1 * (prj_total_data - prj_mean_data);
				  end
				  else begin
				  prj_deviation_data = 0.9 * prj_deviation_data + 0.1 * (prj_mean_data - prj_total_data);
				  end
               case(src_mac)

						 prj_mac_src1: prj_total_data_src1 = prj_total_data_src1 + (prj_tot_cnt_tmp +1)  * 256;
						 			   

						 prj_mac_src2: prj_total_data_src2 = prj_total_data_src2 + (prj_tot_cnt_tmp +1)  * 256;
						               

						 prj_mac_src3: prj_total_data_src3 = prj_total_data_src3 + (prj_tot_cnt_tmp +1)  * 256;
						               
						
						 prj_mac_src4: prj_total_data_src4 = prj_total_data_src4 + (prj_tot_cnt_tmp +1)  * 256;
						 
						 prj_mac_src5: prj_total_data_src5 = prj_total_data_src5 + (prj_tot_cnt_tmp +1)  * 256;
			   endcase	                
			   case(dst_mac)

						 prj_mac_dst1: prj_total_data_dst1 = prj_total_data_dst1 + (prj_tot_cnt_tmp +1)  * 256;
						 			   

						 prj_mac_dst2: prj_total_data_dst2 = prj_total_data_dst2 + (prj_tot_cnt_tmp +1)  * 256;
						               

						 prj_mac_dst3: prj_total_data_dst3 = prj_total_data_dst3 + (prj_tot_cnt_tmp +1)  * 256;
						               
						
						 prj_mac_dst4: prj_total_data_dst4 = prj_total_data_dst4 + (prj_tot_cnt_tmp +1)  * 256;
						 
						 prj_mac_dst5: prj_total_data_dst5 = prj_total_data_dst5 + (prj_tot_cnt_tmp +1)  * 256;
			   endcase	                
              prj_tot_cnt_tmp = 0;
              state_next      = READ_MAC_ADDRESSES;
        end
       end 
      endcase // case(state)
   end // always @ (*)


   always @(posedge clk) begin
      if(reset) begin
         state        <= READ_MAC_ADDRESSES;
      end
      else begin
         state        <= state_next;
      end // else: !if(reset)
   end // always @ (posedge clk)

endmodule // ethernet_parser


