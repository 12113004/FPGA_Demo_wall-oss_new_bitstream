#include "stdint.h"
#include "stdlib.h"
#include "../TOP_defines.h"
#include "General_Map_mvm_bn_res.h"

struct FPGA_HBM_MVM_BN_RES_cfg GetFPGA_HBM_MVM_BN_RES_cfg(int Height, int Win, int Width_in, int Width_out, int Skip_Factor, int RELU_EN, int log2_WT_base_addr_Bank_Step,
                                                  uint64_t DAT_IN_BASE_ADDR, uint64_t HBM00_WT_BASE_ADDR, uint64_t BN_BASE_ADDR, uint64_t Res_Add_BASE_ADDR, uint64_t DAT_OUT_BASE_ADDR)
{
    struct FPGA_HBM_MVM_BN_RES_cfg ret;

    ret.Height                        = Height;//19//
    ret.Width_in                      = Width_in;//4096//
    ret.Width_out                     = Width_out;//128//
    ret.Skip_Factor                   = Skip_Factor;//2//
    
    ret.DAT_DW_L0                     = MAX_DAT_DW;
    ret.DAT_DW_L1                     = MAX_DAT_DW;
    ret.WT_DW                         = MAX_WT_DW;
    ret.BN_DW                         = MAX_BN_DW;
    ret.Tin                           = (base_Tin);
    
    ret.Win                           = Win;//Tout;
    ret.Hin                           = ((Height+ret.Win-1)/ret.Win);
    ret.CHin                          = Width_in;
    ret.CHout                         = Width_out;
    
    ret.Wout                          = ret.Win;
    ret.Hout                          = ret.Hin;
    ret.CHout_div_Tout                = ((ret.CHout+Tout-1)/Tout);
    ret.CHin_div_Tout                 = ((ret.CHin+Tout-1)/Tout);
    ret.CHin_Padding_with_Tout        = (ret.CHin_div_Tout*Tout);
    ret.Tin_div_Tout                  = ((ret.Tin+Tout-1)/Tout);
    ret.CHout_Padding                 = (ret.CHout_div_Tout*Tout);
    
    ret.RELU_EN                       = RELU_EN;//0
    
    ret.DAT_IN_BASE_ADDR              = DAT_IN_BASE_ADDR;
    ret.DAT_IN_BATCH_STRIDE           = (Pixel_Data_Bytes*ret.Win*ret.Hin*ret.CHin_div_Tout);
    ret.DAT_IN_SURFACE_STRIDE         = (Pixel_Data_Bytes*ret.Win*ret.Hin);
    ret.DAT_IN_LINE_STRIDE            = (Pixel_Data_Bytes*ret.Win);
    ret.DAT_IN_scale                  = 0;
    
    ret.WT_CHin_div_Tin               = ((ret.CHin+ret.Tin-1)/ret.Tin);
    ret.WT_CHin_Padding_with_Tin      = (ret.WT_CHin_div_Tin*ret.Tin);
    ret.WT_CHin_div_Tblock            = ((ret.WT_CHin_Padding_with_Tin+T_quant_block-1)/T_quant_block);
    ret.Tblock_div_Tin                = (T_quant_block/ret.Tin);
    
    //ret.WT_CH_Tgroup (T_quant_block*HBM_AXI_DATA_WIDTH/WT_quant_scale_DW) // =2048 CHins
    ret.WT_scale_group_nums           = ((ret.WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)/WT_CH_Tgroup);
    ret.WT_CH_Tgroup_div_Tblock       = (WT_CH_Tgroup/T_quant_block); //2048/128=16
    
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
    ret.log2_WT_base_addr_Bank_Step   = log2_WT_base_addr_Bank_Step;  //8
    ret.WT_base_addr_Bank_Step        = (1<<ret.log2_WT_base_addr_Bank_Step);
    
    //ret.WT_scale_bits (CHout_Padding*WT_CHin_div_Tblock*WT_quant_scale_DW)
    ret.WT_scale_bits                 = (ret.CHout_Padding*HBM_AXI_DATA_WIDTH*ret.WT_scale_group_nums);
    
    ret.WT_SIZE_IN_BYTES              = (((ret.CHout_Padding*ret.WT_CHin_Padding_with_Tin*ret.WT_DW)>>3)+((ret.WT_scale_bits)>>3));
    ret.WT_NUM_DIV_TIN                = (ret.CHout_Padding*ret.WT_CHin_div_Tin);
    ret.WT_scale                      = 0;
    ret.HBM00_WT_BASE_ADDR            = HBM00_WT_BASE_ADDR;
    
    ret.Conv_out_scale                = 0;
    
    //ret.bias
    ret.BN_BASE_ADDR                  = BN_BASE_ADDR;
    ret.BN_SURFACE_STRIDE             = (Tout*MAX_BN_DW*2)>>3;
    ret.BN_num_per_AXI_DW             = (AXI_DAT_WIDTH/(2*ret.BN_DW)); // BN_num_per_AXI_DW <= `Tout
    ret.BN_ch_group_times             = (ret.CHout_Padding/ret.BN_num_per_AXI_DW);
    ret.BN_WT_scale                   = 0;
    ret.BN_BIAS_scale                 = 0;
    
    ret.Res_Add_BASE_ADDR             = Res_Add_BASE_ADDR;
    ret.Res_Add_BATCH_STRIDE          = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_Tout);
    ret.Res_Add_SURFACE_STRIDE        = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.Res_Add_LINE_STRIDE           = (Pixel_Data_Bytes*ret.Wout);
    ret.Res_Add_scale                 = 0;
    
    ret.DAT_OUT_BASE_ADDR             = DAT_OUT_BASE_ADDR;
    ret.DAT_OUT_BATCH_STRIDE          = (Pixel_Data_Bytes*ret.Wout*ret.Hout*ret.CHout_div_Tout);
    ret.DAT_OUT_SURFACE_STRIDE        = (Pixel_Data_Bytes*ret.Wout*ret.Hout);
    ret.DAT_OUT_LINE_STRIDE           = (Pixel_Data_Bytes*ret.Wout);
    ret.DAT_OUT_scale                 = ret.Conv_out_scale;

    return ret;
}

