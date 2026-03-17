void insert_0(struct bin_inf* bin_inf, char *name)
{
    int data[19*32*19];
    uint16_t data_tmp[19*32*32];

    read_bin(bin_inf->bin_data_file, data, 19*32*19);

    for(int i=0; i<19*32; i++)
    {
        for(int j=0;j<19;j++)
            data_tmp[(19+13)*i+j] = (uint16_t)data[19*i+j];
        for(int k=0;k<13;k++)
            data_tmp[(19+13)*i+19+k] = 0;
    }

    //for(int i=0; i<256; i++)
    //    printf("data_tmp[%d]: 0x%04x. \n", i, data_tmp[i]);

    char *write_filename = (char*)malloc(sizeof(char)*200);
    char * filepath="./tests/";
    sprintf(write_filename, "%s%s.bin", filepath, name);
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return;
    }
    printf(" generate %s \n", write_filename);
    fwrite(data_tmp, sizeof(uint16_t), 19*32*32, fp3);
    fclose(fp3);
}

void insert_negative(struct bin_inf* bin_inf, char *name)
{
    int data[19*32*19];
    uint16_t data_tmp[19*32*32];

    read_bin(bin_inf->bin_data_file, data, 19*32*19);

    for(int i=0; i<19*32; i++)
    {
        for(int j=0;j<19;j++)
            data_tmp[(19+13)*i+j] = (uint16_t)data[19*i+j];
        for(int k=0;k<13;k++)
            data_tmp[(19+13)*i+19+k] = 0xfbff;
    }

    //for(int i=0; i<256; i++)
    //    printf("data_tmp[%d]: 0x%04x. \n", i, data_tmp[i]);

    char *write_filename = (char*)malloc(sizeof(char)*200);
    char * filepath="./tests/";
    sprintf(write_filename, "%s%s.bin", filepath, name);
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return;
    }
    printf(" generate %s \n", write_filename);
    fwrite(data_tmp, sizeof(uint16_t), 19*32*32, fp3);
    fclose(fp3);
}