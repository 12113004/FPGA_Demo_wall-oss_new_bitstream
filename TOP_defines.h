///////////////////     Parallelism    ////////////////////
//FPGA define
#define V80    0
#define VCU128 1
#define Other  2
#define FPGA VCU128
//#define Bitstream

#define MAX_DW                      16
#define log2_MAX_DW                 (log2(MAX_DW))
                                    
#if FPGA == V80                          
    #define HBM_Port                64
    #define HBM_AXI_DATA_WIDTH      256
    #define HBM_ADDR_WIDTH          40
    #define SINGLE_HBM_DEPTH        29
    #define DDR_ADDR_WIDTH          44
    #define Tout                    64
    #define L_Tout                  (HBM_Port*HBM_AXI_DATA_WIDTH/MAX_DW)
    #define AXI_BURST_LEN           64
    #define WT_AXI_BURST_LEN        128
#elif FPGA == VCU128                       
    #define HBM_Port                32
    #define HBM_AXI_DATA_WIDTH      256
    #define HBM_ADDR_WIDTH          37
    #define SINGLE_HBM_DEPTH        28
    #define DDR_ADDR_WIDTH          32
    #define Tout                    32
    #define L_Tout                  (HBM_Port*HBM_AXI_DATA_WIDTH/MAX_DW)
    #define AXI_BURST_LEN           32
    #define WT_AXI_BURST_LEN        256
#else                               
    #define HBM_Port                16
    #define HBM_AXI_DATA_WIDTH      512
    #define HBM_ADDR_WIDTH          37
    #define SINGLE_HBM_DEPTH        27
    #define DDR_ADDR_WIDTH          32
    #define Tout                    32
    #define L_Tout                  (HBM_Port*HBM_AXI_DATA_WIDTH/MAX_DW)
    #define AXI_BURST_LEN           64
    #define WT_AXI_BURST_LEN        256
#endif

#define AXI_factor                  (AXI_DATA_WIDTH/HBM_AXI_DATA_WIDTH)
#define log2_AXI_factor             (log2(AXI_factor) )
#define Tout_factor                 (L_Tout/Tout       )
#define log2_Tout_factor            (log2(Tout_factor))
#define log2_HBM_Port               (log2(HBM_Port)   )
#define log2_HBM_AXI_DATA_WIDTH     (log2(HBM_AXI_DATA_WIDTH))
#define SINGLE_DEPTH_x_2            (SINGLE_HBM_DEPTH*2)

#define ASYN_MODE
#ifdef ASYN_MODE
    #define ASYN_FACTOR             2
#else
    #define ASYN_FACTOR             1
#endif


///////////////////   AXI DATA WIDTH    ////////////////////
#define base_Tin                    128
#define log2_base_Tin               (log2(base_Tin) )
#define log2_Tout                   (log2(Tout    ) )
#define log2_L_Tout                 (log2(L_Tout  ) )
#define Tb                          1
#define log2_Tb                     (log2(Tb)       ) 

#define CSR_REG_NUM                 256
#define log2_CSR_REG_NUM            (log2(CSR_REG_NUM))
#define log2_AXI_BURST_LEN          (log2(AXI_BURST_LEN))
#define MCIF_RD_CREDIT_NUM          WT_AXI_BURST_LEN 
#define log2_MCIF_CREDIT_NUM        (log2(MCIF_RD_CREDIT_NUM))

#define MAX_DAT_DW                  MAX_DW
#define log2_MAX_DAT_DW             (log2(MAX_DAT_DW))
#define MAX_DW_Ratio                (MAX_DW)
#define AXI_DATA_WIDTH              (MAX_DAT_DW*Tout*Tb)
#define log2_AXI_DATA_WIDTH         (log2(AXI_DATA_WIDTH))
                                    
#define MAX_WT_DW                   4
#define log2_MAX_WT_DW              (log2(MAX_WT_DW))
#define log2_WT_AXI_BURST_LEN       (log2(WT_AXI_BURST_LEN))

#define log2_CH                     19
#define log2_H                      16
#define log2_W                      log2_H
#define log2_KyKx                   12
#define log2_other                  (log2_CH-log2_base_Tin-4)
#define log2_P                      8
#define log2_S                      8
#define log2_K                      8
                                    
