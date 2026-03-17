//#include "fp_compare.h"
#include <iostream>
#include <iomanip>
#include "./half.hpp"
#include <stdio.h>
#include <string.h>
using namespace std;
using half_float::half;

void uint16_to_half(uint16_t *data_in,  int data_size,  half *half_out){
    for(int i=0; i<data_size; i=i+1)
        half_out[i] = *(half*)&data_in[i];
}

void half_to_uint16(half *half_in,  int data_size,   uint16_t *data_out){
    for(int i=0; i<data_size; i=i+1)
        data_out[i] = *(uint16_t*)&half_in[i];
}

void fp_compare_fp16_with_txt(uint16_t *data_in, int data_size, char * fp16_txt_file){

    half *half_out = (half*) malloc(sizeof(half)*data_size);

    //uint16_to_half(data_in,  data_size,  half_out);
    memcpy(half_out, data_in, data_size);

    FILE *fp_rf;
    if ((fp_rf = fopen(fp16_txt_file, "r")) == NULL)  
        printf("cannot open file: %s !\n", fp16_txt_file);
    
    float rf_data;
    float differ;
    int larger_error_cnt=0;
    for(int i=0;i<data_size;i++)
	{
		fscanf(fp_rf,"%f",&rf_data);
        // differ = rf_data - half_out[i];
        differ = rf_data - *(half*)&data_in[i];
        if(differ>1 || differ<-1){
            //printf("larger error at %d: \t data in = %04x, \t data in half = %f, \t rf data = %f, \t diff = %f \n", i, data_in[i], (float)*(half*)&data_in[i], rf_data, differ);
            larger_error_cnt = larger_error_cnt+1;
        }else{
            //printf("small error at %d: \t data in = %04x, \t data in half = %f, \t rf data = %f, \t diff = %f \n", i, data_in[i], (float)half_out[i], rf_data, differ);
        }
	}
    printf("larger_error_cnt / Total_compare_cnt = %d /%d \n", larger_error_cnt, data_size);
	fclose(fp_rf);
    free(data_in);
    data_in=NULL;
    free(half_out);
    half_out=NULL;
}

void fp_compare_bin_with_txt(char * uint16_bin_file, int data_size, char * fp16_txt_file){

    uint16_t *data_in = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_a;
    if ((fp_a = fopen(uint16_bin_file, "r")) == NULL)  
        printf("cannot open file: %s !\n", uint16_bin_file);
    
    fread(data_in, sizeof(uint16_t), data_size, fp_a);
    fclose(fp_a);

    half *half_out = (half*) malloc(sizeof(half)*data_size);

    uint16_to_half(data_in,  data_size,  half_out);

    FILE *fp_rf;
    if ((fp_rf = fopen(fp16_txt_file, "r")) == NULL)  
        printf("cannot open file: %s !\n", fp16_txt_file);
    
    float rf_data;
    float differ;
    int larger_error_cnt=0;
    for(int i=0;i<data_size;i++)
	{
		fscanf(fp_rf,"%f",&rf_data);
        differ = rf_data - half_out[i];
        if(differ>1 || differ<-1){
            //printf("larger error at %d: \t data in = %04x, \t data in half = %f, \t rf data = %f, \t diff = %f \n", i, data_in[i], (float)half_out[i], rf_data, differ);
            larger_error_cnt = larger_error_cnt+1;
        }else{
            //printf("small error at %d: \t data in = %04x, \t data in half = %f, \t rf data = %f, \t diff = %f \n", i, data_in[i], (float)half_out[i], rf_data, differ);
        }
	}
    printf("larger_error_cnt / Total_compare_cnt = %d /%d \n", larger_error_cnt, data_size);
	fclose(fp_rf);
    free(data_in);
    data_in=NULL;
    free(half_out);
    half_out=NULL;
}

