#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

// #define BLOCK_0
// #define BLOCK_1
// #define BLOCK_2
// #define BLOCK_3
// #define BLOCK_4
// #define BLOCK_5
// #define BLOCK_6
// #define BLOCK_7
// #define BLOCK_8
// #define BLOCK_9
// #define BLOCK_10
// #define BLOCK_11
// #define BLOCK_12
// #define BLOCK_13
// #define BLOCK_14
// #define BLOCK_15
// #define BLOCK_16
// #define BLOCK_17
// #define BLOCK_18
// #define BLOCK_19
// #define BLOCK_20
// #define BLOCK_21
// #define BLOCK_22
// #define BLOCK_23
// #define BLOCK_24
// #define BLOCK_25
// #define BLOCK_26
// #define BLOCK_27
// #define BLOCK_28
// #define BLOCK_29
// #define BLOCK_30
// #define BLOCK_31

// 全局变量 index
int index;

// 2. 根据宏定义给 index 赋值
void set_block_index(void)
{
    #ifdef BLOCK_0
    index = 0;
    #elif defined(BLOCK_1)
    index = 1;
    #elif defined(BLOCK_2)
    index = 2;
    #elif defined(BLOCK_3)
    index = 3;
    #elif defined(BLOCK_4)
    index = 4;
    #elif defined(BLOCK_5)
    index = 5;
    #elif defined(BLOCK_6)
    index = 6;
    #elif defined(BLOCK_7)
    index = 7;
    #elif defined(BLOCK_8)
    index = 8;
    #elif defined(BLOCK_9)
    index = 9;
    #elif defined(BLOCK_10)
    index = 10;
    #elif defined(BLOCK_11)
    index = 11;
    #elif defined(BLOCK_12)
    index = 12;
    #elif defined(BLOCK_13)
    index = 13;
    #elif defined(BLOCK_14)
    index = 14;
    #elif defined(BLOCK_15)
    index = 15;
    #elif defined(BLOCK_16)
    index = 16;
    #elif defined(BLOCK_17)
    index = 17;
    #elif defined(BLOCK_18)
    index = 18;
    #elif defined(BLOCK_19)
    index = 19;
    #elif defined(BLOCK_20)
    index = 20;
    #elif defined(BLOCK_21)
    index = 21;
    #elif defined(BLOCK_22)
    index = 22;
    #elif defined(BLOCK_23)
    index = 23;
    #elif defined(BLOCK_24)
    index = 24;
    #elif defined(BLOCK_25)
    index = 25;
    #elif defined(BLOCK_26)
    index = 26;
    #elif defined(BLOCK_27)
    index = 27;
    #elif defined(BLOCK_28)
    index = 28;
    #elif defined(BLOCK_29)
    index = 29;
    #elif defined(BLOCK_30)
    index = 30;
    #elif defined(BLOCK_31)
    index = 31;
    #else
    // 无任何 BLOCK 定义时的默认值
    index = -1;
    #endif
}

// Read and write command 
// #include "./rw_cmd/qwen3_block_ohbm_test_tb_cmd.h"
// #include "./rw_cmd/qwen3_block_ohbm_test_1124_1554.h"
// #include "./rw_cmd/wall_oss_debug_0317_1602.h"
// #include "./rw_cmd/wall_oss_debug_0320_1810.h"
// #include "./rw_cmd/wall_oss_debug_0321_1359.h"
// #include "./rw_cmd/wall_oss_debug_0321_1447.h"
// #include "./rw_cmd/wall_oss_debug_0321_1724.h"                      // ATTEN_DONE
// #include "./rw_cmd/wall_oss_debug_0322_1429.h"                      // working with O input head = 16
// #include "./rw_cmd/wall_oss_debug_0322_1602.h"                      // MLP
// #include "./rw_cmd/wall_oss_debug_0322_2204.h"                      // 0_test_pass_version
// #include "./rw_cmd/wall_oss_debug_0324_0005.h"                      // muti-one-pass
#include "./rw_cmd/wall_oss_block_all_0324_1019.h"                  // muti-ViT_block

// Tests
#include "./tests/HBM_elementwise_test.cpp"
#include "./tests/HBM_ln_test.cpp"
#include "./tests/HBM_ln_test_inout_head.cpp"
#include "./tests/HBM_emb_test_qwen.cpp"
#include "./tests/HBM_kv2hbm_test.cpp"
#include "./tests/HBM_trp_test.cpp"
#include "./tests/HBM_softmax_test.cpp"
#include "./tests/HBM_f2w_test.cpp"
#include "./tests/HBM_act_test.cpp"
#include "./tests/HBM_mvm_bn_test.cpp"
#include "./tests/HBM_mvm_bn_argmax_test.cpp"

// RW function
#include "./tests/rw_function.cpp"

