#include <stdint.h>
#include <stdlib.h>
#include "../TOP_defines.h"
#include "General_Map_trans.h"

struct FPGA_HBM_TRANSPOSE_cfg GetFPGA_HBM_TRANSPOSE_cfg(int Height, int Width_in, int Hin, int Wout, int Hout, int Ky, int Kx, int log2_WT_base_addr_Bank_Step, int RELU_EN)
{
    struct FPGA_HBM_TRANSPOSE_cfg ret;

    ret.Height                        = Height;//35//
    ret.Width_in                      = Width_in;//256//
    
    ret.DAT_DW_L0                     = MAX_DAT_DW;
    ret.DAT_DW_L1                     = MAX_DAT_DW;
    ret.WT_DW                         = MAX_WT_DW;
    ret.LN_DW                         = MAX_BN_DW;
    ret.Tin                           = base_Tin;
    
    ret.Win                           = (((ret.Height+Tout-1)/Tout)*Tout);
    ret.Hin                           = (Hin);  //1
    ret.CHin                          = ((ret.Width_in+T_quant_block-1)/T_quant_block*T_quant_block);
    ret.CHout                         = (((ret.Height+Tout-1)/Tout)*Tout);
    
    ret.Wout                          = (Wout); //1
    ret.Hout                          = (Hout); //1
    ret.Ky                            = (Ky);   //1
    ret.Kx                            = (Kx);   //1
    
    ret.WT_CHin_div_Tblock            = (ret.CHin/T_quant_block);
    ret.WT_CHin_div_Tin               = ((ret.CHin+ret.Tin-1)/ret.Tin);
    ret.WT_CHin_Padding_with_Tin      = (ret.WT_CHin_div_Tin*ret.Tin);
    ret.Tblock_div_Tin                = (T_quant_block/ret.Tin);
    
    ret.CHout_div_Tout                = ((ret.CHout+Tout-1)/Tout);
    ret.CHin_div_Tout                 = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_Padding_with_Tout        = (ret.CHin_div_Tout*Tout);
    ret.Tin_div_Tout                  = ((ret.Tin+Tout-1)/Tout);
    ret.CHout_Padding                 = (ret.CHout_div_Tout*Tout);
    
    ret.DAT_IN_BASE_ADDR              = 0x00000000;
    ret.DAT_IN_SURFACE_STRIDE         = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE            = (Pixel_Data_Bytes*ret.Win);
    
    ret.WT_BASE_ADDR                  = 0x02000000;
    
    ret.DAT_OUT_BASE_ADDR             = 0x04000000;
    ret.DAT_OUT_SURFACE_STRIDE        = (Pixel_Data_Bytes*ret.Hout*ret.Wout);
    ret.DAT_OUT_LINE_STRIDE           = (Pixel_Data_Bytes*ret.Wout);
    ret.Block_wt_bits                 = (Tout*MAX_DAT_DW+Tout*4*T_quant_block);
    ret.Total_wt_bits                 = (ret.WT_CHin_div_Tblock*ret.CHout_div_Tout*ret.Block_wt_bits);
    
