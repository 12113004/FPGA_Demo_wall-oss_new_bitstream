#pragma once

struct FPGA_HBM_SOFTMAX_cfg
{
    int      Height;
    int      Width_in;
    int      Head;

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
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_BATCH_STRIDE;
    int      DAT_IN_SURFACE_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_BATCH_STRIDE;
    int      DAT_OUT_SURFACE_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
};

///// function for software//////////
struct FPGA_HBM_SOFTMAX_cfg GetFPGA_HBM_SOFTMAX_cfg(int Head, int Height, int Hin, int Width_in, uint64_t DAT_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_SOFTMAX(struct FPGA_HBM_SOFTMAX_cfg cfg, int *dat_in[], int *HBM[]);
void DAT_OUT_TRANS_FUNCTION_SOFTMAX(struct FPGA_HBM_SOFTMAX_cfg cfg, int *HBM[], uint16_t *dat_out[]);

#include "HBM_softmax.cpp"