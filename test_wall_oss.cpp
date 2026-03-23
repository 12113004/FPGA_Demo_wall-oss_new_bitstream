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
#include "./rw_cmd/wall_oss_debug_0322_2204.h" 

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

#define STEP_LN1
#define STEP_MVMBN2
#define STEP_ACT
// #define STEP_MVMBN3
// #define STEP_ELEMENTWISE1
// #define STEP_MVMBN4
// #define STEP_ELEMENTWISE2
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

//    test_load_block00_weights(h2cx_device[0], "BLOCK_write_data_qwen3");

#ifdef STEP_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*LN_WT_BASE_ADDR*/ hbm2, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/input.bin");
    struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/weight.bin");
    struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_ln0, "wall_oss_run/blocks_0", "LN0", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_ln_wt_and_bias_HBM_inf, group);

    // Write command to FPGA
    norm_step_2(user_device, run_token);

    // Read output data from FPGA and compare
    // struct bin_inf* ln0_golden_out_bin_inf = get_bin_inf(0, run_token*1*hidden_dim, "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/output.bin");
    // HBM_ln_receive_and_compare(cfg_ln0, c2hx_device[0], "wall_oss_run/blocks_0", "LN0", ln0_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(ln0_dat_in_bin_inf);
    bin_inf_malloc_free(ln0_weight_bin_inf);
    bin_inf_malloc_free(ln0_bias_bin_inf);
    // bin_inf_malloc_free(ln0_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(ln0_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(ln0_ln_wt_and_bias_HBM_inf, group); 
#endif

#ifdef STEP_MVMBN0_Q
    // ******************************** STEP2 - MVMBN0_Q ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_q = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 4*16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm3, /*BN_BASE_ADDR*/ hbm4, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* mvmbn0_q_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/input.bin");
    struct bin_inf* mvmbn0_q_weight_bin_inf = get_bin_inf(0, hidden_dim*4*16*128,     "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_q_scales_bin_inf = get_bin_inf(0, 4*16*128*hidden_dim/128, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/scale.bin");
    struct bin_inf* mvmbn0_q_wt_bin_inf     = get_bin_inf(0, 4*16*128,                "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_q_bias_bin_inf   = get_bin_inf(0, 4*16*128,                "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_q_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_q_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_q_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_q, "wall_oss_run/blocks_0", "MVMBN0_Q", mvmbn0_q_weight_bin_inf, mvmbn0_q_scales_bin_inf, mvmbn0_q_dat_in_bin_inf, mvmbn0_q_wt_bin_inf, mvmbn0_q_bias_bin_inf,
                mvmbn0_q_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_q_dat_in_HBM_inf, ENABLE, mvmbn0_q_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_q_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_q_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_q_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_3(user_device, run_token);

    // Read output data from FPGA and compare
    // struct bin_inf* mvmbn0_q_golden_out_bin_inf = get_bin_inf(0, run_token*1*4*16*128, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/output.bin");
    // HBM_mvmbn_receive_and_compare(cfg_mvmbn0_q, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_Q",  mvmbn0_q_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn0_q_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn0_q_weight_bin_inf);
    bin_inf_malloc_free(mvmbn0_q_scales_bin_inf);
    bin_inf_malloc_free(mvmbn0_q_wt_bin_inf);
    bin_inf_malloc_free(mvmbn0_q_bias_bin_inf);
    // bin_inf_malloc_free(mvmbn0_q_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn0_q_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_q_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_q_bn_wt_and_bias_in_HBM_inf, group);
    printf("mvmbn_q debug done!");

#endif

#ifdef STEP_EMBQ
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

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], embq_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], embq_pos_in_HBM_inf, group);

    // Write command to FPGA
    rope_step_4(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* embq_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/q_output.bin"); 
    // HBM_emb_receive_and_compare(cfg_embq, c2hx_device[0], "wall_oss_run/blocks_0", "EMBQ", embq_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(embq_dat_in_bin_inf);
    bin_inf_malloc_free(embq_pos_in_bin_inf);
    // bin_inf_malloc_free(embq_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(embq_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(embq_pos_in_HBM_inf, group);
#endif

#ifdef STEP_MVMBN0_K
    //******************************** STEP5 - MVMBN0_K ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_k = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm6, /*BN_BASE_ADDR*/ hbm7, /*DAT_OUT_BASE_ADDR*/ runtime2
    );
    // Input bin_inf
    struct bin_inf* mvmbn0_k_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/input.bin");
    struct bin_inf* mvmbn0_k_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_k_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/scale.bin");
    struct bin_inf* mvmbn0_k_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_k_bias_bin_inf   = get_bin_inf(0, 16*128,                  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_k_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_k_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_k_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_k, "wall_oss_run/blocks_0", "MVMBN0_K", mvmbn0_k_weight_bin_inf, mvmbn0_k_scales_bin_inf, mvmbn0_k_dat_in_bin_inf, mvmbn0_k_wt_bin_inf, mvmbn0_k_bias_bin_inf,
                 mvmbn0_k_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_k_dat_in_HBM_inf, ENABLE, mvmbn0_k_bn_wt_and_bias_in_HBM_inf, ENABLE);
    
    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_k_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_k_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_k_bn_wt_and_bias_in_HBM_inf, group);
    
    // Write command to FPGA
    mvm_f16xi4_step_5(user_device, run_token);
    
    // Read output data from FPGA and compare
    // struct bin_inf* mvmbn0_k_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/output.bin");
    // HBM_mvmbn_receive_and_compare(cfg_mvmbn0_k, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_K", mvmbn0_k_golden_out_bin_inf);
    
    // Malloc free
    bin_inf_malloc_free(mvmbn0_k_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn0_k_weight_bin_inf);
    bin_inf_malloc_free(mvmbn0_k_scales_bin_inf);
    bin_inf_malloc_free(mvmbn0_k_wt_bin_inf);
    bin_inf_malloc_free(mvmbn0_k_bias_bin_inf);
    // bin_inf_malloc_free(mvmbn0_k_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn0_k_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_k_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_k_bn_wt_and_bias_in_HBM_inf, group);      
