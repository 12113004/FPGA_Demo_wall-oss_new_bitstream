#include "./HBM_MVM_BN_RES/HBM_DDR_mvm_bn_res.h"

#define group 32
#define HBM_DDR_DEPTH 19
#define REG_NUM 256*4
#define REAL_BN_WT 1

void HBM_mvmbnres_test_random(struct FPGA_HBM_MVM_BN_RES_cfg cfg, char* path_name, char* head_name, struct bin_inf* weight_bin_inf, struct bin_inf* scales_bin_inf, struct bin_inf* dat_in_bin_inf, struct bin_inf* bn_wt_bin_inf, struct bin_inf* bn_bias_bin_inf, struct bin_inf* res_add_dat_bin_inf, 
                        struct bin_inf* wt_and_scale_in_DDR_inf[], int en_0, struct bin_inf* *dat_in_DDR_inf, int en_1, struct bin_inf* *bn_wt_and_bias_DDR_inf, int en_2, struct bin_inf* *Res_Add_dat_DDR_inf, int en_3)
{
    printf("MVM_BN_RES Test \n");

    // [`WT_DW-1:0] wt [`CHout][`CHin];
    int *wt = (int*)malloc(sizeof(int)*cfg.CHout*cfg.CHin); //[`CHout][`CHin]
    if (wt == NULL){perror("main");return;}
//    read_bin_32b(weight_bin_inf->bin_data_file, wt, cfg.CHout*cfg.CHin);
    for(int i=0;i<cfg.CHout*cfg.CHin;i++)
        wt[i] = 1; 

    // [`WT_quant_scale_DW-1:0] wt_FP_scale [`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
    int *wt_FP_scale = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    if (wt_FP_scale == NULL){perror("main");return;}
//    read_bin(scales_bin_inf->bin_data_file, wt_FP_scale, cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    for(int i=0;i<cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout;i++)
        wt_FP_scale[i] = 0x068e;//0.0001
    
    // [`DAT_DW_L0-1:0] dat_in [`Tb][`Hin][`Win][`CHin];
    int *dat_in[Tb];
    for(int i=0;i<Tb;i++)
    {
        dat_in[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
//        read_bin(dat_in_bin_inf->bin_data_file, dat_in[i], cfg.Hin*cfg.Win*cfg.CHin);
        for(int j=0;j<cfg.Hin*cfg.Win*cfg.CHin;j++)
            dat_in[i][j] = 0x3c00;
    }

    // [`MAX_BN_DW-1:0] bn_wt[`CHout_Padding];
    int *bn_wt= (int*)malloc(sizeof(int)*cfg.CHout_Padding);
    if (bn_wt== NULL){perror("main");return;}
//    read_bin(bn_wt_bin_inf->bin_data_file, bn_wt, cfg.CHout_Padding);
    for(int i=0;i<cfg.CHout_Padding;i++)
        bn_wt[i] = 0x3c00;

    //[`MAX_BN_DW-1:0] bn_bias[`CHout_Padding];
    int *bn_bias = (int*)malloc(sizeof(int)*cfg.CHout_Padding);
    if (bn_bias == NULL){perror("main");return;}
//    read_bin(bn_bias_bin_inf->bin_data_file, bn_bias, cfg.CHout_Padding);
    for(int i=0;i<cfg.CHout_Padding;i++)
        bn_bias[i] = 0;

    // [`MAX_DAT_DW-1:0] Res_Add_dat[`Tb][`Hout][`Wout][`CHout];
    int *Res_Add_dat[Tb];
    for(int i=0;i<Tb;i++)
    {
        Res_Add_dat[i]=(int*)malloc(sizeof(int)*cfg.Hout*cfg.Wout*cfg.CHout);
        if (Res_Add_dat[i] == NULL){perror("main");return;}
//        read_bin(res_add_dat_bin_inf->bin_data_file, Res_Add_dat[i], cfg.Hout*cfg.Wout*cfg.CHout);
        for(int j=0;j<cfg.Hout*cfg.Wout*cfg.CHout;j++)
            Res_Add_dat[i][j] = 0x0;//0x1e25;//0.006
    }

    int *HBM_DDR[group];
    int HBM_DDR_size = cfg.CHin_WT_and_Scale_Bytes*cfg.CHout/4/32;
    for(int i=0;i<group;i++)
    {
        HBM_DDR[i]=(int*)malloc(sizeof(int)*HBM_DDR_size);
        if (HBM_DDR[i] == NULL){perror("main");return;}
    }

    int *dat_in_DDR[1];
    for(int i=0;i<1;i++)
    {
        dat_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32);
        if (dat_in_DDR[i] == NULL){perror("main");return;}
    }

