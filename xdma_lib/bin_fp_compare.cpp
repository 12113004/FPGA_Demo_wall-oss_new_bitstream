#include <iostream>
#include <iomanip>
// #include "./half.hpp"
#include "./xdma_rw.h"
#include <stdio.h>
void bin_to_fp (struct bin_inf* input_bin_inf){

    half* read_half_fp = (half*)malloc(sizeof(half)*input_bin_inf->bin_data_size/2);;

    FILE *bin_file;
    if ((bin_file = fopen(input_bin_inf->bin_data_file, "r")) == NULL)
    { 
        printf("cannot open file: %s !\n", input_bin_inf->bin_data_file);
    }
    fread(read_half_fp, sizeof(half), input_bin_inf->bin_data_size/2, bin_file);
    fclose(bin_file);

    FILE *fp_file;
    char* fp_file_path="./tmp_data/fp_tmp.txt";
    if ((fp_file = fopen(fp_file_path, "w")) == NULL)
    { 
        printf("cannot open file: %s !\n", fp_file_path);
    }
    for(int i=0; i<input_bin_inf->bin_data_size/2;i++){
        fprintf(fp_file, "%f\n", float(read_half_fp[i]));
    }
    fclose(fp_file);
}