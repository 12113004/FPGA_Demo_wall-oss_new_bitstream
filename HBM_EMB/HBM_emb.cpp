#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_emb.h"

struct FPGA_HBM_EMB_cfg GetFPGA_HBM_EMB_cfg(int Head, int Height, int Hin, int Width_in, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t POS_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_EMB_cfg ret;

    ret.Head                   = Head;
    ret.Height                 = Height;
    ret.Width_in               = Width_in;
    ret.Width_out              = ret.Width_in;

    ret.DAT_DW_L0              = MAX_DAT_DW;
    ret.DAT_DW_L1              = MAX_DAT_DW;
    ret.WT_DW                  = MAX_WT_DW;
    ret.BN_DW                  = MAX_BN_DW;
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
    ret.CHin_Padding_div_Head  = (ret.CHin_Padding/ret.Head);
    ret.CHout_div_Tout         = ((ret.CHout+Tout-1)/Tout);
    ret.CHout_div_LTout        = ((ret.CHout+L_Tout-1)/L_Tout);
    ret.CHout_Padding          = (ret.CHout_div_LTout*L_Tout);
    ret.CHout_Padding_div_Head = (ret.CHout_Padding/ret.Head);
    ret.Head_x_CHin            = (ret.Head*ret.CHin);
    ret.Head_x_CHin_div_LTout  = ((ret.Head_x_CHin+L_Tout-1)/L_Tout);
    ret.Head_x_CHout           = (ret.Head*ret.CHout);
    ret.Head_x_CHout_div_LTout = ((ret.Head_x_CHout+L_Tout-1)/L_Tout);
    ret.LTout_div_CHout        = (L_Tout/ret.Width_out);
    ret.Head_x_CH_div_LTout    = ((ret.Head+ret.LTout_div_CHout-1)/ret.LTout_div_CHout);

    ret.DAT_IN_BASE_ADDR       = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE    = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);     // Pixel_Data_Bytes = 32
    ret.DAT_IN_HEAD_STRIDE     = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);     // Pixel_Data_Bytes = 32
    ret.DAT_IN_SURFACE_STRIDE  = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE     = (Pixel_Data_Bytes*ret.Win);
    
    ret.Pos_Num                = MAX_TOKEN;
    ret.POS_IN_BASE_ADDR       = POS_IN_BASE_ADDR;
    ret.POS_HEAD_STRIDE        = (Pixel_Data_Bytes*ret.Pos_Num*ret.CHout_div_LTout);
    ret.POS_LINE_STRIDE        = (Pixel_Data_Bytes*ret.Pos_Num);
    
    ret.DAT_OUT_BASE_ADDR      = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE   = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_HEAD_STRIDE    = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_SURFACE_STRIDE = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE    = (Pixel_Data_Bytes*ret.Wout);

    return ret;
}