    int *bn_wt_and_bias_in_DDR[1];
    for(int i=0;i<1;i++)
    {
        bn_wt_and_bias_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.BN_ch_group_times*AXI_DAT_WIDTH/32);
        if (bn_wt_and_bias_in_DDR[i] == NULL){perror("main");return;}
    }

    int *Res_Add_dat_in_DDR[Tb];
    for(int i=0;i<Tb;i++)
    {
        Res_Add_dat_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32);
        if (Res_Add_dat_in_DDR[i] == NULL){perror("main");return;}
    }
    
    WT_TRANS_FUNCTION_MVM_BN_RES(cfg, wt, wt_FP_scale, HBM_DDR);
    DAT_IN_TRANS_FUNCTION_MVM_BN_RES(cfg, dat_in, dat_in_DDR);
    BN_WT_AND_BIAS_TRANS_FUNCTION_MVM_BN_RES(cfg, bn_wt, bn_bias, bn_wt_and_bias_in_DDR);
    RES_ADD_DAT_TRANS_FUNCTION_MVM_BN_RES(cfg, Res_Add_dat, Res_Add_dat_in_DDR);

    if(en_0)
    {
        char* filepath1 = (char*)malloc(sizeof(char)*200);
        sprintf(filepath1, "./%s/%s", path_name, "MVM_BN_RES_write_to_HBM_bin");
        uint64_t HBM_ddr_base_address =0;
        for(uint64_t m=0; m<group; m++)
        {
            char* write_filename_wt = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename_wt, "%s/%s_HBM_DDR_%02llu.bin", filepath1, head_name, m);
            FILE *fp3=fopen(write_filename_wt,"wb");
            if(fp3==NULL)
            {
            	printf("Can't open file: %s\n",write_filename_wt);
            	return;
            }
            printf(" generate %s \n", write_filename_wt);
            fwrite(HBM_DDR[m], sizeof(int), HBM_DDR_size, fp3);
            fclose(fp3);

            HBM_ddr_base_address = m*(1<<28);
            wt_and_scale_in_DDR_inf[m] = get_bin_inf(HBM_ddr_base_address+cfg.HBM00_WT_BASE_ADDR , HBM_DDR_size*4, write_filename_wt);
        }

        // Free malloc
        free(filepath1);
        filepath1 = NULL;
    }
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "MVM_BN_RES_DDR_bin");
    if(en_1)
    {
        char* write_filename1 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<1; m++){
        sprintf(write_filename1, "%s/%s_%s.bin", filepath2, head_name, "dat_in_DDR");
        FILE *fp3=fopen(write_filename1,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename1);
        	return;
        }
        printf(" generate %s \n", write_filename1);
        fwrite(dat_in_DDR[m], sizeof(int), cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32, fp3);
        fclose(fp3);

        *dat_in_DDR_inf = get_bin_inf(cfg.DAT_IN_BASE_ADDR, cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32*4, write_filename1);
        }
    }
    if(en_2)
    {
        char* write_filename2 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<1; m++){
        sprintf(write_filename2, "%s/%s_%s.bin", filepath2, head_name, "wt_and_bias_in_DDR");
        FILE *fp3=fopen(write_filename2,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename2);
        	return;
        }
        printf(" generate %s \n", write_filename2);
        fwrite(bn_wt_and_bias_in_DDR[m], sizeof(int), cfg.BN_ch_group_times*AXI_DAT_WIDTH/32, fp3);
        fclose(fp3);

        *bn_wt_and_bias_DDR_inf = get_bin_inf(cfg.BN_BASE_ADDR, cfg.BN_ch_group_times*AXI_DAT_WIDTH/32*4, write_filename2);
        }
    }
    if(en_3)
    {
        char* write_filename3 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<Tb; m++){
        sprintf(write_filename3, "%s/%s_%s.bin", filepath2, head_name, "res_add_dat_in_DDR");
        FILE *fp3=fopen(write_filename3,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename3);
        	return;
        }
        printf(" generate %s \n", write_filename3);
        fwrite(Res_Add_dat_in_DDR[m], sizeof(int), cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32, fp3);

        *Res_Add_dat_DDR_inf = get_bin_inf(cfg.Res_Add_BASE_ADDR, cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32*4, write_filename3);
        fclose(fp3);
        }
    }

    // Free malloc
    free(filepath2);
    filepath2 = NULL;
	free(wt);
	wt = NULL;
	free(wt_FP_scale);
	wt_FP_scale = NULL;
	free(dat_in[0]);
	dat_in[0] = NULL;
	free(bn_wt);
	bn_wt = NULL;
	free(bn_bias);
	bn_bias = NULL;
	free(Res_Add_dat[0]);
	Res_Add_dat[0] = NULL;
    for(int i=0;i<group;i++)
    {
	    free(HBM_DDR[i]);
	    HBM_DDR[i] = NULL;
    }
	free(dat_in_DDR[0]);
	dat_in_DDR[0] = NULL;
	free(bn_wt_and_bias_in_DDR[0]);
	bn_wt_and_bias_in_DDR[0] = NULL;
	free(Res_Add_dat_in_DDR[0]);
	Res_Add_dat_in_DDR[0] = NULL;
}

