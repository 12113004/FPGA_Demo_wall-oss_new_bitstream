#pragma once

struct FPGA_HBM_TRANSPOSE_cfg
{
    int Height;
    int Width_in;

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
    int Ky;
    int Kx;

    int WT_CHin_div_Tblock;
    int WT_CHin_div_Tin;
    int WT_CHin_Padding_with_Tin;
    int Tblock_div_Tin;

    int CHout_div_Tout;
    int CHin_div_Tout;
    int CHin_Padding_with_Tout;
    int Tin_div_Tout;
    int CHout_Padding;

    int DAT_IN_BASE_ADDR;
    int DAT_IN_SURFACE_STRIDE;
    int DAT_IN_LINE_STRIDE;

    int WT_BASE_ADDR;

    int DAT_OUT_BASE_ADDR;
    int DAT_OUT_SURFACE_STRIDE;
    int DAT_OUT_LINE_STRIDE;
    int Block_wt_bits;
    int Total_wt_bits;

    //int WT_CH_Tgroup;
    int WT_scale_group_nums;
    int WT_CH_Tgroup_div_Tblock;

    int Group_WT_Bytes;
    int Group_Scale_Bytes;
    int Group_WT_and_Scale_Bytes;
    int Last_Group_CHin;
    int Last_Group_WT_Bytes;
    int Last_Group_Scale_Bytes;
    int Last_Group_WT_and_Scale_Bytes;

    int CHin_WT_Bytes;
    int CHin_Scale_Bytes;
    int CHin_WT_and_Scale_Bytes;
    int log2_WT_base_addr_Bank_Step;
    int WT_base_addr_Bank_Step;
    int HBM00_WT_BASE_ADDR;

    ///////////////// no use ////////////////////
    int RELU_EN;
    int half_clk_period;
    ////////////////////////////////////////////////////////////////////////////////
};

///// function for software//////////
struct FPGA_HBM_TRANSPOSE_cfg GetFPGA_HBM_TRANSPOSE_cfg(int Height, int Width_in, int Hin, int Wout, int Hout, int Ky, int Kx, int log2_WT_base_addr_Bank_Step, int RELU_EN);
void TRANSPOSE_SOFT_FUNCTION(struct FPGA_HBM_TRANSPOSE_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void DAT_IN_TRANS_FUNCTION_TRANSPOSE(struct FPGA_HBM_TRANSPOSE_cfg cfg, int *dat_in[], int *HBM_DDR[]);

#include "HBM_DDR_trans.cpp"