#pragma once

#include "stdio.h"

struct FPGA_HBM_EMB_cfg
{
    int      Height;
    int      Width_in;
    int      Head;
    int      Width_out;

    int      DAT_DW_L0;
    int      DAT_DW_L1;
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
    int      CHin_Padding;
    int      CHin_Padding_div_Head;
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding;
    int      CHout_Padding_div_Head;
    int      Head_x_CHin;
    int      Head_x_CHin_div_LTout;
    int      Head_x_CHout;
    int      Head_x_CHout_div_LTout;
    int      LTout_div_CHout;
    int      Head_x_CH_div_LTout;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_BATCH_STRIDE;
    int      DAT_IN_HEAD_STRIDE;
    int      DAT_IN_SURFACE_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    int      Pos_Num;
    uint64_t POS_IN_BASE_ADDR;
    int      POS_HEAD_STRIDE;
    int      POS_LINE_STRIDE;

    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_BATCH_STRIDE;
    int      DAT_OUT_HEAD_STRIDE;
    int      DAT_OUT_SURFACE_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
};

///// function for software//////////
struct FPGA_HBM_EMB_cfg GetFPGA_HBM_EMB_cfg(int Head, int Height, int Hin, int Width_in, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t POS_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void POS_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *pos_in, int *HBM_DDR[]);
void DAT_OUT_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *HBM[], uint16_t *dat_out[]);

#include "HBM_emb.cpp"