#include "./HBM_ELEMENTWISE/HBM_elementwise.h"

void HBM_elementwise_test(struct FPGA_HBM_ELEMENTWISE_cfg cfg, char* path_name, char* head_name, struct bin_inf* dat_in_A_bin_inf, struct bin_inf* dat_in_B_bin_inf, struct bin_inf* *dat_in_A_HBM_inf, int en_0, struct bin_inf* *dat_in_B_HBM_inf, int en_1)
{
    printf("ELEMENTWISE Test \n");

    half dat_in_half;

    /***** Read Bin *****/
    // bit [`DAT_DW_in-1:0] dat_in_A[`Tb][`Hin][`Win][`CHin];
    int **dat_in_A = (int**)malloc(sizeof(int*)*Tb);
    for(int i=0;i<Tb;i++)
    {
        dat_in_A[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in_A[i] == NULL){perror("main");return;}
        read_bin(dat_in_A_bin_inf->bin_data_file, dat_in_A[i], cfg.Hin*cfg.Win*cfg.CHin);
        //for(int j=0;j<cfg.Win;j++)
        //{
        //    for(int k=0;k<cfg.Hin;k++)
        //    {
        //        for(int m=0;m<cfg.CHin;m++)
        //        {
        //            if(j==0||j==18)
        //            {
        //                dat_in_half = m*0.001;
        //                dat_in_A[i][j*cfg.Hin*cfg.CHin+k*cfg.CHin+m] = *(uint16_t*)&dat_in_half;
        //            }
        //            else
        //            {
        //                dat_in_half = 0;
        //                dat_in_A[i][j*cfg.Hin*cfg.CHin+k*cfg.CHin+m] = *(uint16_t*)&dat_in_half;
        //            }
        //        }
        //    }
        //}
    }
    // bit [`DAT_DW_in-1:0] dat_in_B[`Tb][`Hin][`Win][`CHin];
    int **dat_in_B = (int**)malloc(sizeof(int*)*Tb);
    for(int i=0;i<Tb;i++)
    {
        dat_in_B[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in_B[i] == NULL){perror("main");return;}
        read_bin(dat_in_B_bin_inf->bin_data_file, dat_in_B[i], cfg.Hin*cfg.Win*cfg.CHin);
        //for(int j=0;j<cfg.Win;j++)
        //{
        //    for(int k=0;k<cfg.Hin;k++)
        //    {
        //        for(int m=0;m<cfg.CHin;m++)
        //        {
        //            if(j==0)
        //            {
        //                dat_in_half = m*0.001;
        //                dat_in_B[i][j*cfg.Hin*cfg.CHin+k*cfg.CHin+m] = *(uint16_t*)&dat_in_half;
        //            }
        //            else
        //            {
        //                dat_in_half = 0;
        //                dat_in_B[i][j*cfg.Hin*cfg.CHin+k*cfg.CHin+m] = *(uint16_t*)&dat_in_half;
        //            }
        //        }
        //    }
        //}
    }

    int dat_in_HBM_size = Tb*cfg.Win*cfg.Hin*cfg.CHin_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    int **dat_in_A_HBM = (int**)malloc(sizeof(int*)*group);
    for(int i=0;i<group;i++)
    {
        dat_in_A_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_A_HBM[i] == NULL){perror("main");return;}
    }
    int **dat_in_B_HBM = (int**)malloc(sizeof(int*)*group);
    for(int i=0;i<group;i++)
    {
        dat_in_B_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_B_HBM[i] == NULL){perror("main");return;}
    }

    /***** Transform *****/
    if(en_0) DAT_IN_TRANS_FUNCTION_ELEMENTWISE(cfg, dat_in_A, dat_in_A_HBM);
    if(en_1) DAT_IN_TRANS_FUNCTION_ELEMENTWISE(cfg, dat_in_B, dat_in_B_HBM);

    /***** Generate Bin*****/
    uint64_t HBM_base_address =0;
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "ELEMENTWISE_HBM_bin");
    if(en_0)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename1 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename1, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "dat_in_A", m);
            FILE *fp=fopen(write_filename1,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename1);
            	return;
            }
            printf(" generate %s \n", write_filename1);
            fwrite(dat_in_A_HBM[m], sizeof(int), dat_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            dat_in_A_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_IN_A_BASE_ADDR, dat_in_HBM_size*4, write_filename1);
        }
    }
    if(en_1)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename2 = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename2, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "dat_in_B", m);
            FILE *fp=fopen(write_filename2,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename2);
            	return;
            }
            printf(" generate %s \n", write_filename2);
            fwrite(dat_in_B_HBM[m], sizeof(int), dat_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            dat_in_B_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.DAT_IN_B_BASE_ADDR, dat_in_HBM_size*4, write_filename2);
        }
    }

    // Free malloc
    free(filepath);
    filepath = NULL;
	free(dat_in_A);
	dat_in_A = NULL;
	free(dat_in_B);
	dat_in_B = NULL;
	free(dat_in_A_HBM);
	dat_in_A_HBM = NULL;
	free(dat_in_B_HBM);
	dat_in_B_HBM = NULL;
}

void HBM_elementwise_receive_and_compare(struct FPGA_HBM_ELEMENTWISE_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf)
{
    /***** Generate Bin and Receive Data *****/
    struct bin_inf* *dat_out_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    int dat_out_HBM_size = Tb*cfg.Wout*cfg.Hout*cfg.CHout_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    uint64_t HBM_base_address =0;
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s", path_name, "ELEMENTWISE_FPGA_out_bin/HBM_bin");
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
    DAT_OUT_TRANS_FUNCTION_ELEMENTWISE(cfg, dat_out_HBM, dat_out);

    /***** Generate Bin *****/
    struct bin_inf* dat_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename2 = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "ELEMENTWISE_FPGA_out_bin");
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