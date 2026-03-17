#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_ln.h"

struct FPGA_HBM_LN_cfg GetFPGA_HBM_LN_cfg(int Height, int Hin, int Width_in, uint64_t DAT_IN_BASE_ADDR, uint64_t LN_WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_LN_cfg ret;

    ret.Height                 = Height;
    ret.Width_in               = Width_in;

    ret.DAT_DW_in              = MAX_DAT_DW;
    ret.DAT_DW_out             = MAX_DAT_DW;
    ret.WT_DW                  = MAX_WT_DW;
    ret.LN_DW                  = MAX_BN_DW;
    ret.Tin                    = (base_Tin);

    ret.Win                    = ret.Height;
    ret.Hin                    = Hin;
    ret.CHin                   = ret.Width_in;
    ret.Wout                   = ret.Win;
    ret.Hout                   = ret.Hin;
    ret.CHout                  = ret.CHin;

    ret.CHin_div_Tout          = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_div_LTout         = ((ret.CHin+L_Tout-1)/L_Tout);
    ret.CHin_Padding           = (ret.CHin_div_LTout*L_Tout);
    ret.CHout_div_Tout         = ((ret.CHout+Tout-1)/Tout);
    ret.CHout_div_LTout        = ((ret.CHout+L_Tout-1)/L_Tout);
    ret.CHout_Padding          = (ret.CHout_div_LTout*L_Tout);

    ret.DAT_IN_BASE_ADDR       = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE    = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);     // Pixel_Data_Bytes = 32
    ret.DAT_IN_SURFACE_STRIDE  = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE     = (Pixel_Data_Bytes*ret.Win);

    ret.LN_WT_BASE_ADDR        = LN_WT_BASE_ADDR;
    ret.LN_num_per_AXI_DW      = (MAX_DAT_DW*L_Tout/(2*ret.LN_DW));                         // LN_num_per_AXI_DW <= L_Tout
    ret.LN_ch_group_times      = (ret.CHout_Padding/ret.LN_num_per_AXI_DW);

    ret.DAT_OUT_BASE_ADDR      = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE   = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_SURFACE_STRIDE = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE    = (Pixel_Data_Bytes*ret.Wout);

    return ret;
}

