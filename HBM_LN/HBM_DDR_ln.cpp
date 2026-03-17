#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_ln.h"

struct FPGA_HBM_LN_cfg GetFPGA_HBM_LN_cfg(int Height, int Width_in, int RMS_Norm, int Hin, int RELU_EN, uint64_t DAT_IN_BASE_ADDR, uint64_t LN_WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_LN_cfg ret;

    ret.Height                 = Height;    //19
    ret.Width_in               = Width_in;  //4096

    ret.RMS_Norm               = RMS_Norm;  //0
    ret.Layer_Norm             = (1-ret.RMS_Norm);

    ret.DAT_DW_L0              = MAX_DAT_DW;
    ret.DAT_DW_L1              = MAX_DAT_DW;
    ret.WT_DW                  = MAX_WT_DW;
    ret.LN_DW                  = MAX_BN_DW;
    ret.Tin                    = (base_Tin);

    ret.Win                    = ret.Height;    //(((`Height+`AXI_BURST_LEN-1)/`AXI_BURST_LEN)*`AXI_BURST_LEN)
    ret.Hin                    = (Hin);         //1
    ret.CHin                   = ret.Width_in;
    ret.CHout                  = ret.CHin;

    ret.Wout                   = (ret.Win);
    ret.Hout                   = (ret.Hin);
    ret.CHout_div_Tout         = ((ret.CHout+Tout-1)/Tout);
    ret.CHin_div_Tout          = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_Padding_with_Tout = (ret.CHin_div_Tout*Tout);
    ret.Tin_div_Tout           = ((ret.Tin+Tout-1)/Tout);
    ret.CHout_Padding          = (ret.CHout_div_Tout*Tout);

    ret.DAT_IN_BASE_ADDR       = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE    = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_Tout);
    ret.DAT_IN_SURFACE_STRIDE  = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE     = (Pixel_Data_Bytes*ret.Win);
    ret.DAT_IN_scale           = 0;

    ret.LN_WT_BASE_ADDR        = LN_WT_BASE_ADDR;
    ret.LN_SURFACE_STRIDE      = (Tout*MAX_BN_DW*2)>>3;
    ret.LN_num_per_AXI_DW      = (AXI_DAT_WIDTH/(2*ret.LN_DW)); // LN_num_per_AXI_DW <= Tout
    ret.LN_ch_group_times      = (ret.CHout_Padding/ret.LN_num_per_AXI_DW);

    ret.DAT_OUT_BASE_ADDR      = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE   = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_Tout);
    ret.DAT_OUT_SURFACE_STRIDE = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE    = (Pixel_Data_Bytes*ret.Wout);
    //ret.DAT_OUT_scale          = ret.Conv_out_scale;

    ///////////////// no use ////////////////////
    ret.RELU_EN = RELU_EN;  //0

    return ret;
}

void DAT_IN_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *dat_in[], int *HBM_DDR[])
{
    //bit [`MAX_DAT_DW*`Tout*`Tb-1:0] dat_in_mem [`Hin*`Win*`CHin_div_Tout];
    int *dat_in_mem[MAX_DAT_DW*Tout*Tb/32];
    for (int i = 0; i<MAX_DAT_DW*Tout*Tb/32; i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin_div_Tout);
        if (dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }

    // bit [`MAX_DAT_DW*`Tout-1:0]  tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout];
    int *tp_dat_in_mem[Tb][MAX_DAT_DW*Tout/32];
    for(int j=0;j<MAX_DAT_DW*Tout/32;j++)
    {
        for(int i=0;i<Tb;i++)
        {
            tp_dat_in_mem[i][j] = (int*)malloc(sizeof(int)*cfg.Win*cfg.Hin*cfg.CHin_div_Tout);
            if (tp_dat_in_mem[i][j] == NULL){printf("fail to malloc tp_dat_in_mem \n");}
        }
    }

    // dat_in
    for(int b=0;b<Tb;b++)
        General_Map_Feature_Data(cfg, cfg.Hin, cfg.Win, cfg.CHin, dat_in[b], tp_dat_in_mem[b]);

    for(int i=0;i<cfg.Win*cfg.Hin*cfg.CHin_div_Tout;i++)
    {
        for(int b=0;b<Tb;b++) 
        {
            for(int j=0;j<MAX_DAT_DW*Tout/32;j++)
            {
                dat_in_mem[MAX_DAT_DW*Tout/32*b+j][i]=tp_dat_in_mem[b][j][i];
                //if(i==0&&j<5)
                //{
                //printf("tp_dat_in_mem[%d][%d][%d] = 0x%x. \n\r", b, j, i, tp_dat_in_mem[b][j][i]);
                //printf("dat_in_mem[%d][%d] = 0x%x. \n\r", MAX_DAT_DW*Tout/32*b+j, i, dat_in_mem[MAX_DAT_DW*Tout/32*b+j][i]);
                //}
            }
        }
    }

	for(int i=0;i<cfg.Win*cfg.Hin*cfg.CHin_div_Tout;i++)
    {
		for(int j=0;j<Tb*AXI_DAT_WIDTH/32;j++)
		{
            HBM_DDR[0][i*AXI_DAT_WIDTH/32+j] = dat_in_mem[j][i];
            //if(i==0&&j<5)
            //printf("HBM_DDR[0][%d] = 0x%x. \n\r", i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][i*AXI_DAT_WIDTH/32+j]);
        }
    }

    // Free malloc
    for(int i=0;i<MAX_DAT_DW*Tout*Tb/32;i++)
    {
        free(dat_in_mem[i]);
        dat_in_mem[i] = NULL;
    }
    for(int i=0;i<MAX_DAT_DW*Tout/32;i++)
    {
        free(tp_dat_in_mem[0][i]);
        tp_dat_in_mem[0][i] = NULL;
    }
}

