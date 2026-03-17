#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_f2w.h"

struct FPGA_HBM_F2W_cfg GetFPGA_HBM_F2W_cfg(int This_Token, int Last_Token, int Original_Feature_Head, int Weight_Head, int MAX_CH_per_HEAD, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_F2W_cfg ret;

    ret.This_Token                     = This_Token;
    ret.Last_Token                     = Last_Token;
    ret.MAX_CH_per_HEAD                = MAX_CH_per_HEAD;
    ret.Token_CHin                     = (This_Token);
    ret.Token_CHout                    = ret.MAX_CH_per_HEAD;
    ret.Token_Len                      = (ret.This_Token-ret.Last_Token);
    ret.Original_Feature_Head          = Original_Feature_Head;
    ret.Weight_Head                    = Weight_Head;
    ret.MAX_TOKEN                      = MAX_TOKEN;

    ret.DAT_DW_in                      = MAX_DAT_DW;
    ret.DAT_DW_out                     = MAX_DAT_DW;
    ret.WT_DW                          = MAX_DAT_DW;
    ret.BN_DW                          = MAX_BN_DW;
    ret.Tin                            = (base_Tin);

    ret.Win                            = (ret.Token_Len);
    ret.Hin                            = 1;
    ret.CHin                           = ((ret.Token_CHin+Tout-1)/Tout*Tout);
    ret.Wout                           = (ret.Token_Len);
    ret.Hout                           = 1;
    ret.CHout                          = ((ret.Token_CHout+Tout-1)/Tout*Tout);

    ret.Tin_div_Tout                   = ((ret.Tin+Tout-1)/Tout);
    ret.CHin_div_Tout                  = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_div_LTout                 = ((ret.CHin+L_Tout-1)/L_Tout);
    ret.CHin_Padding_with_LTout        = (ret.CHin_div_LTout*L_Tout);
    ret.CHout_div_Tout                 = ((ret.CHout+Tout-1)/Tout);
    ret.CHout_div_LTout                = ((ret.CHout+L_Tout-1)/L_Tout);
    ret.CHout_Padding_with_LTout       = (ret.CHout_div_LTout*L_Tout);
    ret.CHout_Padding_with_Tout        = (ret.CHout_div_Tout*Tout);
                                    
    ret.DAT_IN_BASE_ADDR               = DAT_IN_BASE_ADDR;
    ret.DAT_IN_HEAD_STRIDE             = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);
    ret.DAT_IN_SURFACE_STRIDE          = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE             = (Pixel_Data_Bytes*ret.Win);

    ret.WT_BASE_ADDR                   = WT_BASE_ADDR;
    //ret.WT_CHin                        = (ret.MAX_TOKEN);
    ret.WT_CHin                        = (ret.Token_Len);
    ret.WT_CHout                       = (ret.MAX_CH_per_HEAD);
    ret.WT_CHout_div_Tout              = ((ret.WT_CHout+Tout-1)/Tout);
    ret.WT_CHin_div_Tin                = ((ret.CHin+ret.Tin-1)/ret.Tin);
    ret.WT_CHout_Padding_with_Tout     = (ret.WT_CHout_div_Tout*Tout);
    ret.WT_CHin_Padding_with_Tin       = (ret.WT_CHin_div_Tin*ret.Tin);
    ret.WT_BYTES_PER_HEAD              = (ret.WT_CHout_Padding_with_Tout*ret.WT_CHin_Padding_with_Tin*MAX_DAT_DW/8);
    ret.WT_BYTES_PER_CHOUT             = (ret.WT_BYTES_PER_HEAD/ret.WT_CHout_Padding_with_Tout);
    ret.WT_HEAD_STRIDE                 = ((MAX_DAT_DW*ret.WT_CHout/8)*ret.MAX_TOKEN/HBM_Port);
    ret.WT_LINE_STRIDE                 = ((MAX_DAT_DW*Tout/8)*ret.MAX_TOKEN/HBM_Port);

    ret.DAT_OUT_BASE_ADDR              = DAT_OUT_BASE_ADDR;
    ret.Head_x_CHout                   = (ret.Original_Feature_Head/ret.Weight_Head*ret.CHout);
    ret.Head_x_CHout_div_LTout         = ((ret.Head_x_CHout+L_Tout-1)/L_Tout);
    ret.LTout_div_CHout                = (L_Tout/ret.MAX_CH_per_HEAD);
    ret.Head_x_CH_div_LTout            = ((ret.MAX_CH_per_HEAD*ret.Original_Feature_Head/ret.Weight_Head+L_Tout-1)/L_Tout);
    ret.DAT_OUT_ONE_HEAD_STRIDE        = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_ALL_HEAD_STRIDE        = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.Head_x_CH_div_LTout);
    ret.DAT_OUT_SURFACE_STRIDE         = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE            = (Pixel_Data_Bytes*ret.Wout);

    return ret;
}