void DAT_IN_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *dat_in[], int *HBM[])
{
    // bit [`MAX_DAT_DW*`L_Tout*`Tb-1:0] dat_in_mem [`Hin*`Win*`CHin_div_LTout];
    int **dat_in_mem = (int**)malloc(sizeof(int*)*Tb*cfg.Hin*cfg.Win*cfg.CHin_div_LTout);
    for (int i=0;i<cfg.Hin*cfg.Win*cfg.CHin_div_LTout;i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if(dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }

    // bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_LTout];
    int ***tp_dat_in_mem = (int***)malloc(sizeof(int**)*Tb);
    for(int i=0;i<Tb;i++)
    {
        tp_dat_in_mem[i] = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.CHin_div_LTout);
        for(int j=0;j<cfg.Win*cfg.Hin*cfg.CHin_div_LTout;j++)
        {
            tp_dat_in_mem[i][j] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
            if(tp_dat_in_mem[i][j] == NULL){printf("fail to malloc tp_dat_in_mem \n");}
        }
    }

    // dat_in
    for(int b=0;b<Tb;b++)
        General_Map_Feature_Data(cfg, cfg.Hin, cfg.Win, cfg.CHin, dat_in[b], tp_dat_in_mem[b]);

    for(int b=0;b<Tb;b++) 
    {
        for(int i=0;i<cfg.Win*cfg.Hin*cfg.CHin_div_LTout;i++)
        {
            for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
                dat_in_mem[b*cfg.Win*cfg.Hin*cfg.CHin_div_LTout+i][j] = tp_dat_in_mem[b][i][j];
        }
    }

    for(int i=0;i<Tb*cfg.Win*cfg.Hin*cfg.CHin_div_LTout;i++)   // depth
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)             // 8192bit/32bit = 256
        {
            if(j>=8* 0 & j<8* 1) HBM[ 0][i*HBM_AXI_DATA_WIDTH/32+(j-8* 0)] = dat_in_mem[i][j];
            if(j>=8* 1 & j<8* 2) HBM[ 1][i*HBM_AXI_DATA_WIDTH/32+(j-8* 1)] = dat_in_mem[i][j];
            if(j>=8* 2 & j<8* 3) HBM[ 2][i*HBM_AXI_DATA_WIDTH/32+(j-8* 2)] = dat_in_mem[i][j];
            if(j>=8* 3 & j<8* 4) HBM[ 3][i*HBM_AXI_DATA_WIDTH/32+(j-8* 3)] = dat_in_mem[i][j];
            if(j>=8* 4 & j<8* 5) HBM[ 4][i*HBM_AXI_DATA_WIDTH/32+(j-8* 4)] = dat_in_mem[i][j];
            if(j>=8* 5 & j<8* 6) HBM[ 5][i*HBM_AXI_DATA_WIDTH/32+(j-8* 5)] = dat_in_mem[i][j];
            if(j>=8* 6 & j<8* 7) HBM[ 6][i*HBM_AXI_DATA_WIDTH/32+(j-8* 6)] = dat_in_mem[i][j];
            if(j>=8* 7 & j<8* 8) HBM[ 7][i*HBM_AXI_DATA_WIDTH/32+(j-8* 7)] = dat_in_mem[i][j];
            if(j>=8* 8 & j<8* 9) HBM[ 8][i*HBM_AXI_DATA_WIDTH/32+(j-8* 8)] = dat_in_mem[i][j];
            if(j>=8* 9 & j<8*10) HBM[ 9][i*HBM_AXI_DATA_WIDTH/32+(j-8* 9)] = dat_in_mem[i][j];
            if(j>=8*10 & j<8*11) HBM[10][i*HBM_AXI_DATA_WIDTH/32+(j-8*10)] = dat_in_mem[i][j];
            if(j>=8*11 & j<8*12) HBM[11][i*HBM_AXI_DATA_WIDTH/32+(j-8*11)] = dat_in_mem[i][j];
            if(j>=8*12 & j<8*13) HBM[12][i*HBM_AXI_DATA_WIDTH/32+(j-8*12)] = dat_in_mem[i][j];
            if(j>=8*13 & j<8*14) HBM[13][i*HBM_AXI_DATA_WIDTH/32+(j-8*13)] = dat_in_mem[i][j];
            if(j>=8*14 & j<8*15) HBM[14][i*HBM_AXI_DATA_WIDTH/32+(j-8*14)] = dat_in_mem[i][j];
            if(j>=8*15 & j<8*16) HBM[15][i*HBM_AXI_DATA_WIDTH/32+(j-8*15)] = dat_in_mem[i][j];
            if(j>=8*16 & j<8*17) HBM[16][i*HBM_AXI_DATA_WIDTH/32+(j-8*16)] = dat_in_mem[i][j];
            if(j>=8*17 & j<8*18) HBM[17][i*HBM_AXI_DATA_WIDTH/32+(j-8*17)] = dat_in_mem[i][j];
            if(j>=8*18 & j<8*19) HBM[18][i*HBM_AXI_DATA_WIDTH/32+(j-8*18)] = dat_in_mem[i][j];
            if(j>=8*19 & j<8*20) HBM[19][i*HBM_AXI_DATA_WIDTH/32+(j-8*19)] = dat_in_mem[i][j];
            if(j>=8*20 & j<8*21) HBM[20][i*HBM_AXI_DATA_WIDTH/32+(j-8*20)] = dat_in_mem[i][j];
            if(j>=8*21 & j<8*22) HBM[21][i*HBM_AXI_DATA_WIDTH/32+(j-8*21)] = dat_in_mem[i][j];
            if(j>=8*22 & j<8*23) HBM[22][i*HBM_AXI_DATA_WIDTH/32+(j-8*22)] = dat_in_mem[i][j];
            if(j>=8*23 & j<8*24) HBM[23][i*HBM_AXI_DATA_WIDTH/32+(j-8*23)] = dat_in_mem[i][j];
            if(j>=8*24 & j<8*25) HBM[24][i*HBM_AXI_DATA_WIDTH/32+(j-8*24)] = dat_in_mem[i][j];
            if(j>=8*25 & j<8*26) HBM[25][i*HBM_AXI_DATA_WIDTH/32+(j-8*25)] = dat_in_mem[i][j];
            if(j>=8*26 & j<8*27) HBM[26][i*HBM_AXI_DATA_WIDTH/32+(j-8*26)] = dat_in_mem[i][j];
            if(j>=8*27 & j<8*28) HBM[27][i*HBM_AXI_DATA_WIDTH/32+(j-8*27)] = dat_in_mem[i][j];
            if(j>=8*28 & j<8*29) HBM[28][i*HBM_AXI_DATA_WIDTH/32+(j-8*28)] = dat_in_mem[i][j];
            if(j>=8*29 & j<8*30) HBM[29][i*HBM_AXI_DATA_WIDTH/32+(j-8*29)] = dat_in_mem[i][j];
            if(j>=8*30 & j<8*31) HBM[30][i*HBM_AXI_DATA_WIDTH/32+(j-8*30)] = dat_in_mem[i][j];
            if(j>=8*31 & j<8*32) HBM[31][i*HBM_AXI_DATA_WIDTH/32+(j-8*31)] = dat_in_mem[i][j];  
        }
    }

    // Free malloc
    free(dat_in_mem);
    dat_in_mem = NULL;
    free(tp_dat_in_mem);
    tp_dat_in_mem = NULL;
}