// step switch

#define STEP_LN0
// #define STEP_MVMBN0_Q
// #define STEP_EMBQ
// #define STEP_MVMBN0_K
// #define STEP_EMBK
// #define STEP_KV2HBMK_K
// #define STEP_TRP
// #define STEP_MASK
// #define STEP_SOFTMAX
// #define STEP_MVMBN0_V
// #define STEP_KV2HBM_V
// #define STEP_F2W
// #define STEP_MVMBN1
// #define STEP_ELEMENTWISE0

// #define STEP_LN1
// #define STEP_MVMBN2
// #define STEP_ACT
// #define STEP_MVMBN3
// #define STEP_ELEMENTWISE1
// #define STEP_MVMBN4
#define STEP_ELEMENTWISE2
// #define STEP_LN_Outlayer
// #define STEP_MVMBN_Argmax
// #define test_BLOCK00

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
    
    
    int run_token    = 648;
    int last_token   = 0;
    int hidden_dim   = 1280;
    int kvcache_mode = (run_token-last_token == 1)? 1 : 0;

    char** filename = (char**)malloc(sizeof(char*)*4);
    for(int i=0;i<4;i++)
        filename[i] = (char*)malloc(sizeof(char)*200);
    set_block_index();

    // test_load_params(h2cx_device[0], ".");

#ifdef STEP_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*LN_WT_BASE_ADDR*/ hbm2, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    sprintf(filename[0],  "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/input.bin", index, index);
    printf("%s\n", filename[0]);
    struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  filename[0]);
    struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm1/weight.bin");
    struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_ln0, "wall_oss_run/blocks_0", "LN0", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_dat_in_HBM_inf, group);

    test(user_device, run_token, last_token);

#endif

#ifdef STEP_ELEMENTWISE2
    // ******************************** STEP22 ELEMENTWISE2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280,
        /*DAT_IN_A_BASE_ADDR*/ runtime0, /*DAT_IN_B_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    sprintf(filename[0],  "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/input.bin", index+1, index+1);
    printf("%s\n", filename[0]);
    // Read output data from FPGA and compare
    struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0,         filename[0]);
    HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "wall_oss_run/blocks_0", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);

    // Malloc free
    // bin_inf_malloc_free(elementwise2_dat_in_A_bin_inf);
    // bin_inf_malloc_free(elementwise2_dat_in_B_bin_inf);
    bin_inf_malloc_free(elementwise2_golden_out_bin_inf);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_A_HBM_inf, group);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_B_HBM_inf, group);
#endif 

#ifdef STEP_LN_Outlayer
   // ******************************** STEP23 - LN_Outlayer ******************************** //
   // Parameter Config
   struct FPGA_HBM_LN_cfg cfg_ln_outlayer_input = GetFPGA_HBM_LN_cfg(
       /*Height*/ 19, /*Hin*/ 1, /*Width_in*/ 4096,
       /*DAT_IN_BASE_ADDR*/ runtime0, /*LN_WT_BASE_ADDR*/ hbm18, /*DAT_OUT_BASE_ADDR*/ runtime4
   );
   struct FPGA_HBM_LN_cfg cfg_ln_outlayer_output = GetFPGA_HBM_LN_cfg(
       /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096,
       /*DAT_IN_BASE_ADDR*/ runtime0, /*LN_WT_BASE_ADDR*/ hbm18, /*DAT_OUT_BASE_ADDR*/ runtime4
   );

   // Input bin_inf
   struct bin_inf* ln_outlayer_dat_in_bin_inf   = get_bin_inf(0, 19*1*4096, "./random_test/1226_version/LN/input_token19_ln_outlayer_test_demaped.bin");
   struct bin_inf* ln_outlayer_weight_bin_inf   = get_bin_inf(0, 1*4096, "./rw_data/chatglm2_minmax_initial/output_layer/final_layernorm_LN_k.bin");
   struct bin_inf* ln_outlayer_bias_bin_inf     = get_bin_inf(0, 4096, "./rw_data/bn_and_k_bias_0.bin");
   // Output bin_inf
   struct bin_inf* *ln_outlayer_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
   struct bin_inf* *ln_outlayer_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

   // Transform data
   HBM_ln_test(cfg_ln_outlayer_input, "BLOCK_write_data/OutLayer", "LN_Outlayer", ln_outlayer_dat_in_bin_inf, ln_outlayer_weight_bin_inf, ln_outlayer_bias_bin_inf, ln_outlayer_dat_in_HBM_inf, ENABLE, ln_outlayer_ln_wt_and_bias_HBM_inf, ENABLE);

   // Write data to FPGA
   HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln_outlayer_dat_in_HBM_inf, group);
   HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln_outlayer_ln_wt_and_bias_HBM_inf, group);

   // Write command to FPGA
   norm_step_21(user_device, run_token);

   // Read output data from FPGA and compare
   struct bin_inf* ln_outlayer_golden_out_bin_inf = get_bin_inf(0, 0, "./random_test/1226_version/LN/output_token19_ln_outlayer_test_demaped.bin");
   HBM_ln_receive_and_compare(cfg_ln_outlayer_output, c2hx_device[0], "BLOCK_read_data", "LN_Outlayer", ln_outlayer_golden_out_bin_inf);

   // Malloc free
   bin_inf_malloc_free(ln_outlayer_dat_in_bin_inf);
   bin_inf_malloc_free(ln_outlayer_weight_bin_inf);
   bin_inf_malloc_free(ln_outlayer_bias_bin_inf);
   bin_inf_malloc_free(ln_outlayer_golden_out_bin_inf );
   HBM_bin_inf_malloc_free(ln_outlayer_dat_in_HBM_inf, group);
   HBM_bin_inf_malloc_free(ln_outlayer_ln_wt_and_bias_HBM_inf, group);
