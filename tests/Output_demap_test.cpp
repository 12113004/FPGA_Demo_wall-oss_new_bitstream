#include "../HBM_LN/HBM_DDR_ln.h"
#include "../xdma_lib/half.hpp"

void Ouput_demap_test(struct FPGA_HBM_LN_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* dat_in_bin_inf)
{
    /***** Bin to Array *****/
    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem_tmp[`Hout][`Wout][`CHout_div_Tout];
    int *dat_out_mem_tmp = (int*)malloc(sizeof(int)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem_tmp == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    read_bin(dat_in_bin_inf->bin_data_file, dat_out_mem_tmp, AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);

    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem[`Hout][`Wout][`CHout_div_Tout];
    uint16_t *dat_out_mem = (uint16_t*)malloc(sizeof(uint16_t)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem == NULL){printf("fail to malloc dat_out_mem \n");}

    for(int i=0;i<Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout;i++)
    {
       for(int j=0;j<AXI_DAT_WIDTH/16;j++)
       {
           dat_out_mem[i*AXI_DAT_WIDTH/16+j] = (uint16_t)dat_out_mem_tmp[i*AXI_DAT_WIDTH/16+j]&0x0000ffff;
           //printf("dat_out_mem[%d] = 0x%04x ; dat_out_mem_tmp[%d] = 0x%08x. \n", i*AXI_DAT_WIDTH/16+j, dat_out_mem[i*AXI_DAT_WIDTH/16+j], i*AXI_DAT_WIDTH/16+j, dat_out_mem_tmp[i*AXI_DAT_WIDTH/16+j]);
       }
    }

    // bit [DAT_DW_L1-1:0] dat_out [`Tb][`Hout][`Wout][`CHout_Padding];
    uint16_t *dat_out[Tb];
    for(int i=0;i<Tb;i++)
    {
       dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout_Padding);
       if (dat_out[i] == NULL){perror("main");return;}
    }
    
    /***** Array Demap *****/
    DAT_OUT_DEMAP_FUNCTION_LN(cfg, dat_out_mem, dat_out);

    // Demaped Array to Bin
    struct bin_inf* l0_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename = (char*)malloc(sizeof(char)*200);
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "step_data_1128/demaped_bin");
    for(int m=0; m<1; m++){
    sprintf(write_filename, "%s/%s_%s.bin", filepath, head_name, "test_demaped");
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return;
    }
    printf(" generate %s \n", write_filename);
    fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout_Padding, fp3);
    fclose(fp3);

    l0_out_demaped_inf = get_bin_inf( 0, cfg.Hout*cfg.Wout*cfg.CHout_Padding, write_filename);
    }

    // Free malloc
    free(dat_out_mem_tmp);
    dat_out_mem_tmp  = NULL;
    free(dat_out_mem);
    dat_out_mem = NULL;
    free(dat_out[0]);
    dat_out[0] = NULL;
    free(l0_out_demaped_inf);
    l0_out_demaped_inf= NULL;
    free(write_filename);
    write_filename = NULL;
    free(filepath);
    filepath = NULL;
}

void Random_Ouput_demap_test(struct FPGA_HBM_LN_cfg cfg, HANDLE c2hx_device, char* path_name, struct bin_inf* dat_in_bin_inf)
{
    /***** Bin to Array *****/
    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem_tmp[`Hout][`Wout][`CHout_div_Tout];
    int *dat_out_mem_tmp = (int*)malloc(sizeof(int)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem_tmp == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    read_bin(dat_in_bin_inf->bin_data_file, dat_out_mem_tmp, AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);

    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem[`Hout][`Wout][`CHout_div_Tout];
    uint16_t *dat_out_mem = (uint16_t*)malloc(sizeof(uint16_t)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem == NULL){printf("fail to malloc dat_out_mem \n");}

    for(int i=0;i<Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout;i++)
    {
       for(int j=0;j<AXI_DAT_WIDTH/16;j++)
       {
           dat_out_mem[i*AXI_DAT_WIDTH/16+j] = (uint16_t)dat_out_mem_tmp[i*AXI_DAT_WIDTH/16+j]&0x0000ffff;
           //printf("dat_out_mem[%d] = 0x%04x ; dat_out_mem_tmp[%d] = 0x%08x. \n", i*AXI_DAT_WIDTH/16+j, dat_out_mem[i*AXI_DAT_WIDTH/16+j], i*AXI_DAT_WIDTH/16+j, dat_out_mem_tmp[i*AXI_DAT_WIDTH/16+j]);
       }
    }

    // bit [DAT_DW_L1-1:0] dat_out [`Tb][`Hout][`Wout][`CHout_Padding];
    uint16_t *dat_out[Tb];
    for(int i=0;i<Tb;i++)
    {
       dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout_Padding);
       if (dat_out[i] == NULL){perror("main");return;}
    }
    
    /***** Array Demap *****/
    DAT_OUT_DEMAP_FUNCTION_LN(cfg, dat_out_mem, dat_out);

    // Demaped Array to Bin
    struct bin_inf* l0_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename = (char*)malloc(sizeof(char)*200);
    for(int m=0; m<1; m++){
    sprintf(write_filename, "./%s_demaped.bin", path_name);
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return;
    }
    printf(" generate %s \n", write_filename);
    fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout_Padding, fp3);
    fclose(fp3);

    l0_out_demaped_inf = get_bin_inf( 0, cfg.Hout*cfg.Wout*cfg.CHout_Padding, write_filename);
    }

    // Free malloc
    free(dat_out_mem_tmp);
    dat_out_mem_tmp  = NULL;
    free(dat_out_mem);
    dat_out_mem = NULL;
    free(dat_out[0]);
    dat_out[0] = NULL;
    free(l0_out_demaped_inf);
    l0_out_demaped_inf= NULL;
    free(write_filename);
    write_filename = NULL;
}