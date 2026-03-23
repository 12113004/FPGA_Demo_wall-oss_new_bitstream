#include "../HBM_ACT/HBM_act.h"

void HBM_act_test(struct FPGA_HBM_ACT_cfg cfg, char* path_name, char* head_name, struct bin_inf* dat_in_bin_inf, struct bin_inf* dat_in_HBM_inf[], int en_0, struct bin_inf* parameter_in_HBM_inf[], int en_1)
{
    printf("ACT test \n");
    
    /***** Read Bin *****/
    // bit [`DAT_DW_in-1:0] dat_in[`Tb][`Hin][`Win][`CHin];
    int **dat_in = (int**)malloc(sizeof(int*)*Tb);
    for (int i=0;i<Tb;i++) 
    {
        dat_in[i] = (int *)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
        read_bin(dat_in_bin_inf->bin_data_file, dat_in[i], cfg.Hin*cfg.Win*cfg.CHin);
    }

    // bit [`MAX_DAT_DW-1:0] FP16_wt[16];
    int wt[16]       = {0x0000, 0x920a, 0xa015, 0xa66e, 0xaafa, 0xade4, 0x2157, 0x31d2, 0x365e, 0x3902, 0x3ab4, 0x3bee, 0x3c5c, 0x3c23, 0x3c04, 0x3c00};
    // bit [`MAX_DAT_DW-1:0] FP16_bias[16];
    int bias[16]     = {0x0000, 0xa035, 0xabfd, 0xb120, 0xb4aa, 0xb6b8, 0xb432, 0xae6e, 0x9eaf, 0xa1ca, 0xaf61, 0xb43d, 0xb6a0, 0xb292, 0xa85d, 0x0000};
    // bit [`MAX_DAT_DW-1:0] FP16_x_region[16];
    int x_region[16] = {0xca00, 0xc77d, 0xc5b4, 0xc478, 0xc2d9, 0xbe28, 0xbb8d, 0xb6f4, 0x255e, 0x37dd, 0x3bcc, 0x3e15, 0x4386, 0x45aa, 0x4900, 0x0000};   

    int **dat_in_HBM = (int**)malloc(sizeof(int*)*group);
    int dat_in_HBM_size = Tb*cfg.Win*cfg.Hin*cfg.CHin_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    for(int i=0;i<group;i++)
    {
        dat_in_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_HBM[i] == NULL){perror("main");return;}
    }

    int **parameter_in_HBM = (int**)malloc(sizeof(int*)*group/32);
    int parameter_in_HBM_size = 3*HBM_AXI_DATA_WIDTH/32;
    for(int i=0;i<group/32;i++)
    {
        parameter_in_HBM[i] = (int*)malloc(sizeof(int)*parameter_in_HBM_size);
        if (parameter_in_HBM[i] == NULL){perror("main");return;}
    }
    
    /***** Transform *****/
    if(en_0) DAT_IN_TRANS_FUNCTION_ACT(cfg, dat_in, dat_in_HBM);
    if(en_1) PARAMETER_IN_TRANS_FUNCTION_ACT(cfg, wt, bias, x_region, parameter_in_HBM);
    
    /***** Generate Bin*****/
    uint64_t HBM_base_address =0;
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "ACT_HBM_bin");
    if(en_0)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename1 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename1, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "dat_in", m);
            FILE *fp=fopen(write_filename1,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename1);
            	return;
            }
            printf(" generate %s \n", write_filename1);
            fwrite(dat_in_HBM[m], sizeof(int), dat_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            dat_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_IN_BASE_ADDR, dat_in_HBM_size*4, write_filename1);
        }
    }
    if(en_1)
    {
        for(uint64_t m=0;m<group/32;m++)
        {
            char *write_filename2 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename2, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "parameter_in", m);
            FILE *fp=fopen(write_filename2,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename2);
            	return;
            }
            printf(" generate %s \n", write_filename2);
            fwrite(parameter_in_HBM[m], sizeof(int), parameter_in_HBM_size, fp);
            fclose(fp);

            parameter_in_HBM_inf[m] = get_bin_inf(cfg.WT_BASE_ADDR, parameter_in_HBM_size*4, write_filename2);
        }
    }

    // Free malloc
    free(filepath);
    filepath = NULL;
	free(dat_in);
	dat_in = NULL;
	free(dat_in_HBM);
	dat_in_HBM = NULL;
	free(parameter_in_HBM);
	parameter_in_HBM = NULL;
}

void HBM_act_receive_and_compare(struct FPGA_HBM_ACT_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf)
{
    /***** Generate Bin and Receive Data *****/
    struct bin_inf* *dat_out_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    int dat_out_HBM_size = Tb*cfg.Wout*cfg.Hout*cfg.CHout_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    uint64_t HBM_base_address =0;
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s", path_name, "ACT_FPGA_out_bin/HBM_bin");
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
    for (int i=0;i<Tb;i++) {
        dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout);
        if (dat_out[i] == NULL){perror("main");return;}
    }
    
    /***** Detransform*****/
    DAT_OUT_TRANS_FUNCTION_ACT(cfg, dat_out_HBM, dat_out);

    /***** Generate Bin *****/
    struct bin_inf* dat_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename2 = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "ACT_FPGA_out_bin");
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