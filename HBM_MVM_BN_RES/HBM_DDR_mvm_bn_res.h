#pragma once

struct FPGA_HBM_MVM_BN_RES_cfg
{
    int Height;
    int Width_in;
    int Width_out;
    int Skip_Factor;
    
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
    
    int RELU_EN;
    
    uint64_t DAT_IN_BASE_ADDR;
    int DAT_IN_BATCH_STRIDE;
    int DAT_IN_SURFACE_STRIDE;
    int DAT_IN_LINE_STRIDE;
    int DAT_IN_scale;
    
    int WT_CHin_Padding_with_Tin;
    int WT_CHin_div_Tblock;
    int Tblock_div_Tin;
    
    //int WT_CH_Tgroup (`T_quant_block*`HBM_AXI_DATA_WIDTH/`WT_quant_scale_DW) // =2048 CHins
    int WT_scale_group_nums;
    int WT_CH_Tgroup_div_Tblock; //2048/128=16
    
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
    
    //int WT_scale_bits (`CHout_Padding*`WT_CHin_div_Tblock*`WT_quant_scale_DW)
    int WT_scale_bits;
    int WT_CHin_div_Tin;
    int WT_SIZE_IN_BYTES;
    int WT_NUM_DIV_TIN;
    int WT_scale;
    uint64_t HBM00_WT_BASE_ADDR;
    
    int Conv_out_scale;
    
    //int bias
    uint64_t BN_BASE_ADDR;
    int BN_SURFACE_STRIDE;
    int BN_num_per_AXI_DW;
    int BN_ch_group_times;
    int BN_WT_scale;
    int BN_BIAS_scale;
    
    uint64_t Res_Add_BASE_ADDR;
    int Res_Add_BATCH_STRIDE;
    int Res_Add_SURFACE_STRIDE;
    int Res_Add_LINE_STRIDE;
    int Res_Add_scale;
    
    uint64_t DAT_OUT_BASE_ADDR;
    int DAT_OUT_BATCH_STRIDE;
    int DAT_OUT_SURFACE_STRIDE;
    int DAT_OUT_LINE_STRIDE;
    int DAT_OUT_scale;
};

///// function for software//////////
struct FPGA_HBM_MVM_BN_RES_cfg GetFPGA_HBM_MVM_BN_RES_cfg(int Height, int Win, int Width_in, int Width_out, int Skip_Factor, int RELU_EN, int log2_WT_base_addr_Bank_Step,
                                                  uint64_t DAT_IN_BASE_ADDR, uint64_t HBM00_WT_BASE_ADDR, uint64_t BN_BASE_ADDR, uint64_t Res_Add_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR);
void WT_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg , int *wt, int *wt_FP_scale, int *HBM_DDR[]);
void DAT_IN_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int *dat_in[], int *HBM_DDR[]);
void BN_WT_AND_BIAS_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, uint16_t *bn_bias, int *bn_wt, int *HBM_DDR[]);
void RES_ADD_DAT_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int *Res_Add_dat[],int *HBM_DDR[]);
void DAT_OUT_DEMAP_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[]);

#include "HBM_DDR_mvm_bn_res.cpp"