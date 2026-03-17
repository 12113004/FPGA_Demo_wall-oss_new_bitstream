#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_mvm.h"

struct FPGA_HBM_MVM_cfg GetFPGA_HBM_MVM_cfg(int Height, int Hin, int Width_in, int Width_out, uint64_t DAT_IN_BASE_ADDR, uint64_t HBM00_WT_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_MVM_cfg ret;

    ret.Height                        = Height;
    ret.Width_in                      = Width_in;
    ret.Width_out                     = Width_out;
    
    ret.DAT_DW_L0                     = MAX_DAT_DW;
    ret.DAT_DW_L1                     = MAX_DAT_DW;
    ret.WT_DW                         = MAX_WT_DW;
    ret.BN_DW                         = MAX_BN_DW;
    ret.Tin                           = (base_Tin);

    ret.Win                           = ret.Height;
    ret.Hin                           = Hin;
    ret.CHin                          = ((ret.Width_in+Tout-1)/Tout*Tout);
    ret.Wout                          = ret.Win;
    ret.Hout                          = ret.Hin;
    ret.CHout                         = ((ret.Width_out+Tout-1)/Tout*Tout);

    ret.Tin_div_Tout                  = ((ret.Tin+Tout-1)/Tout);
    ret.CHin_div_Tout                 = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_div_LTout                = ((ret.CHin+L_Tout-1)/L_Tout);
    ret.CHin_Padding                  = (ret.CHin_div_LTout*L_Tout);
    ret.CHout_div_Tout                = ((ret.CHout+Tout-1)/Tout);
    ret.CHout_div_LTout               = ((ret.CHout+L_Tout-1)/L_Tout);
    ret.CHout_Padding                 = (ret.CHout_div_LTout*L_Tout);
    ret.CHout_Padding_with_Tout       = (ret.CHout_div_Tout*Tout);

    ret.DAT_IN_BASE_ADDR              = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE           = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);
    ret.DAT_IN_HEAD_STRIDE            = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_LTout);
    ret.DAT_IN_SURFACE_STRIDE         = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE            = (Pixel_Data_Bytes*ret.Win);

    ret.HBM00_WT_BASE_ADDR            = HBM00_WT_BASE_ADDR;
    ret.WT_CHin_div_Tin               = ((ret.CHin+ret.Tin-1)/ret.Tin);
    ret.WT_CHin_Padding_with_Tin      = (ret.WT_CHin_div_Tin*ret.Tin);
    ret.WT_CHout_Padding_with_Tout    = (ret.CHout_div_Tout*Tout);
    ret.WT_CHin_div_Tblock            = ((ret.WT_CHin_Padding_with_Tin+T_quant_block-1)/T_quant_block);
    ret.Tblock_div_Tin                = (T_quant_block/ret.Tin);

    ret.WT_scale_group_nums           = ((ret.WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    ret.WT_CH_Tgroup_div_Tblock       = (WT_CH_Tgroup/T_quant_block);

    ret.Group_WT_Bytes                = (WT_CH_Tgroup*ret.WT_DW/8);
    ret.Group_Scale_Bytes             = (HBM_AXI_DATA_WIDTH/8);
    ret.Group_WT_and_Scale_Bytes      =(ret.Group_WT_Bytes+ret.Group_Scale_Bytes);
    ret.Last_Group_CHin               = ((ret.WT_CHin_Padding_with_Tin%WT_CH_Tgroup)==0? WT_CH_Tgroup : (ret.WT_CHin_Padding_with_Tin%WT_CH_Tgroup));
    ret.Last_Group_WT_Bytes           = (ret.Last_Group_CHin*ret.WT_DW/8);
    ret.Last_Group_Scale_Bytes        = (HBM_AXI_DATA_WIDTH/8);
    ret.Last_Group_WT_and_Scale_Bytes =(ret.Last_Group_WT_Bytes+ret.Last_Group_Scale_Bytes);

    ret.CHin_WT_Bytes                 = (ret.WT_CHin_Padding_with_Tin*ret.WT_DW/8);
    ret.CHin_Scale_Bytes              = (HBM_AXI_DATA_WIDTH*ret.WT_scale_group_nums/8);
    ret.CHin_WT_and_Scale_Bytes       = (ret.CHin_WT_Bytes+ret.CHin_Scale_Bytes);
    ret.WT_scale_bits                 = (ret.WT_CHout_Padding_with_Tout*HBM_AXI_DATA_WIDTH*ret.WT_scale_group_nums);
    ret.WT_SIZE_IN_BYTES              = (((ret.WT_CHout_Padding_with_Tout*ret.WT_CHin_Padding_with_Tin*ret.WT_DW)>>3)+((ret.WT_scale_bits)>>3));
    ret.WT_BYTES_per_CH               = (ret.WT_SIZE_IN_BYTES/ret.WT_CHout_Padding_with_Tout);
    ret.WT_NUM_DIV_TIN                = (ret.WT_CHout_Padding_with_Tout*ret.WT_CHin_div_Tin);

    ret.DAT_OUT_BASE_ADDR             = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE          = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_HEAD_STRIDE           = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_LTout);
    ret.DAT_OUT_SURFACE_STRIDE        = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE           = (Pixel_Data_Bytes*ret.Wout);

    return ret;
}

