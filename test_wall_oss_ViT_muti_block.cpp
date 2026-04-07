#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

// #define LOAD_WEIGHT

// #define BLOCK_EMB

#define BLOCK_0
#define BLOCK_1
#define BLOCK_2
#define BLOCK_3
#define BLOCK_4
#define BLOCK_5
#define BLOCK_6
#define BLOCK_7
#define BLOCK_8
#define BLOCK_9
#define BLOCK_10
#define BLOCK_11
#define BLOCK_12
#define BLOCK_13
#define BLOCK_14
#define BLOCK_15
#define BLOCK_16
#define BLOCK_17
#define BLOCK_18
#define BLOCK_19
#define BLOCK_20
#define BLOCK_21
#define BLOCK_22
#define BLOCK_23
#define BLOCK_24
#define BLOCK_25
#define BLOCK_26
#define BLOCK_27
#define BLOCK_28
#define BLOCK_29
#define BLOCK_30
#define BLOCK_31

#define BLOCK_Merger

// 全局变量 index
int index;
int end_index;

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

void set_block_end_index(void)
{
    #ifdef BLOCK_31
    end_index = 31;
    #elif defined(BLOCK_30)
    end_index = 30;
    #elif defined(BLOCK_29)
    end_index = 29;
    #elif defined(BLOCK_28)
    end_index = 28;
    #elif defined(BLOCK_27)
    end_index = 27;
    #elif defined(BLOCK_26)
    end_index = 26;
    #elif defined(BLOCK_25)
    end_index = 25;
    #elif defined(BLOCK_24)
    end_index = 24;
    #elif defined(BLOCK_23)
    end_index = 23;
    #elif defined(BLOCK_22)
    end_index = 22;
    #elif defined(BLOCK_21)
    end_index = 21;
    #elif defined(BLOCK_20)
    end_index = 20;
    #elif defined(BLOCK_19)
    end_index = 19;
    #elif defined(BLOCK_18)
    end_index = 18;
    #elif defined(BLOCK_17)
    end_index = 17;
    #elif defined(BLOCK_16)
    end_index = 16;
    #elif defined(BLOCK_15)
    end_index = 15;
    #elif defined(BLOCK_14)
    end_index = 14;
    #elif defined(BLOCK_13)
    end_index = 13;
    #elif defined(BLOCK_12)
    end_index = 12;
    #elif defined(BLOCK_11)
    end_index = 11;
    #elif defined(BLOCK_10)
    end_index = 10;
    #elif defined(BLOCK_9)
    end_index = 9;
    #elif defined(BLOCK_8)
    end_index = 8;
    #elif defined(BLOCK_7)
    end_index = 7;
    #elif defined(BLOCK_6)
    end_index = 6;
    #elif defined(BLOCK_5)
    end_index = 5;
    #elif defined(BLOCK_4)
    end_index = 4;
    #elif defined(BLOCK_3)
    end_index = 3;
    #elif defined(BLOCK_2)
    end_index = 2;
    #elif defined(BLOCK_1)
    end_index = 1;
    #elif defined(BLOCK_0)
    end_index = 0;
    #else
    // 无任何 BLOCK 定义时的默认值
    end_index = -1;
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
// #include "./rw_cmd/wall_oss_block_all_0324_1019.h"                  // muti-ViT_block
#include "./rw_cmd/wall_oss_block_all_0407_1844.h" 

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

// ViT
#define STEP_ViT_LN0
// #define STEP_ViT_ELEMENTWISE2
// Merger
#define STEP_Merger_LN0
#define STEP_Merger_MVMBN0
#define STEP_Merger_MVMBN2

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
    set_block_end_index();

#ifdef LOAD_WEIGHT
    test_load_params(h2cx_device[0], ".");
#endif

#ifdef STEP_ViT_LN0
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

    test_ViT(user_device, run_token, last_token);

#endif

#ifdef STEP_ViT_ELEMENTWISE2
    // ******************************** STEP22 ELEMENTWISE2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280,
        /*DAT_IN_A_BASE_ADDR*/ runtime0, /*DAT_IN_B_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    sprintf(filename[0],  "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/input.bin", end_index+1, end_index+1);
    printf("%s\n", filename[0]);
    // Read output data from FPGA and compare
    #ifdef BLOCK_31
        struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0,         "./wall_oss/RMSNORM_visual_merger_ln_q/input.bin");
    #else
        struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0,         filename[0]);
    #endif

    HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "wall_oss_run/blocks_0", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);

    // Malloc free
    // bin_inf_malloc_free(elementwise2_dat_in_A_bin_inf);
    // bin_inf_malloc_free(elementwise2_dat_in_B_bin_inf);
    bin_inf_malloc_free(elementwise2_golden_out_bin_inf);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_A_HBM_inf, group);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_B_HBM_inf, group);
#endif 

#ifdef STEP_Merger_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_merger_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*LN_WT_BASE_ADDR*/ hbm548, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    struct bin_inf* merger_ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/RMSNORM_visual_merger_ln_q/input.bin");
    struct bin_inf* merger_ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/RMSNORM_visual_merger_ln_q/weight.bin");
    struct bin_inf* merger_ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *merger_ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *merger_ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_merger_ln0, "wall_oss_run/Merger", "LN1", merger_ln0_dat_in_bin_inf, merger_ln0_weight_bin_inf, merger_ln0_bias_bin_inf, merger_ln0_dat_in_HBM_inf, ENABLE, merger_ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    #ifndef BLOCK_31
        HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], merger_ln0_dat_in_HBM_inf, group);
    #endif
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], merger_ln0_ln_wt_and_bias_HBM_inf, group);

    // Write command to FPGA
    test_Merger_1(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    struct bin_inf* merger_ln0_golden_out_bin_inf = get_bin_inf(0, run_token*1*hidden_dim, "./wall_oss/RMSNORM_visual_merger_ln_q/output.bin");
    HBM_ln_receive_and_compare(cfg_merger_ln0, c2hx_device[0], "wall_oss_run/Merger", "LN1", merger_ln0_golden_out_bin_inf, TRUE);

    // Malloc free
    bin_inf_malloc_free(merger_ln0_dat_in_bin_inf);
    bin_inf_malloc_free(merger_ln0_weight_bin_inf);
    bin_inf_malloc_free(merger_ln0_bias_bin_inf);
    bin_inf_malloc_free(merger_ln0_golden_out_bin_inf);
    HBM_bin_inf_malloc_free(merger_ln0_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(merger_ln0_ln_wt_and_bias_HBM_inf, group); 
#endif

#ifdef STEP_Merger_MVMBN0
    // ******************************** STEP1 - MVMBN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token/4, /*Hin*/ 1, /*Width_in*/ hidden_dim*4, /*Width_out*/ hidden_dim*4,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm549, /*BN_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ hbm550
    );
    // Input bin_inf
    // struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/LINEAR_visual_merger_mlp_0/input.bin");
    struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss_run/Merger/LN_FPGA_out_bin/LN1_dat_out_demaped.bin");
    struct bin_inf* mvmbn2_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/LINEAR_visual_merger_mlp_0/weight_int4.bin"); 
    struct bin_inf* mvmbn2_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/LINEAR_visual_merger_mlp_0/scale.bin");
    struct bin_inf* mvmbn2_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn2_bias_bin_inf   = get_bin_inf(0, 12288,            "./wall_oss/LINEAR_visual_merger_mlp_0/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn2_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn2, "wall_oss_run/Merger", "MVMBN2", mvmbn2_weight_bin_inf, mvmbn2_scales_bin_inf, mvmbn2_dat_in_bin_inf, mvmbn2_wt_bin_inf, mvmbn2_bias_bin_inf,
                    mvmbn2_wt_and_scale_in_HBM_inf, ENABLE, mvmbn2_dat_in_HBM_inf, ENABLE, mvmbn2_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_wt_and_scale_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    test_Merger_2(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* mvmbn2_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/LINEAR_visual_merger_mlp_0/output.bin");
    // HBM_mvmbn_receive_and_compare(cfg_mvmbn2, c2hx_device[0], "wall_oss_run/Merger", "MVMBN2", mvmbn2_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn2_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn2_weight_bin_inf);
    bin_inf_malloc_free(mvmbn2_scales_bin_inf);
    bin_inf_malloc_free(mvmbn2_wt_bin_inf);
    bin_inf_malloc_free(mvmbn2_bias_bin_inf);
    // bin_inf_malloc_free(mvmbn2_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn2_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn2_bn_wt_and_bias_in_HBM_inf, group);
#endif 

#ifdef STEP_Merger_MVMBN2
    // ******************************** STEP19 - MVMBN3 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn3 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token/4, /*Hin*/ 1, /*Width_in*/ hidden_dim*4, /*Width_out*/ 2048,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm552, /*BN_BASE_ADDR*/ hbm553, /*DAT_OUT_BASE_ADDR*/ runtime4
    );
    // Read output data from FPGA and compare
    struct bin_inf* mvmbn3_golden_out_bin_inf = get_bin_inf(0, 22*1*4096, "./wall_oss/LINEAR_visual_merger_mlp_2/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn3, c2hx_device[0], "wall_oss_run/Merger", "MVMBN3", mvmbn3_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn3_golden_out_bin_inf );
#endif

}