void LN_WEIGHT_AND_BIAS_TRANS_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, int *LN_weight, int *LN_bias, int *HBM_DDR[])
{
    // bit [`AXI_DAT_WIDTH-1:0] LN_wt_in_mem[2*((`CHout_Padding+`Tout-1)/`Tout)];
    uint16_t *LN_wt_in_mem_16b[AXI_DAT_WIDTH/16]; 
    for(uint16_t i=0;i<AXI_DAT_WIDTH/16;i++)
    {
        LN_wt_in_mem_16b[i] = (uint16_t*)malloc(sizeof(uint16_t)*2*((cfg.CHout_Padding+Tout-1)/Tout));
        if (LN_wt_in_mem_16b[i] == NULL){printf("fail to malloc LN_wt_in_mem_16b \n");}
    }
    int *LN_wt_in_mem[AXI_DAT_WIDTH/32]; 
    for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        LN_wt_in_mem[i] = (int*)malloc(sizeof(int)*2*((cfg.CHout_Padding+Tout-1)/Tout));
        if (LN_wt_in_mem[i] == NULL){printf("fail to malloc LN_wt_in_mem \n");}
    }

    for(int i=0;i<2;i++)
    {
        for(int j=0;j<cfg.CHout_div_Tout;j++)
        {
            for(int k=0;k<Tout;k++)
            {
                if(j*Tout+k<cfg.CHout)
                {
                    if (i==0) LN_wt_in_mem_16b[k][i*cfg.CHout_div_Tout+j] = (uint16_t)LN_weight[j*Tout+k];
                    if (i==1) LN_wt_in_mem_16b[k][i*cfg.CHout_div_Tout+j] = (uint16_t)LN_bias[j*Tout+k];
                }
                else
                    LN_wt_in_mem_16b[k][i*cfg.CHout_div_Tout+j]=0;
            }

            for(int m=0;m<Tout/2;m++)
                LN_wt_in_mem[m][i*cfg.CHout_div_Tout+j] = (int)(((LN_wt_in_mem_16b[2*m+1][i*cfg.CHout_div_Tout+j]<<16)&0xffff0000) + (LN_wt_in_mem_16b[2*m][i*cfg.CHout_div_Tout+j]&0x0000ffff));
        }
    }



	for(int i=0;i<cfg.LN_ch_group_times;i++)
    {
		for(int j=0;j<AXI_DAT_WIDTH/32;j++)
        {
			HBM_DDR[0][i*AXI_DAT_WIDTH/32+j] = LN_wt_in_mem[j][i];
            //printf("HBM_DDR[0][%d] = 0x%x", cfg.LN_WT_BASE_ADDR/4+i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][cfg.LN_WT_BASE_ADDR/4+i*AXI_DAT_WIDTH/32+j]);
        }
    }

    // Free malloc
    for(uint16_t i=0;i<AXI_DAT_WIDTH/16;i++)
    {
        free(LN_wt_in_mem_16b[i]);
        LN_wt_in_mem_16b[i] = NULL;
    }
    for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        free(LN_wt_in_mem[i]); 
        LN_wt_in_mem[i] = NULL;
    }
}

void DAT_OUT_DEMAP_FUNCTION_LN(struct FPGA_HBM_LN_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[])
{
    // bit [`MAX_DAT_DW*`Tout-1:0] tp_dat_out_mem [`Tb][`Hout*`Wout*`CHout_div_Tout];
    uint16_t *tp_dat_out_mem[Tb][MAX_DAT_DW*Tout/16];
    for(int j=0;j<MAX_DAT_DW*Tout/16;j++)
    {
        for(int i=0;i<Tb;i++)
        {
            tp_dat_out_mem[i][j] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
            if (tp_dat_out_mem[i][j] == NULL){printf("fail to malloc tp_dat_out_mem \n");}
        }
    }
        
    for(int i=0;i<cfg.Wout*cfg.Hout*cfg.CHout_div_Tout;i++)
    {
        for(int b=0;b<Tb;b++) 
        {
            for(int j=0;j<AXI_DAT_WIDTH/16;j++)
            {
                tp_dat_out_mem[b][j][i]=dat_out_mem[Tb*AXI_DAT_WIDTH/16*i+AXI_DAT_WIDTH/16*b+j];
                //printf("tp_dat_out_mem[%d][%d][%d] = 0x%x.\n\r", b, j, i, tp_dat_out_mem[b][j][i]);
            }
        }
    }

    for(int b=0;b<Tb;b++) 
        General_DeMap_Feature_Data(cfg, cfg.Hout, cfg.Wout, cfg.CHout, tp_dat_out_mem[b], dat_out[b]);
    
    // Free malloc
    for(int i=0;i<MAX_DAT_DW*Tout/16;i++)
    {
        free(tp_dat_out_mem[0][i]);
        tp_dat_out_mem[0][i] = NULL;
    }
}