void HBM_mvmbnres_test(struct FPGA_HBM_MVM_BN_RES_cfg cfg, char* path_name, char* head_name, struct bin_inf* weight_bin_inf, struct bin_inf* scales_bin_inf, struct bin_inf* dat_in_bin_inf, struct bin_inf* bn_wt_bin_inf, struct bin_inf* bn_bias_bin_inf, struct bin_inf* res_add_dat_bin_inf, 
                        struct bin_inf* wt_and_scale_in_DDR_inf[], int en_0, struct bin_inf* *dat_in_DDR_inf, int en_1, struct bin_inf* *bn_wt_and_bias_DDR_inf, int en_2, struct bin_inf* *Res_Add_dat_DDR_inf, int en_3)
{
    printf("MVM_BN_RES Test \n");

    // [`WT_DW-1:0] wt [`CHout][`CHin];
    int *wt = (int*)malloc(sizeof(int)*cfg.CHout*cfg.CHin); //[`CHout][`CHin]
    if (wt == NULL){perror("main");return;}
    read_bin_32b(weight_bin_inf->bin_data_file, wt, cfg.CHout*cfg.CHin);

    // [`WT_quant_scale_DW-1:0] wt_FP_scale [`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
    int *wt_FP_scale = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    if (wt_FP_scale == NULL){perror("main");return;}
    read_bin(scales_bin_inf->bin_data_file, wt_FP_scale, cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    
    // [`DAT_DW_L0-1:0] dat_in [`Tb][`Hin][`Win][`CHin];
    int *dat_in[Tb];
    for(int i=0;i<Tb;i++)
    {
        dat_in[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
        read_bin(dat_in_bin_inf->bin_data_file, dat_in[i], cfg.Hin*cfg.Win*cfg.CHin);
    }

    // [`MAX_BN_DW-1:0] bn_wt[`CHout_Padding];
    int *bn_wt= (int*)malloc(sizeof(int)*cfg.CHout_Padding);
    if (bn_wt== NULL){perror("main");return;}
    read_bin(bn_wt_bin_inf->bin_data_file, bn_wt, cfg.CHout_Padding);

    //[`MAX_BN_DW-1:0] bn_bias[`CHout_Padding];
    int *bn_bias = (int*)malloc(sizeof(int)*cfg.CHout_Padding);
    if (bn_bias == NULL){perror("main");return;}
    read_bin(bn_bias_bin_inf->bin_data_file, bn_bias, cfg.CHout_Padding);

    // [`MAX_DAT_DW-1:0] Res_Add_dat[`Tb][`Hout][`Wout][`CHout];
    int *Res_Add_dat[Tb];
    for(int i=0;i<Tb;i++)
    {
        Res_Add_dat[i]=(int*)malloc(sizeof(int)*cfg.Hout*cfg.Wout*cfg.CHout);
        if (Res_Add_dat[i] == NULL){perror("main");return;}
        read_bin(res_add_dat_bin_inf->bin_data_file, Res_Add_dat[i], cfg.Hout*cfg.Wout*cfg.CHout);
    }

    int *HBM_DDR[group];
    int HBM_DDR_size = cfg.CHin_WT_and_Scale_Bytes*cfg.CHout/4/32;
    for(int i=0;i<group;i++)
    {
        HBM_DDR[i]=(int*)malloc(sizeof(int)*HBM_DDR_size);
        if (HBM_DDR[i] == NULL){perror("main");return;}
    }

    int *dat_in_DDR[1];
    for(int i=0;i<1;i++)
    {
        dat_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32);
        if (dat_in_DDR[i] == NULL){perror("main");return;}
    }

    int *bn_wt_and_bias_in_DDR[1];
    for(int i=0;i<1;i++)
    {
        bn_wt_and_bias_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.BN_ch_group_times*AXI_DAT_WIDTH/32);
        if (bn_wt_and_bias_in_DDR[i] == NULL){perror("main");return;}
    }

