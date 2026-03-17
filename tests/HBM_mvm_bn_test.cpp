#include "./HBM_MVM_BN/HBM_mvm_bn.h"

void HBM_mvmbn_test(struct FPGA_HBM_MVM_BN_cfg cfg, char* path_name, char* head_name, struct bin_inf* weight_in_bin_inf, struct bin_inf* scales_in_bin_inf, struct bin_inf* dat_in_bin_inf, struct bin_inf* bn_wt_in_bin_inf, struct bin_inf* bn_bias_in_bin_inf, 
                  struct bin_inf* wt_and_scale_in_HBM_inf[], int en_0, struct bin_inf* dat_in_HBM_inf[], int en_1, struct bin_inf* bn_wt_and_bias_in_HBM_inf[], int en_2)
{
    printf("MVM_BN Test \n");

    // [`WT_DW-1:0] wt [`CHout][`CHin];
    int *wt = (int*)malloc(sizeof(int)*cfg.CHout*cfg.CHin);
    if (wt == NULL){perror("main");return;}
    read_bin_32b(weight_in_bin_inf->bin_data_file, wt, cfg.CHout*cfg.CHin);

    // [`WT_quant_scale_DW-1:0] wt_FP_scale [`CHout_div_Tout][`WT_CHin_div_Tblock][`Tout];
    int *wt_FP_scale = (int*)malloc(sizeof(int)*cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    if (wt_FP_scale == NULL){perror("main");return;}
    read_bin(scales_in_bin_inf->bin_data_file, wt_FP_scale, cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout);
    
    // [`DAT_DW_L0-1:0] dat_in [`Tb][`Hin][`Win][`CHin];
    int **dat_in = (int**)malloc(sizeof(int*)*Tb);
    for(int i=0;i<Tb;i++)
    {
        dat_in[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
        read_bin(dat_in_bin_inf->bin_data_file, dat_in[i], cfg.Hin*cfg.Win*cfg.CHin);
    }

    // [`MAX_BN_DW-1:0] bn_wt[`CHout_Padding];
    int *bn_wt= (int*)malloc(sizeof(int)*cfg.CHout);
    if (bn_wt== NULL){perror("main");return;}
    read_bin(bn_wt_in_bin_inf->bin_data_file, bn_wt, cfg.CHout);

    //[`MAX_BN_DW-1:0] bn_bias[`CHout_Padding];
    int *bn_bias = (int*)malloc(sizeof(int)*cfg.CHout);
    if (bn_bias == NULL){perror("main");return;}
    read_bin(bn_bias_in_bin_inf->bin_data_file, bn_bias, cfg.CHout);

    int **wt_and_scale_in_HBM = (int**)malloc(sizeof(int*)*group);
    int wt_and_scale_in_HBM_size = cfg.CHin_WT_and_Scale_Bytes*cfg.CHout/4/group;
    for(int i=0;i<group;i++)
    {
        wt_and_scale_in_HBM[i]=(int*)malloc(sizeof(int)*wt_and_scale_in_HBM_size);
        if (wt_and_scale_in_HBM[i] == NULL){perror("main");return;}
    }

    int **dat_in_HBM = (int**)malloc(sizeof(int*)*group);
    int dat_in_HBM_size = Tb*cfg.Win*cfg.Hin*cfg.CHin_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    for(int i=0;i<group;i++)
    {
        dat_in_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_HBM[i] == NULL){perror("main");return;}
    }

    int **bn_wt_and_bias_in_HBM = (int**)malloc(sizeof(int*)*group);
    int bn_wt_and_bias_in_HBM_size = cfg.BN_ch_group_times*MAX_DAT_DW*L_Tout/32/group;
    for(int i=0;i<group;i++)
    {
        bn_wt_and_bias_in_HBM[i] = (int*)malloc(sizeof(int)*bn_wt_and_bias_in_HBM_size);
        if (bn_wt_and_bias_in_HBM[i] == NULL){perror("main");return;}
    }
    
    // for(int i=0;i<cfg.CHout*cfg.CHin;i++)
    //    if(i<4096) wt[i] = 1;
    // for(int i=0;i<cfg.CHout_div_Tout*cfg.WT_CHin_div_Tblock*Tout;i++)
    //    if(i<32) wt_FP_scale[i] = 0x3c00;
    //for(int j=0;j<cfg.Hin*cfg.Win*cfg.CHin;j++)
    //    dat_in[0][j] = 0x3c00;
    //for(int i=0;i<cfg.CHout;i++)
    //    bn_wt[i] = 0x3c00;
    //for(int i=0;i<cfg.CHout;i++)
    //    bn_bias[i] = 0x0000;

    if(en_0) WT_AND_SCALE_TRANS_FUNCTION_MVM_BN(cfg, wt, wt_FP_scale, wt_and_scale_in_HBM);
    if(en_1) DAT_IN_TRANS_FUNCTION_MVM_BN(cfg, dat_in, dat_in_HBM);
    if(en_2) BN_WT_AND_BIAS_IN_TRANS_FUNCTION_MVM_BN(cfg, bn_wt, bn_bias, bn_wt_and_bias_in_HBM);

    uint64_t HBM_base_address =0;
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "MVM_BN_HBM_bin");
    if(en_0)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char *write_filename1 = (char *)malloc(sizeof(char)*200);
            sprintf(write_filename1, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "wt_and_scale", m);
            FILE *fp=fopen(write_filename1,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename1);
            	return;
            }
            printf(" generate %s \n", write_filename1);
            fwrite(wt_and_scale_in_HBM[m], sizeof(int), wt_and_scale_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            wt_and_scale_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.HBM00_WT_BASE_ADDR , wt_and_scale_in_HBM_size*4, write_filename1);
        }
    }
    if(en_1)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename2 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename2, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "dat_in", m);
            FILE *fp=fopen(write_filename2,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename2);
            	return;
            }
            printf(" generate %s \n", write_filename2);
            fwrite(dat_in_HBM[m], sizeof(int), dat_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            dat_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_IN_BASE_ADDR, dat_in_HBM_size*4, write_filename2);
        }
    }
    if(en_2)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename3 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename3, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "bn_wt_and_bias_in", m);
            FILE *fp=fopen(write_filename3,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename3);
            	return;
            }
            printf(" generate %s \n", write_filename3);
            fwrite(bn_wt_and_bias_in_HBM[m], sizeof(int), bn_wt_and_bias_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            bn_wt_and_bias_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.BN_BASE_ADDR, bn_wt_and_bias_in_HBM_size*4, write_filename3);
        }
    }

    // Free malloc
    free(filepath);
    filepath = NULL;
	free(wt);
	wt = NULL;
	free(wt_FP_scale);
	wt_FP_scale = NULL;
	free(dat_in);
	dat_in = NULL;
	free(bn_wt);
	bn_wt = NULL;
	free(bn_bias);
	bn_bias = NULL;
	free(wt_and_scale_in_HBM);
	wt_and_scale_in_HBM = NULL;
	free(dat_in_HBM);
	dat_in_HBM = NULL;
	free(bn_wt_and_bias_in_HBM);
	bn_wt_and_bias_in_HBM = NULL;
}

