void read_txt(char* txt_file, int *data_in, int size){
    float *txt_data = (float*)malloc(sizeof(float)*1);
    FILE *fp1;
	fp1=fopen(txt_file, "r");
	if (fp1==NULL)
	{
		printf("Can't open %s \n", txt_file);
		exit(0);
	}
	for(int i=0;i<size;i++)
	{
		fscanf(fp1,"%f",&txt_data[0]);

        data_in[i] = (int) txt_data[0];

	}
	fclose(fp1);
}

void read_bin(char* bin_file, int *data_in, int size){
    
	FILE *fp1 = fopen(bin_file,"rb");
    if(fp1 == NULL)
    {
    	printf("Can't open file: %s\n",bin_file);
    	//return 0;
    }
    
	uint16_t *u16_data_in = (uint16_t*)malloc(sizeof(uint16_t)*size);;

    fread(u16_data_in, sizeof(uint16_t), size, fp1);

	for(int i=0;i<size;i++)
	{
        data_in[i] = (int)u16_data_in[i];

		//printf("data_in[%d] = %x , u16_data_in[%d] = %x \n", i, data_in[i], i, u16_data_in[i]);
	}
    free(u16_data_in); 
	u16_data_in = NULL;
    fclose(fp1);
}

void read_bin_with_head(char* bin_file, int *data_in[], int head, int size){
    
	FILE *fp1 = fopen(bin_file,"rb");
    if(fp1 == NULL)
    {
    	printf("Can't open file: %s\n",bin_file);
    	//return 0;
    }
    
	uint16_t *u16_data_in = (uint16_t*)malloc(sizeof(uint16_t)*head*size);;

    fread(u16_data_in, sizeof(uint16_t), head*size, fp1);

	for(int i=0;i<head;i++)
	{
		for(int j=0;j<size;j++)
			data_in[i][j] = (int)u16_data_in[i*size+j];
	}
    free(u16_data_in); 
	u16_data_in = NULL;
    fclose(fp1);
}

void read_bin_32b(char* bin_file, int *data_in, int size){
    
	FILE *fp1=fopen(bin_file,"rb");
    if(fp1==NULL)
    {
    	printf("Can't open file: %s\n",bin_file);
    	//return 0;
    }
    
	int *data_in_tmp=(int*)malloc(sizeof(int)*size);;


    fread(data_in_tmp, sizeof(int), size, fp1);

	for(int i=0;i<size;i++)
	{
        data_in[i] = data_in_tmp[i];

		//printf("data_in[%d] = %x , u16_data_in[%d] = %x \n", i, data_in[i], i, u16_data_in[i]);
	}
    free(data_in_tmp); 
	data_in_tmp = NULL;
    fclose(fp1);
}