#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_act.h"

struct FPGA_HBM_ACT_cfg GetFPGA_HBM_ACT_cfg(int Height, int Width_in, int Hin, uint64_t DAT_IN_BASE_ADDR, uint64_t WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_ACT_cfg ret;

    ret.Height                   = Height;    //32
    ret.Width_in                 = Width_in;    //128

    ret.DAT_DW_L0                = MAX_DAT_DW;
    ret.DAT_DW_L1                = MAX_DAT_DW;
    ret.WT_DW                    = MAX_WT_DW;
    ret.BN_DW                    = MAX_BN_DW;
    ret.Tin                      = (base_Tin);

    ret.Win                      = ret.Height;
    ret.Hin                      = Hin;    //(1)
    ret.CHin                     = ret.Width_in;
    ret.CHout                    = ret.CHin;

    ret.Wout                     = (ret.Win);
    ret.Hout                     = (ret.Hin);
    ret.CHout_div_Tout           = ((ret.CHout+Tout-1)/Tout);
    ret.CHin_div_Tout            = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_Padding_with_Tout   = (ret.CHin_div_Tout*Tout);
    ret.Tin_div_Tout             = ((ret.Tin+Tout-1)/Tout);
    ret.CHout_Padding            = (ret.CHout_div_Tout*Tout);

    ret.DAT_IN_BASE_ADDR         = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE      = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_Tout);
    ret.DAT_IN_SURFACE_STRIDE    = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE       = (Pixel_Data_Bytes*ret.Win);
    ret.DAT_IN_scale             = 0;

    ret.WT_BASE_ADDR             = WT_BASE_ADDR;

    ret.DAT_OUT_BASE_ADDR        = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE     = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_Tout);
    ret.DAT_OUT_SURFACE_STRIDE   = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE      = (Pixel_Data_Bytes*ret.Wout);
//    ret.DAT_OUT_scale            = ret.Conv_out_scale;

    return ret;
}

void DAT_IN_TRANS_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, int *dat_in[], int *HBM_DDR[])
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

void PARAMETER_IN_TRANS_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, int *wt, int *bias, int* x_region, int *HBM_DDR[])
{
    // bit [`AXI_DAT_WIDTH-1:0] parameter_in_mem[3];
    int *parameter_in_mem[AXI_DAT_WIDTH/32];
    for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        parameter_in_mem[i] = (int*)malloc(sizeof(int)*3);
        if (parameter_in_mem[i] == NULL){printf("fail to malloc parameter_in_mem[%d] \n", i);}
    }

    for(int i=0;i<8;i++)
    {
        parameter_in_mem[i][0] = ((wt[2*i+1]<<16)&0xffff0000) + (wt[2*i]&0x0000ffff);
        parameter_in_mem[i][1] = ((bias[2*i+1]<<16)&0xffff0000) + (bias[2*i]&0x0000ffff);
        parameter_in_mem[i][2] = ((x_region[2*i+1]<<16)&0xffff0000) + (x_region[2*i]&0x0000ffff);
        //printf("parameter_in_mem[%d][0] = 0x%x ; wt[%d] = 0x%x ; wt[%d] = 0x%x. \n", i, parameter_in_mem[i][0], 2*i+1, wt[2*i+1], 2*i, wt[2*i]);
        //printf("parameter_in_mem[%d][1] = 0x%x ; wt[%d] = 0x%x ; wt[%d] = 0x%x. \n", i, parameter_in_mem[i][1], 2*i+1, bias[2*i+1], 2*i, bias[2*i]);
        //printf("parameter_in_mem[%d][2] = 0x%x ; wt[%d] = 0x%x ; wt[%d] = 0x%x. \n\n", i, parameter_in_mem[i][2], 2*i+1, x_region[2*i+1], 2*i, x_region[2*i]);

        // The highest 256 bits is 0
        parameter_in_mem[i+8][0] = 0;
        parameter_in_mem[i+8][1] = 0;
        parameter_in_mem[i+8][2] = 0;
    }
    
	for(int i=0;i<3;i++)
    {
		for(int j=0;j<AXI_DAT_WIDTH/32;j++)
		{
            HBM_DDR[0][i*AXI_DAT_WIDTH/32+j] = parameter_in_mem[j][i];
            //if(i==0&&j<5)
            //printf("HBM_DDR[0][%d] = 0x%x. \n\r", i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][i*AXI_DAT_WIDTH/32+j]);
        }
    }

    // Free malloc
    for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        free(parameter_in_mem[i]);
        parameter_in_mem[i] = NULL;
    }
}

void DAT_OUT_DEMAP_FUNCTION_ACT(struct FPGA_HBM_ACT_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[])
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
        General_DeMap_Feature_Data(cfg, cfg.Hout, cfg.Wout, cfg.CHout_Padding, tp_dat_out_mem[b], dat_out[b]);
    
    // Free malloc
    for(int i=0;i<MAX_DAT_DW*Tout/16;i++)
    {
        free(tp_dat_out_mem[0][i]);
        tp_dat_out_mem[0][i] = NULL;
    }
}