#endif 

#ifdef STEP_MVMBN_Argmax
    // ******************************** STEP24 - MVMBN_Argmax ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn_argmax_0 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096, /*Width_out*/ 37984/*151936/4*/,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*HBM00_WT_BASE_ADDR*/ hbm651, /*BN_BASE_ADDR*/ hbm652, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn_argmax_1 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096, /*Width_out*/ 37984/*151936/2*/,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*HBM00_WT_BASE_ADDR*/ hbm651+2506944, /*BN_BASE_ADDR*/ hbm652, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn_argmax_2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096, /*Width_out*/ 37984/*151936/4*/,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*HBM00_WT_BASE_ADDR*/ hbm651+2506944*2, /*BN_BASE_ADDR*/ hbm652, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn_argmax_3 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096, /*Width_out*/ 37984/*151936/2*/,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*HBM00_WT_BASE_ADDR*/ hbm651+2506944*3, /*BN_BASE_ADDR*/ hbm652, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn_argmax = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 1, /*Hin*/ 1, /*Width_in*/ 4096, /*Width_out*/ 151936,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*HBM00_WT_BASE_ADDR*/ hbm651, /*BN_BASE_ADDR*/ hbm652, /*DAT_OUT_BASE_ADDR*/ runtime1
    );

    // Input bin_inf
    struct bin_inf* mvmbn_argmax_dat_in_bin_inf   = get_bin_inf(0, 1*22*4096, "./qwen3_data/Qwen3_xiao/test_LN0/LN0_input.bin");
    struct bin_inf* mvmbn_argmax_0_weight_bin_inf = get_bin_inf(0, 37984*4096, "./rw_data/qwen3_all_weights/lm_head.weights_1.bin"); 
    struct bin_inf* mvmbn_argmax_0_scales_bin_inf = get_bin_inf(0, 37984*32, "./rw_data/qwen3_all_weights/lm_head.scales_1.bin");
    struct bin_inf* mvmbn_argmax_1_weight_bin_inf = get_bin_inf(0, 37984*4096, "./rw_data/qwen3_all_weights/lm_head.weights_2.bin"); 
    struct bin_inf* mvmbn_argmax_1_scales_bin_inf = get_bin_inf(0, 37984*32, "./rw_data/qwen3_all_weights/lm_head.scales_2.bin");
    struct bin_inf* mvmbn_argmax_2_weight_bin_inf = get_bin_inf(0, 37984*4096, "./rw_data/qwen3_all_weights/lm_head.weights_3.bin"); 
    struct bin_inf* mvmbn_argmax_2_scales_bin_inf = get_bin_inf(0, 37984*32, "./rw_data/qwen3_all_weights/lm_head.scales_3.bin");
    struct bin_inf* mvmbn_argmax_3_weight_bin_inf = get_bin_inf(0, 37984*4096, "./rw_data/qwen3_all_weights/lm_head.weights_4.bin"); 
    struct bin_inf* mvmbn_argmax_3_scales_bin_inf = get_bin_inf(0, 37984*32, "./rw_data/qwen3_all_weights/lm_head.scales_4.bin");
    struct bin_inf* mvmbn_argmax_wt_bin_inf       = get_bin_inf(0, 151936, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn_argmax_bias_bin_inf     = get_bin_inf(0, 151936, "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn_argmax_0_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn_argmax_1_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn_argmax_2_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn_argmax_3_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn_argmax_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn_argmax_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_data_and_bn_test(cfg_mvmbn_argmax, "BLOCK_write_data_qwen3/BLOCK_OutLayer_test", "MVMBN_Argmax", mvmbn_argmax_dat_in_bin_inf, mvmbn_argmax_wt_bin_inf, mvmbn_argmax_bias_bin_inf,  
                mvmbn_argmax_dat_in_HBM_inf, ENABLE, mvmbn_argmax_bn_wt_and_bias_in_HBM_inf, ENABLE);
    HBM_mvmbn_wt_test(cfg_mvmbn_argmax_0, "BLOCK_write_data_qwen3/BLOCK_OutLayer_test", "MVMBN_Argmax_0", mvmbn_argmax_0_weight_bin_inf, mvmbn_argmax_0_scales_bin_inf,
                mvmbn_argmax_0_wt_and_scale_in_HBM_inf, ENABLE);
    HBM_mvmbn_wt_test(cfg_mvmbn_argmax_1, "BLOCK_write_data_qwen3/BLOCK_OutLayer_test", "MVMBN_Argmax_1", mvmbn_argmax_1_weight_bin_inf, mvmbn_argmax_1_scales_bin_inf, 
                mvmbn_argmax_1_wt_and_scale_in_HBM_inf, ENABLE);
    HBM_mvmbn_wt_test(cfg_mvmbn_argmax_2, "BLOCK_write_data_qwen3/BLOCK_OutLayer_test", "MVMBN_Argmax_2", mvmbn_argmax_2_weight_bin_inf, mvmbn_argmax_2_scales_bin_inf,
                mvmbn_argmax_2_wt_and_scale_in_HBM_inf, ENABLE);
    HBM_mvmbn_wt_test(cfg_mvmbn_argmax_3, "BLOCK_write_data_qwen3/BLOCK_OutLayer_test", "MVMBN_Argmax_3", mvmbn_argmax_3_weight_bin_inf, mvmbn_argmax_3_scales_bin_inf, 
                mvmbn_argmax_3_wt_and_scale_in_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_0_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_1_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_3_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn_argmax_bn_wt_and_bias_in_HBM_inf, group);

    // Malloc free
    bin_inf_malloc_free(mvmbn_argmax_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_0_weight_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_0_scales_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_1_weight_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_1_scales_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_2_weight_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_2_scales_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_3_weight_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_3_scales_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_wt_bin_inf);
    bin_inf_malloc_free(mvmbn_argmax_bias_bin_inf);
    HBM_bin_inf_malloc_free(mvmbn_argmax_0_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn_argmax_1_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn_argmax_2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn_argmax_3_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn_argmax_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_794(user_device);

    // Read output data from FPGA and compare
    HBM_mvmbn_argmax_receive_and_find_ID(cfg_mvmbn_argmax, c2hx_device[0], "BLOCK_read_data", "MVMBN_Argmax");
 
    // Read ID from FPGA and compare
    printf("ID = %d\n", CSB_Read(user_device, 40));
#endif

#ifdef test_BLOCK00
   test_load_params(h2cx_device[0], "BLOCK_write_data");

   // Parameter Config
   struct FPGA_HBM_LN_cfg cfg_ln0 = GetFPGA_HBM_LN_cfg(
      /*Height*/ 22, /*Hin*/ 1, /*Width_in*/ 4096,
      /*DAT_IN_BASE_ADDR*/ runtime0, /*LN_WT_BASE_ADDR*/ hbm0, /*DAT_OUT_BASE_ADDR*/ runtime1
   );
   // Input bin_inf
   struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_LN0/LN0_input.bin");
   struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*4096, "./qwen3_data/Qwen3_xiao/test_LN0/LN0_weights.bin");
   struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, 4096, "./rw_data/bn_and_k_bias_0.bin");
   // Output bin_inf
   struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
   struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

   // Transform data
   HBM_ln_test(cfg_ln0, "BLOCK_write_data/BLOCK00", "LN0", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

   // Write data to FPGA
   HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_dat_in_HBM_inf, group);

   // start test BLOCK00
   test(user_device, run_token, last_token);

   // Parameter Config
   struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
      /*Height*/ 22, /*Hin*/ 1, /*Width_in*/ 4096,
      /*DAT_IN_A_BASE_ADDR*/ runtime2, /*DAT_IN_B_BASE_ADDR*/ runtime1, /*DAT_OUT_BASE_ADDR*/ runtime0
   );
   // Read output data from FPGA and compare
   struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE2/ELEMENTWISE2_output.bin");
   HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "BLOCK_read_data", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);

   bin_inf_malloc_free(elementwise2_golden_out_bin_inf);
   bin_inf_malloc_free(ln0_dat_in_bin_inf);
   bin_inf_malloc_free(ln0_weight_bin_inf);
   bin_inf_malloc_free(ln0_bias_bin_inf);
   HBM_bin_inf_malloc_free(ln0_dat_in_HBM_inf, group);
   HBM_bin_inf_malloc_free(ln0_ln_wt_and_bias_HBM_inf, group);
#endif

}