void WT_AND_SCALE_TRANS_FUNCTION_MVM(struct FPGA_HBM_MVM_cfg cfg , int *wt, int *wt_FP_scale, int *HBM[])
{
    //int  HBM_wt_FP_scale[cfg.CHout_div_Tout][cfg.WT_scale_group_nums][Tout/HBM_Port][HBM_Port];
    int *HBM_wt_FP_scale = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port*(HBM_AXI_DATA_WIDTH/32));
    if (HBM_wt_FP_scale == NULL){printf("fail to malloc HBM_wt_FP_scale \n");}

    //int  HBM_wt_mem[cfg.CHout_div_Tout][Tout/HBM_Port][HBM_Port];
    int  *HBM_wt_mem = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*Tout/HBM_Port*HBM_Port*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)); 
    if (HBM_wt_mem == NULL){printf("fail to malloc HBM_wt_mem \n");}

    int wt_start_ch_in;
    int wt_end_ch_in;
    int wt_addr_bias;
    int tmp_wt;

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
                            tmp_wt=wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p];
                            tmp = tmp + ( (tmp_wt&(0x0000000f)) << cfg.WT_DW*p ); 
                        }
                        
                    }
                    HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + j*HBM_Port + k)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+m] = tmp;
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
                   for(int n=0;n<cfg.WT_CH_Tgroup_div_Tblock*16/32;n++)
                   {
                        int tmp = 0;
                        for(int p=0; p<2; p++)
                        {
                            if((i*Tout+k*HBM_Port+m<cfg.CHout) && (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p<cfg.WT_CHin_div_Tblock))
                               tmp = tmp +  ((wt_FP_scale[i*cfg.WT_CHin_div_Tblock*Tout + (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p) + (k*HBM_Port+m)*cfg.WT_CHin_div_Tblock]&0x0000ffff) << p*WT_quant_scale_DW);
                        }
                        HBM_wt_FP_scale[(i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n]=tmp;
                        //if((i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n == 8)
                        //   {printf("HBM_wt_FP_scale[%d] = %x.\n\r", (i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n, HBM_wt_FP_scale[(i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n]);}
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
					int scale_addr_bias=static_cast<int>((i*cfg.CHin_WT_and_Scale_Bytes*8/32+j*cfg.Group_WT_and_Scale_Bytes*8/32)*(Tout/HBM_Port)
                                        + ((j==cfg.WT_scale_group_nums-1)? (k*cfg.Last_Group_WT_and_Scale_Bytes*8/32) : (k*cfg.Group_WT_and_Scale_Bytes*8/32)));
					for(int n=0;n<HBM_AXI_DATA_WIDTH/32;n++)
                    {    
						if(m==  0){HBM[ 0][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  1){HBM[ 1][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  2){HBM[ 2][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  3){HBM[ 3][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  4){HBM[ 4][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  5){HBM[ 5][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  6){HBM[ 6][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  7){HBM[ 7][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  8){HBM[ 8][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m==  9){HBM[ 9][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 10){HBM[10][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 11){HBM[11][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 12){HBM[12][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 13){HBM[13][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 14){HBM[14][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 15){HBM[15][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 16){HBM[16][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 17){HBM[17][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 18){HBM[18][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 19){HBM[19][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 20){HBM[20][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 21){HBM[21][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 22){HBM[22][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 23){HBM[23][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 24){HBM[24][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 25){HBM[25][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 26){HBM[26][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 27){HBM[27][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 28){HBM[28][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 29){HBM[29][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 30){HBM[30][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
                        if(m== 31){HBM[31][scale_addr_bias+n]=(HBM_wt_FP_scale[(i*cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port + j*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*HBM_AXI_DATA_WIDTH/32+n]);}
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
                    for(int n = cfg.WT_DW*wt_start_ch_in/32;n<cfg.WT_DW*wt_end_ch_in/32;n++)
                    {
                        if( m == 0){ HBM[ 0][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];} //cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32
                        if( m == 1){ HBM[ 1][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 2){ HBM[ 2][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 3){ HBM[ 3][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 4){ HBM[ 4][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 5){ HBM[ 5][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 6){ HBM[ 6][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 7){ HBM[ 7][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 8){ HBM[ 8][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if( m == 9){ HBM[ 9][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 10){ HBM[10][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 11){ HBM[11][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 12){ HBM[12][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 13){ HBM[13][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 14){ HBM[14][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 15){ HBM[15][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 16){ HBM[16][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 17){ HBM[17][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 18){ HBM[18][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 19){ HBM[19][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 20){ HBM[20][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 21){ HBM[21][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 22){ HBM[22][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 23){ HBM[23][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 24){ HBM[24][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 25){ HBM[25][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 26){ HBM[26][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 27){ HBM[27][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 28){ HBM[28][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 29){ HBM[29][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 30){ HBM[30][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                        if(m == 31){ HBM[31][wt_addr_bias+n-(cfg.WT_DW*wt_start_ch_in/32)]=HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + k*HBM_Port + m)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+n];}
                    }
                }
            }
        }
    }

    // Free malloc
    free(HBM_wt_FP_scale);
    HBM_wt_FP_scale = NULL;
    free(HBM_wt_mem);
    HBM_wt_mem = NULL;
}

void DAT_IN_TRANS_FUNCTION_MVM(struct FPGA_HBM_MVM_cfg cfg, int *dat_in[], int *HBM[])
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

void DAT_OUT_TRANS_FUNCTION_MVM(struct FPGA_HBM_MVM_cfg cfg, int *HBM[], uint16_t *dat_out[])
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