    int *Res_Add_dat_in_DDR[Tb];
    for(int i=0;i<Tb;i++)
    {
        Res_Add_dat_in_DDR[i] = (int*)malloc(sizeof(int)*cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32);
        if (Res_Add_dat_in_DDR[i] == NULL){perror("main");return;}
    }
    
    WT_TRANS_FUNCTION_MVM_BN_RES(cfg, wt, wt_FP_scale, HBM_DDR);
    DAT_IN_TRANS_FUNCTION_MVM_BN_RES(cfg, dat_in, dat_in_DDR);
    BN_WT_AND_BIAS_TRANS_FUNCTION_MVM_BN_RES(cfg, bn_wt, bn_bias, bn_wt_and_bias_in_DDR);
    RES_ADD_DAT_TRANS_FUNCTION_MVM_BN_RES(cfg, Res_Add_dat, Res_Add_dat_in_DDR);

    if(en_0)
    {
        char* filepath1 = (char*)malloc(sizeof(char)*200);
        sprintf(filepath1, "./%s/%s", path_name, "MVM_BN_RES_write_to_HBM_bin");
        uint64_t HBM_ddr_base_address =0;
        for(uint64_t m=0; m<group; m++)
        {
            char* write_filename_wt = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename_wt, "%s/%s_HBM_DDR_%02llu.bin", filepath1, head_name, m);
            FILE *fp3=fopen(write_filename_wt,"wb");
            if(fp3==NULL)
            {
            	printf("Can't open file: %s\n",write_filename_wt);
            	return;
            }
            printf(" generate %s \n", write_filename_wt);
            fwrite(HBM_DDR[m], sizeof(int), HBM_DDR_size, fp3);
            fclose(fp3);

            HBM_ddr_base_address = m*(1<<28);
            wt_and_scale_in_DDR_inf[m] = get_bin_inf(HBM_ddr_base_address+cfg.HBM00_WT_BASE_ADDR , HBM_DDR_size*4, write_filename_wt);
        }

        // Free malloc
        free(filepath1);
        filepath1 = NULL;
    }
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "MVM_BN_RES_DDR_bin");
    if(en_1)
    {
        char* write_filename1 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<1; m++){
        sprintf(write_filename1, "%s/%s_%s.bin", filepath2, head_name, "dat_in_DDR");
        FILE *fp3=fopen(write_filename1,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename1);
        	return;
        }
        printf(" generate %s \n", write_filename1);
        fwrite(dat_in_DDR[m], sizeof(int), cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32, fp3);
        fclose(fp3);

        *dat_in_DDR_inf = get_bin_inf(cfg.DAT_IN_BASE_ADDR, cfg.Win*cfg.Hin*cfg.CHin_div_Tout*Tb*AXI_DAT_WIDTH/32*4, write_filename1);
        }
    }
    if(en_2)
    {
        char* write_filename2 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<1; m++){
        sprintf(write_filename2, "%s/%s_%s.bin", filepath2, head_name, "wt_and_bias_in_DDR");
        FILE *fp3=fopen(write_filename2,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename2);
        	return;
        }
        printf(" generate %s \n", write_filename2);
        fwrite(bn_wt_and_bias_in_DDR[m], sizeof(int), cfg.BN_ch_group_times*AXI_DAT_WIDTH/32, fp3);
        fclose(fp3);

        *bn_wt_and_bias_DDR_inf = get_bin_inf(cfg.BN_BASE_ADDR, cfg.BN_ch_group_times*AXI_DAT_WIDTH/32*4, write_filename2);
        }
    }
    if(en_3)
    {
        char* write_filename3 = (char*)malloc(sizeof(char)*200);
        for(int m=0; m<Tb; m++){
        sprintf(write_filename3, "%s/%s_%s.bin", filepath2, head_name, "res_add_dat_in_DDR");
        FILE *fp3=fopen(write_filename3,"wb");
        if(fp3==NULL)
        {
        	printf("Can't open file: %s\n",write_filename3);
        	return;
        }
        printf(" generate %s \n", write_filename3);
        fwrite(Res_Add_dat_in_DDR[m], sizeof(int), cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32, fp3);

        *Res_Add_dat_DDR_inf = get_bin_inf(cfg.Res_Add_BASE_ADDR, cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*Tb*AXI_DAT_WIDTH/32*4, write_filename3);
        fclose(fp3);
        }
    }

    // Free malloc
    free(filepath2);
    filepath2 = NULL;
	free(wt);
	wt = NULL;
	free(wt_FP_scale);
	wt_FP_scale = NULL;
	free(dat_in[0]);
	dat_in[0] = NULL;
	free(bn_wt);
	bn_wt = NULL;
	free(bn_bias);
	bn_bias = NULL;
	free(Res_Add_dat[0]);
	Res_Add_dat[0] = NULL;
    for(int i=0;i<group;i++)
    {
	    free(HBM_DDR[i]);
	    HBM_DDR[i] = NULL;
    }
	free(dat_in_DDR[0]);
	dat_in_DDR[0] = NULL;
	free(bn_wt_and_bias_in_DDR[0]);
	bn_wt_and_bias_in_DDR[0] = NULL;
	free(Res_Add_dat_in_DDR[0]);
	Res_Add_dat_in_DDR[0] = NULL;
}