    //ret.WT_CH_Tgroup                = (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW) // =2048 CHins
    ret.WT_scale_group_nums           = ((ret.WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    ret.WT_CH_Tgroup_div_Tblock       = (WT_CH_Tgroup/T_quant_block); //2048/128=16
    
    ret.Group_WT_Bytes                = (WT_CH_Tgroup*ret.WT_DW/8);
    ret.Group_Scale_Bytes             = (HBM_AXI_DATA_WIDTH/8);
    ret.Group_WT_and_Scale_Bytes      = (ret.Group_WT_Bytes+ret.Group_Scale_Bytes);
    ret.Last_Group_CHin               = (ret.WT_CHin_Padding_with_Tin%WT_CH_Tgroup);
    ret.Last_Group_WT_Bytes           = (ret.Last_Group_CHin*ret.WT_DW/8);
    ret.Last_Group_Scale_Bytes        = (HBM_AXI_DATA_WIDTH/8);
    ret.Last_Group_WT_and_Scale_Bytes = (ret.Last_Group_WT_Bytes+ret.Last_Group_Scale_Bytes);
    
    ret.CHin_WT_Bytes                 = (ret.WT_CHin_Padding_with_Tin*ret.WT_DW/8);
    ret.CHin_Scale_Bytes              = (HBM_AXI_DATA_WIDTH*ret.WT_scale_group_nums/8);
    ret.CHin_WT_and_Scale_Bytes       = (ret.CHin_WT_Bytes+ret.CHin_Scale_Bytes);
    ret.log2_WT_base_addr_Bank_Step   = log2_WT_base_addr_Bank_Step;  //8
    ret.WT_base_addr_Bank_Step        = (1<<ret.log2_WT_base_addr_Bank_Step);
    ret.HBM00_WT_BASE_ADDR            = 0x00000000;
    
    ///////////////// no use ////////////////////
    ret.RELU_EN                       = RELU_EN;  //0
    ////////////////////////////////////////////////////////////////////////////////

    return ret;
}

void TRANSPOSE_SOFT_FUNCTION(struct FPGA_HBM_TRANSPOSE_cfg cfg, int *dat_in_32b[], int *HBM_DDR[])
{
    //bit [`DAT_DW_L0-1:0] dat_in[`Tb][`Hin][`Win][`CHin]
    uint16_t *dat_in_16b[Tb];
    for(int i=0; i<Tb; i++)
    {
        dat_in_16b[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in_16b[i] == NULL){printf("fail to malloc dat_in_16b[%d].\n", i);}
    } 
    // real real_dat_in[`Tb][`Hin][`Win][`CHin]
    half *real_dat_in[Tb];
    for(int i=0; i<Tb; i++)
    {
        real_dat_in[i] = (half*)malloc(sizeof(half)*cfg.Hin*cfg.Win*cfg.CHin);
        if (real_dat_in[i] == NULL){printf("fail to malloc real_dat_in[%d].\n", i);}
    } 
    // real real_wt_chout_chin[1][1][`CHout_Padding][`CHin_Padding_with_Tout];//[Ky][Kx][CHout][CHin]
    half *real_wt_chout_chin[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            real_wt_chout_chin[i][j] = (half*)malloc(sizeof(half)*cfg.CHout_Padding*cfg.CHin_Padding_with_Tout);
            if (real_wt_chout_chin[i][j] == NULL){printf("fail to malloc real_wt_chout_chin[%d][%d].\n", i, j);}
        }
    } 
    // real real_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block]
    half *real_wt_block[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            real_wt_block[i][j] = (half*)malloc(sizeof(half)*cfg.CHout_div_Tout*cfg.CHin*T_quant_block*Tout*T_quant_block);
            if (real_wt_block[i][j] == NULL){printf("fail to malloc real_wt_block[%d][%d].\n", i, j);}
        }
    } 
    // real new_real_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block]
    half *new_real_wt_block[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            new_real_wt_block[i][j] = (half*)malloc(sizeof(half)*cfg.CHout_div_Tout*cfg.CHin*T_quant_block*Tout*T_quant_block);
            if (new_real_wt_block[i][j] == NULL){printf("fail to malloc new_real_wt_vlock[%d][%d].\n", i, j);}
        }
    } 
    // real real_scale[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout]
    half *real_scale[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            real_scale[i][j] = (half*)malloc(sizeof(half)*cfg.CHout_div_Tout*cfg.CHin*T_quant_block*Tout);
            if (real_scale[i][j] == NULL){printf("fail to malloc real_scale[%d][%d].\n", i, j);}
        }
    } 
    // bit [15:0] FP16_scale[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout]
    uint16_t *FP16_scale[1][1];
    for(uint16_t i=0; i<1; i++)
    {
        for(uint16_t j=0; j<1; j++)
        {
            FP16_scale[i][j] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.CHout_div_Tout*cfg.CHin/T_quant_block*Tout);
            if (FP16_scale[i][j] == NULL){printf("fail to malloc FP16_scale[%d][%d].\n", i, j);}
        }
    } 
    int *FP16_scale_int[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            FP16_scale_int[i][j] = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.CHin/T_quant_block*Tout);
            if (FP16_scale_int[i][j] == NULL){printf("fail to malloc FP16_scale_int[%d][%d].\n", i, j);}
        }
    } 
    // bit [3:0] INT4_wt_block[1][1][`CHout_div_Tout][`CHin/`T_quant_block][`Tout][`T_quant_block]
    uint8_t *INT4_wt_block[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            INT4_wt_block[i][j] = (uint8_t*)malloc(sizeof(uint8_t)*cfg.CHout_div_Tout*cfg.CHin*T_quant_block*Tout*T_quant_block);
            if (INT4_wt_block[i][j] == NULL){printf("fail to malloc INT4_wt_block[%d][%d].\n", i, j);}
        }
    } 
    // bit [3:0] INT4_wt_chout_chin[1][1][`CHout_Padding][`CHin_Padding_with_Tout]
    int *INT4_wt_chout_chin[1][1];
    for(int i=0; i<1; i++)
    {
        for(int j=0; j<1; j++)
        {
            INT4_wt_chout_chin[i][j] = (int*)malloc(sizeof(int)*cfg.CHout_Padding*cfg.CHin_Padding_with_Tout);
            if (INT4_wt_chout_chin[i][j] == NULL){printf("fail to malloc INT4_wt_chout_chin[%d][%d].\n", i, j);}
        }
    }

    // [`HBM_AXI_DATA_WIDTH-1:0] HBM_wt_FP_scale [`CHout_div_Tout][`WT_scale_group_nums][`Tout/`HBM_Port][`HBM_Port];
    int *HBM_wt_FP_scale = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port*(HBM_AXI_DATA_WIDTH/32));
    if (HBM_wt_FP_scale == NULL){printf("fail to malloc HBM_wt_FP_scale.\n");}

    // [`WT_DW*`WT_CHin_Padding_with_Tin-1:0] HBM_wt_mem [`CHout_div_Tout][`Tout/`HBM_Port][`HBM_Port];
    int  *HBM_wt_mem = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*Tout/HBM_Port*HBM_Port*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)); 
    if (HBM_wt_mem == NULL){printf("fail to malloc HBM_wt_mem.\n");}

    int wt_start_ch_in;
    int wt_end_ch_in;
    int wt_addr_bias;
    int tmp_wt;

    for(int i=0;i<Tb;i++)
    {
        for(int j=0;j<cfg.Hin*cfg.Win*cfg.CHin;j++)
            dat_in_16b[i][j] = (uint16_t)dat_in_32b[i][j];
    }
    
    for(int i=0;i<Tb;i++)
        uint16_to_half(dat_in_16b[i], cfg.Hin*cfg.Win*cfg.CHin, real_dat_in[i]);
    //for(int i=0;i<10;i++)
    //    printf("dat_in_16[0][%d]: %d ; real_dat_in[0][%d]: %f. \n", i, dat_in_16b[0][i], i, (double)real_dat_in[0][i]);

	for(int b=0;b<Tb;b++)
    {
        for(int i=0;i<cfg.Hin;i++)          // 1
        {
            for(int j=0;j<cfg.Win;j++)      // 64
            {
                for(int k=0;k<cfg.CHin;k++) // 256
                {
                    if((j<cfg.Height)&(k<cfg.Width_in)) //Height = 35 ; Width_in = 256
                    {
                        real_wt_chout_chin[0][0][i*cfg.Win*cfg.CHin+j*cfg.CHin+k]=real_dat_in[b][i*cfg.Win*cfg.CHin+j*cfg.CHin+k];
                        //printf("real_wt_chout_chin[0][0][%d]=%f. \n", i*cfg.Win*cfg.CHin+j*cfg.CHin+k, (double)real_wt_chout_chin[0][0][i*cfg.Win*cfg.CHin+j*cfg.CHin+k]);
                    }
                }
            }
        }
    }
            
    for(int i=0;i<cfg.CHout_div_Tout;i++)           // 2
    {
        for(int j=0;j<cfg.CHin/T_quant_block;j++)   // 256/128
        {
            for(int tout=0;tout<Tout;tout++)        // 32
            {
				half real_tp_max=half(0);
				for(int tq=0;tq<T_quant_block;tq++) // 128
                {
					real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=real_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq];
					//printf("real_wt_block[0][0][%d]=%f ; ", i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq, (double)real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]);
					//printf("real_wt_chout_chin[0][0][%d]=%f. \n", (i*Tout+tout)*cfg.CHin+j*T_quant_block+tq, (double)real_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq]);

					if(real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>real_tp_max)    real_tp_max= real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq];
					if(real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]<(-real_tp_max)) real_tp_max=-real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq];
                }
				
				for(int tq=0;tq<T_quant_block;tq++)
                {
					real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout]=real_tp_max*2/28;
                    //printf("real_tp_max=%f ; real_scale[0][0][%d]=%f. \n", (double)real_tp_max, i*cfg.CHin/T_quant_block*Tout+j*Tout+tout, (double)real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout]);

                    half_to_uint16(&real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout], 1, &FP16_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout]);
                    //printf("FP16_scale[0][0][%d]=%x ; real_scale[0][0][%d]=%f. \n", i*cfg.CHin/T_quant_block*Tout+j*Tout+tout, FP16_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout], i*cfg.CHin/T_quant_block*Tout+j*Tout+tout, (double)real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout]);

					if (real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout]==0)
					   new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0;
					else
					   new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]/real_scale[0][0][i*cfg.CHin/T_quant_block*Tout+j*Tout+tout];
                    //printf("new_real_wt_block[0][0][tin%d]=%f. \n", i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq, (double)new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]);
					if     (new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>13)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=7;
					else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>11)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=6;
					else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]> 9)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=5;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]> 7)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=4;
					else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]> 5)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=3;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]> 3)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=2;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]> 1)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=1;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-1)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0;
					else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-3)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1001;//-1;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-5)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1010;//-2;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-7)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1011;//-3;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-9)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1100;//-4;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-11)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1101;//-5;
                    else if(new_real_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>-13)INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1110;//-6;
                    else                                                 INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]=0b1111;//-7;
                    //if(INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]>>3)
                    //    printf("INT4_wt_block[0][0][tin%d]=%d. \n", i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq, (int8_t)((((~(INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq])&0x7)+1)|0xf8)));
                    //else
                    //    printf("INT4_wt_block[0][0][tin%d]=%d. \n", i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq, INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]);

                    INT4_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq]=(int)(INT4_wt_block[0][0][i*cfg.CHin/T_quant_block*Tout*T_quant_block+j*Tout*T_quant_block+tout*T_quant_block+tq]);
                    //if(INT4_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq]>>3)
                    //    printf("INT4_wt_chout_chin[chin%d]=%d. \n", (i*Tout+tout)*cfg.CHin+j*T_quant_block+tq, (int)((((~(INT4_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq])&0x7)+1)|0xfffffff8)));
                    //else 
                    //    printf("INT4_wt_chout_chin[chin%d]=%d. \n", (i*Tout+tout)*cfg.CHin+j*T_quant_block+tq, INT4_wt_chout_chin[0][0][(i*Tout+tout)*cfg.CHin+j*T_quant_block+tq]);
                }
            }
        }
    }

    printf("test \n");
        // wt
         for(int i=0;i<cfg.CHout_div_Tout;i++)
         {
             for(int j=0;j<Tout/HBM_Port;j++)
             {
                for(int k=0;k<HBM_Port;k++)
                {
                    for(int m=0;m<cfg.WT_CHin_Padding_with_Tin*cfg.WT_DW/32;m++)
                    {
                        int tmp = 0;
                        for(int p=0; p<8; p++)
                        {
                            if((i*Tout+j*HBM_Port+k<cfg.CHout) && (m*8+p < cfg.CHin))
                            {
                                if(INT4_wt_chout_chin[0][0][(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]<0)
                                {
                                       tmp_wt=8-INT4_wt_chout_chin[0][0][(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p];
                                }else
                                {
                                      tmp_wt=INT4_wt_chout_chin[0][0][(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p];
                                }
                                //tmp = tmp + ( (INT4_wt_chout_chin_x8[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]&(0x0000000f)) << cfg.WT_DW*p ); 
                                tmp = tmp + ( (tmp_wt&(0x0000000f)) << cfg.WT_DW*p ); 
                                //if(m==0 && i==0 && j==0 && k==0){printf("INT4_wt_chout_chin_x8 tmp[%d] = %x , INT4_wt_chout_chin_x8[%d] =  %x\n", p, tmp, p, INT4_wt_chout_chin_x8[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]);}
                            }

                        }
                        HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + j*HBM_Port + k)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+m] = tmp;
                        //if(k==1){printf("HBM_wt_mem[%d][%d][%d][%d] = %04x. \n", i, j, k, m, HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + j*HBM_Port + k)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+m]);}
                    }    
                }
            }
        }

        for(int i=0;i<cfg.CHout_div_Tout*cfg.CHin/T_quant_block*Tout;i++)
            FP16_scale_int[0][0][i] = (int)FP16_scale[0][0][i];

        for(int i=0;i<cfg.CHout_div_Tout;i++)
        {
            for(int j=0;j<cfg.WT_scale_group_nums;j++)
            {
                for(int k=0;k<Tout/HBM_Port;k++)
                {
                    for(int m=0;m<HBM_Port;m++)
                    {
                       for(int n=0;n<cfg.WT_CH_Tgroup_div_Tblock*16/32;n++)
                       {
                            int tmp = 0;
                            for(int p=0; p<2; p++)
                            {
                                if((i*Tout+k*HBM_Port+m<cfg.CHout) && (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p<cfg.WT_CHin_div_Tblock))
                                {
                                   tmp = tmp + ((FP16_scale_int[0][0][i*cfg.WT_CHin_div_Tblock*Tout + (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p)*Tout + (k*HBM_Port+m)]&0x0000ffff) << p*WT_quant_scale_DW);                            
                                }
                            }
                            HBM_wt_FP_scale[(i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n]=tmp;
                            //if(m==1){printf("HBM_wt_FP_scale[%d][%d][%d][%d] = %04x. \n", i, j, k, m, HBM_wt_FP_scale[(i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n]);}
                       }
                   }
               }
           }
        }

        std::cout << "***WT_CHin_Padding_with_Tin=" << cfg.WT_CHin_Padding_with_Tin << std::endl;
        std::cout << "***WT_CHin_div_Tblock=" << cfg.WT_CHin_div_Tblock << std::endl;
        std::cout << "***Tblock_div_Tin=" << cfg.Tblock_div_Tin << std::endl;
        std::cout << "***WT_CH_Tgroup=" << WT_CH_Tgroup << std::endl;
        std::cout << "***WT_scale_group_nums=" << cfg.WT_scale_group_nums << std::endl;
        std::cout << "***WT_CH_Tgroup_div_Tblock=" <<cfg.WT_CH_Tgroup_div_Tblock << std::endl;
        std::cout << "***Group_WT_Bytes=" << cfg.Group_WT_Bytes << std::endl;
        std::cout << "***Group_Scale_Bytes=" <<cfg. Group_Scale_Bytes << std::endl;
        std::cout << "***Group_WT_and_Scale_Bytes=" << cfg.Group_WT_and_Scale_Bytes << std::endl;
        std::cout << "***Last_Group_CHin=" << cfg.Last_Group_CHin << std::endl;
        std::cout << "***Last_Group_WT_Bytes=" <<cfg. Last_Group_WT_Bytes << std::endl;
        std::cout << "***Last_Group_Scale_Bytes=" <<cfg. Last_Group_Scale_Bytes << std::endl;
        std::cout << "***Last_Group_WT_and_Scale_Bytes=" << cfg.Last_Group_WT_and_Scale_Bytes << std::endl;
        std::cout << "***CHin_WT_Bytes=" << cfg.CHin_WT_Bytes << std::endl;
        std::cout << "***CHin_Scale_Bytes=" << cfg.CHin_Scale_Bytes << std::endl;
        std::cout << "***CHin_WT_and_Scale_Bytes=" <<cfg.CHin_WT_and_Scale_Bytes << std::endl;


        for(int i=0;i<cfg.CHout_div_Tout;i++) 
        {
            for(int j=0;j<cfg.WT_scale_group_nums;j++)
            {
                for(int k=0;k<Tout/HBM_Port;k++)
                {
    				for(int m=0;m<HBM_Port;m++)
                    {
    					int scale_addr_bias=static_cast<int>((i*cfg.CHin_WT_and_Scale_Bytes*8/32+j*cfg.Group_WT_and_Scale_Bytes*8/32)*(Tout/HBM_Port)
    					                + ((j==cfg.WT_scale_group_nums-1)? (k*cfg.Last_Group_WT_and_Scale_Bytes*8/32) : (k*cfg.Group_WT_and_Scale_Bytes*8/32)));
    									//+cfg.HBM00_WT_BASE_ADDR/4+cfg.WT_base_addr_Bank_Step/4*m);
    					//if(m==1){ std::cout << "***scale_addr_bias="<<scale_addr_bias<<std::endl;}
    					for(int n=0;n<HBM_AXI_DATA_WIDTH/32;n++)
                        {    
                            if(m == 1)
                                {printf("scale_addr_bias + n: %d.\n\r", scale_addr_bias+n);}
    						if(m==  0){HBM_DDR[ 0][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  1){HBM_DDR[ 1][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  2){HBM_DDR[ 2][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  3){HBM_DDR[ 3][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  4){HBM_DDR[ 4][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  5){HBM_DDR[ 5][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  6){HBM_DDR[ 6][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  7){HBM_DDR[ 7][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  8){HBM_DDR[ 8][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m==  9){HBM_DDR[ 9][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 10){HBM_DDR[10][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 11){HBM_DDR[11][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 12){HBM_DDR[12][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 13){HBM_DDR[13][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 14){HBM_DDR[14][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 15){HBM_DDR[15][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 16){HBM_DDR[16][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 17){HBM_DDR[17][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 18){HBM_DDR[18][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 19){HBM_DDR[19][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 20){HBM_DDR[20][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 21){HBM_DDR[21][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 22){HBM_DDR[22][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 23){HBM_DDR[23][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 24){HBM_DDR[24][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 25){HBM_DDR[25][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 26){HBM_DDR[26][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 27){HBM_DDR[27][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 28){HBM_DDR[28][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 29){HBM_DDR[29][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 30){HBM_DDR[30][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            if(m== 31){HBM_DDR[31][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                            //if(m== 1){  std::cout << "addr=" << scale_addr_bias + n << ", scale_HBM_DDR[CHout_div_Tout" << i << "][Group" << j << "][Tout_div_Port" << k << "][Port" << m << "][CHin" << n << "]=" << (HBM_wt_FP_scale[i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m]>> (32 * n)& mask) << std::endl; }

                         }
                    }

                }
           }
        }

    for(int i=0;i<cfg.CHout_div_Tout;i++)
    {
    	for(int j=0;j<cfg.WT_scale_group_nums;j++)
        {
    		for(int k=0;k<Tout/HBM_Port;k++)
            {
    			for(int m=0;m<HBM_Port;m++)
                {
                    wt_start_ch_in=j*WT_CH_Tgroup;
                    wt_end_ch_in=static_cast<int>(j==cfg.WT_scale_group_nums-1)?cfg.WT_CHin_Padding_with_Tin:(j+1)*WT_CH_Tgroup;
                    wt_addr_bias=static_cast<int>((i*cfg.CHin_WT_and_Scale_Bytes+j*cfg.Group_WT_and_Scale_Bytes)*8/32*(Tout/HBM_Port)+cfg.Group_Scale_Bytes*8/32
                                + ((j==cfg.WT_scale_group_nums-1)? (k*cfg.Last_Group_WT_and_Scale_Bytes*8/32) : (k*cfg.Group_WT_and_Scale_Bytes*8/32)));
                                //+cfg.HBM00_WT_BASE_ADDR/4+cfg.WT_base_addr_Bank_Step/4*m);
                    for(int n = cfg.WT_DW*wt_start_ch_in/32;n<cfg.WT_DW*wt_end_ch_in/32;n++)
                    {
                        if( m == 0){ HBM_DDR[ 0][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];} //cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32
                        if( m == 1){ HBM_DDR[ 1][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 2){ HBM_DDR[ 2][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 3){ HBM_DDR[ 3][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 4){ HBM_DDR[ 4][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 5){ HBM_DDR[ 5][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 6){ HBM_DDR[ 6][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 7){ HBM_DDR[ 7][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 8){ HBM_DDR[ 8][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 9){ HBM_DDR[ 9][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 10){ HBM_DDR[10][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 11){ HBM_DDR[11][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 12){ HBM_DDR[12][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 13){ HBM_DDR[13][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 14){ HBM_DDR[14][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 15){ HBM_DDR[15][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 16){ HBM_DDR[16][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 17){ HBM_DDR[17][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 18){ HBM_DDR[18][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 19){ HBM_DDR[19][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 20){ HBM_DDR[20][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 21){ HBM_DDR[21][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 22){ HBM_DDR[22][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 23){ HBM_DDR[23][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 24){ HBM_DDR[24][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 25){ HBM_DDR[25][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 26){ HBM_DDR[26][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 27){ HBM_DDR[27][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 28){ HBM_DDR[28][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 29){ HBM_DDR[29][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 30){ HBM_DDR[30][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 31){ HBM_DDR[31][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                    }
                }
            }
        }
    }
}

void DAT_IN_TRANS_FUNCTION_TRANSPOSE(struct FPGA_HBM_TRANSPOSE_cfg cfg, int *dat_in[], int *HBM_DDR[])
{
    // [MAX_DAT_DW*Tout*Tb] dat_in_mem[`Hin*`Win*`CHin_div_Tout]
    int *dat_in_mem[MAX_DAT_DW*Tout*Tb/32];
    for (int i = 0; i<MAX_DAT_DW*Tout*Tb/32; i++)
    {
        dat_in_mem[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin_div_Tout);
        if (dat_in_mem[i] == NULL){printf("fail to malloc dat_in_mem \n");}
    }

    // [MAX_DAT_DW*Tout] tp_dat_in_mem[`Tb][`Win*`Hin*`CHin_div_Tout] 
    int *tp_dat_in_mem[Tb][MAX_DAT_DW*Tout/32];
    for(int j=0;j<MAX_DAT_DW*Tout/32;j++)
    {
        for(int i=0;i<Tb;i++)
        {
            tp_dat_in_mem[i][j] = (int*)malloc(sizeof(int)*cfg.Win*cfg.Hin*cfg.CHin_div_Tout);
            if (tp_dat_in_mem[i][j] == NULL){printf("fail to malloc tp_dat_in_mem \n");}
        }
    }

    // Dat_in
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
            //printf("HBM_DDR[0][%d] = 0x%x ; dat_in_mem[%d][%d] = 0x%x. \n\r", i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][i*AXI_DAT_WIDTH/32+j], j, i, dat_in_mem[j][i]);
        }
    }
}
