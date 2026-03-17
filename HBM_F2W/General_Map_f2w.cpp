#include "stdint.h"
#include "../TOP_defines.h"
#include "HBM_f2w.h"

void General_Map_Feature_Data(struct FPGA_HBM_F2W_cfg cfg, int Height, int Width, int CH, int *in, int *mem[])
{
    for(int i=0;i<Height;i++)
    {
        for(int j=0;j<Width;j++)
        {
            for(int k=0;k<CH;k=k+L_Tout)
            {
                int tmp[MAX_DAT_DW*L_Tout/32*2] = {0};
                for(int kk=k;kk<k+L_Tout;kk++)
                {
                    if(kk<CH) 
                        tmp[kk-k]=in[i*cfg.Win*cfg.CHin+j*cfg.CHin+kk]&0x0000ffff;
                    else
                        tmp[kk-k]=0;
                }
                for(int m=0;m<MAX_DAT_DW*L_Tout/32;m++)
                    mem[Height*Width*(k/L_Tout)+i*Width+j][m]=(tmp[2*m+1]<<MAX_DAT_DW)+tmp[2*m];
            }
        }
    }
}

void General_Map_Wt_Data(struct FPGA_HBM_F2W_cfg cfg, int *in, int *mem[])
{
    for(int k=0;k<cfg.WT_CHout;k=k+Tout)
    {
        for(int j=0;j<cfg.WT_CHin;j++)
        {
            int tmp[MAX_DAT_DW*Tout/32*2] = {0};
            for(int kk=k;kk<k+Tout;kk++)
            {
                if(kk<cfg.WT_CHout) 
                    tmp[kk-k]=in[j*cfg.WT_CHout+kk]&0x0000ffff;
                else
                    tmp[kk-k]=0;
            }
            for(int m=0;m<MAX_DAT_DW*Tout/32;m++)
                mem[cfg.WT_CHin*(k/Tout)+j][m]=(tmp[2*m+1]<<MAX_DAT_DW)+tmp[2*m];
        }
    }
}

void General_DeMap_Feature_Data(struct FPGA_HBM_F2W_cfg cfg, int Height, int Width, int CH, uint16_t *mem[], uint16_t *out)
{
	for(int i=0;i<Height;i++)
    {
		for(int j=0;j<Width;j++)
        {
			for(int k=0;k<CH;k=k+L_Tout)
			{
				for(int kk=k;kk<k+L_Tout;kk++)
                {
					if(kk<CH) 
					   out[i*cfg.Wout*cfg.Head_x_CHout+j*cfg.Head_x_CHout+kk]=mem[Height*Width*(k/L_Tout)+i*Width+j][kk-k];
                }
            }
        }
    }
}