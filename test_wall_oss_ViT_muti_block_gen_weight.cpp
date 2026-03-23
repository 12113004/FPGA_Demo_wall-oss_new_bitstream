#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

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
#include "./rw_cmd/wall_oss_debug_0324_0005.h"                          

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

// #define STEP_LN0
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

// ******************************** Config ******************************** //
int run_token    = 648;
int last_token   = 0;
int hidden_dim   = 1280;
int kvcache_mode = (run_token-last_token == 1)? 1 : 0;

void generate_wt_emb_and_act()
{
    // ******************************** STEP4 - EMBQ ******************************** //
    // Parameter Config
    struct FPGA_HBM_EMB_cfg cfg_embq = GetFPGA_HBM_EMB_cfg(
        /*Head*/ 16*4, /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime2, /*POS_IN_BASE_ADDR*/ hbm5, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    struct bin_inf* embq_dat_in_bin_inf = get_bin_inf(0, run_token*4*16*128,     "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/q_input.bin");
    struct bin_inf* embq_pos_in_bin_inf = get_bin_inf(0, 2048*128,               "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/cos_sin.bin"); 
    // Output bin_inf
    struct bin_inf* *embq_dat_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *embq_pos_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_emb_test(cfg_embq, "wall_oss_run/blocks_0", "EMBQ", embq_dat_in_bin_inf, embq_pos_in_bin_inf, embq_dat_in_HBM_inf, ENABLE, embq_pos_in_HBM_inf, ENABLE);

    // Malloc free
    bin_inf_malloc_free(embq_dat_in_bin_inf);
    bin_inf_malloc_free(embq_pos_in_bin_inf);
    // bin_inf_malloc_free(embq_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(embq_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(embq_pos_in_HBM_inf, group);

    // ******************************** STEP18 - ACT ******************************** //
    // Parameter Config
    struct FPGA_HBM_ACT_cfg cfg_act = GetFPGA_HBM_ACT_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 3456,
        /*DAT_IN_BASE_ADDR*/ runtime3, /*WT_BASE_ADDR*/ hbm16, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* act_dat_in_bin_inf = get_bin_inf(0, 22*1*12288,         "./wall_oss/blocks_0/ACT/input.bin");
    // Output bin_inf
    struct bin_inf* *act_dat_in_HBM_inf       = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *act_parameter_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group/32);

    // Transform data
    HBM_act_test(cfg_act, "wall_oss_run/blocks_0", "ACT", act_dat_in_bin_inf, act_dat_in_HBM_inf, ENABLE, act_parameter_in_HBM_inf, ENABLE);

    // Malloc free
    bin_inf_malloc_free(act_dat_in_bin_inf );
    HBM_bin_inf_malloc_free(act_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(act_parameter_in_HBM_inf, group/32);
}

void generate_wt_BLOCK(char* name1, char* name2, char* name3, int index)// name1: wall_oss；name2：wall_oss_run
{
    char** filename = (char**)malloc(sizeof(char*)*4);
    for(int i=0;i<4;i++)
        filename[i] = (char*)malloc(sizeof(char)*200);
    sprintf(filename[0], "wall_oss_run/blocks_%d", index);

    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*LN_WT_BASE_ADDR*/ hbm2, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/weight.bin", index, index);
    struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm1/input.bin");
    // struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm1/weight.bin");
    struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            filename[1]);
    struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_ln_test(cfg_ln0, filename[0], "LN0", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // ******************************** STEP2 - MVMBN0_Q ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_q = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 4*16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm3, /*BN_BASE_ADDR*/ hbm4, /*DAT_OUT_BASE_ADDR*/ runtime2
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_q_proj/weight_int4.bin",    index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_q_proj/scale.bin",          index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_q_proj/bias.bin",           index, index);
    // struct bin_inf* mvmbn0_q_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_q_proj/input.bin");
    // struct bin_inf* mvmbn0_q_weight_bin_inf = get_bin_inf(0, hidden_dim*4*16*128,     "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_q_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn0_q_scales_bin_inf = get_bin_inf(0, 4*16*128*hidden_dim/128, "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_q_proj/scale.bin");
    // struct bin_inf* mvmbn0_q_wt_bin_inf     = get_bin_inf(0, 4*16*128,                "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn0_q_bias_bin_inf   = get_bin_inf(0, 4*16*128,                "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_q_proj/bias.bin");
    struct bin_inf* mvmbn0_q_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_q_proj/input.bin");
    struct bin_inf* mvmbn0_q_weight_bin_inf = get_bin_inf(0, hidden_dim*4*16*128,     filename[1]); 
    struct bin_inf* mvmbn0_q_scales_bin_inf = get_bin_inf(0, 4*16*128*hidden_dim/128, filename[2]);
    struct bin_inf* mvmbn0_q_wt_bin_inf     = get_bin_inf(0, 4*16*128,                "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_q_bias_bin_inf   = get_bin_inf(0, 4*16*128,                filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_q_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_q_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_q_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_q, filename[0], "MVMBN0_Q", mvmbn0_q_weight_bin_inf, mvmbn0_q_scales_bin_inf, mvmbn0_q_dat_in_bin_inf, mvmbn0_q_wt_bin_inf, mvmbn0_q_bias_bin_inf,
                mvmbn0_q_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_q_dat_in_HBM_inf, ENABLE, mvmbn0_q_bn_wt_and_bias_in_HBM_inf, ENABLE);

    //******************************** STEP5 - MVMBN0_K ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_k = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm6, /*BN_BASE_ADDR*/ hbm7, /*DAT_OUT_BASE_ADDR*/ runtime2
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_k_proj/weight_int4.bin",    index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_k_proj/scale.bin",          index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_k_proj/bias.bin",           index, index);
    // struct bin_inf* mvmbn0_k_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_k_proj/input.bin");
    // struct bin_inf* mvmbn0_k_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_k_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn0_k_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_k_proj/scale.bin");
    // struct bin_inf* mvmbn0_k_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn0_k_bias_bin_inf   = get_bin_inf(0, 16*128,                  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_k_proj/bias.bin");
    struct bin_inf* mvmbn0_k_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_k_proj/input.bin");
    struct bin_inf* mvmbn0_k_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       filename[1]); 
    struct bin_inf* mvmbn0_k_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   filename[2]);
    struct bin_inf* mvmbn0_k_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_k_bias_bin_inf   = get_bin_inf(0, 16*128,                  filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_k_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_k_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_k_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_k, filename[0], "MVMBN0_K", mvmbn0_k_weight_bin_inf, mvmbn0_k_scales_bin_inf, mvmbn0_k_dat_in_bin_inf, mvmbn0_k_wt_bin_inf, mvmbn0_k_bias_bin_inf,
                 mvmbn0_k_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_k_dat_in_HBM_inf, ENABLE, mvmbn0_k_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // ******************************** STEP15 ELEMENTWISE0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise0 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ 16*run_token*((run_token-1)/512+1), /*Hin*/ 1, /*Width_in*/ 512,
        /*DAT_IN_A_BASE_ADDR*/ runtime2, /*DAT_IN_B_BASE_ADDR*/ hbm8, /*DAT_OUT_BASE_ADDR*/ runtime3 
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/ATTN_MASK/input.bin", index);
    // struct bin_inf* elementwise0_dat_in_A_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/blocks_0/ATTN_MASK/input.bin");
    // struct bin_inf* elementwise0_dat_in_B_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/blocks_0/ATTN_MASK/mask.bin");
    struct bin_inf* elementwise0_dat_in_A_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/blocks_0/ATTN_MASK/input.bin");
    struct bin_inf* elementwise0_dat_in_B_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   filename[1]);
    // Output bin_inf
    struct bin_inf* *elementwise0_dat_in_A_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *elementwise0_dat_in_B_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_elementwise_test(cfg_elementwise0, filename[0], "ATTEN_MASK", elementwise0_dat_in_A_bin_inf, elementwise0_dat_in_B_bin_inf, elementwise0_dat_in_A_HBM_inf, ENABLE, elementwise0_dat_in_B_HBM_inf, ENABLE);

    // ******************************** STEP11 - MVMBN0_V ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_v = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm9, /*BN_BASE_ADDR*/ hbm10, /*DAT_OUT_BASE_ADDR*/ runtime4
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_v_proj/weight_int4.bin",    index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_v_proj/scale.bin",          index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_qkv_v_proj/bias.bin",           index, index);
    // struct bin_inf* mvmbn0_v_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_v_proj/input.bin");
    // struct bin_inf* mvmbn0_v_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_v_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn0_v_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_v_proj/scale.bin");
    // struct bin_inf* mvmbn0_v_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn0_v_bias_bin_inf   = get_bin_inf(0, 16*128,                  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_v_proj/bias.bin");
    struct bin_inf* mvmbn0_v_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_qkv_v_proj/input.bin");
    struct bin_inf* mvmbn0_v_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       filename[1]);
    struct bin_inf* mvmbn0_v_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   filename[2]);
    struct bin_inf* mvmbn0_v_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_v_bias_bin_inf   = get_bin_inf(0, 16*128,                  filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_v_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_v_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_v_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_v, filename[0], "MVMBN0_V", mvmbn0_v_weight_bin_inf, mvmbn0_v_scales_bin_inf, mvmbn0_v_dat_in_bin_inf, mvmbn0_v_wt_bin_inf, mvmbn0_v_bias_bin_inf,
                 mvmbn0_v_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_v_dat_in_HBM_inf, ENABLE, mvmbn0_v_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // ******************************** STEP14 - MVMBN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn1 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 16*4*128, /*Width_out*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime3, /*HBM00_WT_BASE_ADDR*/ hbm11, /*BN_BASE_ADDR*/ hbm12, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_proj/weight_int4.bin",  index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_proj/scale.bin",        index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_attn_proj/bias.bin",         index, index);
    // struct bin_inf* mvmbn1_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_proj/input.bin");
    // struct bin_inf* mvmbn1_weight_bin_inf = get_bin_inf(0, 4096*4096,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn1_scales_bin_inf = get_bin_inf(0, 32*4096,              "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_proj/scale.bin");
    // struct bin_inf* mvmbn1_wt_bin_inf     = get_bin_inf(0, 4096,                 "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn1_bias_bin_inf   = get_bin_inf(0, 4096,                 "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_proj/bias.bin");
    struct bin_inf* mvmbn1_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_attn_proj/input.bin");
    struct bin_inf* mvmbn1_weight_bin_inf = get_bin_inf(0, 4096*4096,            filename[1]);
    struct bin_inf* mvmbn1_scales_bin_inf = get_bin_inf(0, 32*4096,              filename[2]);
    struct bin_inf* mvmbn1_wt_bin_inf     = get_bin_inf(0, 4096,                 "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn1_bias_bin_inf   = get_bin_inf(0, 4096,                 filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn1_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn1_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn1_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn1, filename[0], "MVMBN1", mvmbn1_weight_bin_inf, mvmbn1_scales_bin_inf, mvmbn1_dat_in_bin_inf, mvmbn1_wt_bin_inf, mvmbn1_bias_bin_inf,
                    mvmbn1_wt_and_scale_in_HBM_inf, ENABLE, mvmbn1_dat_in_HBM_inf, ENABLE, mvmbn1_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // ******************************** STEP16 - LN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln1 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*LN_WT_BASE_ADDR*/ hbm13, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm2/weight.bin", index, index);
    // struct bin_inf* ln1_dat_in_bin_inf   = get_bin_inf(0, 22*1*4096,         "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm2/input.bin");
    // struct bin_inf* ln1_weight_bin_inf   = get_bin_inf(0, 1*4096,            "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm2/weight.bin");
    // struct bin_inf* ln1_bias_bin_inf     = get_bin_inf(0, 4096,              "./rw_data/bn_and_k_bias_0.bin");
    struct bin_inf* ln1_dat_in_bin_inf   = get_bin_inf(0, 22*1*4096,         "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm2/input.bin");
    struct bin_inf* ln1_weight_bin_inf   = get_bin_inf(0, 1*4096,            filename[1]);
    struct bin_inf* ln1_bias_bin_inf     = get_bin_inf(0, 4096,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln1_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln1_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_ln_test(cfg_ln1, filename[0], "LN1", ln1_dat_in_bin_inf, ln1_weight_bin_inf, ln1_bias_bin_inf, ln1_dat_in_HBM_inf, ENABLE, ln1_ln_wt_and_bias_HBM_inf, ENABLE);

    // ******************************** STEP17 - MVMBN2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm14, /*BN_BASE_ADDR*/ hbm15, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_gate_proj/weight_int4.bin",  index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_gate_proj/scale.bin",        index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_gate_proj/bias.bin",         index, index);
    // struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_gate_proj/input.bin");
    // struct bin_inf* mvmbn2_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_gate_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn2_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_gate_proj/scale.bin");
    // struct bin_inf* mvmbn2_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn2_bias_bin_inf   = get_bin_inf(0, 12288,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_gate_proj/bias.bin");
    struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_gate_proj/input.bin");
    struct bin_inf* mvmbn2_weight_bin_inf = get_bin_inf(0, 12288*4096,       filename[1]);
    struct bin_inf* mvmbn2_scales_bin_inf = get_bin_inf(0, 12288*32,         filename[2]);
    struct bin_inf* mvmbn2_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn2_bias_bin_inf   = get_bin_inf(0, 12288,            filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn2_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn2, filename[0], "MVMBN2", mvmbn2_weight_bin_inf, mvmbn2_scales_bin_inf, mvmbn2_dat_in_bin_inf, mvmbn2_wt_bin_inf, mvmbn2_bias_bin_inf,
                    mvmbn2_wt_and_scale_in_HBM_inf, ENABLE, mvmbn2_dat_in_HBM_inf, ENABLE, mvmbn2_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // ******************************** STEP19 - MVMBN3 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn3 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm17, /*BN_BASE_ADDR*/ hbm18, /*DAT_OUT_BASE_ADDR*/ runtime3
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_up_proj/weight_int4.bin",  index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_up_proj/scale.bin",        index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_up_proj/bias.bin",         index, index);
    // struct bin_inf* mvmbn3_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_up_proj/input.bin");
    // struct bin_inf* mvmbn3_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_up_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn3_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_up_proj/scale.bin");
    // struct bin_inf* mvmbn3_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn3_bias_bin_inf   = get_bin_inf(0, 12288,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_up_proj/bias.bin");
    struct bin_inf* mvmbn3_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_up_proj/input.bin");
    struct bin_inf* mvmbn3_weight_bin_inf = get_bin_inf(0, 12288*4096,       filename[1]);
    struct bin_inf* mvmbn3_scales_bin_inf = get_bin_inf(0, 12288*32,         filename[2]);
    struct bin_inf* mvmbn3_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn3_bias_bin_inf   = get_bin_inf(0, 12288,            filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn3_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn3, filename[0], "MVMBN3", mvmbn3_weight_bin_inf, mvmbn3_scales_bin_inf, mvmbn3_dat_in_bin_inf, mvmbn3_wt_bin_inf, mvmbn3_bias_bin_inf,
                    mvmbn3_wt_and_scale_in_HBM_inf, ENABLE, mvmbn3_dat_in_HBM_inf, ENABLE, mvmbn3_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // ******************************** STEP21 - MVMBN4 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn4 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 3456, /*Width_out*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm19, /*BN_BASE_ADDR*/ hbm20, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    sprintf(filename[1], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_down_proj/weight_int4.bin",  index, index);
    sprintf(filename[2], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_down_proj/scale.bin",        index, index);
    sprintf(filename[3], "./wall_oss/blocks_%d/LINEAR_visual_blocks_%d_mlp_down_proj/bias.bin",         index, index);
    // struct bin_inf* mvmbn4_dat_in_bin_inf = get_bin_inf(0, run_token*3456,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_down_proj/input.bin");
    // struct bin_inf* mvmbn4_weight_bin_inf = get_bin_inf(0, 3456*1280,       "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_down_proj/weight_int4.bin"); 
    // struct bin_inf* mvmbn4_scales_bin_inf = get_bin_inf(0, 1280*27,         "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_down_proj/scale.bin");
    // struct bin_inf* mvmbn4_wt_bin_inf     = get_bin_inf(0, 1280,            "./rw_data/bn_wt_1.bin");
    // struct bin_inf* mvmbn4_bias_bin_inf   = get_bin_inf(0, 1280,            "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_down_proj/bias.bin");
    struct bin_inf* mvmbn4_dat_in_bin_inf = get_bin_inf(0, run_token*3456,  "./wall_oss/blocks_0/LINEAR_visual_blocks_0_mlp_down_proj/input.bin");
    struct bin_inf* mvmbn4_weight_bin_inf = get_bin_inf(0, 3456*1280,       filename[1]);
    struct bin_inf* mvmbn4_scales_bin_inf = get_bin_inf(0, 1280*27,         filename[2]);
    struct bin_inf* mvmbn4_wt_bin_inf     = get_bin_inf(0, 1280,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn4_bias_bin_inf   = get_bin_inf(0, 1280,            filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn4_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn4, filename[0], "MVMBN4", mvmbn4_weight_bin_inf, mvmbn4_scales_bin_inf, mvmbn4_dat_in_bin_inf, mvmbn4_wt_bin_inf, mvmbn4_bias_bin_inf,
                    mvmbn4_wt_and_scale_in_HBM_inf, ENABLE, mvmbn4_dat_in_HBM_inf, ENABLE, mvmbn4_bn_wt_and_bias_in_HBM_inf, ENABLE);

}

int __cdecl main()
{
    int  model_part      = 0;
    char *read_filename  = "";
    char *write_filename = "";
    char BLOCK_name[20];

    switch(model_part)
    {
        case 0:
            generate_wt_emb_and_act();
            generate_wt_BLOCK(read_filename, write_filename, "VL_BLOCK00", 0);
        // {
        //     for(int i=0;i<3;i++)
        //     {
        //         sprintf(BLOCK_name, "VL_BLOCK%02d", i);
        //         generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
        //     }
        // } 
        break;
        // LLM ACT and EMB
        case 1:
            generate_wt_emb_and_act();
        break;
        // LLM BLOCK00 - BLOCK06
        case 2:
        {
            for(int i=0;i<7;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        } //D:\BaiduNetdiskDownload\FPGA_Demo_qwen2v5\rw_data\qwen2v5_vl_all_weights\BLOCK_write_data_qwen2v5_vl
        break;
        // LLM BLOCK07 - BLOCK13
        case 3:
        {
            for(int i=7;i<14;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        }
        break;
        // LLM BLOCK14 - BLOCK20
        case 4:
        {
            for(int i=14;i<21;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        }
        break;
        // LLM BLOCK21 - BLOCK27
        case 5:
        {
            for(int i=21;i<28;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        }
        break;
        // LLM BLOCK28 - BLOCK34
        case 6:
        {
            for(int i=28;i<35;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        }
        break;
        // LLM BLOCK35 - BLOCK39
        case 7:
        {
            for(int i=35;i<36;i++)
            {
                sprintf(BLOCK_name, "VL_BLOCK%02d", i);
                generate_wt_BLOCK(read_filename, write_filename, BLOCK_name, i);
            }
        }
        break;
        // LLM Outlayer
        case 8:
            // generate_wt_merger();
        break;
        default:;
        break;
    }
}

