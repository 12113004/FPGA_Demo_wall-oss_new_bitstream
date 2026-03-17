#pragma once

struct FPGA_HBM_MVM_BN_Argmax_cfg
{
    int      Height;
    int      Width_in;
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

    int      Tin_div_Tout;
    int      CHin_div_Tout;
    int      CHin_div_LTout;
    int      CHin_Padding;
    int      CHout_div_Tout;
    int      CHout_div_LTout;
    int      CHout_Padding;
    int      CHout_Padding_with_Tout;

    uint64_t DAT_IN_BASE_ADDR;
    int      DAT_IN_BATCH_STRIDE;
    int      DAT_IN_HEAD_STRIDE;
    int      DAT_IN_SURFACE_STRIDE;
    int      DAT_IN_LINE_STRIDE;

    uint64_t HBM00_WT_BASE_ADDR;
    int      WT_CHin_div_Tin;
    int      WT_CHin_Padding_with_Tin;
    int      WT_CHout_Padding_with_Tout;
    int      WT_CHin_div_Tblock;
    int      Tblock_div_Tin;

    int      WT_scale_group_nums;
    int      WT_CH_Tgroup_div_Tblock;

    int      Group_WT_Bytes;
    int      Group_Scale_Bytes;
    int      Group_WT_and_Scale_Bytes;
    int      Last_Group_CHin;
    int      Last_Group_WT_Bytes;
    int      Last_Group_Scale_Bytes;
    int      Last_Group_WT_and_Scale_Bytes;

    int      CHin_WT_Bytes;
    int      CHin_Scale_Bytes;
    int      CHin_WT_and_Scale_Bytes;
    int      WT_scale_bits;
    int      WT_SIZE_IN_BYTES;
    int      WT_BYTES_per_CH;
    int      WT_NUM_DIV_TIN;
    
    uint64_t BN_BASE_ADDR;
    int      BN_SURFACE_STRIDE;
    int      BN_CH_per_AXI_DW;
    int      BN_ch_group_times;
    int      BN_SIZE_IN_BYTES;
    int      BN_WT_scale;
    int      BN_BIAS_scale;
    
    uint64_t DAT_OUT_BASE_ADDR;
    int      DAT_OUT_BATCH_STRIDE;
    int      DAT_OUT_HEAD_STRIDE;
    int      DAT_OUT_SURFACE_STRIDE;
    int      DAT_OUT_LINE_STRIDE;
};

///// function for software//////////
struct FPGA_HBM_MVM_BN_Argmax_cfg GetFPGA_HBM_MVM_BN_Argmax_cfg(int Height, int Hin, int Width_in, int Width_out, uint64_t DAT_IN_BASE_ADDR, uint64_t HBM00_WT_BASE_ADDR, uint64_t BN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void WT_AND_SCALE_TRANS_FUNCTION_MVM_BN_Argmax(struct FPGA_HBM_MVM_BN_Argmax_cfg cfg , int *wt, int *wt_FP_scale, int *HBM[]);
void DAT_IN_TRANS_FUNCTION_MVM_BN_Argmax(struct FPGA_HBM_MVM_BN_Argmax_cfg cfg, int *dat_in[], int *HBM[]);
void BN_WT_AND_BIAS_IN_TRANS_FUNCTION_MVM_BN_Argmax(struct FPGA_HBM_MVM_BN_Argmax_cfg cfg, int *bn_bias, int *bn_wt, int *HBM[]);
void DAT_OUT_TRANS_FUNCTION_MVM_BN_Argmax(struct FPGA_HBM_MVM_BN_Argmax_cfg cfg, int *HBM[], uint16_t *dat_out[]);

#include "HBM_mvm_bn_argmax.cpp"