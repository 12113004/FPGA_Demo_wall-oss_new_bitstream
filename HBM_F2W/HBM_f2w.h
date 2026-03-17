#pragma once

struct FPGA_HBM_F2W_cfg
{
    int      This_Token;
    int      Last_Token;
    int      MAX_CH_per_HEAD;
    int      Token_CHin;
    int      Token_CHout;
    int      Token_Len;
    int      Original_Feature_Head;
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

    int      Tin_div_Tout;
    int      CHin_div_Tout;
    int      CHin_div_LTout;
    int      CHin_Padding_with_LTout;
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding_with_LTout;
    int      CHout_Padding_with_Tout;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_HEAD_STRIDE;
    int      DAT_IN_SURFACE_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    uint64_t WT_BASE_ADDR;
    int      WT_CHin;
    int      WT_CHout;
    int      WT_CHout_div_Tout;
    int      WT_CHin_div_Tin;
    int      WT_CHout_Padding_with_Tout;
    int      WT_CHin_Padding_with_Tin;
    int      WT_BYTES_PER_HEAD;
    int      WT_BYTES_PER_CHOUT;
    int      WT_HEAD_STRIDE;
    int      WT_LINE_STRIDE;

    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_ONE_HEAD_STRIDE;
    int      DAT_OUT_ALL_HEAD_STRIDE;
    int      DAT_OUT_SURFACE_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
    int      Head_x_CHout;
    int      Head_x_CHout_div_LTout;
    int      LTout_div_CHout;
    int      Head_x_CH_div_LTout;
};

///// function for software//////////
struct FPGA_HBM_F2W_cfg GetFPGA_HBM_F2W_cfg(int This_Token, int Last_Token, int Original_Feature_Head, int Weight_Head, int MAX_CH_per_HEAD, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_F2W(struct FPGA_HBM_F2W_cfg cfg, int *dat_in[], int *HBM[]);
void WT_IN_TRANS_FUNCTION_F2W(struct FPGA_HBM_TRP_cfg cfg, int *wt_in[], int *HBM[]);
void DAT_OUT_TRANS_FUNCTION_F2W(struct FPGA_HBM_F2W_cfg cfg, int *HBM[], uint16_t *dat_out[]);

#include "HBM_f2w.cpp"