#define MAX_FP_DW                   (16+4)// for Float point adder 
#define MAX_Matrix_FP_DW            (16+4)// for Float point adder

#define T_quant_block    128 //base_Tin //equal to the base_Tin
#define log2_T_quant_block (log2(T_quant_block)) //(log2_base_Tin)
#define WT_quant_scale_DW 16
#define WT_CH_Tgroup (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW)
#define log2_WT_CH_Tgroup (log2(WT_CH_Tgroup))
                                    
#define MAX_BN_DW                   16                                                //MAX_DAT_DW
#define log2_MAX_BN_DW              (log2(MAX_BN_DW))
#define MAX_BN_CH                   1024
#define BN_SRAM_DEPTH               (MAX_BN_CH/MAX_BN_DW/2)
#define log2_BN_SRAM_DEPTH          (log2(BN_SRAM_DEPTH))
#define BN_SRAM_WIDTH               (MAX_BN_DW*2*Tout     )
#define BN_REG_WIDTH                (HBM_AXI_DATA_WIDTH    )
#define BN_CH_PER_AXI               (HBM_AXI_DATA_WIDTH/MAX_BN_DW/2)
#define BN_REG_DEPTH                (MAX_BN_CH/BN_CH_PER_AXI/HBM_Port)
         
#define Pixel_Data_Width            (HBM_AXI_DATA_WIDTH)
#define Pixel_Data_Bytes            ((HBM_AXI_DATA_WIDTH)>>3)        
#define Pixel_BN_Data_Bytes         ((Tout*MAX_BN_DW)>>3)

///////////////////   ON-CHIP DAT BUF  ////////////////////
#define DAT_BRAM_NUM                HBM_Port
#define log2_DAT_BRAM_NUM           (log2(DAT_BRAM_NUM))
#define log2_TOTAL_DAT_BRAM_BITS    (23) //23= 8Mb for VCU128, single BRAM buf is 512(depth)*72(width)= 36864 bit
#define TOTAL_DAT_BRAM_BITS         (1<<log2_TOTAL_DAT_BRAM_BITS)
#define SINGLE_DAT_BRAM_BITS        (TOTAL_DAT_BRAM_BITS/DAT_BRAM_NUM)  
#define SINGLE_DAT_BRAM_WIDTH       (HBM_AXI_DATA_WIDTH)
#define SINGLE_DAT_BRAM_DEPTH       (SINGLE_DAT_BRAM_BITS/SINGLE_DAT_BRAM_WIDTH)
#define log2_SINGLE_DAT_BRAM_DEPTH  (log2(SINGLE_DAT_BRAM_DEPTH))
#define log2_ID0_BRAM_DEPTH         (log2_SINGLE_DAT_BRAM_DEPTH   )
#define ID0_BRAM_DEPTH              (1<<log2_ID0_BRAM_DEPTH       )
#define ID0_BRAM_WIDTH              (HBM_AXI_DATA_WIDTH*HBM_Port )

///////////////////   ON-CHIP WT BUF*2  ////////////////////
#define log2_TOTAL_WT_BRAM_BITS     (24) //24= 16Mb for VCU128, single BRAM buf is 512(depth)*72(width)= 36864 bit
#define TOTAL_WT_BRAM_BITS          (1<<log2_TOTAL_WT_BRAM_BITS)

#define WT_BRAM_NUM                 HBM_Port
#define log2_WT_BRAM_NUM            (log2(WT_BRAM_NUM))
#define log2_TOTAL_WT1_BRAM_BITS    (log2_TOTAL_WT_BRAM_BITS-1)
#define TOTAL_WT1_BRAM_BITS         (1<<log2_TOTAL_WT1_BRAM_BITS)
#define SINGLE_WT_BRAM_BITS         (TOTAL_WT1_BRAM_BITS/WT_BRAM_NUM)  
#define SINGLE_WT_BRAM_WIDTH        (HBM_AXI_DATA_WIDTH)
#define SINGLE_WT_BRAM_DEPTH        (SINGLE_WT_BRAM_BITS/SINGLE_WT_BRAM_WIDTH)
#define log2_SINGLE_WT_BRAM_DEPTH   (log2(SINGLE_WT_BRAM_DEPTH) )
#define log2_ID1_BRAM_DEPTH         (log2_SINGLE_WT_BRAM_DEPTH    )
#define ID1_BRAM_DEPTH              (1<<log2_ID1_BRAM_DEPTH       )
#define ID1_BRAM_WIDTH              (HBM_AXI_DATA_WIDTH*HBM_Port )

