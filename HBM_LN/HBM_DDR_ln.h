#pragma once

struct FPGA_HBM_LN_cfg
{
    int Height;
    int Width_in;
    
    int RMS_Norm;
    int Layer_Norm;
    
    int DAT_DW_L0;
    int DAT_DW_L1;
    int WT_DW;
    int LN_DW;
    int Tin;
    
    int Win;
    int Hin;
    int CHin;
    int CHout;
    
    int Wout;
    int Hout;
    int CHout_div_Tout;
    int CHin_div_Tout;
    int CHin_Padding_with_Tout;
    int Tin_div_Tout;
    int CHout_Padding;
    
    uint64_t DAT_IN_BASE_ADDR;
    int DAT_IN_BATCH_STRIDE;
    int DAT_IN_SURFACE_STRIDE;
    int DAT_IN_LINE_STRIDE;
    int DAT_IN_scale;
    
    uint64_t LN_WT_BASE_ADDR;
    int LN_SURFACE_STRIDE;
    int LN_num_per_AXI_DW;
    int LN_ch_group_times;
    
    uint64_t DAT_OUT_BASE_ADDR;
    int DAT_OUT_BATCH_STRIDE;
    int DAT_OUT_SURFACE_STRIDE;
    int DAT_OUT_LINE_STRIDE;
    int DAT_OUT_scale;
    
    ///////////////// no use ////////////////////
    int RELU_EN;
};

///// function for software//////////
struct FPGA_HBM_LN_cfg GetFPGA_HBM_LN_cfg(int Height, int Width_in, int RMS_Norm, int Hin, int RELU_EN, uint64_t DAT_IN_BASE_ADDR, uint64_t LN_WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void LN_WEIGHT_AND_BIAS_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *LN_weight, int *LN_bias, int *HBM_DDR[]);
void DAT_OUT_DEMAP_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[]);

#include "HBM_DDR_ln.cpp"