void LN_WEIGHT_AND_BIAS_IN_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *LN_weight, int *LN_bias, int *HBM[])
{
    // bit [`MAX_BN_DW*2-1:0] LN_wt_and_bias [`CHout];                           width = 32
    int *LN_wt_and_bias_in = (int*)malloc(sizeof(int)*cfg.CHout);
    if (LN_bias == NULL){perror("main");return;}

    // bit [`MAX_DAT_DW*`L_Tout-1:0] LN_wt_and_bias_mem[2*`CHout_div_LTout];     width = 16*512 = 8192
    int **LN_wt_and_bias_in_mem = (int**)malloc(sizeof(int*)*2*cfg.CHout_div_LTout);
    for(int i=0;i<2*cfg.CHout_div_LTout;i++)
    {
        LN_wt_and_bias_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if (LN_wt_and_bias_in_mem[i] == NULL){printf("fail to malloc LN_wt_and_bias_in_mem \n");}
    }

    for(int k=0;k<cfg.CHout;k++)
        LN_wt_and_bias_in[k] = ((LN_weight[k]<<16)&0xffff0000) + (LN_bias[k]&0x0000ffff);

    for(int i=0;i<cfg.LN_ch_group_times;i++)
    {
        for(int j=0;j<cfg.LN_num_per_AXI_DW;j++)
        {
            if(i*cfg.LN_num_per_AXI_DW+j<cfg.CHin)
                LN_wt_and_bias_in_mem[i][j] = LN_wt_and_bias_in[i*cfg.LN_num_per_AXI_DW+j];
            else
                LN_wt_and_bias_in_mem[i][j] = 0;
        }
    }

    for(int i=0;i<cfg.LN_ch_group_times;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
        {
            if(j>=8* 0 & j<8* 1) HBM[ 0][i*HBM_AXI_DATA_WIDTH/32+(j-8* 0)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 1 & j<8* 2) HBM[ 1][i*HBM_AXI_DATA_WIDTH/32+(j-8* 1)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 2 & j<8* 3) HBM[ 2][i*HBM_AXI_DATA_WIDTH/32+(j-8* 2)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 3 & j<8* 4) HBM[ 3][i*HBM_AXI_DATA_WIDTH/32+(j-8* 3)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 4 & j<8* 5) HBM[ 4][i*HBM_AXI_DATA_WIDTH/32+(j-8* 4)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 5 & j<8* 6) HBM[ 5][i*HBM_AXI_DATA_WIDTH/32+(j-8* 5)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 6 & j<8* 7) HBM[ 6][i*HBM_AXI_DATA_WIDTH/32+(j-8* 6)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 7 & j<8* 8) HBM[ 7][i*HBM_AXI_DATA_WIDTH/32+(j-8* 7)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 8 & j<8* 9) HBM[ 8][i*HBM_AXI_DATA_WIDTH/32+(j-8* 8)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8* 9 & j<8*10) HBM[ 9][i*HBM_AXI_DATA_WIDTH/32+(j-8* 9)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*10 & j<8*11) HBM[10][i*HBM_AXI_DATA_WIDTH/32+(j-8*10)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*11 & j<8*12) HBM[11][i*HBM_AXI_DATA_WIDTH/32+(j-8*11)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*12 & j<8*13) HBM[12][i*HBM_AXI_DATA_WIDTH/32+(j-8*12)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*13 & j<8*14) HBM[13][i*HBM_AXI_DATA_WIDTH/32+(j-8*13)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*14 & j<8*15) HBM[14][i*HBM_AXI_DATA_WIDTH/32+(j-8*14)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*15 & j<8*16) HBM[15][i*HBM_AXI_DATA_WIDTH/32+(j-8*15)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*16 & j<8*17) HBM[16][i*HBM_AXI_DATA_WIDTH/32+(j-8*16)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*17 & j<8*18) HBM[17][i*HBM_AXI_DATA_WIDTH/32+(j-8*17)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*18 & j<8*19) HBM[18][i*HBM_AXI_DATA_WIDTH/32+(j-8*18)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*19 & j<8*20) HBM[19][i*HBM_AXI_DATA_WIDTH/32+(j-8*19)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*20 & j<8*21) HBM[20][i*HBM_AXI_DATA_WIDTH/32+(j-8*20)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*21 & j<8*22) HBM[21][i*HBM_AXI_DATA_WIDTH/32+(j-8*21)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*22 & j<8*23) HBM[22][i*HBM_AXI_DATA_WIDTH/32+(j-8*22)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*23 & j<8*24) HBM[23][i*HBM_AXI_DATA_WIDTH/32+(j-8*23)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*24 & j<8*25) HBM[24][i*HBM_AXI_DATA_WIDTH/32+(j-8*24)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*25 & j<8*26) HBM[25][i*HBM_AXI_DATA_WIDTH/32+(j-8*25)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*26 & j<8*27) HBM[26][i*HBM_AXI_DATA_WIDTH/32+(j-8*26)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*27 & j<8*28) HBM[27][i*HBM_AXI_DATA_WIDTH/32+(j-8*27)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*28 & j<8*29) HBM[28][i*HBM_AXI_DATA_WIDTH/32+(j-8*28)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*29 & j<8*30) HBM[29][i*HBM_AXI_DATA_WIDTH/32+(j-8*29)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*30 & j<8*31) HBM[30][i*HBM_AXI_DATA_WIDTH/32+(j-8*30)] = LN_wt_and_bias_in_mem[i][j];
            if(j>=8*31 & j<8*32) HBM[31][i*HBM_AXI_DATA_WIDTH/32+(j-8*31)] = LN_wt_and_bias_in_mem[i][j];  
        }
    }

    // Free malloc
    free(LN_wt_and_bias_in);
    LN_wt_and_bias_in = NULL;
    free(LN_wt_and_bias_in_mem); 
    LN_wt_and_bias_in_mem = NULL;
}