void DAT_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *dat_in[], int *HBM[])
{
    //bit [`DAT_DW_in-1:0] tp_dat_in [`Hin][`Win][`Head_x_CHin];
    int *tp_dat_in = (int*)malloc(sizeof(int*)*cfg.Hin*cfg.Win*cfg.Head_x_CHin);
    if(tp_dat_in == NULL){printf("fail to malloc tp_dat_in \n");}

    //bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_in_mem [`Win*`Hin*`Head_x_CHin_div_LTout];
    int **tp_dat_in_mem = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout);
    for(int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        tp_dat_in_mem[i] = (int*)malloc(sizeof(int*)*MAX_DAT_DW*L_Tout/32);
        if(tp_dat_in_mem[i] == NULL){printf("fail to malloc tp_dat_in_mem \n");}
    }
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_in_mem [`Win*`Hin*`Head_x_CHin_div_LTout];
    int **dat_in_mem = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout);
    for (int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if(dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }
    
    // dat_in
    for(int b=0;b<cfg.Head;b++)
    {
        for(int i=0;i<cfg.Hin;i++)
        {
            for(int j=0;j<cfg.Win;j++)
            {
                for(int k=0;k<cfg.CHin;k++)
                    tp_dat_in[i*cfg.Win*cfg.Head*cfg.CHin+j*cfg.Head*cfg.CHin+b*cfg.CHin+k] = dat_in[b][i*cfg.Win*cfg.CHin+j*cfg.CHin+k];   //Head, Hin, Win, CHin -> Hin, Win, Head_x_CHin
            }
        }
    }
    
    General_Map_Feature_Data(cfg, cfg.Hin, cfg.Win, cfg.Head_x_CHin, tp_dat_in, tp_dat_in_mem);
        
    for(int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
            dat_in_mem[i][j] = tp_dat_in_mem[i][j];
    }

    for(int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)   // depth
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

void POS_IN_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *pos_in, int *HBM[])
{
    // bit [`MAX_DAT_DW*`L_Tout-1:0] pos_in_mem[`Pos_Num*`CHin_div_LTout/2];
    int **pos_in_mem = (int**)malloc(sizeof(int*)*cfg.Pos_Num*cfg.CHin_div_LTout);
    for(int i=0;i<cfg.Pos_Num*cfg.CHin_div_LTout;i++)
    {
        pos_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if (pos_in_mem[i] == NULL){printf("fail to malloc pos_in_mem \n");}
    }

    General_Map_Pos_Data(cfg, pos_in, pos_in_mem);

    for(int i=0;i<cfg.Pos_Num*cfg.CHin_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)             // 8192bit/32bit = 256
        {
            if(j>=8* 0 & j<8* 1) HBM[ 0][i*HBM_AXI_DATA_WIDTH/32+(j-8* 0)] = pos_in_mem[i][j];
            if(j>=8* 1 & j<8* 2) HBM[ 1][i*HBM_AXI_DATA_WIDTH/32+(j-8* 1)] = pos_in_mem[i][j];
            if(j>=8* 2 & j<8* 3) HBM[ 2][i*HBM_AXI_DATA_WIDTH/32+(j-8* 2)] = pos_in_mem[i][j];
            if(j>=8* 3 & j<8* 4) HBM[ 3][i*HBM_AXI_DATA_WIDTH/32+(j-8* 3)] = pos_in_mem[i][j];
            if(j>=8* 4 & j<8* 5) HBM[ 4][i*HBM_AXI_DATA_WIDTH/32+(j-8* 4)] = pos_in_mem[i][j];
            if(j>=8* 5 & j<8* 6) HBM[ 5][i*HBM_AXI_DATA_WIDTH/32+(j-8* 5)] = pos_in_mem[i][j];
            if(j>=8* 6 & j<8* 7) HBM[ 6][i*HBM_AXI_DATA_WIDTH/32+(j-8* 6)] = pos_in_mem[i][j];
            if(j>=8* 7 & j<8* 8) HBM[ 7][i*HBM_AXI_DATA_WIDTH/32+(j-8* 7)] = pos_in_mem[i][j];
            if(j>=8* 8 & j<8* 9) HBM[ 8][i*HBM_AXI_DATA_WIDTH/32+(j-8* 8)] = pos_in_mem[i][j];
            if(j>=8* 9 & j<8*10) HBM[ 9][i*HBM_AXI_DATA_WIDTH/32+(j-8* 9)] = pos_in_mem[i][j];
            if(j>=8*10 & j<8*11) HBM[10][i*HBM_AXI_DATA_WIDTH/32+(j-8*10)] = pos_in_mem[i][j];
            if(j>=8*11 & j<8*12) HBM[11][i*HBM_AXI_DATA_WIDTH/32+(j-8*11)] = pos_in_mem[i][j];
            if(j>=8*12 & j<8*13) HBM[12][i*HBM_AXI_DATA_WIDTH/32+(j-8*12)] = pos_in_mem[i][j];
            if(j>=8*13 & j<8*14) HBM[13][i*HBM_AXI_DATA_WIDTH/32+(j-8*13)] = pos_in_mem[i][j];
            if(j>=8*14 & j<8*15) HBM[14][i*HBM_AXI_DATA_WIDTH/32+(j-8*14)] = pos_in_mem[i][j];
            if(j>=8*15 & j<8*16) HBM[15][i*HBM_AXI_DATA_WIDTH/32+(j-8*15)] = pos_in_mem[i][j];
            if(j>=8*16 & j<8*17) HBM[16][i*HBM_AXI_DATA_WIDTH/32+(j-8*16)] = pos_in_mem[i][j];
            if(j>=8*17 & j<8*18) HBM[17][i*HBM_AXI_DATA_WIDTH/32+(j-8*17)] = pos_in_mem[i][j];
            if(j>=8*18 & j<8*19) HBM[18][i*HBM_AXI_DATA_WIDTH/32+(j-8*18)] = pos_in_mem[i][j];
            if(j>=8*19 & j<8*20) HBM[19][i*HBM_AXI_DATA_WIDTH/32+(j-8*19)] = pos_in_mem[i][j];
            if(j>=8*20 & j<8*21) HBM[20][i*HBM_AXI_DATA_WIDTH/32+(j-8*20)] = pos_in_mem[i][j];
            if(j>=8*21 & j<8*22) HBM[21][i*HBM_AXI_DATA_WIDTH/32+(j-8*21)] = pos_in_mem[i][j];
            if(j>=8*22 & j<8*23) HBM[22][i*HBM_AXI_DATA_WIDTH/32+(j-8*22)] = pos_in_mem[i][j];
            if(j>=8*23 & j<8*24) HBM[23][i*HBM_AXI_DATA_WIDTH/32+(j-8*23)] = pos_in_mem[i][j];
            if(j>=8*24 & j<8*25) HBM[24][i*HBM_AXI_DATA_WIDTH/32+(j-8*24)] = pos_in_mem[i][j];
            if(j>=8*25 & j<8*26) HBM[25][i*HBM_AXI_DATA_WIDTH/32+(j-8*25)] = pos_in_mem[i][j];
            if(j>=8*26 & j<8*27) HBM[26][i*HBM_AXI_DATA_WIDTH/32+(j-8*26)] = pos_in_mem[i][j];
            if(j>=8*27 & j<8*28) HBM[27][i*HBM_AXI_DATA_WIDTH/32+(j-8*27)] = pos_in_mem[i][j];
            if(j>=8*28 & j<8*29) HBM[28][i*HBM_AXI_DATA_WIDTH/32+(j-8*28)] = pos_in_mem[i][j];
            if(j>=8*29 & j<8*30) HBM[29][i*HBM_AXI_DATA_WIDTH/32+(j-8*29)] = pos_in_mem[i][j];
            if(j>=8*30 & j<8*31) HBM[30][i*HBM_AXI_DATA_WIDTH/32+(j-8*30)] = pos_in_mem[i][j];
            if(j>=8*31 & j<8*32) HBM[31][i*HBM_AXI_DATA_WIDTH/32+(j-8*31)] = pos_in_mem[i][j];  
        }
    }

    // Free malloc
    free(pos_in_mem);
    pos_in_mem = NULL;
}

void DAT_OUT_TRANS_FUNCTION_EMB(struct FPGA_HBM_EMB_cfg cfg, int *HBM[], uint16_t *dat_out[])
{
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_out_mem [`Hout* `Wout *`Head_x_CHout_div_LTout];
    int **dat_out_mem_tmp = (int**)malloc(sizeof(int*)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
    for (int i=0;i<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        dat_out_mem_tmp[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if (dat_out_mem_tmp[i] == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    }
    uint16_t **dat_out_mem = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
    for (int i=0;i<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        dat_out_mem[i] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
        if (dat_out_mem[i] == NULL){printf("fail to malloc dat_out_mem \n");}
    }
    // bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_out_mem [`Hout*`Wout*`Head_x_CHout_div_LTout];
    uint16_t **tp_dat_out_mem = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
    for (int i=0;i<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        tp_dat_out_mem[i] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
        if (tp_dat_out_mem[i] == NULL){printf("fail to malloc tp_dat_out_mem \n");}
    }
    //bit [`DAT_DW_out-1:0] tp_dat_out [`Hout][`Wout][`Head_x_CHout];
    uint16_t *tp_dat_out = (uint16_t*)malloc(sizeof(uint16_t*)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout);
    if(tp_dat_out == NULL){printf("fail to malloc tp_dat_out \n");}

    for(int i=0;i<cfg.Wout*cfg.Hout*cfg.Head_x_CHout_div_LTout;i++)
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

    for(int i=0;i<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32; j++)
        {
            dat_out_mem[i][j*2  ] = (uint16_t)(dat_out_mem_tmp[i][j]>> 0);
            dat_out_mem[i][j*2+1] = (uint16_t)(dat_out_mem_tmp[i][j]>>16);
        }
    }

    for(int i=0;i<cfg.Wout*cfg.Hout*cfg.Head_x_CHout_div_LTout;i++)
        tp_dat_out_mem[i] = dat_out_mem[i];

    General_DeMap_Feature_Data(cfg, cfg.Hout, cfg.Wout, cfg.Head_x_CHout, tp_dat_out_mem, tp_dat_out);
    
    for(int b=0;b<cfg.Head;b++)
    {
        for(int i=0;i<cfg.Hout;i++)
        {
            for(int j=0;j<cfg.Wout;j++) //cfg.Token-cfg.last_token
            {
                for(int k=0;k<cfg.CHout;k++)
                    dat_out[b][i*cfg.Wout*cfg.CHout+j*cfg.CHout+k] = tp_dat_out[i*cfg.Wout*cfg.Head*cfg.CHout+j*cfg.Head*cfg.CHout+b*cfg.CHout+k];   //Hout, Wout, Head_x_CHout -> Head, Hout, Wout, CHout
            }
        }
    }
    
    // Free malloc
    free(dat_out_mem_tmp);
    dat_out_mem_tmp = NULL;
    free(dat_out_mem);
    dat_out_mem = NULL;
    free(tp_dat_out_mem);
    tp_dat_out_mem = NULL;
    free(tp_dat_out);
    tp_dat_out = NULL;
}