void HBM_mvmbn_receive_and_compare(struct FPGA_HBM_MVM_BN_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf)
{
    /***** Generate Bin and Receive Data *****/
    struct bin_inf* *dat_out_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    int dat_out_HBM_size = Tb*cfg.Wout*cfg.Hout*cfg.CHout_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    uint64_t HBM_base_address =0;
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s", path_name, "MVM_BN_FPGA_out_bin/HBM_bin");
    for(uint64_t m=0;m<group;m++)
    {
        char* write_filename1 = (char*)malloc(sizeof(char)*200);
        sprintf(write_filename1, "%s/%s_%s_HBM_%02llu.bin", filepath1, head_name, "dat_out", m);
        HBM_base_address = m*(1<<28);
        dat_out_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_OUT_BASE_ADDR, dat_out_HBM_size*4, write_filename1);
        // Reveicve Data
        c2hx_device_read_bin(c2hx_device, dat_out_HBM_inf[m]);
    }

    /***** Read Bin *****/
    int **dat_out_HBM = (int**)malloc(sizeof(int*)*group);
    for(int i=0;i<group;i++)
    {
        dat_out_HBM[i] = (int*)malloc(sizeof(int)*dat_out_HBM_size);
        if (dat_out_HBM[i] == NULL){perror("fail to malloc dat_out_mem_tmp");}
    }
    for(int m=0;m<group;m++)
        read_bin_32b(dat_out_HBM_inf[m]->bin_data_file, dat_out_HBM[m], dat_out_HBM_size);

    // bit [`DAT_DW_out-1:0] dat_out [`Tb][`Hout][`Wout][`CHout];
    uint16_t **dat_out = (uint16_t**)malloc(sizeof(uint16_t*)*Tb);
    for(int i=0;i<Tb;i++)
    {
       dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout);
       if (dat_out[i] == NULL){perror("main");return;}
    }
    
    /***** Detransform*****/
    DAT_OUT_TRANS_FUNCTION_MVM_BN(cfg, dat_out_HBM, dat_out);

    /***** Generate Bin *****/
    struct bin_inf* dat_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename2 = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "MVM_BN_FPGA_out_bin");
    sprintf(write_filename2, "%s/%s_%s.bin", filepath2, head_name, "dat_out_demaped");
    FILE *fp=fopen(write_filename2,"wb");
    if(fp==NULL)
    {
    	printf("Can't open file: %s\n",write_filename2);
    	return;
    }
    printf(" generate %s \n", write_filename2);
    for(int m=0; m<Tb; m++)
        fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout, fp);
    fclose(fp);
    dat_out_demaped_inf = get_bin_inf( 0, Tb*cfg.Hout*cfg.Wout*cfg.CHout, write_filename2);
    
    /***** Compare Bin *****/
    fp_compare_bin_with_bin(dat_out_demaped_inf->bin_data_file, dat_out_demaped_inf->bin_data_size, standard_out_bin_inf->bin_data_file);

    // Free malloc
    free(filepath1);
    filepath1 = NULL;
    free(filepath2);
    filepath2 = NULL;
    free(dat_out);
    dat_out = NULL;
    free(dat_out_HBM);
    dat_out_HBM = NULL;
    free(write_filename2);
    write_filename2 = NULL;
    for(int i=0;i<group;i++)
    {
        free(dat_out_HBM_inf[i]->bin_data_file);
        dat_out_HBM_inf[i]->bin_data_file = NULL;
    }
    free(dat_out_HBM_inf);
    dat_out_HBM_inf = NULL;
    free(dat_out_demaped_inf);
    dat_out_demaped_inf = NULL;
}