void HBM_mvmbnres_receive_and_compare(struct FPGA_HBM_MVM_BN_RES_cfg cfg, HANDLE c2hx_device,char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf)
{
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s/%s_%s", path_name, "MVM_BN_RES_FPGA_out_bin", head_name, "dat_out.bin");
    struct bin_inf* l0_out_inf = get_bin_inf(cfg.DAT_OUT_BASE_ADDR, cfg.Wout*cfg.Hout*cfg.CHout_div_Tout*AXI_DAT_WIDTH/32*4, filepath1);

    // Read data
    c2hx_device_read_bin(c2hx_device, l0_out_inf);

    /***** Bin to Array *****/
    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem_tmp[`Hout][`Wout][`CHout_div_Tout];
    int *dat_out_mem_tmp = (int*)malloc(sizeof(int)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem_tmp == NULL){printf("fail to malloc dat_out_mem_tmp \n");}
    read_bin(l0_out_inf->bin_data_file, dat_out_mem_tmp, AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);

    // [`AXI_DAT_WIDTH*Tb-1:0] dat_out_mem[`Hout][`Wout][`CHout_div_Tout];
    uint16_t *dat_out_mem = (uint16_t*)malloc(sizeof(uint16_t)*AXI_DAT_WIDTH/16*Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout);
    if (dat_out_mem == NULL){printf("fail to malloc dat_out_mem \n");}

    for(int i=0;i<Tb*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout;i++)
    {
        for(int j=0;j<AXI_DAT_WIDTH/16;j++)
        {
            dat_out_mem[i*AXI_DAT_WIDTH/16+j] = (uint16_t)dat_out_mem_tmp[i*AXI_DAT_WIDTH/16+j]&0x0000ffff;
            //printf("dat_out_mem[%d] = %d \n",  i*AXI_DAT_WIDTH/16+j, dat_out_mem[i*AXI_DAT_WIDTH/16+j]);
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
    DAT_OUT_DEMAP_FUNCTION_MVM_BN_RES(cfg, dat_out_mem, dat_out);

    // Demaped Array to Bin
    struct bin_inf* l0_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "MVM_BN_RES_FPGA_out_bin");
    for(int m=0; m<1; m++){
    sprintf(write_filename, "%s/%s_%s.bin", filepath2, head_name, "dat_out_demaped");
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return;
    }
//    printf(" generate %s \n", write_filename);
    fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout_Padding, fp3);
    fclose(fp3);

    l0_out_demaped_inf = get_bin_inf( 0, cfg.Hout*cfg.Wout*cfg.CHout_Padding, write_filename);
    }
    for(int i=0;i<Tb;i++)
        fp_compare_bin_with_bin(l0_out_demaped_inf->bin_data_file, l0_out_demaped_inf->bin_data_size, standard_out_bin_inf->bin_data_file);

    // Free malloc
    free(filepath1);
    filepath1 = NULL;
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
    free(filepath2);
    filepath2 = NULL;
}