void DAT_OUT_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *HBM[], uint16_t *dat_out[])
{
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_out_mem [`Hout*`Wout*`CHout_div_LTout];
    int  **dat_out_mem_tmp = (int**)malloc(sizeof(int*)*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_LTout);
    for (int i=0;i<cfg.Hout*cfg.Wout*cfg.CHout_div_LTout;i++)
    {
        dat_out_mem_tmp[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if (dat_out_mem_tmp[i] == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    }
    uint16_t **dat_out_mem = (uint16_t**)malloc(sizeof(uint16_t*)*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_LTout);
    for (int i=0;i<cfg.Hout*cfg.Wout*cfg.CHout_div_LTout;i++)
    {
        dat_out_mem[i] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
        if (dat_out_mem[i] == NULL){printf("fail to malloc dat_out_mem \n");}
    }

    uint16_t ***tp_dat_out_mem = (uint16_t***)malloc(sizeof(uint16_t**)*Tb);
    for(int i=0;i<Tb;i++)
    {
        tp_dat_out_mem[i] = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Hout*cfg.Wout*cfg.CHout_div_LTout);
        for(int j=0;j<cfg.Hout*cfg.Wout*cfg.CHout_div_LTout;j++)
        {
            tp_dat_out_mem[i][j] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
            if(tp_dat_out_mem[i][j] == NULL){printf("fail to malloc tp_dat_out_mem \n");}
        }
    }

    for(int i=0;i<Tb*cfg.Wout*cfg.Hout*cfg.CHout_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
        {
            if(j>=8* 0 & j<8* 1) dat_out_mem_tmp[i][j] = HBM[ 0][i*HBM_AXI_DATA_WIDTH/32+(j-8* 0)];
            if(j>=8* 1 & j<8* 2) dat_out_mem_tmp[i][j] = HBM[ 1][i*HBM_AXI_DATA_WIDTH/32+(j-8* 1)];
            if(j>=8* 2 & j<8* 3) dat_out_mem_tmp[i][j] = HBM[ 2][i*HBM_AXI_DATA_WIDTH/32+(j-8* 2)];
            if(j>=8* 3 & j<8* 4) dat_out_mem_tmp[i][j] = HBM[ 3][i*HBM_AXI_DATA_WIDTH/32+(j-8* 3)];
            if(j>=8* 4 & j<8* 5) dat_out_mem_tmp[i][j] = HBM[ 4][i*HBM_AXI_DATA_WIDTH/32+(j-8* 4)];
            if(j>=8* 5 & j<8* 6) dat_out_mem_tmp[i][j] = HBM[ 5][i*HBM_AXI_DATA_WIDTH/32+(j-8* 5)];
            if(j>=8* 6 & j<8* 7) dat_out_mem_tmp[i][j] = HBM[ 6][i*HBM_AXI_DATA_WIDTH/32+(j-8* 6)];
            if(j>=8* 7 & j<8* 8) dat_out_mem_tmp[i][j] = HBM[ 7][i*HBM_AXI_DATA_WIDTH/32+(j-8* 7)];
            if(j>=8* 8 & j<8* 9) dat_out_mem_tmp[i][j] = HBM[ 8][i*HBM_AXI_DATA_WIDTH/32+(j-8* 8)];
            if(j>=8* 9 & j<8*10) dat_out_mem_tmp[i][j] = HBM[ 9][i*HBM_AXI_DATA_WIDTH/32+(j-8* 9)];
            if(j>=8*10 & j<8*11) dat_out_mem_tmp[i][j] = HBM[10][i*HBM_AXI_DATA_WIDTH/32+(j-8*10)];
            if(j>=8*11 & j<8*12) dat_out_mem_tmp[i][j] = HBM[11][i*HBM_AXI_DATA_WIDTH/32+(j-8*11)];
            if(j>=8*12 & j<8*13) dat_out_mem_tmp[i][j] = HBM[12][i*HBM_AXI_DATA_WIDTH/32+(j-8*12)];
            if(j>=8*13 & j<8*14) dat_out_mem_tmp[i][j] = HBM[13][i*HBM_AXI_DATA_WIDTH/32+(j-8*13)];
            if(j>=8*14 & j<8*15) dat_out_mem_tmp[i][j] = HBM[14][i*HBM_AXI_DATA_WIDTH/32+(j-8*14)];
            if(j>=8*15 & j<8*16) dat_out_mem_tmp[i][j] = HBM[15][i*HBM_AXI_DATA_WIDTH/32+(j-8*15)];
            if(j>=8*16 & j<8*17) dat_out_mem_tmp[i][j] = HBM[16][i*HBM_AXI_DATA_WIDTH/32+(j-8*16)];
            if(j>=8*17 & j<8*18) dat_out_mem_tmp[i][j] = HBM[17][i*HBM_AXI_DATA_WIDTH/32+(j-8*17)];
            if(j>=8*18 & j<8*19) dat_out_mem_tmp[i][j] = HBM[18][i*HBM_AXI_DATA_WIDTH/32+(j-8*18)];
            if(j>=8*19 & j<8*20) dat_out_mem_tmp[i][j] = HBM[19][i*HBM_AXI_DATA_WIDTH/32+(j-8*19)];
            if(j>=8*20 & j<8*21) dat_out_mem_tmp[i][j] = HBM[20][i*HBM_AXI_DATA_WIDTH/32+(j-8*20)];
            if(j>=8*21 & j<8*22) dat_out_mem_tmp[i][j] = HBM[21][i*HBM_AXI_DATA_WIDTH/32+(j-8*21)];
            if(j>=8*22 & j<8*23) dat_out_mem_tmp[i][j] = HBM[22][i*HBM_AXI_DATA_WIDTH/32+(j-8*22)];
            if(j>=8*23 & j<8*24) dat_out_mem_tmp[i][j] = HBM[23][i*HBM_AXI_DATA_WIDTH/32+(j-8*23)];
            if(j>=8*24 & j<8*25) dat_out_mem_tmp[i][j] = HBM[24][i*HBM_AXI_DATA_WIDTH/32+(j-8*24)];
            if(j>=8*25 & j<8*26) dat_out_mem_tmp[i][j] = HBM[25][i*HBM_AXI_DATA_WIDTH/32+(j-8*25)];
            if(j>=8*26 & j<8*27) dat_out_mem_tmp[i][j] = HBM[26][i*HBM_AXI_DATA_WIDTH/32+(j-8*26)];
            if(j>=8*27 & j<8*28) dat_out_mem_tmp[i][j] = HBM[27][i*HBM_AXI_DATA_WIDTH/32+(j-8*27)];
            if(j>=8*28 & j<8*29) dat_out_mem_tmp[i][j] = HBM[28][i*HBM_AXI_DATA_WIDTH/32+(j-8*28)];
            if(j>=8*29 & j<8*30) dat_out_mem_tmp[i][j] = HBM[29][i*HBM_AXI_DATA_WIDTH/32+(j-8*29)];
            if(j>=8*30 & j<8*31) dat_out_mem_tmp[i][j] = HBM[30][i*HBM_AXI_DATA_WIDTH/32+(j-8*30)];
            if(j>=8*31 & j<8*32) dat_out_mem_tmp[i][j] = HBM[31][i*HBM_AXI_DATA_WIDTH/32+(j-8*31)];  
        }
    }

    for(int i=0;i<Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32; j++)
        {
            dat_out_mem[i][j*2  ] = (uint16_t)(dat_out_mem_tmp[i][j]>> 0);
            dat_out_mem[i][j*2+1] = (uint16_t)(dat_out_mem_tmp[i][j]>>16);
        }
    }

    for(int b=0;b<Tb;b++) 
    {
        for(int i=0;i<cfg.Hout*cfg.Wout*cfg.CHout_div_LTout;i++)
        {
            for(int j=0;j<MAX_DAT_DW*L_Tout/16;j++)
                tp_dat_out_mem[b][i][j] = dat_out_mem[b*cfg.Hout*cfg.Wout*cfg.CHout_div_LTout+i][j];
        }
    }

    for(int b=0;b<Tb;b++) 
        General_DeMap_Feature_Data(cfg, cfg.Hout, cfg.Wout, cfg.CHout, tp_dat_out_mem[b], dat_out[b]);
    
    // Free malloc
    free(dat_out_mem_tmp);
    dat_out_mem_tmp = NULL;
    free(dat_out_mem);
    dat_out_mem = NULL;
    free(tp_dat_out_mem);
    tp_dat_out_mem = NULL;
}