void WT_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg , int *wt, int *wt_FP_scale, int *HBM_DDR[])
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
//                            if(wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]<0)
//                            {
//                                   tmp_wt=8-wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p];
//                            }else
//                            {
                                  tmp_wt=wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p];
//                            }
                            //tmp = tmp + ( (wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]&(0x0000000f)) << cfg.WT_DW*p ); 
                            tmp = tmp + ( (tmp_wt&(0x0000000f)) << cfg.WT_DW*p ); 
                            //if(m==0 && i==0 && j==0 && k==0){printf("wt tmp[%d] = %x , wt[%d] =  %x\n", p, tmp, p, wt[(cfg.CHin*(i*Tout+j*HBM_Port+k))+m*8+p]);}
                        }
                        
                    }
                    HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + j*HBM_Port + k)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+m] = tmp;
                    //if(k==1){printf("HBM_wt_mem[%d][%d][%d][%d] = %04x \n", i, j, k, m, HBM_wt_mem[(i*Tout/HBM_Port*HBM_Port + j*HBM_Port + k)*(cfg.WT_DW*cfg.WT_CHin_Padding_with_Tin/32)+m]);}
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
                            {
                               //tmp = tmp +  ((wt_FP_scale[i*cfg.WT_CHin_div_Tblock*Tout + (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p)*Tout + (k*HBM_Port+m)]&0x0000ffff) << p*WT_quant_scale_DW);                            
                               tmp = tmp +  ((wt_FP_scale[i*cfg.WT_CHin_div_Tblock*Tout + (j*cfg.WT_CH_Tgroup_div_Tblock+n*2+p) + (k*HBM_Port+m)*cfg.WT_CHin_div_Tblock]&0x0000ffff) << p*WT_quant_scale_DW);                            

                            }
                        }
                        HBM_wt_FP_scale[(i*(cfg.WT_scale_group_nums*Tout/HBM_Port*HBM_Port) + j*(Tout/HBM_Port*HBM_Port) + k*HBM_Port + m)*cfg.WT_CH_Tgroup_div_Tblock*16/32+n]=tmp;
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

    // Free malloc
    free(HBM_wt_FP_scale);
    HBM_wt_FP_scale = NULL;
    free(HBM_wt_mem);
    HBM_wt_mem = NULL;
}

void DAT_IN_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int *dat_in[], int *HBM_DDR[])
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

void BN_WT_AND_BIAS_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int *bn_wt, int *bn_bias, int *HBM_DDR[])
{
    // [`MAX_BN_DW*2-1:0] bn_wt_and_bias[`CHout_div_Tout*`Tout];
    int *bn_wt_and_bias = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*Tout);
    if (bn_wt_and_bias == NULL){printf("fail to malloc bn_wt_and_bias \n");}

    // [`AXI_DAT_WIDTH-1:0]bn_wt_and_bias_mem[`BN_ch_group_times]
    int *bn_wt_and_bias_mem[AXI_DAT_WIDTH/32];
    for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        bn_wt_and_bias_mem[i] = (int*)malloc(sizeof(int)*cfg.BN_ch_group_times);
        if (bn_wt_and_bias_mem[i] == NULL){printf("fail to malloc bn_wt_and_bias_mem \n");}
    }

    for(int i=0;i<cfg.CHout_div_Tout*Tout;i++)
    {
        bn_wt_and_bias[i] = ((bn_wt[i]<<16)&0xffff0000) + (bn_bias[i]&0x0000ffff);
        //printf("bn_wt_and_bias[%d] = 0x%x.\n", i, bn_wt_and_bias[i]);
    }


    // bn_wt_and_bias
    for(int i=0;i<cfg.BN_ch_group_times;i++)
    {
		for(int j=0;j<cfg.BN_num_per_AXI_DW;j++)
		{
			if(AXI_DAT_WIDTH>=(2*cfg.BN_DW))
                bn_wt_and_bias_mem[j][i] = bn_wt_and_bias[i*cfg.BN_num_per_AXI_DW+j];
                //printf("bn_wt_and_bias[%d][%d] = 0x%x.\n", j, i, bn_wt_and_bias_mem[i][j]);
	        else
                printf("Error!! AXI_DAT_WIDTH=%d, 2*BN_DW=%d,  AXI_DAT_WIDTH too small!",AXI_DAT_WIDTH, (2*cfg.BN_DW));
        }
    }

	for(int i=0;i<cfg.BN_ch_group_times;i++)
    {
		for(int j=0;j<AXI_DAT_WIDTH/32;j++)
		{
            HBM_DDR[0][i*AXI_DAT_WIDTH/32+j] = bn_wt_and_bias_mem[j][i];
            //printf("HBM_DDR[0][%d] = 0x%x.\n\r", i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][i*AXI_DAT_WIDTH/32+j]);
        }
    }

    // Free malloc
    free(bn_wt_and_bias);
    bn_wt_and_bias = NULL;
	for(int i=0;i<AXI_DAT_WIDTH/32;i++)
    {
        free(bn_wt_and_bias_mem[i]);
        bn_wt_and_bias_mem[i] = NULL;
    }
}