#define log2_TOTAL_WT2_BRAM_BITS    (log2_TOTAL_WT1_BRAM_BITS     ) //23= 8Mb for VCU128
#define TOTAL_WT2_BRAM_BITS         (1<<log2_TOTAL_WT2_BRAM_BITS  )
#define log2_ID2_BRAM_DEPTH         (log2_ID1_BRAM_DEPTH          )
#define ID2_BRAM_DEPTH              (1<<log2_ID2_BRAM_DEPTH       )
#define ID2_BRAM_WIDTH              (HBM_AXI_DATA_WIDTH*HBM_Port )

#define MIN_CH_Quant_Block          128
#define TOTAL_WT_SCALE_BITS         (TOTAL_WT_BRAM_BITS/(MIN_CH_Quant_Block*MAX_WT_DW/WT_quant_scale_DW))      
#define WT_SCALE_BUF_WIDTH          (WT_quant_scale_DW*Tout)
#define WT_SCALE_BUF_DEPTH          (TOTAL_WT_SCALE_BITS/WT_SCALE_BUF_WIDTH)
#define log2_WT_SCALE_BUF_DEPTH     (log2(WT_SCALE_BUF_DEPTH))

///////////////////   Conv and Matmul delay  ////////////////////
#define RD_BRAM_LATENCY             2
#define BUF_ADDR_LATENCY            2
#define TOTAL_BUF_RD_LATENCY        (BUF_ADDR_LATENCY+RD_BRAM_LATENCY)

#define ACC_LATENCY                 3
#define Column_Systolic_MAC             
#define Tn                          (Tout/2)    //the num of vector size
#define log2_Tn                     (log2(Tn))
#define Systolic_Delay              (Tout/Tn)

#define DSP_Delay                   2
#define Add_Tree_Delay              (2*log2_base_Tin+7)
#define MAC_LATENCY                 (Add_Tree_Delay + DSP_Delay +Systolic_Delay)
#define CACC_LATENCY                ACC_LATENCY
#define BN_LATENCY                  3

#ifdef  Column_Systolic_MAC         
    #define FSM_CREDIT_NUM          (TOTAL_BUF_RD_LATENCY+Tout/Tn+1)      //it means how many feature pixels there are in the Conv_stripe 
    #define log2FSM_CREDIT_NUM      (log2(FSM_CREDIT_NUM))
    #define CACC_OUT_DEP            (FSM_CREDIT_NUM+MAC_LATENCY+CACC_LATENCY+BN_LATENCY) // (Tout)+(log2_base_Tin+Tout)+1
    #define log2_CACC_OUT_DEP       (log2(CACC_OUT_DEP))
#else                               
    #define FSM_CREDIT_NUM          (TOTAL_BUF_RD_LATENCY+Tout+1)    //it means how many feature pixels there are in the Conv_stripe 
    #define log2FSM_CREDIT_NUM      (log2(FSM_CREDIT_NUM))
    #define CACC_OUT_DEP            (FSM_CREDIT_NUM+MAC_LATENCY+CACC_LATENCY+BN_LATENCY)  //BN_fifo_dep
    #define log2_CACC_OUT_DEP       (log2(CACC_OUT_DEP))
#endif


#define FP16INT4_MAC_BUF_ADD_OUT    "1111110"
#define FP16INT4_MAC_ADD_R_DW       5
#define CONV_OUT_DW                 20    //(MAX_DAT_DW+MAX_WT_DW) // for example: multiply=datawidth*2, 

///////////////////   Pool  ////////////////////
#define AXI_BURST_LEN_POOL             AXI_BURST_LEN
#define log2_AXI_BURST_LEN_POOL        log2_AXI_BURST_LEN
                                       
