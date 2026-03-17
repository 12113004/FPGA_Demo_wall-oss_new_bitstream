#pragma once

struct FPGA_HBM_KV2HBM_cfg
{
    int      This_Token;
    int      Last_Token;
    int      MAX_CH_per_HEAD;
    int      Token_CHin;
    int      Token_CHout;
    int      Token_Len;
    int      Weight_Head;
    int      MAX_TOKEN;

    int      DAT_DW_in;
    int      DAT_DW_out;
    int      WT_DW;
    int      BN_DW;
    int      Tin;

    int      Win;
    int      Hin;
    int      CHin;
    int      Wout;
    int      Hout;
    int      CHout;

    int      CHin_div_Tout;
    int      CHin_div_LTout;
    int      CHin_Padding_with_LTout;
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding_with_LTout;
    int      CHout_Padding_with_Tout;
    int      LTout_div_CHin;
    int      Head_x_CHin;
    int      Head_x_CHin_div_LTout;
    int      WT_HEAD_STRIDE;
    int      WT_LINE_STRIDE;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_BATCH_STRIDE;
    int      DAT_IN_HEAD_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_BATCH_STRIDE;
    int      DAT_OUT_HEAD_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
};

enum KV_Mode
{
    K_Mode, 
    V_Mode
};

///// function for software//////////
struct FPGA_HBM_KV2HBM_cfg GetFPGA_HBM_KV2HBM_cfg(int This_Token, int Last_Token, int Weight_Head, int MAX_CH_per_HEAD, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_KV2HBM(struct FPGA_HBM_KV2HBM_cfg cfg, int *dat_in[], int *HBM[], int *dat_out_software[]);
void DAT_OUT_TRANS_FUNCTION_KV2HBM(struct FPGA_HBM_KV2HBM_cfg cfg, enum KV_Mode KV_Mode, int *HBM[], uint16_t *dat_out[]);

#include "HBM_kv2hbm.cpp"