#endif

#ifdef STEP_EMBK
    // ******************************** STEP7 - EMBK ******************************** //
    // Parameter Config
    struct FPGA_HBM_EMB_cfg cfg_embk = GetFPGA_HBM_EMB_cfg(
        /*Head*/ 16, /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime2, /*POS_IN_BASE_ADDR*/ hbm5, /*DAT_OUT_BASE_ADDR*/ runtime4
    );

    // Input bin_inf
    struct bin_inf* embk_dat_in_bin_inf = get_bin_inf(0, run_token*16*128,   "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/k_input.bin");
    struct bin_inf* embk_pos_in_bin_inf = get_bin_inf(0, 2048*128,           "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/cos_sin.bin"); 
    // Output bin_inf
    struct bin_inf* *embk_dat_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *embk_pos_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_emb_test(cfg_embk, "wall_oss_run/blocks_0", "EMBK", embk_dat_in_bin_inf, embk_pos_in_bin_inf, embk_dat_in_HBM_inf, ENABLE, embk_pos_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], embk_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], embk_pos_in_HBM_inf, group);

    // Write command to FPGA
    rope_step_6(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* embk_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/k_output.bin");
    // HBM_emb_receive_and_compare(cfg_embk, c2hx_device[0], "wall_oss_run/blocks_0", "EMBK", embk_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(embk_dat_in_bin_inf);
    bin_inf_malloc_free(embk_pos_in_bin_inf);
    // bin_inf_malloc_free(embk_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(embk_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(embk_pos_in_HBM_inf, group);
#endif

#ifdef STEP_KV2HBMK_K
    // ******************************** STEP8 - KV2HBMK_K ******************************** //
    // Parameter Config
    struct FPGA_HBM_KV2HBM_cfg cfg_kv2hbmk = GetFPGA_HBM_KV2HBM_cfg(
        /*This_Token*/ run_token, /*Last_Token*/ last_token, /*Weight_Head*/ 16, /*MAX_CH_per_HEAD*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ hbm_cache0
    );

    // Input bin_inf
    struct bin_inf* kv2hbmk_dat_in_bin_inf = get_bin_inf(0, run_token*16*128, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/k_output.bin");
    // Output bin_inf
    struct bin_inf* *kv2hbmk_dat_in_HBM_inf    = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *kv2hbmk_golden_out_bin_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_kv2hbm_test(cfg_kv2hbmk, "wall_oss_run/blocks_0", "KV2HBMK", kv2hbmk_dat_in_bin_inf, kv2hbmk_dat_in_HBM_inf, kv2hbmk_golden_out_bin_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], kv2hbmk_dat_in_HBM_inf, group);

    // Write command to FPGA
    kvcache2hbm_step_7(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // HBM_kv2hbm_receive_and_compare(cfg_kv2hbmk, c2hx_device[0], "wall_oss_run/blocks_0", "KV2HBMK", kv2hbmk_golden_out_bin_inf[0], K_Mode);

    // Malloc free
    bin_inf_malloc_free(kv2hbmk_dat_in_bin_inf);
    bin_inf_malloc_free(kv2hbmk_golden_out_bin_inf[0]);
    HBM_bin_inf_malloc_free(kv2hbmk_dat_in_HBM_inf, group);
#endif

#ifdef STEP_TRP
    // ******************************** STEP9 - TRP ******************************** //
    // Parameter Config
    struct FPGA_HBM_TRP_cfg cfg_trp = GetFPGA_HBM_TRP_cfg(
        /*This_Token*/ run_token, /*Last_Token*/ last_token, /*Original_Feature_Head*/ 16*4, /*Weight_Head*/ 16, /*MAX_CH_per_HEAD*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime3, /*WT_BASE_ADDR*/ hbm_cache0, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* trp_dat_in_bin_inf = get_bin_inf(0, run_token*4*16*128,      "./wall_oss/blocks_0/TRP/q.bin");
    struct bin_inf* trp_wt_in_bin_inf  = get_bin_inf(0, run_token*16*128,        "./wall_oss/blocks_0/TRP/k.bin");
    // Output bin_inf
    struct bin_inf* *trp_dat_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *trp_wt_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_trp_test(cfg_trp, "wall_oss_run/blocks_0", "TRP", trp_dat_in_bin_inf, trp_wt_in_bin_inf, trp_dat_in_HBM_inf, ENABLE, trp_wt_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], trp_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], trp_wt_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xf16_step_8(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* trp_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/TRP/attn_weights.bin");
    // HBM_trp_receive_and_compare(cfg_trp, c2hx_device[0], "wall_oss_run/blocks_0", "TRP", trp_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(trp_dat_in_bin_inf);
    bin_inf_malloc_free(trp_wt_in_bin_inf);
    // bin_inf_malloc_free(trp_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(trp_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(trp_wt_in_HBM_inf, group);
#endif

#ifdef STEP_MASK
    // ******************************** STEP15 ELEMENTWISE0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise0 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ 16*run_token*((run_token-1)/512+1), /*Hin*/ 1, /*Width_in*/ 512,
        /*DAT_IN_A_BASE_ADDR*/ runtime2, /*DAT_IN_B_BASE_ADDR*/ hbm8, /*DAT_OUT_BASE_ADDR*/ runtime3 
    );
    
    // Input bin_inf
    struct bin_inf* elementwise0_dat_in_A_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/blocks_0/ATTN_MASK/input.bin");
    struct bin_inf* elementwise0_dat_in_B_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/blocks_0/ATTN_MASK/mask.bin");
    // struct bin_inf* elementwise0_dat_in_B_bin_inf  = get_bin_inf(0, 16*4*run_token*672,   "./wall_oss/back/zero_16x648x648.bin");
    // Output bin_inf
    struct bin_inf* *elementwise0_dat_in_A_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *elementwise0_dat_in_B_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_elementwise_test(cfg_elementwise0, "wall_oss_run/blocks_0", "ATTEN_MASK", elementwise0_dat_in_A_bin_inf, elementwise0_dat_in_B_bin_inf, elementwise0_dat_in_A_HBM_inf, ENABLE, elementwise0_dat_in_B_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise0_dat_in_A_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise0_dat_in_B_HBM_inf, group);

    // Write command to FPGA
    elementwise_step_9(user_device);
    // elementwise_step_9(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* elementwise0_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ATTN_MASK/output.bin");
    // HBM_elementwise_receive_and_compare(cfg_elementwise0, c2hx_device[0], "wall_oss_run/blocks_0", "ATTEN_MASK", elementwise0_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(elementwise0_dat_in_A_bin_inf);
    bin_inf_malloc_free(elementwise0_dat_in_B_bin_inf);
    // bin_inf_malloc_free(elementwise0_golden_out_bin_inf);
    HBM_bin_inf_malloc_free(elementwise0_dat_in_A_HBM_inf, group);
    HBM_bin_inf_malloc_free(elementwise0_dat_in_B_HBM_inf, group);
#endif

#ifdef STEP_SOFTMAX
    // ******************************** STEP10 - SOFTMAX ******************************** //
    // Parameter Config
    struct FPGA_HBM_SOFTMAX_cfg cfg_softmax = GetFPGA_HBM_SOFTMAX_cfg(
        /*Head*/ 16, /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ run_token,
        /*DAT_IN_BASE_ADDR*/ runtime3, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* softmax_dat_in_bin_inf = get_bin_inf(0, 16*run_token*run_token, "./wall_oss/blocks_0/SOFTMAX/input.bin");
    // Output bin_inf
    struct bin_inf* *softmax_dat_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_softmax_test(cfg_softmax , "wall_oss_run/blocks_0", "SOFTMAX", softmax_dat_in_bin_inf, softmax_dat_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], softmax_dat_in_HBM_inf, group);

    // Write command to FPGA
    softmax_step_10(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* softmax_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/SOFTMAX/output.bin");
    // HBM_softmax_receive_and_compare(cfg_softmax, c2hx_device[0], "wall_oss_run/blocks_0", "SOFTMAX", softmax_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(softmax_dat_in_bin_inf);
    // bin_inf_malloc_free(softmax_golden_out_bin_inf);
    HBM_bin_inf_malloc_free(softmax_dat_in_HBM_inf, group);
#endif

#ifdef STEP_MVMBN0_V
    // ******************************** STEP11 - MVMBN0_V ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn0_v = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim, /*Width_out*/ 16*128,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm9, /*BN_BASE_ADDR*/ hbm10, /*DAT_OUT_BASE_ADDR*/ runtime4
    );
    // Input bin_inf
    struct bin_inf* mvmbn0_v_dat_in_bin_inf = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/input.bin");
    struct bin_inf* mvmbn0_v_weight_bin_inf = get_bin_inf(0, hidden_dim*16*128,       "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_v_scales_bin_inf = get_bin_inf(0, 16*128*hidden_dim/128,   "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/scale.bin");
    struct bin_inf* mvmbn0_v_wt_bin_inf     = get_bin_inf(0, 16*128,                  "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_v_bias_bin_inf   = get_bin_inf(0, 16*128,                  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_v_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_v_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn0_v_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // Transform data
    HBM_mvmbn_test(cfg_mvmbn0_v, "wall_oss_run/blocks_0", "MVMBN0_V", mvmbn0_v_weight_bin_inf, mvmbn0_v_scales_bin_inf, mvmbn0_v_dat_in_bin_inf, mvmbn0_v_wt_bin_inf, mvmbn0_v_bias_bin_inf,
                 mvmbn0_v_wt_and_scale_in_HBM_inf, ENABLE, mvmbn0_v_dat_in_HBM_inf, ENABLE, mvmbn0_v_bn_wt_and_bias_in_HBM_inf, ENABLE);
    
    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_v_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_v_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn0_v_bn_wt_and_bias_in_HBM_inf, group);
    
    // Write command to FPGA
    mvm_f16xi4_step_11(user_device, run_token);
    
    // Read output data from FPGA and compare
    // struct bin_inf* mvmbn0_v_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/output.bin");
    // HBM_mvmbn_receive_and_compare(cfg_mvmbn0_v, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_V", mvmbn0_v_golden_out_bin_inf);
    
    // Malloc free
    bin_inf_malloc_free(mvmbn0_v_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn0_v_weight_bin_inf);
    bin_inf_malloc_free(mvmbn0_v_scales_bin_inf);
    bin_inf_malloc_free(mvmbn0_v_wt_bin_inf);
    bin_inf_malloc_free(mvmbn0_v_bias_bin_inf);
    // bin_inf_malloc_free(mvmbn0_v_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn0_v_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_v_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn0_v_bn_wt_and_bias_in_HBM_inf, group);
#endif

#ifdef STEP_KV2HBM_V
    // ******************************** STEP12 - KV2HBM_V ******************************** //
    // Parameter Config
    struct FPGA_HBM_KV2HBM_cfg cfg_kv2hbmv = GetFPGA_HBM_KV2HBM_cfg(
        /*This_Token*/ run_token, /*Last_Token*/ last_token, /*Weight_Head*/ 8, /*MAX_CH_per_HEAD*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ hbm_cache1
    );

    // Input bin_inf
    struct bin_inf* kv2hbmv_dat_in_bin_inf = get_bin_inf(0, 8*19*128, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/output.bin");
    // Output bin_inf
    struct bin_inf* *kv2hbmv_dat_in_HBM_inf    = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *kv2hbmv_golden_out_bin_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_kv2hbm_test(cfg_kv2hbmv, "wall_oss_run/blocks_0", "KV2HBMV", kv2hbmv_dat_in_bin_inf, kv2hbmv_dat_in_HBM_inf, kv2hbmv_golden_out_bin_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], kv2hbmv_dat_in_HBM_inf, group);

    // Write command to FPGA
    kvcache2hbm_step_12(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // HBM_kv2hbm_receive_and_compare(cfg_kv2hbmv, c2hx_device[0], "wall_oss_run/blocks_0",  "KV2HBMV", kv2hbmv_golden_out_bin_inf[0], V_Mode);

    // Malloc free
    bin_inf_malloc_free(kv2hbmv_dat_in_bin_inf);
    bin_inf_malloc_free(kv2hbmv_golden_out_bin_inf[0]);
    HBM_bin_inf_malloc_free(kv2hbmv_dat_in_HBM_inf, group);
#endif

#ifdef STEP_F2W
    // ******************************** STEP13 - F2W ******************************** //
    // Parameter Config
    struct FPGA_HBM_F2W_cfg cfg_f2w = GetFPGA_HBM_F2W_cfg(
        /*This_Token*/ run_token, /*Last_Token*/ last_token, /*Original_Feature_Head*/ 16, /*Weight_Head*/ 16, /*MAX_CH_per_HEAD*/ 128, /*MAX_TOKEN*/ 2048, 
        /*DAT_IN_BASE_ADDR*/ runtime2, /*WT_BASE_ADDR*/ hbm_cache1, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    struct bin_inf* f2w_dat_in_bin_inf = get_bin_inf(0, 32*22*128,       "./wall_oss/blocks_0/F2W/attn_weights.bin");
    struct bin_inf* f2w_wt_in_bin_inf  = get_bin_inf(0, 8*22*128,        "./wall_oss/blocks_0/F2W/v.bin");
    // Output bin_inf
    struct bin_inf* *f2w_dat_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *f2w_wt_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_f2w_test(cfg_f2w, "wall_oss_run/blocks_0", "F2W", f2w_dat_in_bin_inf, f2w_wt_in_bin_inf, f2w_dat_in_HBM_inf, ENABLE, f2w_wt_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], f2w_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], f2w_wt_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xf16_step_13(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    // struct bin_inf* f2w_golden_out_bin_inf = get_bin_inf(0, 32*22*128, "./wall_oss/blocks_0/F2W/attn_output.bin");
    // HBM_f2w_receive_and_compare(cfg_f2w, c2hx_device[0], "wall_oss_run/blocks_0", "F2W", f2w_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(f2w_dat_in_bin_inf);
    bin_inf_malloc_free(f2w_wt_in_bin_inf);
    // bin_inf_malloc_free(f2w_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(f2w_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(f2w_wt_in_HBM_inf, group);
#endif

#ifdef STEP_MVMBN1
    // ******************************** STEP14 - MVMBN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn1 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 16*4*128, /*Width_out*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime3, /*HBM00_WT_BASE_ADDR*/ hbm11, /*BN_BASE_ADDR*/ hbm12, /*DAT_OUT_BASE_ADDR*/ runtime0
    );

    // Input bin_inf
    struct bin_inf* mvmbn1_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,            "./wall_oss/blocks_0/05_LINEAR_visual_blocks_0_attn_proj/input.bin");
    struct bin_inf* mvmbn1_weight_bin_inf = get_bin_inf(0, 4096*4096,            "./wall_oss/blocks_0/05_LINEAR_visual_blocks_0_attn_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn1_scales_bin_inf = get_bin_inf(0, 32*4096,              "./wall_oss/blocks_0/05_LINEAR_visual_blocks_0_attn_proj/scale.bin");
    struct bin_inf* mvmbn1_wt_bin_inf     = get_bin_inf(0, 4096,                 "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn1_bias_bin_inf   = get_bin_inf(0, 4096,                 "./wall_oss/blocks_0/05_LINEAR_visual_blocks_0_attn_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn1_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn1_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn1_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn1, "wall_oss_run/blocks_0", "MVMBN1", mvmbn1_weight_bin_inf, mvmbn1_scales_bin_inf, mvmbn1_dat_in_bin_inf, mvmbn1_wt_bin_inf, mvmbn1_bias_bin_inf,
                    mvmbn1_wt_and_scale_in_HBM_inf, ENABLE, mvmbn1_dat_in_HBM_inf, ENABLE, mvmbn1_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn1_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn1_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn1_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_14 (user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn1_golden_out_bin_inf = get_bin_inf(0, 0,                "./wall_oss/blocks_0/05_LINEAR_visual_blocks_0_attn_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn1, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN1", mvmbn1_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn1_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn1_weight_bin_inf);
    bin_inf_malloc_free(mvmbn1_scales_bin_inf);
    bin_inf_malloc_free(mvmbn1_wt_bin_inf);
    bin_inf_malloc_free(mvmbn1_bias_bin_inf);
    bin_inf_malloc_free(mvmbn1_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn1_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn1_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn1_bn_wt_and_bias_in_HBM_inf, group);
#endif 

#ifdef STEP_ELEMENTWISE0
    // // ******************************** STEP15 ELEMENTWISE0 ******************************** //
    // // Parameter Config
    // struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise0 = GetFPGA_HBM_ELEMENTWISE_cfg(
    //     /*Height*/ 22, /*Hin*/ 1, /*Width_in*/ 4096,
    //     /*DAT_IN_A_BASE_ADDR*/ runtime2, /*DAT_IN_B_BASE_ADDR*/ runtime0, /*DAT_OUT_BASE_ADDR*/ runtime1 
    // );

    // // Input bin_inf
    // struct bin_inf* elementwise0_dat_in_A_bin_inf  = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE0/ELEMENTWISE0_input.bin");
    // struct bin_inf* elementwise0_dat_in_B_bin_inf  = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE0/residual.bin");
    // // Output bin_inf
    // struct bin_inf* *elementwise0_dat_in_A_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // struct bin_inf* *elementwise0_dat_in_B_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // // Transform data
    // HBM_elementwise_test(cfg_elementwise0, "BLOCK_write_data/BLOCK00", "ELEMENTWISE0", elementwise0_dat_in_A_bin_inf, elementwise0_dat_in_B_bin_inf, elementwise0_dat_in_A_HBM_inf, ENABLE, elementwise0_dat_in_B_HBM_inf, ENABLE);

    // // Write data to FPGA
    // // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise0_dat_in_A_HBM_inf, group);
    // // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise0_dat_in_B_HBM_inf, group);

    // Write command to FPGA
    elementwise_step_15(user_device, run_token);

    // // Read output data from FPGA and compare
    // struct bin_inf* elementwise0_golden_out_bin_inf = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE0/ELEMENTWISE0_output.bin");
    // HBM_elementwise_receive_and_compare(cfg_elementwise0, c2hx_device[0], "BLOCK_read_data", "ELEMENTWISE0", elementwise0_golden_out_bin_inf);

    // // Malloc free
    // bin_inf_malloc_free(elementwise0_dat_in_A_bin_inf);
    // bin_inf_malloc_free(elementwise0_dat_in_B_bin_inf);
    // bin_inf_malloc_free(elementwise0_golden_out_bin_inf);
    // HBM_bin_inf_malloc_free(elementwise0_dat_in_A_HBM_inf, group);
    // HBM_bin_inf_malloc_free(elementwise0_dat_in_B_HBM_inf, group);
#endif

#ifdef STEP_LN1
    // ******************************** STEP16 - LN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln1 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime4, /*LN_WT_BASE_ADDR*/ hbm13, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    // Input bin_inf
    struct bin_inf* ln1_dat_in_bin_inf   = get_bin_inf(0, 22*1*4096,         "./wall_oss/blocks_0/03_RMSNORM_visual_blocks_0_norm2/input.bin");
    struct bin_inf* ln1_weight_bin_inf   = get_bin_inf(0, 1*4096,            "./wall_oss/blocks_0/03_RMSNORM_visual_blocks_0_norm2/weight.bin");
    struct bin_inf* ln1_bias_bin_inf     = get_bin_inf(0, 4096,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln1_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln1_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_ln1, "wall_oss_run/blocks_0", "LN1", ln1_dat_in_bin_inf, ln1_weight_bin_inf, ln1_bias_bin_inf, ln1_dat_in_HBM_inf, ENABLE, ln1_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln1_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln1_ln_wt_and_bias_HBM_inf, group);

    // Write command to FPGA
    norm_step_16(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* ln1_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/03_RMSNORM_visual_blocks_0_norm2/output.bin");
    HBM_ln_receive_and_compare(cfg_ln1, c2hx_device[0], "wall_oss_run/blocks_0", "LN1", ln1_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(ln1_dat_in_bin_inf);
    bin_inf_malloc_free(ln1_weight_bin_inf);
    bin_inf_malloc_free(ln1_bias_bin_inf);
    bin_inf_malloc_free(ln1_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(ln1_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(ln1_ln_wt_and_bias_HBM_inf, group);
#endif

#ifdef STEP_MVMBN2
    // ******************************** STEP17 - MVMBN2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm14, /*BN_BASE_ADDR*/ hbm15, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/06_LINEAR_visual_blocks_0_mlp_gate_proj/input.bin");
    struct bin_inf* mvmbn2_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/blocks_0/06_LINEAR_visual_blocks_0_mlp_gate_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn2_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/blocks_0/06_LINEAR_visual_blocks_0_mlp_gate_proj/scale.bin");
    struct bin_inf* mvmbn2_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn2_bias_bin_inf   = get_bin_inf(0, 12288,            "./wall_oss/blocks_0/06_LINEAR_visual_blocks_0_mlp_gate_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn2_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn2, "wall_oss_run/blocks_0", "MVMBN2", mvmbn2_weight_bin_inf, mvmbn2_scales_bin_inf, mvmbn2_dat_in_bin_inf, mvmbn2_wt_bin_inf, mvmbn2_bias_bin_inf,
                    mvmbn2_wt_and_scale_in_HBM_inf, ENABLE, mvmbn2_dat_in_HBM_inf, ENABLE, mvmbn2_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_17(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn2_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/06_LINEAR_visual_blocks_0_mlp_gate_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn2, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN2", mvmbn2_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn2_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn2_weight_bin_inf);
    bin_inf_malloc_free(mvmbn2_scales_bin_inf);
    bin_inf_malloc_free(mvmbn2_wt_bin_inf);
    bin_inf_malloc_free(mvmbn2_bias_bin_inf);
    bin_inf_malloc_free(mvmbn2_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn2_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn2_bn_wt_and_bias_in_HBM_inf, group);
#endif 

#ifdef STEP_ACT
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

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], act_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], act_parameter_in_HBM_inf, group/32);

    // Write command to FPGA
    activate_step_18(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* act_golden_out_bin_inf = get_bin_inf(0, 0,          "./wall_oss/blocks_0/ACT/output.bin");
    HBM_act_receive_and_compare(cfg_act, c2hx_device[0], "BLOCK_read_data", "ACT", act_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(act_dat_in_bin_inf );
    bin_inf_malloc_free(act_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(act_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(act_parameter_in_HBM_inf, group/32);
#endif

#ifdef STEP_MVMBN3
    // ******************************** STEP19 - MVMBN3 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn3 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3420,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm17, /*BN_BASE_ADDR*/ hbm18, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    struct bin_inf* mvmbn3_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/blocks_0/07_LINEAR_visual_blocks_0_mlp_up_proj/input.bin");
    struct bin_inf* mvmbn3_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/blocks_0/07_LINEAR_visual_blocks_0_mlp_up_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn3_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/blocks_0/07_LINEAR_visual_blocks_0_mlp_up_proj/scale.bin");
    struct bin_inf* mvmbn3_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn3_bias_bin_inf   = get_bin_inf(0, 12288,            "./wall_oss/blocks_0/07_LINEAR_visual_blocks_0_mlp_up_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn3_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn3, "wall_oss_run/blocks_0", "MVMBN3", mvmbn3_weight_bin_inf, mvmbn3_scales_bin_inf, mvmbn3_dat_in_bin_inf, mvmbn3_wt_bin_inf, mvmbn3_bias_bin_inf,
                    mvmbn3_wt_and_scale_in_HBM_inf, ENABLE, mvmbn3_dat_in_HBM_inf, ENABLE, mvmbn3_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_19(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn3_golden_out_bin_inf = get_bin_inf(0, 22*1*4096, "./wall_oss/blocks_0/07_LINEAR_visual_blocks_0_mlp_up_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn3, c2hx_device[0], "BLOCK_read_data", "MVMBN3", mvmbn3_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn3_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn3_weight_bin_inf);
    bin_inf_malloc_free(mvmbn3_scales_bin_inf);
    bin_inf_malloc_free(mvmbn3_wt_bin_inf);
    bin_inf_malloc_free(mvmbn3_bias_bin_inf);
    bin_inf_malloc_free(mvmbn3_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn3_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn3_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn3_bn_wt_and_bias_in_HBM_inf, group);
#endif

#ifdef STEP_ELEMENTWISE1
    // // ******************************** STEP20 ELEMENTWISE1 ******************************** //
    // // Parameter Config
    // struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise1 = GetFPGA_HBM_ELEMENTWISE_cfg(
    //     /*Height*/ 22, /*Hin*/ 1, /*Width_in*/ 12288,
    //     /*DAT_IN_A_BASE_ADDR*/ runtime3, /*DAT_IN_B_BASE_ADDR*/ runtime2, /*DAT_OUT_BASE_ADDR*/ runtime0
    // );

    // // Input bin_inf
    // struct bin_inf* elementwise1_dat_in_A_bin_inf  = get_bin_inf(0, 22*1*12288, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE1/ELEMENTWISE1_input1.bin");
    // struct bin_inf* elementwise1_dat_in_B_bin_inf  = get_bin_inf(0, 22*1*12288, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE1/ELEMENTWISE1_input2.bin");
    // // Output bin_inf
    // struct bin_inf* *elementwise1_dat_in_A_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    // struct bin_inf* *elementwise1_dat_in_B_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // // Transform data
    // HBM_elementwise_test(cfg_elementwise1, "BLOCK_write_data/BLOCK00", "ELEMENTWISE1", elementwise1_dat_in_A_bin_inf, elementwise1_dat_in_B_bin_inf, elementwise1_dat_in_A_HBM_inf, ENABLE, elementwise1_dat_in_B_HBM_inf, ENABLE);

    // // Write data to FPGA
    // // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise1_dat_in_A_HBM_inf, group);
    // // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise1_dat_in_B_HBM_inf, group);

    // Write command to FPGA
    elementwise_step_20(user_device, run_token);

    // // Read output data from FPGA and compare
    // struct bin_inf* elementwise1_golden_out_bin_inf = get_bin_inf(0, 22*1*12288, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE1/ELEMENTWISE1_output.bin");
    // // HBM_elementwise_receive_and_compare(cfg_elementwise1, c2hx_device[0], "BLOCK_read_data", "ELEMENTWISE1", elementwise1_golden_out_bin_inf);

    // // Malloc free
    // bin_inf_malloc_free(elementwise1_dat_in_A_bin_inf);
    // bin_inf_malloc_free(elementwise1_dat_in_B_bin_inf);
    // bin_inf_malloc_free(elementwise1_golden_out_bin_inf);
    // HBM_bin_inf_malloc_free(elementwise1_dat_in_A_HBM_inf, group);
    // HBM_bin_inf_malloc_free(elementwise1_dat_in_B_HBM_inf, group);
#endif 

#ifdef STEP_MVMBN4
    // ******************************** STEP21 - MVMBN4 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn4 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 3420, /*Width_out*/ 1280,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*HBM00_WT_BASE_ADDR*/ hbm19, /*BN_BASE_ADDR*/ hbm20, /*DAT_OUT_BASE_ADDR*/ runtime0
    );

    // Input bin_inf
    struct bin_inf* mvmbn4_dat_in_bin_inf = get_bin_inf(0, run_token*3420,  "./wall_oss/blocks_0/08_LINEAR_visual_blocks_0_mlp_down_proj/input.bin");
    struct bin_inf* mvmbn4_weight_bin_inf = get_bin_inf(0, 3420*1280,       "./wall_oss/blocks_0/08_LINEAR_visual_blocks_0_mlp_down_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn4_scales_bin_inf = get_bin_inf(0, 1280*27,         "./wall_oss/blocks_0/08_LINEAR_visual_blocks_0_mlp_down_proj/scale.bin");
    struct bin_inf* mvmbn4_wt_bin_inf     = get_bin_inf(0, 1280,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn4_bias_bin_inf   = get_bin_inf(0, 1280,            "./wall_oss/blocks_0/08_LINEAR_visual_blocks_0_mlp_down_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn4_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn4, "wall_oss_run/blocks_0", "MVMBN4", mvmbn4_weight_bin_inf, mvmbn4_scales_bin_inf, mvmbn4_dat_in_bin_inf, mvmbn4_wt_bin_inf, mvmbn4_bias_bin_inf,
                    mvmbn4_wt_and_scale_in_HBM_inf, ENABLE, mvmbn4_dat_in_HBM_inf, ENABLE, mvmbn4_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_21 (user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn4_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/08_LINEAR_visual_blocks_0_mlp_down_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn4, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN4", mvmbn4_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(mvmbn4_dat_in_bin_inf);
    bin_inf_malloc_free(mvmbn4_weight_bin_inf);
    bin_inf_malloc_free(mvmbn4_scales_bin_inf);
    bin_inf_malloc_free(mvmbn4_wt_bin_inf);
    bin_inf_malloc_free(mvmbn4_bias_bin_inf);
    bin_inf_malloc_free(mvmbn4_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(mvmbn4_wt_and_scale_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn4_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(mvmbn4_bn_wt_and_bias_in_HBM_inf, group);
#endif 

#ifdef STEP_ELEMENTWISE2
   // ******************************** STEP22 ELEMENTWISE2 ******************************** //
   // Parameter Config
   struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
       /*Height*/ 22, /*Hin*/ 1, /*Width_in*/ 4096,
       /*DAT_IN_A_BASE_ADDR*/ runtime2, /*DAT_IN_B_BASE_ADDR*/ runtime1, /*DAT_OUT_BASE_ADDR*/ runtime0
   );

   // Input bin_inf
   struct bin_inf* elementwise2_dat_in_A_bin_inf  = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE2/ELEMENTWISE2_input.bin");
   struct bin_inf* elementwise2_dat_in_B_bin_inf  = get_bin_inf(0, 22*1*4096, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE2/residual.bin");
   // Output bin_inf
   struct bin_inf* *elementwise2_dat_in_A_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
   struct bin_inf* *elementwise2_dat_in_B_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

   // Transform data
   HBM_elementwise_test(cfg_elementwise2, "BLOCK_write_data/BLOCK00", "ELEMENTWISE2", elementwise2_dat_in_A_bin_inf, elementwise2_dat_in_B_bin_inf, elementwise2_dat_in_A_HBM_inf, ENABLE, elementwise2_dat_in_B_HBM_inf, ENABLE);

   // Write data to FPGA
   // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise2_dat_in_A_HBM_inf, group);
   // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], elementwise2_dat_in_B_HBM_inf, group);

   // Write command to FPGA
   elementwise_step_22(user_device, run_token);

   // Read output data from FPGA and compare
   struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen3_data/Qwen3_xiao/test_ELEMENTWISE2/ELEMENTWISE2_output.bin");
   HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "BLOCK_read_data", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);

   // Malloc free
   bin_inf_malloc_free(elementwise2_dat_in_A_bin_inf);
   bin_inf_malloc_free(elementwise2_dat_in_B_bin_inf);
   bin_inf_malloc_free(elementwise2_golden_out_bin_inf);
   HBM_bin_inf_malloc_free(elementwise2_dat_in_A_HBM_inf, group);
   HBM_bin_inf_malloc_free(elementwise2_dat_in_B_HBM_inf, group);
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