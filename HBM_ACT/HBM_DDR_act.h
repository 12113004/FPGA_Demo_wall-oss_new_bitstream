#pragma once

struct FPGA_HBM_ACT_cfg
{
    int Height;
    int Width_in;

    int DAT_DW_L0;
    int DAT_DW_L1;
    int WT_DW;
    int BN_DW;
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

    uint64_t WT_BASE_ADDR;

    uint64_t DAT_OUT_BASE_ADDR;
    int DAT_OUT_BATCH_STRIDE;
    int DAT_OUT_SURFACE_STRIDE;
    int DAT_OUT_LINE_STRIDE;
    int DAT_OUT_scale;
};

///// function for software//////////
struct FPGA_HBM_ACT_cfg GetFPGA_HBM_ACT_cfg(int Height, int Width_in, int Hin, uint64_t DAT_IN_BASE_ADDR, uint64_t WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void PARAMETER_IN_TRANS_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, int *wt, int *bias, int* x_region, int *HBM_DDR[]);
void DAT_OUT_DEMAP_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[]);

#include "HBM_DDR_act.cpp"