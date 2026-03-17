#include "stdint.h"
#include "../TOP_defines.h"
#include "HBM_DDR_trans.h"

void General_Map_Feature_Data(struct FPGA_HBM_TRANSPOSE_cfg cfg, int Height, int Width, int CH, int *in, int *mem[])
{
//    printf("Height=%0d, Width=%0d, CH=%0d",Height,Width,CH);
    //for(int i=0;i<cfg.Hin*cfg.Win*cfg.CHin;i++)
    //{
    //    if(i<10)
    //    printf("in[%d]: %x.\n\r", i, in[i]);
    //}

    for(int i=0;i<Height;i++)
    {
        for(int j=0;j<Width;j++)
        {
            for(int k=0;k<CH;k=k+Tout)
            {
                int tmp[MAX_DAT_DW*Tout/32*2] = {0};
                for(int kk=k;kk<k+Tout;kk++)
                {
                    if(kk<CH) 
                       tmp[kk-k]=in[i*cfg.CHin*cfg.Win+j*cfg.CHin+kk]&0x0000ffff;
                    else
                       tmp[kk-k]=0;
                    //if(kk-k<10&&i==0&&j==0&&k==0)
                    //printf("tmp[%d]: %x; in[%d]&0x0000ffff: %x. \n\r", kk-k, tmp[kk-k], i*cfg.CHin*cfg.Win+j*cfg.CHin+kk, in[i*cfg.CHin*cfg.Win+j*cfg.CHin+kk]&0x0000ffff);
                }
                for(int m=0;m<MAX_DAT_DW*Tout/32;m++)
                {
                    mem[m][Height*Width*(k/Tout)+i*Width+j]=(tmp[2*m+1]<<MAX_DAT_DW)+tmp[2*m];
                    //if(m<5&&i==0&&j==0&&k==0)
                    //printf("mem[%d][%d]]: %x ; tmp[%d]<<%d: %x ; tmp[%d] = %x \n\r", m, Height*Width*(k/Tout)+i*Width+j, mem[m][Height*Width*(k/Tout)+i*Width+j], 2*m+1, MAX_DAT_DW, tmp[2*m+1]<<MAX_DAT_DW, 2*m, tmp[2*m]);
                }
            }
        }
    }
}
