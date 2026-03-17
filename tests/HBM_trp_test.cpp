#include "./HBM_TRP/HBM_trp.h"

void HBM_trp_test(struct FPGA_HBM_TRP_cfg cfg, char* path_name, char* head_name, struct bin_inf* dat_in_bin_inf, struct bin_inf* wt_in_bin_inf, struct bin_inf* *dat_in_HBM_inf, int en_0, struct bin_inf* *wt_in_HBM_inf, int en_1)
{
    printf("TRP Test \n");

    /***** Read Bin *****/
    // bit [`DAT_DW_in-1:0] dat_in [`Original_Feature_Head][`Hin][`Win][`CHin];
    int **dat_in = (int**)malloc(sizeof(int*)*cfg.Original_Feature_Head);
    for(int i=0;i<cfg.Original_Feature_Head;i++)
    {
        dat_in[i] = (int*)malloc(sizeof(int)*cfg.Hin*cfg.Win*cfg.CHin);
        if (dat_in[i] == NULL){perror("main");return;}
    }
    read_bin_with_head(dat_in_bin_inf->bin_data_file, dat_in, cfg.Original_Feature_Head, cfg.Hin*cfg.Win*cfg.CHin);
    
    // bit [`DAT_DW_in-1:0] wt_in [`Weight_Head][`WT_CHin][`WT_CHout];
    int **wt_in = (int**)malloc(sizeof(int*)*cfg.Weight_Head);
    for(int i=0;i<cfg.Weight_Head;i++)
    {
        wt_in[i] = (int*)malloc(sizeof(int)*cfg.WT_CHin*cfg.WT_CHout);
        if (wt_in[i] == NULL){perror("main");return;}
    }
    read_bin_with_head(wt_in_bin_inf->bin_data_file, wt_in, cfg.Weight_Head, cfg.WT_CHin*cfg.WT_CHout);

    int **dat_in_HBM = (int**)malloc(sizeof(int*)*group);
    int dat_in_HBM_size = cfg.Weight_Head*cfg.Win*cfg.Hin*cfg.Head_x_CHin_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    for(int i=0;i<group;i++)
    {
        dat_in_HBM[i] = (int*)malloc(sizeof(int)*dat_in_HBM_size);
        if (dat_in_HBM[i] == NULL){perror("main");return;}
    }

    int **wt_in_HBM = (int**)malloc(sizeof(int*)*group);
    int wt_in_HBM_size = cfg.Weight_Head*cfg.WT_HEAD_STRIDE/4;
    for(int i=0;i<group;i++)
    {
        wt_in_HBM[i] = (int*)malloc(sizeof(int)*wt_in_HBM_size);
        if (wt_in_HBM[i] == NULL){perror("main");return;}
    }
    
    // for(int i=0;i<cfg.Original_Feature_Head;i++)
    // {
    //     for(int j=0;j<cfg.Hin;j++)
    //     {
    //         for(int k=0;k<cfg.Win;k++)
    //         {
    //             for(int m=0;m<cfg.CHin;m++)
    //             {
    //                 //if((m<32) || ((m>63) && (m<96)))
    //                 //    dat_in[i][j*cfg.Win*cfg.CHin+k*cfg.CHin+m] = 0x3c00;
    //                 //else
    //                 //    dat_in[i][j*cfg.Win*cfg.CHin+k*cfg.CHin+m] = 0x0000;
    //                 // if(m<32) 
    //                 //     dat_in[i][j*cfg.Win*cfg.CHin+k*cfg.CHin+m] = 0x3c00;
    //                 // else 
    //                     dat_in[i][j*cfg.Win*cfg.CHin+k*cfg.CHin+m] = 0x3c00;
    //             }
    //         }
    //     }
    // }
    // for(int i=0;i<cfg.Weight_Head;i++)
    // {
    //     for(int j=0;j<cfg.WT_CHout;j++)
    //     {
    //         for(int k=0;k<cfg.WT_CHin;k++)
    //         {
    //             // if(i==0)
    //             //     wt_in[i][k*cfg.WT_CHout+j] = 0x3c00;
    //             // else if(i==1)
    //             //     wt_in[i][k*cfg.WT_CHout+j] = 0x4000;
    //             // else if(i==2)
    //             //     wt_in[i][k*cfg.WT_CHout+j] = 0x4200;
    //             // else if(i==3)
    //             //     wt_in[i][k*cfg.WT_CHout+j] = 0x4400;
    //             // else if(i==4)
    //             //     wt_in[i][k*cfg.WT_CHout+j] = 0x4600;    
    //             // else
    //                 wt_in[i][k*cfg.WT_CHout+j] = 0x3c00;            
    //         }
    //     }
    // }

    /***** Transform *****/
    if(en_0) DAT_IN_TRANS_FUNCTION_TRP(cfg, dat_in, dat_in_HBM);
    if(en_1) WT_IN_TRANS_FUNCTION_TRP(cfg, wt_in, wt_in_HBM);

    /***** Generate Bin*****/
    uint64_t HBM_base_address =0;
    char* filepath = (char*)malloc(sizeof(char)*200);
    sprintf(filepath, "./%s/%s", path_name, "TRP_HBM_bin");
    if(en_0)
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
    }
    if(en_1)
    {
        for(uint64_t m=0;m<group;m++)
        {
            char* write_filename = (char*)malloc(sizeof(char)*200);
            sprintf(write_filename, "%s/%s_%s_HBM_%02llu.bin", filepath, head_name, "wt_in", m);
            FILE *fp=fopen(write_filename,"wb");
            if(fp==NULL)
            {
            	printf("Can't open file: %s\n",write_filename);
            	return;
            }
            printf(" generate %s \n", write_filename);
            fwrite(wt_in_HBM[m], sizeof(int), wt_in_HBM_size, fp);
            fclose(fp);

            HBM_base_address = m*(1<<28);
            wt_in_HBM_inf[m] = get_bin_inf(HBM_base_address+cfg.WT_BASE_ADDR, wt_in_HBM_size*4, write_filename);
        }
    }

    // Free malloc
    free(filepath);
    filepath = NULL;
	free(dat_in);
	dat_in = NULL;
	free(wt_in);
	wt_in = NULL;
	free(dat_in_HBM);
	dat_in_HBM = NULL;
	free(wt_in_HBM);
	wt_in_HBM = NULL;
}

