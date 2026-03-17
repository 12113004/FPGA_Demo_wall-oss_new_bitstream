#include<stdio.h>
#include "xdma_rw.h"

/// <summary>
/// 比较两个文件内容是否相同
/// </summary>
/// <param name="file1">文件1的路径</param>
/// <param name="file2">文件2的路径</param>
/// <returns>是否相同，1相同，0不相同</returns>
int Compare(const char * file1, const char* file2, int* error_cnt)
{
	FILE* f1, * f2;
	int size1, size2;
	error_cnt[0] = 0;
	unsigned char buffer1[1024], buffer2[1024];
	f1 = fopen(file1, "rb+");
	f2 = fopen(file2, "rb+");
	int isTheSame = 1;
	if (!f1){printf("can not open %s file",file1);}
	if (!f2){printf("can not open %s file",file2);}
	while (1)
	{
		size1 = fread(buffer1, 1, 1024, f1);
		size2 = fread(buffer2, 1, 1024, f2);
		if (size1 != size2)
		{
			isTheSame = 0;
		}
		if (size1 < 1)
			break;
		for (int i = 0; i < size1; i++)
		{
			if (buffer1[i] != buffer2[i])
			{
				error_cnt[0]=error_cnt[0]+1;
				//goto end;
				isTheSame = 0;
			}
		}
	}
end:
	if (f1)
		fclose(f1);
	if (f2)
		fclose(f2);
	return isTheSame;
}

void verify_data_write(HANDLE device, struct bin_inf* verify_file_inf){
    
	struct bin_inf* tmp_read_file_inf = get_bin_inf(verify_file_inf->bin_data_to_FPGA_bar, verify_file_inf->bin_data_size, "./tmp_data/read.bin");
    c2hx_device_read_bin(device, tmp_read_file_inf);
	int* error_cnt = (int*)malloc(sizeof(int)*1);
	if (Compare(verify_file_inf->bin_data_file, tmp_read_file_inf->bin_data_file, error_cnt))
    {
    	printf("The read data is the same as the written data: %s \n",verify_file_inf->bin_data_file);
    }
    else
    {
    	printf("Error: the read data is different with the written data: %s \n", verify_file_inf->bin_data_file);
    }
	free(error_cnt);
	error_cnt=NULL;
}

void verify_FPGA_output(struct bin_inf* verify_file_inf, struct bin_inf* golden_file_inf){
    
    int* error_cnt = (int*)malloc(sizeof(int)*1);
	if (Compare(verify_file_inf->bin_data_file, golden_file_inf->bin_data_file, error_cnt))
    {
    	printf("%s == %s \n",verify_file_inf->bin_data_file, golden_file_inf->bin_data_file);
    }
    else
    {
    	printf("Error: %s != %s, error_cnt = %d \n", verify_file_inf->bin_data_file, golden_file_inf->bin_data_file, error_cnt[0]);
    }
	free(error_cnt);
	error_cnt=NULL;
}

