#pragma once

struct FPGA_HBM_LN_inout_head_cfg
{
    int      This_Token;
    int      Last_Token;
    int      Padding_Feature_Head;
    int      True_CH_per_HEAD;

    int      MAX_CH_per_HEAD;
    int      Layer_Norm;
    int      KV_cache_mode;
    int      Token_Len;
    int      Token_CHin;
    int      Token_CHout;

    int      DAT_DW_in;
    int      DAT_DW_out;
    int      WT_DW;
    int      LN_DW;
    int      Tin;

    int      Win;
    int      Hin;
    int      CHin;
    int      CHout;
    int      Wout;
    int      Hout;

    int      CHin_div_Tout;
    int      CHin_div_LTout;
    int      CHin_Padding_with_LTout;
    int      CHin_Padding_div_Head;
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding_with_LTout;
    int      CHout_Padding_div_Head;

    int      INOUT_HEAD_MODE;
    int      LTout_div_CHout;
    int      Head_x_CH_div_LTout;
    int      Group_Head_Num;

    int      Head_x_CHin;
    int      Head_x_CHin_div_LTout;
    int      Head_x_CHout;
    int      Head_x_CHout_div_LTout;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_BATCH_STRIDE;
    int      DAT_IN_HEAD_STRIDE;
    int      DAT_IN_SURFACE_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    uint64_t LN_WT_BASE_ADDR;
    int      LN_CH_per_AXI_DW;
    int      LN_CH_Group_Nums;
    int      log2_MAX_REQ_BITS;

    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_BATCH_STRIDE;
    int      DAT_OUT_HEAD_STRIDE;
    int      DAT_OUT_SURFACE_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
};

///// function for software//////////
struct FPGA_HBM_LN_inout_head_cfg GetFPGA_HBM_LN_inout_head_cfg(int This_Token, int Last_Token, int Padding_Feature_Head, int True_CH_per_HEAD, uint64_t DAT_IN_BASE_ADDR, uint64_t LN_WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_LN_inout_head(struct FPGA_HBM_LN_inout_head_cfg cfg, int *dat_in[], int *HBM[]);
void LN_WEIGHT_AND_BIAS_IN_TRANS_FUNCTION_LN_inout_head(struct FPGA_HBM_LN_inout_head_cfg cfg, int *LN_weight, int *LN_bias, int *HBM[]);
void DAT_OUT_TRANS_FUNCTION_LN_inout_head(struct FPGA_HBM_LN_inout_head_cfg cfg, int *HBM[], uint16_t *dat_out[]);

#include "HBM_ln_inout_head.cpp"