void HBM_trp_receive_and_compare(struct FPGA_HBM_TRP_cfg cfg, HANDLE c2hx_device, char* path_name, char* head_name, struct bin_inf* standard_out_bin_inf)
{
    /***** Generate Bin and Receive Data *****/
    struct bin_inf* *dat_out_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    int dat_out_HBM_size = cfg.Original_Feature_Head*cfg.Wout*cfg.Hout*cfg.CHout_div_LTout*MAX_DAT_DW*L_Tout/32/group;
    uint64_t HBM_base_address =0;
    char* filepath1 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath1, "./%s/%s", path_name, "TRP_FPGA_out_bin/HBM_bin");
        
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

    // bit [`DAT_DW_out-1:0] dat_out [`Original_Feature_Head][`Hout][`Wout][`CHout];
    uint16_t **dat_out = (uint16_t**)malloc(sizeof(uint16_t*)*cfg.Original_Feature_Head);
    for (int i=0;i<cfg.Original_Feature_Head;i++) {
        dat_out[i] = (uint16_t*)malloc(sizeof(uint16_t)*cfg.Hout*cfg.Wout*cfg.CHout);
        if (dat_out[i] == NULL){perror("main");return;}
    }
    
    /***** Detransform*****/
    DAT_OUT_TRANS_FUNCTION_TRP(cfg, dat_out_HBM, dat_out);

    /***** Generate Bin *****/
    struct bin_inf* dat_out_demaped_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    char* write_filename2 = (char*)malloc(sizeof(char)*200);
    char* filepath2 = (char*)malloc(sizeof(char)*200);
    sprintf(filepath2, "./%s/%s", path_name, "TRP_FPGA_out_bin");
    sprintf(write_filename2, "%s/%s_%s.bin", filepath2, head_name, "dat_out_demaped");
    FILE *fp=fopen(write_filename2,"wb");
    if(fp==NULL)
    {
    	printf("Can't open file: %s\n",write_filename2);
    	return;
    }
    printf(" generate %s \n", write_filename2);
    for(int m=0; m<cfg.Original_Feature_Head; m++)
        fwrite(dat_out[m], sizeof(uint16_t), cfg.Hout*cfg.Wout*cfg.CHout, fp);
    fclose(fp);
    dat_out_demaped_inf = get_bin_inf( 0, cfg.Original_Feature_Head*cfg.Hout*cfg.Wout*cfg.CHout, write_filename2);
    
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