void DAT_IN_TRANS_FUNCTION_F2W(struct FPGA_HBM_F2W_cfg cfg, int *dat_in[], int *HBM[])
{
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_in_mem [`Original_Feature_Head*`Hin*`Win*`CHin_div_LTout];
    int **dat_in_mem = (int**)malloc(sizeof(int*)*cfg.Original_Feature_Head*cfg.Hin*cfg.Win*cfg.CHin_div_LTout);
    for (int i=0;i<cfg.Original_Feature_Head*cfg.Hin*cfg.Win*cfg.CHin_div_LTout;i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if(dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }

    // bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_in_mem [`Original_Feature_Head][`Win*`Hin*`CHin_div_LTout];
    int ***tp_dat_in_mem = (int***)malloc(sizeof(int**)*cfg.Original_Feature_Head);
    for(int i=0;i<cfg.Original_Feature_Head;i++)
    {
        tp_dat_in_mem[i] = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.CHin_div_LTout);
        for(int j=0;j<cfg.Win*cfg.Hin*cfg.CHin_div_LTout;j++)
        {
            tp_dat_in_mem[i][j] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
            if(tp_dat_in_mem[i][j] == NULL){printf("fail to malloc tp_dat_in_mem \n");}
        }
    }

    // dat_in
    for(int b=0;b<cfg.Original_Feature_Head;b++)
        General_Map_Feature_Data(cfg, cfg.Hin, cfg.Win, cfg.CHin, dat_in[b], tp_dat_in_mem[b]);

    for(int b=0;b<cfg.Original_Feature_Head;b++) 
    {
        for(int i=0;i<cfg.Win*cfg.Hin*cfg.CHin_div_LTout;i++)
        {
            for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
                dat_in_mem[b*cfg.Win*cfg.Hin*cfg.CHin_div_LTout+i][j] = tp_dat_in_mem[b][i][j];
        }
    }

    for(int i=0;i<cfg.Original_Feature_Head*cfg.Win*cfg.Hin*cfg.CHin_div_LTout;i++)   // depth
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

void WT_IN_TRANS_FUNCTION_F2W(struct FPGA_HBM_F2W_cfg cfg, int *wt_in[], int *HBM[])
{
    // bit [`MAX_DAT_DW*`Tout-1:0] wt_in_mem [`Weight_Head][`WT_CHout_div_Tout][`MAX_TOKEN];
    int ****tp_wt_in_mem = (int****)malloc(sizeof(int***)*cfg.Weight_Head);
    for (int i=0;i<cfg.Weight_Head;i++)
    {
        tp_wt_in_mem[i] = (int***)malloc(sizeof(int**)*cfg.WT_CHout_div_Tout);
        for (int j=0;j<cfg.WT_CHout_div_Tout;j++)
        {
            tp_wt_in_mem[i][j] = (int**)malloc(sizeof(int*)*cfg.MAX_TOKEN);
            for (int k=0;k<cfg.MAX_TOKEN;k++)
            {
                tp_wt_in_mem[i][j][k] = (int*)malloc(sizeof(int)*MAX_DAT_DW*Tout/16);
                if(tp_wt_in_mem[i][j][k] == NULL){printf("fail to malloc tp_wt_in_mem \n");}
            }
        }
    }
    int ****wt_in_mem = (int****)malloc(sizeof(int***)*cfg.Weight_Head);
    for (int i=0;i<cfg.Weight_Head;i++)
    {
        wt_in_mem[i] = (int***)malloc(sizeof(int**)*cfg.WT_CHout_div_Tout);
        for (int j=0;j<cfg.WT_CHout_div_Tout;j++)
        {
            wt_in_mem[i][j] = (int**)malloc(sizeof(int*)*cfg.MAX_TOKEN);
            for (int k=0;k<cfg.MAX_TOKEN;k++)
            {
                wt_in_mem[i][j][k] = (int*)malloc(sizeof(int)*MAX_DAT_DW*Tout/32);
                if(wt_in_mem[i][j][k] == NULL){printf("fail to malloc wt_in_mem \n");}
            }
        }
    }

    for(int i=0;i<cfg.Weight_Head;i++)      
    {
        for(int j=0;j<cfg.WT_CHout_div_Tout;j++) 
        {
            for(int k=0;k<cfg.This_Token;k++)  // This_Token 0-32-64..., 1-33-65...MaxToken
            {
                for(int m=0;m<Tout;m++)
                    tp_wt_in_mem[i][j][k][m] = wt_in[i][k*cfg.WT_CHout+j*Tout+m];
                // Combine fp16 to 32bit
                for(int n=0;n<MAX_DAT_DW*Tout/32;n++)
                    wt_in_mem[i][j][k][n] = ((tp_wt_in_mem[i][j][k][2*n+1]<<16)&0xffff0000)+(tp_wt_in_mem[i][j][k][2*n]&0x0000ffff);
            }
        }
    }

    for(int i=0;i<cfg.Weight_Head;i++)
    {
        for(int k=0;k<=cfg.This_Token/Tout;k++)  // token group
        {
            for(int s=0;s<(Tout/HBM_Port);s++)
            {
                for(int r=0;r<HBM_Port;r++)
                {
                    if((k*Tout+s*HBM_Port+r)<cfg.This_Token)
                    {
                        for(int j=0;j<cfg.WT_CHout_div_Tout;j++)
                        {
                            for(int m=0;m<AXI_DATA_WIDTH/32;m++)
                            {
                                int current_port;
                                int current_Token;
                                int Kcache_addr_in_HBM;
                                current_port       = r;
                                current_Token      = k*Tout+s*HBM_Port+r;
                                Kcache_addr_in_HBM = i*cfg.WT_HEAD_STRIDE                +
                                                     k*(AXI_DATA_WIDTH/8)*(Tout/HBM_Port)+
                                                     j*cfg.WT_LINE_STRIDE                +
                                                     s*(AXI_DATA_WIDTH/8);   
                                                     // (byte) i:8*MAX_TOKEN    j:64

                                if(current_port== 0) HBM[ 0][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 1) HBM[ 1][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 2) HBM[ 2][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 3) HBM[ 3][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 4) HBM[ 4][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 5) HBM[ 5][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 6) HBM[ 6][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 7) HBM[ 7][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 8) HBM[ 8][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port== 9) HBM[ 9][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==10) HBM[10][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==11) HBM[11][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==12) HBM[12][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==13) HBM[13][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==14) HBM[14][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==15) HBM[15][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==16) HBM[16][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==17) HBM[17][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==18) HBM[18][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==19) HBM[19][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==20) HBM[20][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==21) HBM[21][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==22) HBM[22][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==23) HBM[23][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==24) HBM[24][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==25) HBM[25][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==26) HBM[26][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==27) HBM[27][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==28) HBM[28][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==29) HBM[29][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==30) HBM[30][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                                if(current_port==31) HBM[31][Kcache_addr_in_HBM/4+m] = wt_in_mem[i][j][current_Token][m];
                            }
                        }
                    }
                }
            }
        }
    }

    // Free malloc
    free(tp_wt_in_mem);
    tp_wt_in_mem = NULL;
    free(wt_in_mem);
    wt_in_mem = NULL;
}

void DAT_OUT_TRANS_FUNCTION_F2W(struct FPGA_HBM_F2W_cfg cfg, int *HBM[], uint16_t *dat_out[])
{
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_out_mem [`Weight_Head*`Hout*`Wout*`Head_x_CHout_div_LTout];
    int **dat_out_mem_tmp = (int**)malloc(sizeof(int*)*cfg.Weight_Head*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
    for (int i=0;i<cfg.Weight_Head*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        dat_out_mem_tmp[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if (dat_out_mem_tmp[i] == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    }
    uint16_t **dat_out_mem = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Weight_Head*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
    for (int i=0;i<cfg.Weight_Head*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
    {
        dat_out_mem[i] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
        if(dat_out_mem[i] == NULL){printf("fail to malloc dat_out_mem \n");}
    }

    // bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_out_mem [`Weight_Head ][`Hout* `Wout *`Head_x_CHout_div_LTout];
    uint16_t ***tp_dat_out_mem = (uint16_t***)malloc(sizeof(uint16_t**)*cfg.Weight_Head);
    for(int i=0;i<cfg.Weight_Head;i++)
    {
        tp_dat_out_mem[i] = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout);
        for(int j=0;j<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;j++)
        {
            tp_dat_out_mem[i][j] = (uint16_t*)malloc(sizeof(uint16_t)*MAX_DAT_DW*L_Tout/16);
            if(tp_dat_out_mem[i][j] == NULL){printf("fail to malloc tp_dat_out_mem \n");}
        }
    }
    
    //bit [`DAT_DW_out-1:0] tp_dat_out [`Weight_Head ][`Hout][`Wout][`Head_x_CHout];
    uint16_t **tp_dat_out = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Weight_Head);
    for(int i=0;i<cfg.Weight_Head;i++)
    {
        tp_dat_out[i] = (uint16_t*)malloc(sizeof(int)*cfg.Hout*cfg.Wout*cfg.Head_x_CHout);
        if(tp_dat_out[i] == NULL){printf("fail to malloc tp_dat_out \n");}
    }

    for(int i=0;i<cfg.Weight_Head*cfg.Wout*cfg.Hout*cfg.Head_x_CHout_div_LTout;i++)     // depth
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

    for(int i=0;i<cfg.Weight_Head*cfg.Win*cfg.Hin*cfg.Head_x_CHout_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32; j++)
        {
            dat_out_mem[i][j*2  ] = (uint16_t)(dat_out_mem_tmp[i][j]>> 0);
            dat_out_mem[i][j*2+1] = (uint16_t)(dat_out_mem_tmp[i][j]>>16);
        }
    }

    for(int a=0;a<cfg.Weight_Head;a++) 
    {
        for(int i=0;i<cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout;i++)
        {
            for(int j=0;j<MAX_DAT_DW*L_Tout/16;j++)
                tp_dat_out_mem[a][i][j] = dat_out_mem[a*cfg.Hout*cfg.Wout*cfg.Head_x_CHout_div_LTout+i][j];
        }
    }

    for(int a=0;a<cfg.Weight_Head;a++) 
        General_DeMap_Feature_Data(cfg, cfg.Hout, cfg.Wout, cfg.Head_x_CHout, tp_dat_out_mem[a], tp_dat_out[a]);
    
    for(int a=0;a<cfg.Weight_Head;a++)
    {
        for(int b=0;b<cfg.Original_Feature_Head/cfg.Weight_Head;b++)
        {
            for(int i=0;i<cfg.Hout;i++)
            {
                for(int j=0;j<cfg.Wout;j++)
                {
                    for(int k=0;k<cfg.CHout;k++)
                        dat_out[a*(cfg.Original_Feature_Head/cfg.Weight_Head)+b][i*cfg.Wout*cfg.CHout+j*cfg.CHout+k] = 
                        tp_dat_out[a][i*cfg.Wout*(cfg.Original_Feature_Head/cfg.Weight_Head)*cfg.CHout+
                        j*(cfg.Original_Feature_Head/cfg.Weight_Head)*cfg.CHout+b*cfg.CHout+k];
                }
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