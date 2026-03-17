#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_kv2hbm.h"

struct FPGA_HBM_KV2HBM_cfg GetFPGA_HBM_KV2HBM_cfg(int This_Token, int Last_Token, int Weight_Head, int MAX_CH_per_HEAD, int MAX_TOKEN, uint64_t DAT_IN_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_KV2HBM_cfg ret;

    ret.This_Token                     = This_Token;
    ret.Last_Token                     = Last_Token;
    ret.MAX_CH_per_HEAD                = MAX_CH_per_HEAD;
    ret.Token_CHin                     = ret.MAX_CH_per_HEAD;
    ret.Token_CHout                    = (ret.Token_CHin);
    ret.Token_Len                      = (ret.This_Token-ret.Last_Token);
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

    ret.CHin_div_Tout                  = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_div_LTout                 = ((ret.CHin+L_Tout-1)/L_Tout);
    ret.CHin_Padding_with_LTout        = (ret.CHin_div_LTout*L_Tout);
    ret.CHout_div_Tout                 = ((ret.CHout+Tout-1)/Tout);
    ret.CHout_div_LTout                = ((ret.CHout+L_Tout-1)/L_Tout);
    ret.CHout_Padding_with_LTout       = (ret.CHout_div_LTout*L_Tout);
    ret.CHout_Padding_with_Tout        = (ret.CHout_div_Tout*Tout);
    ret.LTout_div_CHin                 = (L_Tout/ret.MAX_CH_per_HEAD);
    ret.Head_x_CHin                    = (ret.Weight_Head*ret.MAX_CH_per_HEAD);
    ret.Head_x_CHin_div_LTout          = ((ret.Head_x_CHin+L_Tout-1)/L_Tout);
    ret.WT_HEAD_STRIDE                 = ((MAX_DAT_DW*ret.MAX_CH_per_HEAD/8)*ret.MAX_TOKEN/HBM_Port);
    ret.WT_LINE_STRIDE                 = ((MAX_DAT_DW*Tout/8)*ret.MAX_TOKEN/HBM_Port);

    ret.DAT_IN_BASE_ADDR               = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE            = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);
    ret.DAT_IN_HEAD_STRIDE             = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);
    ret.DAT_IN_LINE_STRIDE             = (Pixel_Data_Bytes*ret.Win);

    ret.DAT_OUT_BASE_ADDR              = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE           = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_HEAD_STRIDE            = ret.WT_HEAD_STRIDE;
    ret.DAT_OUT_LINE_STRIDE            = ret.WT_LINE_STRIDE;

    return ret;
}