#define POOL_BRAM_DEPTH                ((1<<18)/Tout/MAX_DAT_DW/Tb)  //18: 256Kb for ASIC.
#define log2_POOL_BRAM_DEPTH           (log2(POOL_BRAM_DEPTH))       //20: 1Mb
#define Pool_credit_cnt                4
#define log2_Pool_credit_cnt           (log2(Pool_credit_cnt))
#define Pool_avg_dw                    16

///////////////////   Elementwise  ////////////////////
#define AXI_BURST_LEN_ElementWise      AXI_BURST_LEN
#define log2_AXI_BURST_LEN_ElementWise log2_AXI_BURST_LEN
#define Elementwise_credit_cnt         3
#define log2_Elementwise_credit_cnt    (log2(Elementwise_credit_cnt))

#define AXI_BURST_LEN_EMB              AXI_BURST_LEN
#define log2_AXI_BURST_LEN_EMB         log2_AXI_BURST_LEN

///////////////////   Transformer   ////////////////////
#define EMB_credit_cnt                 12 //rd_BRAM=2clk, cal=4clk, other for backup
#define log2_Emb_credit_cnt            (log2(EMB_credit_cnt))
#define log2_Matrix_pixel              16
#define log2_Matrix_ch                 16

#define EW_credit_cnt                  12 //rd_BRAM=2clk, cal=2clk, other for backup
#define log2_EW_credit_cnt             (log2(EW_credit_cnt))

#define AXI_BURST_LEN_SOFTMAX          AXI_BURST_LEN
#define log2_AXI_BURST_LEN_SOFTMAX     (log2(AXI_BURST_LEN_SOFTMAX))
                                       
#define fp16_divider_latency           21
#define fp20_divider_latency           27
#define Softmax_delay                  19 //rd_BRAM=2clk, get_exp=4clk, cal_S2=4clk, other for backup
#define Softmax_DW                     20
#define Tout_ACC_ADD_R_DW              5
                                       
#define LN_div_delay                   16
#define Sqrt_delay                     9
#define LN_stage1_delay                (2+Sqrt_delay+1)
#define LN_cal_delay                   9
#define LN_credit_cnt                  (LN_cal_delay+7)//7 is for backup
                                       
#define ACT_credit_cnt                 16 //rd_BRAM=2clk, cal_S1=6clk, other for backup
#define Linear_DW                      16
#define F2WTrans_scale_credit_cnt      2
#define F2WTrans_dat_credit_cnt        2

#define Transpose_credit_cnt           3
#define log2_Transpose_credit_cnt      (log2(Transpose_credit_cnt)+2)
#define COMP_DELAY1                    "1111111" //log2_Tout
#define COMP_DELAY2                    "1111"  //log2_T_quant_block-log2_Tout
                                       
#define CFG_ID_WIDTH                   (8)
#define CFG_ARAAY_WIDTH                (AXI_DATA_WIDTH*2)                  //1024bit -> 128 step
#define CFG_ARAAY_DEPTH                ((CFG_ARAAY_WIDTH/CFG_ID_WIDTH)*2)//           256 groups
#define AUX_WT_BUF_DEPTH               (1024)
#define log2_AUX_WT_BUF_DEPTH          (log2(AUX_WT_BUF_DEPTH))
#define AUX_WT_CREDIT_NUM              (4)

#define KVCACHE_CREDIT_NUM             (AXI_DATA_WIDTH/HBM_AXI_DATA_WIDTH+1)


///////////////////  Tout_ACC delay  ////////////////////
#define Tout_ACC_Out_Width_FP20        
#define Tout_ACC_BUF_ADD_OUT           "111110"
#define Tout_ACC_tp_DW                 24
#define Tout_SQUARE_ACC_Out_Width_FP20
#define Tout_SQUARE_ACC_BUF_ADD_OUT    "111110"
#define Tout_SQUARE_ACC_tp_DW          33

///////////////////  FP16*FP16 delay  ////////////////////
#define FP16FP16_MAC_BUF_ADD_OUT       "11110"
#define FP16FP16_MAC_ADD_R_DW          5
#define FP16FP16_MAC_Out_Width_FP20
#define sCONV_OUT_DW                   20

#define group HBM_Port