void fp_compare_bin_with_bin(char * uint16_bin_file, int data_size, char * golden_bin_file){

    uint16_t *data_in = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_a;
    if ((fp_a = fopen(uint16_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", uint16_bin_file);
    
    fread(data_in, sizeof(uint16_t), data_size, fp_a);
    fclose(fp_a);

    // half *half_out = (half*) malloc(sizeof(half)*data_size);

    // uint16_to_half(data_in,  data_size,  half_out);
    
    uint16_t *data_golden = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_rf;
    if ((fp_rf = fopen(golden_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", golden_bin_file);
    fread(data_golden, sizeof(uint16_t), data_size, fp_rf);
    fclose(fp_rf);
    
    FILE *output_bin_comapare_log;
    if ((output_bin_comapare_log = fopen("output_bin_compare_fp16.log", "wb")) == NULL)  
        printf("cannot open file: %s !\n", "output_bin_compare_fp16.log");

    float rf_data;
    float differ;
    float deviation_rate;
    int larger_error_cnt=0;
    int larger_error_cnt2=0;
    int data_not_zero_cnt=0;
    // original
    // for(int i=0;i<data_size;i++)
	// { 
    //     differ = *(half*)&data_in[i] - *(half*)&data_golden[i];
    //     deviation_rate = differ*100/(*(half*)&data_golden[i]);
    //     if((deviation_rate>1 || deviation_rate<-1) && ( differ>0.01 || differ<-0.01 ))
    //     {
    //         fprintf(output_bin_comapare_log, "larger error at %d: \t data in = %04x, \tdata in half = %f, \t golden in = %04x, \t golden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
    //         larger_error_cnt = larger_error_cnt+1;
    //     }
    //     else
    //         fprintf(output_bin_comapare_log, "small  error at %d: \t data in = %04x, \tdata in half = %f, \t golden in = %04x, \t golden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
    //     if((deviation_rate>3 || deviation_rate<-3) && ( differ>0.03 || differ<-0.03 ))
    //         larger_error_cnt2 = larger_error_cnt2+1;
    //     if(*(half*)&data_in[i]!=16)
    //         data_not_zero_cnt = data_not_zero_cnt+1;
	// }

    for(int i=0;i<data_size;i++)
	{ 
        differ = *(half*)&data_in[i] - *(half*)&data_golden[i];
        deviation_rate = differ*100/(*(half*)&data_golden[i]);
        if((deviation_rate>1 || deviation_rate<-1) && ( differ>0.01 || differ<-0.01 ))
        {
            fprintf(output_bin_comapare_log, "larger error at %d: \t data in = %04x, \tdata in half = %f, \t golden in = %04x, \t golden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
            larger_error_cnt = larger_error_cnt+1;
        }
        else
            fprintf(output_bin_comapare_log, "small  error at %d: \t data in = %04x, \tdata in half = %f, \t golden in = %04x, \t golden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
        if((deviation_rate>3 || deviation_rate<-3) && ( differ>0.03 || differ<-0.03 ))
            larger_error_cnt2 = larger_error_cnt2+1;
        if(*(half*)&data_in[i]!=16)
            data_not_zero_cnt = data_not_zero_cnt+1;
	}
    printf("0.01_larger_error_cnt / Total_compare_cnt = %d /%d \n", larger_error_cnt, data_size);
    printf("0.03_larger_error_cnt / Total_compare_cnt = %d /%d \n", larger_error_cnt2, data_size);
    printf("data not zero are %d. \n", data_not_zero_cnt);

    fclose(fp_rf);
    free(data_in);
    data_in=NULL;
    free(data_golden);
    data_golden=NULL;
    fclose(output_bin_comapare_log);
}

void bit_compare_bin_with_bin(char * uint16_bin_file, int data_size, char * golden_bin_file){

    uint16_t *data_in = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_a;
    if ((fp_a = fopen(uint16_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", uint16_bin_file);
    
    fread(data_in, sizeof(uint16_t), data_size, fp_a);
    fclose(fp_a);

    // half *half_out = (half*) malloc(sizeof(half)*data_size);

    // uint16_to_half(data_in,  data_size,  half_out);
    
    uint16_t *data_golden = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_rf;
    if ((fp_rf = fopen(golden_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", golden_bin_file);
    fread(data_golden, sizeof(uint16_t), data_size, fp_rf);
    fclose(fp_rf);
    
    FILE *output_bin_comapare_log;
    if ((output_bin_comapare_log = fopen("output_bin_compare_bit.log", "wb")) == NULL)  
        printf("cannot open file: %s !\n", "output_bin_compare_bit.log");

    int different_cnt=0;
    for(int i=0;i<data_size;i++)
	{ 
        if(data_in[i] != data_golden[i])
        {
            fprintf(output_bin_comapare_log, "Diff at %d: \t data in = %04x, \t golden in = %04x \n", i, data_in[i],  data_golden[i]);
            different_cnt = different_cnt+1;
        }
        else
            fprintf(output_bin_comapare_log, "Same at %d: \t data in = %04x, \t golden in = %04x \n", i, data_in[i],  data_golden[i]);
	}
    printf("Different_cnt / Total_compare_cnt = %d /%d \n", different_cnt, data_size);

    fclose(fp_rf);
    free(data_in);
    data_in=NULL;
    free(data_golden);
    data_golden=NULL;
    fclose(output_bin_comapare_log);
}

void bit_compare_fp16_show_bin_with_bin(char * uint16_bin_file, int data_size, char * golden_bin_file){

    uint16_t *data_in = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_a;
    if ((fp_a = fopen(uint16_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", uint16_bin_file);
    
    fread(data_in, sizeof(uint16_t), data_size, fp_a);
    fclose(fp_a);

    // half *half_out = (half*) malloc(sizeof(half)*data_size);

    // uint16_to_half(data_in,  data_size,  half_out);
    
    uint16_t *data_golden = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_rf;
    if ((fp_rf = fopen(golden_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", golden_bin_file);
    fread(data_golden, sizeof(uint16_t), data_size, fp_rf);
    fclose(fp_rf);
    
    FILE *output_bin_comapare_log;
    if ((output_bin_comapare_log = fopen("output_bin_compare_bit_show_fp16.log", "wb")) == NULL)  
        printf("cannot open file: %s !\n", "output_bin_compare_bit_show_fp16.log");

    float rf_data;
    float differ;
    float deviation_rate;
    int different_cnt=0;
    for(int i=0;i<data_size;i++)
	{ 
        differ = *(half*)&data_in[i] - *(half*)&data_golden[i];
        deviation_rate = differ*100/(*(half*)&data_golden[i]);
        if(data_in[i] != data_golden[i])
        {
            fprintf(output_bin_comapare_log, "different at %d: \t data in = %04x, \t data in half = %f, \t golden in = %04x, \tgolden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
            different_cnt = different_cnt+1;
        }
        else
            fprintf(output_bin_comapare_log, "same      at %d: \t data in = %04x, \t data in half = %f, \t golden in = %04x, \tgolden in half = %f, \t diff = %f, \t deviaton rate = %.04f%% \n", i, data_in[i], (float)*(half*)&data_in[i], data_golden[i], (float)*(half*)&data_golden[i], differ, deviation_rate);
	}
    printf("Different_cnt / Total_compare_cnt = %d /%d \n", different_cnt, data_size);

    fclose(fp_rf);
    free(data_in);
    data_in=NULL;
    free(data_golden);
    data_golden=NULL;
    fclose(output_bin_comapare_log);
}

void printf_fp16_log(char * uint16_bin_file, int data_size, char * name){

    uint16_t *data_in = (uint16_t*) malloc(sizeof(uint16_t)*data_size);
    FILE *fp_a;
    if ((fp_a = fopen(uint16_bin_file, "rb")) == NULL)  
        printf("cannot open file: %s !\n", uint16_bin_file);
    
    fread(data_in, sizeof(uint16_t), data_size, fp_a);
    fclose(fp_a);
    
    FILE *output_bin_comapare_log;
    if ((output_bin_comapare_log = fopen(name, "wb")) == NULL)  
        printf("cannot open file: %s !\n", name);

    for(int i=0;i<data_size;i++)
            fprintf(output_bin_comapare_log, "%d: \t data in = %04x, \t data in half = %f \n", i, data_in[i], (float)*(half*)&data_in[i]); 
    printf("Total Id num: %d \n", data_size);

    free(data_in);
    data_in=NULL;
    fclose(output_bin_comapare_log);
}