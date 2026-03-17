#pragma once

#include "stdio.h"

struct FPGA_HBM_EMB_cfg
{
    int Height;
    int Head;
    int Width_in;

    int Pos_Num;

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

    uint64_t POS_IN_BASE_ADDR;
    int POS_LINE_STRIDE;

    uint64_t DAT_IN_BASE_ADDR;
    int DAT_IN_BATCH_STRIDE;
    int DAT_IN_SURFACE_STRIDE;
    int DAT_IN_LINE_STRIDE;
    int DAT_IN_scale;

    uint64_t DAT_OUT_BASE_ADDR;
    int DAT_OUT_BATCH_STRIDE;
    int DAT_OUT_SURFACE_STRIDE;
    int DAT_OUT_LINE_STRIDE;
    int DAT_OUT_scale;
};

///// function for software//////////
struct FPGA_HBM_EMB_cfg GetFPGA_HBM_EMB_cfg(int Height, int Head, int Width_in, int Pos_Num, uint64_t POS_IN_BASE_ADDR, uint64_t DAT_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void DAT_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void POS_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *pos_in, int *HBM_DDR[]);
void DAT_OUT_DEMAP_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[]);

#include "HBM_DDR_emb.cpp"