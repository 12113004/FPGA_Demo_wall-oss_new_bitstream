
void split_half(struct bin_inf* bin_inf)
{
    int* data = (int*)malloc(sizeof(int)*19*1*13696*2);
    read_bin(bin_inf->bin_data_file, data, 19*1*13696*2);

    uint16_t* data_1 = (uint16_t*)malloc(sizeof(uint16_t)*19*1*13696);
    uint16_t* data_2 = (uint16_t*)malloc(sizeof(uint16_t)*19*1*13696);

    for(int i=0; i<19*1*13696*2; i++)
    {
        if(i<19*1*13696)
            data_1[i] = (uint16_t)data[i];
        else
        {
            data_2[i-19*1*13696] = (uint16_t)data[i];
            //printf("data_2[%d]: 0x%04x ; data[%d]: 0x%04x. \n", i-19*1*13696, data_2[i-19*1*13696], i, data[i]);
        }
    }

    char * filepath="./00_ACT_DDR_bin/";
    char *write_filename = (char*)malloc(sizeof(char)*200);
    sprintf(write_filename, "%s%s.bin", filepath, "Dense_4h_output_first_part");
    FILE *fp3=fopen(write_filename,"wb");
    if(fp3==NULL)
    {
    	printf("Can't open file: %s\n",write_filename);
    	return 0;
    }
    printf(" generate %s \n", write_filename);
    fwrite(data_1, sizeof(uint16_t), 19*1*13696, fp3);
    fclose(fp3);

    char *write_filename2 = (char*)malloc(sizeof(char)*200);
    sprintf(write_filename2, "%s%s.bin", filepath, "Dense_4h_output_second_part");
    FILE *fp4=fopen(write_filename2,"wb");
    if(fp4==NULL)
    {
    	printf("Can't open file: %s\n",write_filename2);
    	return 0;
    }
    printf(" generate %s \n", write_filename2);
    fwrite(data_2, sizeof(uint16_t), 19*1*13696, fp4);
    fclose(fp4);
}