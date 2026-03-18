#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

// Tests
#include "./tests/HBM_elementwise_test.cpp"
#include "./tests/HBM_ln_test.cpp"
#include "./tests/HBM_emb_test.cpp"
#include "./tests/HBM_kv2hbm_test.cpp"
#include "./tests/HBM_trp_test.cpp"
#include "./tests/HBM_softmax_test.cpp"
#include "./tests/HBM_f2w_test.cpp"
#include "./tests/HBM_act_minicpm_test.cpp"
#include "./tests/HBM_cnn_test.cpp"
#include "./tests/HBM_mvm_bn_test.cpp"
#include "./tests/HBM_mvm_bn_argmax_test.cpp"

// RW function
#include "./tests/rw_function.cpp"

int __cdecl main()
{
     //****************************** open device ****************************// 
    HANDLE user_device;
    HANDLE bypass_device;
    HANDLE c2hx_device[NUM_OF_RW_CH];
    HANDLE h2cx_device[NUM_OF_RW_CH];   
    open_device(&user_device, &bypass_device, &c2hx_device[0], &h2cx_device[0]);
    //****************************** open device ****************************//

    LARGE_INTEGER start_cfg;
    LARGE_INTEGER stop_cfg;
    LARGE_INTEGER start_run;
    LARGE_INTEGER stop_run;
    LARGE_INTEGER freq;
    double time_sec0;

      //////////////////////////////////////////////////////////////////////////////////////
    // ************************************************************************************ //
    // **********************************   VISION   ************************************** //
    // ************************************************************************************ //
      //////////////////////////////////////////////////////////////////////////////////////

    // Combine output data from FPGA and compare 
    struct bin_inf* mvmbn0_q_dat_in_bin_inf = get_bin_inf(0, 1*621*2304, "./rw_data/minicpm_llm_test/attention/test_q/language_block_0_qkv_proj_input.bin");
    struct bin_inf* mvmbn0_q_weight_bin_inf = get_bin_inf(0, 18432*2304, "./rw_data/minicpm_llm_test/attention/test_q/language_block_w_q_weight_int32_padded.bin"); 
    struct bin_inf* mvmbn0_q_scales_bin_inf = get_bin_inf(0, 18432*18, "./rw_data/minicpm_llm_test/attention/test_q/language_block_q_weight_scales_padded.bin");   

    int *dat_in = (int*)malloc(sizeof(int)*621*2304);
    if (dat_in == NULL){perror("main");return 0;}
    read_bin(mvmbn0_q_dat_in_bin_inf->bin_data_file, dat_in, 621*2304);
    int *weight_in = (int*)malloc(sizeof(int)*18432*2304);
    if (weight_in == NULL){perror("main");return 0;}
    read_bin_32b(mvmbn0_q_weight_bin_inf->bin_data_file, weight_in, 18432*2304);
    int *scales_in = (int*)malloc(sizeof(int)*18432*18);
    if (scales_in == NULL){perror("main");return 0;}
    read_bin(mvmbn0_q_scales_bin_inf->bin_data_file, scales_in, 18432*18);
    
    half *dat_in_half = (half*)malloc(sizeof(half)*621*2304);
    if (dat_in_half == NULL){perror("main");return 0;}
    int *weight_in_CHin = (int*)malloc(sizeof(int)*2304);
    if (weight_in_CHin == NULL){perror("main");return 0;}
    half *scales_in_half= (half*)malloc(sizeof(half)*18432*18);
    if (scales_in_half == NULL){perror("main");return 0;}

    int Token = 545;
    int CHout = 1600;
    for(int i=0; i<2304; i=i+1)
        dat_in_half[i] = *(half*)&dat_in[i+Token*2304];
    for(int i=0; i<2304; i=i+1)
        weight_in_CHin[i] = weight_in[i+CHout*2304];
    for(int i=0; i<18; i=i+1)
        scales_in_half[i] = *(half*)&scales_in[i+CHout*18];

    half *dat_out_in_half1 = (half*)malloc(sizeof(half)*2304);
    half dat_out_in_half2 = (half)0;
    for(int i=0; i<18; i=i+1)
    {
        for(int j=0; j<128; j=j+1)
            dat_out_in_half1[i*128+j] = dat_in_half[i*128+j] * weight_in_CHin[i*128+j] * scales_in_half[i];
    }

    for(int i=0; i<2304; i=i+1)
        dat_out_in_half2 = dat_out_in_half2 + dat_out_in_half1[i];

    printf("result: %f\n", (float)dat_out_in_half2);
}