#include "./HBM_KV2HBM/HBM_kv2hbm.h"

void HBM_kv2hbm_test(struct FPGA_HBM_KV2HBM_cfg cfg, char* path_name, char* head_name, struct bin_inf* dat_in_bin_inf, struct bin_inf* *dat_in_HBM_inf, struct bin_inf* *dat_out_software_HBM_inf, int en)
{
    printf("KV2HBM Test \n");

    /***** Read Bin *****/
    // bit [`DAT_DW_in-1:0] dat_in [`Weight_Head][`Hin][`Win][`CHin];
    int **dat_in = (int**)malloc(sizeof(int*)*cfg.Weight_Head);
    for(int i=0;i<cfg.Weight_Head;i++)
    {
        dat_in[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
    }
    read_bin_with_head(dat_in_bin_inf->bin_data_file, dat_in, cfg.Weight_Head, cfg.Hin*cfg.Win*cfg.CHin);

    int **dat_in_HBM = (int**)malloc(sizeof(int*)*group);
    int dat_in_HBM_size = cfg.Weight_Head*cfg.Win*cfg.Hin*cfg.CHin_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    for(int i=0;i<group;i++)
    {
        dat_in_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_HBM[i] == NULL){perror("main");return;}
    }

    int **dat_out_software = (int**)malloc(sizeof(int*)*cfg.Weight_Head);
    int dat_out_software_size = cfg.Win*cfg.Hin*cfg.CHin_div_Tout*AXI_DATA_WIDTH/32;
    for(int i=0;i<cfg.Weight_Head;i++)
    {
        dat_out_software[i] = (int*)malloc(sizeof(int)*dat_out_software_size);
        if(dat_out_software[i] == NULL){perror("main");return;}
    }

    /***** Transform *****/
    if(en) DAT_IN_TRANS_FUNCTION_KV2HBM(cfg, dat_in, dat_in_HBM, dat_out_software);

    /***** Generate Bin*****/
    uint64_t HBM_base_address =0;
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "KV2HBM_HBM_bin");
    if(en)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "dat_in", m);
            FILE *fp=fopen(write_filename,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename);
            	return;
            }
            printf(" generate %s \n", write_filename);
            fwrite(dat_in_HBM[m], sizeof(int), dat_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            dat_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_IN_BASE_ADDR, dat_in_HBM_size*4, write_filename);
        }

        char* write_filename2 = (char*)malloc(sizeof(char)*200);
        sprintf(write_filename2, "%s/%s_%s_golden.bin", filepath, head_name, "dat_out_software");
        FILE *fp=fopen(write_filename2,"wb");
        if(fp==NULL)
        {
        	printf("Can't open file: %s\n",write_filename2);
        	return;
        }
        printf(" generate %s \n", write_filename2);
        for(int m=0; m<cfg.Weight_Head; m++)
            fwrite(dat_out_software[m], sizeof(int), dat_out_software_size, fp);
        fclose(fp);
        dat_out_software_HBM_inf[0] = get_bin_inf(0, 0, write_filename2);
    }

    // Free malloc
    free(filepath);
    filepath = NULL;
	free(dat_in);
	dat_in = NULL;
	free(dat_in_HBM);
	dat_in_HBM = NULL;
	free(dat_out_software);
	dat_out_software = NULL;
}

void HBM_kv2hbm_receive_and_compare(struct FPGA_HBM_KV2HBM_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf, enum KV_Mode KV_Mode)
{
    /***** Generate Bin and Receive Data *****/
    struct bin_inf* *dat_out_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    int dat_out_HBM_size = cfg.Weight_Head*cfg.WT_HEAD_STRIDE/4;
    uint64_t HBM_base_address =0;
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s", path_name, "KV2HBM_FPGA_out_bin/HBM_bin");
        
    for(uint64_t m=0;m<group;m++)
    {
        char* write_filename = (char*)malloc(sizeof(char)*200);
        sprintf(write_filename, "%s/%s_%s_HBM_%02llu.bin", filepath1, head_name, "dat_out", m);
        HBM_base_address = m*(1<<28);
        dat_out_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_OUT_BASE_ADDR, dat_out_HBM_size*4, write_filename);
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

    //bit [`MAX_DAT_DW*`Tout-1:0] software_out [`Weight_Head][`Win*`Hin*`CHin_div_Tout];
    uint16_t **dat_out = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Weight_Head);
    for (int i=0;i<cfg.Weight_Head;i++)
    {
        dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout*MAX_DAT_DW*Tout/16);
        if (dat_out[i] == NULL){perror("main");return;}
    }

    /***** Detransform*****/
    DAT_OUT_TRANS_FUNCTION_KV2HBM(cfg, KV_Mode, dat_out_HBM, dat_out);

    /***** Generate Bin *****/
    struct bin_inf* dat_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename2 = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "KV2HBM_FPGA_out_bin");
    sprintf(write_filename2, "%s/%s_%s.bin", filepath2, head_name, "dat_out_demaped");
    FILE *fp=fopen(write_filename2,"wb");
    if(fp==NULL)
    {
    	printf("Can't open file: %s\n",write_filename2);
    	return;
    }
    printf(" generate %s \n", write_filename2);
    for(int m=0; m<cfg.Weight_Head; m++)
        fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout_div_Tout*MAX_DAT_DW*Tout/16, fp);
    fclose(fp);
    dat_out_demaped_inf = get_bin_inf( 0, cfg.Weight_Head*cfg.Hout*cfg.Wout*cfg.CHout_div_Tout*MAX_DAT_DW*Tout/16, write_filename2);
    
    /***** Compare Bin *****/
    fp_compare_bin_with_bin(dat_out_demaped_inf->bin_data_file, dat_out_demaped_inf->bin_data_size, standard_out_bin_inf->bin_data_file);

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