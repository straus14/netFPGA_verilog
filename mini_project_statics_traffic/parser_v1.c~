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
		output reg 								  cond_src,
		output reg 								  cond_dst,
		output reg [24:0]                  prj_counter,
		output reg [99:0]						  prj_stored_src_mac,
		output reg [99:0]						  prj_stored_dst_mac,
		output reg [47:0]						  prj_mac1,
		output reg [24:0]						  prj_mac1_src_cnt,	    
		output reg [47:0]						  prj_mac2,
		output reg [24:0]						  prj_mac2_src_cnt,
		output reg [47:0]						  prj_mac3,
		output reg [24:0]						  prj_mac3_src_cnt,
		output reg [47:0]						  prj_mac4,
		output reg [24:0]						  prj_mac4_src_cnt,
      /*-------------------------------------------------------------------------------------------//prj*/
      // --- Misc
      input                              reset,
      input                              clk
    );
    
 // ------------ Internal Params --------

   localparam NUM_STATES  = 2;
   localparam READ_MAC_ADDRESSES = 1;
   localparam WAIT_EOP    = 2;

   // ------------- Regs/ wires -----------

   reg [NUM_STATES-1:0]                state;
   reg [NUM_STATES-1:0]                state_next;


   // ------------ Logic ----------------

   always @(*) begin
      src_mac      = 0;
      dst_mac      = 0;
      eth_done     = 0;
      src_port     = 0;
		/*-------------------------------------------------------------------------------------------//prj*/
		cond_src		 = 0;
		cond_dst		 = 0;
		prj_stored_src_mac		= 0;
		prj_stored_dst_mac		= 0;
		prj_counter		 = 0;
      	prj_mac1 = 0;
      	prj_mac1_src_cnt = 0;
		prj_mac2 = 0;
		prj_mac2_src_cnt = 0;
		prj_mac3 = 0;
		prj_mac3_src_cnt = 0;
		prj_mac4 = 0;
		prj_mac4_src_cnt = 0;
		/*-------------------------------------------------------------------------------------------//prj*/
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
				  
				  // compare the source mac and increment counter
/*				  if(prj_stored_src_mac == 0) 
							cond_src = 0;
				  else
							cond_src = 1;
				  end
				  if(prj_stored_dst_mac == 0) 
							cond_dst = 0;
				  else
							cond_dst = 1;
				  end				  
				  switch(cond_src)
						case 0:
							//store first mac
						case 1:
				  end
				  
				  switch(cond_dst)
						case 0:
							//store first mac
							
						case 1:
				  end
*/
				  case(src_mac)

						 prj_mac1: prj_mac1_src_cnt = prj_mac1_src_cnt+1;	

						 prj_mac2: prj_mac2_src_cnt = prj_mac2_src_cnt+1;

						 prj_mac3: prj_mac3_src_cnt = prj_mac3_src_cnt+1;
						
						 prj_mac4: prj_mac4_src_cnt = prj_mac4_src_cnt+1;
						
						//default:// (prj_mac1 == 0) ? prj_mac1=src_mac:( (prj_mac2 == 0) ? (prj_mac2= src_mac):((prj_mac3 == 0) ? (prj_mac3= src_mac):(prj_mac4 = src_mac) )  );
								
				  endcase
		/*-------------------------------------------------------------------------------------------//prj*/
           end
        end // case: READ_WORD_1

        WAIT_EOP: begin
           if(valid && tlast)
              state_next      = READ_MAC_ADDRESSES;
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