void DAT_IN_TRANS_FUNCTION_KV2HBM(struct FPGA_HBM_KV2HBM_cfg cfg, int *dat_in[], int *HBM[], int *dat_out_software[])
{
    //bit [`MAX_DAT_DW*`Tout-1:0] software_out [`Weight_Head][`Win*`Hin*`CHin_div_Tout];
    int ***tp_dat_out_software = (int***)malloc(sizeof(int**)*cfg.Weight_Head);
    for (int i=0;i<cfg.Weight_Head;i++)
    {
        tp_dat_out_software[i] = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.CHin_div_Tout);
        for(int j=0;j<cfg.Win*cfg.Hin*cfg.CHin_div_Tout;j++)
        {
            tp_dat_out_software[i][j] = (int*)malloc(sizeof(int)*MAX_DAT_DW*Tout/32);
            if(tp_dat_out_software[i][j] == NULL){printf("fail to malloc dat_in_mem \n");}
        }
    }

    // bit [`DAT_DW_in-1:0] tp_dat_in [`Hin][`Win][`Head_x_CHin];
    int *tp_dat_in = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.Head_x_CHin);
    if(tp_dat_in == NULL){printf("fail to malloc tp_dat_in \n");}

    // bit [`MAX_DAT_DW*`L_Tout-1:0] tp_dat_in_mem [`Win*`Hin*`Head_x_CHin_div_LTout];
    int **tp_dat_in_mem = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout);
    for (int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        tp_dat_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if(tp_dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }
    // bit [`MAX_DAT_DW*`L_Tout-1:0] dat_in_mem [`Win*`Hin*`Head_x_CHin_div_LTout];
    int **dat_in_mem = (int**)malloc(sizeof(int*)*cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout);
    for (int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*MAX_DAT_DW*L_Tout/32);
        if(dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }

    /* Software Process */
    for(int b=0;b<cfg.Weight_Head;b++)
        General_Map_Feature_Data_software(cfg, cfg.Hin, cfg.Win, cfg.CHin, dat_in[b], tp_dat_out_software[b]);
    
    for(int i=0;i<cfg.Weight_Head;i++)      
    {
        for(int j=0;j<cfg.Win*cfg.Hin*cfg.CHin_div_Tout;j++)  // This_Token 0-32-64..., 1-33-65...
        {
            for(int m=0;m<AXI_DATA_WIDTH/32;m++)
                dat_out_software[i][j*AXI_DATA_WIDTH/32+m] = tp_dat_out_software[i][j][m];
        }
    }
        
    /* haedware data transfer */
    for(int b=0;b<cfg.Weight_Head;b++)
    {
        for(int i=0;i<cfg.Hin;i++)
        {
            for(int j=0;j<cfg.Win;j++)
            {
                for(int k=0;k<cfg.CHin;k++)
                    tp_dat_in[i*cfg.Win*cfg.Weight_Head*cfg.CHin+j*cfg.Weight_Head*cfg.CHin+b*cfg.CHin+k]=dat_in[b][i*cfg.Win*cfg.CHin+j*cfg.CHin+k]; //Weight_Head, Hin, Win, CHin -> Hin, Win, Head_x_CHin
            }
        }
    }

    General_Map_Feature_Data(cfg, cfg.Hin, cfg.Win, cfg.Head_x_CHin, tp_dat_in, tp_dat_in_mem);
        
    for(int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
            dat_in_mem[i][j] = tp_dat_in_mem[i][j];
    }
    
    for(int i=0;i<cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout;i++)
    {
        for(int j=0;j<MAX_DAT_DW*L_Tout/32;j++)
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
    free(tp_dat_in);
    tp_dat_in = NULL;
    free(tp_dat_in_mem);
    tp_dat_in_mem = NULL;
    free(dat_in_mem);
    dat_in_mem = NULL;
}

void DAT_OUT_TRANS_FUNCTION_KV2HBM(struct FPGA_HBM_KV2HBM_cfg cfg, enum KV_Mode KV_Mode, int *HBM[], uint16_t *dat_out[])
{

    //bit [`AXI_DATA_WIDTH-1:0] dat_out_hardware [`Weight_Head][`CHout_div_Tout][`Wout];
    int ****dat_out_mem_tmp = (int****)malloc(sizeof(int***)*cfg.Weight_Head);
    for (int i=0;i<cfg.Weight_Head;i++)
    {
        dat_out_mem_tmp[i] = (int***)malloc(sizeof(int**)*cfg.CHout_div_Tout);
        for (int j=0;j<cfg.CHout_div_Tout;j++)
        {
            dat_out_mem_tmp[i][j] = (int**)malloc(sizeof(int*)*cfg.Wout);
            for (int k=0;k<cfg.Wout;k++)
            {
                dat_out_mem_tmp[i][j][k] = (int*)malloc(sizeof(int)*AXI_DATA_WIDTH/32);
                if(dat_out_mem_tmp[i][j][k] == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
            }
        }
    }
    uint16_t ****dat_out_mem = (uint16_t****)malloc(sizeof(uint16_t***)*cfg.Weight_Head);
    for (uint16_t i=0;i<cfg.Weight_Head;i++)
    {
        dat_out_mem[i] = (uint16_t***)malloc(sizeof(uint16_t**)*cfg.CHout_div_Tout);
        for (uint16_t j=0;j<cfg.CHout_div_Tout;j++)
        {
            dat_out_mem[i][j] = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Wout);
            for (uint16_t k=0;k<cfg.Wout;k++)
            {
                dat_out_mem[i][j][k] = (uint16_t*)malloc(sizeof(uint16_t)*AXI_DATA_WIDTH/16);
                if(dat_out_mem[i][j][k] == NULL){printf("fail to malloc dat_out_mem \n");}
            }
        }
    }

    if(KV_Mode == K_Mode)
    {
        for(int i=0;i<cfg.Weight_Head;i++)      
        {
            for(int k=0;k<=cfg.This_Token/Tout;k++)  // token group
            {
                for(int s=0;s<(Tout/HBM_Port);s++)
                {
                    for(int r=0;r<HBM_Port;r++)
                    {
                        if(((k*Tout+s*HBM_Port+r)<cfg.This_Token)&((k*Tout+s*HBM_Port+r)>=cfg.Last_Token))
                        {
                            for(int j=0;j<cfg.CHout_div_Tout;j++)
                            {
                                for(int m=0;m<AXI_DATA_WIDTH/32;m++)
                                {
                                    int current_port;
                                    int current_Token;
                                    int Kcache_addr_in_HBM;
                                    current_port       = r;
                                    current_Token      = k*Tout+s*HBM_Port+r;
                                    Kcache_addr_in_HBM = i*cfg.WT_HEAD_STRIDE                                   +
                                                         k*cfg.CHout_div_Tout*(AXI_DATA_WIDTH/8)*(Tout/HBM_Port)+
                                                         j*(AXI_DATA_WIDTH/8)*(Tout/HBM_Port)                   +
                                                         s*(AXI_DATA_WIDTH/8);
                                                         // (byte) i:8*MAX_TOKEN    j:64

                                    if(current_port== 0) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 0][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 1) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 1][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 2) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 2][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 3) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 3][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 4) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 4][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 5) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 5][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 6) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 6][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 7) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 7][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 8) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 8][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 9) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 9][Kcache_addr_in_HBM/4+m];
                                    if(current_port==10) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[10][Kcache_addr_in_HBM/4+m];
                                    if(current_port==11) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[11][Kcache_addr_in_HBM/4+m];
                                    if(current_port==12) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[12][Kcache_addr_in_HBM/4+m];
                                    if(current_port==13) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[13][Kcache_addr_in_HBM/4+m];
                                    if(current_port==14) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[14][Kcache_addr_in_HBM/4+m];
                                    if(current_port==15) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[15][Kcache_addr_in_HBM/4+m];
                                    if(current_port==16) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[16][Kcache_addr_in_HBM/4+m];
                                    if(current_port==17) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[17][Kcache_addr_in_HBM/4+m];
                                    if(current_port==18) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[18][Kcache_addr_in_HBM/4+m];
                                    if(current_port==19) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[19][Kcache_addr_in_HBM/4+m];
                                    if(current_port==20) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[20][Kcache_addr_in_HBM/4+m];
                                    if(current_port==21) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[21][Kcache_addr_in_HBM/4+m];
                                    if(current_port==22) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[22][Kcache_addr_in_HBM/4+m];
                                    if(current_port==23) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[23][Kcache_addr_in_HBM/4+m];
                                    if(current_port==24) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[24][Kcache_addr_in_HBM/4+m];
                                    if(current_port==25) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[25][Kcache_addr_in_HBM/4+m];
                                    if(current_port==26) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[26][Kcache_addr_in_HBM/4+m];
                                    if(current_port==27) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[27][Kcache_addr_in_HBM/4+m];
                                    if(current_port==28) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[28][Kcache_addr_in_HBM/4+m];
                                    if(current_port==29) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[29][Kcache_addr_in_HBM/4+m];
                                    if(current_port==30) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[30][Kcache_addr_in_HBM/4+m];
                                    if(current_port==31) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[31][Kcache_addr_in_HBM/4+m];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if(KV_Mode == V_Mode)
    {
        for(int i=0;i<cfg.Weight_Head;i++)      
        {
            for(int k=0;k<=cfg.This_Token/Tout;k++)  // token group
            {
                for(int s=0;s<(Tout/HBM_Port);s++)
                {
                    for(int r=0;r<HBM_Port;r++)
                    {
                        if (((k*Tout+s*HBM_Port+r)<cfg.This_Token)&((k*Tout+s*HBM_Port+r)>=cfg.Last_Token))
                        {
                            for(int j=0;j<cfg.CHout_div_Tout;j++)
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

                                    if(current_port== 0) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 0][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 1) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 1][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 2) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 2][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 3) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 3][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 4) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 4][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 5) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 5][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 6) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 6][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 7) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 7][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 8) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 8][Kcache_addr_in_HBM/4+m];
                                    if(current_port== 9) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[ 9][Kcache_addr_in_HBM/4+m];
                                    if(current_port==10) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[10][Kcache_addr_in_HBM/4+m];
                                    if(current_port==11) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[11][Kcache_addr_in_HBM/4+m];
                                    if(current_port==12) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[12][Kcache_addr_in_HBM/4+m];
                                    if(current_port==13) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[13][Kcache_addr_in_HBM/4+m];
                                    if(current_port==14) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[14][Kcache_addr_in_HBM/4+m];
                                    if(current_port==15) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[15][Kcache_addr_in_HBM/4+m];
                                    if(current_port==16) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[16][Kcache_addr_in_HBM/4+m];
                                    if(current_port==17) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[17][Kcache_addr_in_HBM/4+m];
                                    if(current_port==18) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[18][Kcache_addr_in_HBM/4+m];
                                    if(current_port==19) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[19][Kcache_addr_in_HBM/4+m];
                                    if(current_port==20) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[20][Kcache_addr_in_HBM/4+m];
                                    if(current_port==21) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[21][Kcache_addr_in_HBM/4+m];
                                    if(current_port==22) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[22][Kcache_addr_in_HBM/4+m];
                                    if(current_port==23) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[23][Kcache_addr_in_HBM/4+m];
                                    if(current_port==24) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[24][Kcache_addr_in_HBM/4+m];
                                    if(current_port==25) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[25][Kcache_addr_in_HBM/4+m];
                                    if(current_port==26) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[26][Kcache_addr_in_HBM/4+m];
                                    if(current_port==27) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[27][Kcache_addr_in_HBM/4+m];
                                    if(current_port==28) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[28][Kcache_addr_in_HBM/4+m];
                                    if(current_port==29) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[29][Kcache_addr_in_HBM/4+m];
                                    if(current_port==30) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[30][Kcache_addr_in_HBM/4+m];
                                    if(current_port==31) dat_out_mem_tmp[i][j][current_Token-cfg.Last_Token][m] = HBM[31][Kcache_addr_in_HBM/4+m];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(int i=0;i<cfg.Weight_Head;i++)      
    {
        for(int j=0;j<cfg.CHout_div_Tout;j++) 
        {
            for(int k=0;k<cfg.Wout;k++)  // This_Token 0-32-64..., 1-33-65...MaxToken
            {
                for(int m=0;m<AXI_DATA_WIDTH/32;m++)
                {
                    dat_out_mem[i][j][k][m*2  ] = (uint16_t)(dat_out_mem_tmp[i][j][k][m]>> 0);
                    dat_out_mem[i][j][k][m*2+1] = (uint16_t)(dat_out_mem_tmp[i][j][k][m]>>16);
                }
            }
        }
    }

    for(int i=0;i<cfg.Weight_Head;i++)      
    {
        for(int j=0;j<cfg.CHout_div_Tout;j++) 
        {
            for(int k=0;k<cfg.This_Token-cfg.Last_Token;k++)  // This_Token 0-32-64..., 1-33-65...
            {
                for(int m=0;m<AXI_DATA_WIDTH/16;m++)
                    dat_out[i][j*cfg.Win*AXI_DATA_WIDTH/16+k*AXI_DATA_WIDTH/16+m] = dat_out_mem[i][j][k][m];
            }
        }
    }
    
    // Free malloc
    free(dat_out_mem_tmp);
    dat_out_mem_tmp = NULL;
    free(dat_out_mem);
    dat_out_mem = NULL;
}