void RES_ADD_DAT_TRANS_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int *Res_Add_dat[],int *HBM_DDR[])
{
    //for(int i=0;i<cfg.Hout*cfg.Wout*cfg.CHout;i++)
    //{
    //    printf("Res_Add_dat[0][%d]: %x.\n\r", i, Res_Add_dat[0][i]);
    //}

    // [MAX_DAT_DW*Tout*Tb] Res_Add_mem[`Hout*`Wout*`CHout_div_Tout]
    int *Res_Add_mem[MAX_DAT_DW*Tout*Tb/32];
    for (int i = 0; i<MAX_DAT_DW*Tout*Tb/32; i++)
    {
        Res_Add_mem[i] = (int*)malloc(sizeof(int)*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
        if (Res_Add_mem[i] == NULL){printf("fail to malloc Res_Add_mem \n");}
    }

    // [MAX_DAT_DW*Tout] tp_Res_Add_mem[`Tb][`Wout*`Hout*`CHout_div_Tout]
    int *tp_Res_Add_mem[Tb][MAX_DAT_DW*Tout/32];
    for(int j=0;j<MAX_DAT_DW*Tout/32;j++)
    {
        for(int i=0;i<Tb;i++)
        {
            tp_Res_Add_mem[i][j] = (int*)malloc(sizeof(int)*cfg.Wout*cfg.Hout*cfg.CHout_div_Tout);
            if (tp_Res_Add_mem[i][j] == NULL){printf("fail to malloc tp_Res_Add_mem \n");}
        }
    }

    // Res_Add
	for(int b=0;b<Tb;b++)
        General_Map_Res_Add_Data(cfg, cfg.Hout, cfg.Wout, cfg.CHout, Res_Add_dat[b], tp_Res_Add_mem[b]);

    for(int i=0;i<cfg.Wout*cfg.Hout*cfg.CHout_div_Tout;i++)
    {
        for(int b=0;b<Tb;b++) 
        {
            for(int j=0;j<MAX_DAT_DW*Tout/32;j++)
            {
                Res_Add_mem[MAX_DAT_DW*Tout/32*b+j][i]=tp_Res_Add_mem[b][j][i];
                //printf("Res_Add_mem[%d][%d] = 0x%x. \n\r", MAX_DAT_DW*Tout/32*b+j, i, Res_Add_mem[MAX_DAT_DW*Tout/32*b+j][i]);
            }
        }
    }

	for(int i=0;i<cfg.Wout*cfg.Hout*cfg.CHout_div_Tout;i++)
    {
		for(int j=0;j<Tb*AXI_DAT_WIDTH/32;j++)
		{
            HBM_DDR[0][i*AXI_DAT_WIDTH/32+j] = Res_Add_mem[j][i];
            //printf("HBM_DDR[0][%d] = 0x%x. \n\r", i*AXI_DAT_WIDTH/32+j, HBM_DDR[0][i*AXI_DAT_WIDTH/32+j]);
        }
    }

    // Free malloc
    for (int i = 0; i<MAX_DAT_DW*Tout*Tb/32; i++)
    {
        free(Res_Add_mem[i]);
        Res_Add_mem[i] = NULL;
    }
	for(int i=0;i<MAX_DAT_DW*Tout/32;i++)
    {
        free(tp_Res_Add_mem[0][i]);
        tp_Res_Add_mem[0][i] = NULL;
    }
}

void DAT_OUT_DEMAP_FUNCTION_MVM_BN_RES(struct FPGA_HBM_MVM_BN_RES_cfg cfg, uint16_t *dat_out_mem, uint16_t *dat_out[])
{
    // [`MAX_DAT_DW*`Tout-1:0]tp_dat_out_mem[`Tb][`Hout*`Wout*`CHout_div_Tout];
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

