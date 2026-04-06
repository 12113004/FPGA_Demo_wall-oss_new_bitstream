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
// #include "./rw_cmd/wall_oss_debug_0322_2204.h" 
// #include "./rw_cmd/wall_oss_debug_0325_2046.h" 
// #include "./rw_cmd/wall_oss_debug_0329_2256.h" 
// #include "./rw_cmd/wall_oss_debug_0330_1805.h" 
// #include "./rw_cmd/wall_oss_debug_0401_1029.h" 
#include "./rw_cmd/wall_oss_debug_0405_2241.h" 


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
// #define STEP_MVMBN0
// #define STEP_ACT
// #define STEP_MVMBN2

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
        /*DAT_IN_BASE_ADDR*/ runtime0, /*LN_WT_BASE_ADDR*/ hbm0, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    // Input bin_inf
    struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/RMSNORM_visual_merger_ln_q/input.bin");
    struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/RMSNORM_visual_merger_ln_q/weight.bin");
    struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_ln0, "wall_oss_run/Merger", "LN1", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_ln_wt_and_bias_HBM_inf, group);

    // Write command to FPGA
    norm_step_1(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* ln0_golden_out_bin_inf = get_bin_inf(0, run_token*1*hidden_dim, "./wall_oss/RMSNORM_visual_merger_ln_q/output.bin");
    HBM_ln_receive_and_compare(cfg_ln0, c2hx_device[0], "wall_oss_run/Merger", "LN1", ln0_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(ln0_dat_in_bin_inf);
    bin_inf_malloc_free(ln0_weight_bin_inf);
    bin_inf_malloc_free(ln0_bias_bin_inf);
    // bin_inf_malloc_free(ln0_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(ln0_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(ln0_ln_wt_and_bias_HBM_inf, group); 
#endif

#ifdef STEP_MVMBN0
    // ******************************** STEP17 - MVMBN2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 2048, /*Width_out*/ 11008,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm12, /*BN_BASE_ADDR*/ hbm13, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* mvmbn2_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_gate_proj/input.bin");
    struct bin_inf* mvmbn2_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_gate_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn2_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_gate_proj/scale.bin");
    struct bin_inf* mvmbn2_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn2_bias_bin_inf   = get_bin_inf(0, 12288,            "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn2_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn2_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn2, "wall_oss_run/model_layers_0", "E0_MVMBN2", mvmbn2_weight_bin_inf, mvmbn2_scales_bin_inf, mvmbn2_dat_in_bin_inf, mvmbn2_wt_bin_inf, mvmbn2_bias_bin_inf,
                    mvmbn2_wt_and_scale_in_HBM_inf, ENABLE, mvmbn2_dat_in_HBM_inf, ENABLE, mvmbn2_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn2_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_16(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn2_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_gate_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn2, c2hx_device[0], "wall_oss_run/model_layers_0", "E0_MVMBN2", mvmbn2_golden_out_bin_inf);

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
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 11008,
        /*DAT_IN_BASE_ADDR*/ runtime2, /*WT_BASE_ADDR*/ hbm14, /*DAT_OUT_BASE_ADDR*/ runtime3
    );

    // Input bin_inf
    struct bin_inf* act_dat_in_bin_inf = get_bin_inf(0, 22*1*12288,         "./wall_oss/model_layers_0/E0_ACT/output.bin");
    // Output bin_inf
    struct bin_inf* *act_dat_in_HBM_inf       = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *act_parameter_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group/32);

    // Transform data
    HBM_act_test(cfg_act, "wall_oss_run/model_layers_0", "E0_ACT", act_dat_in_bin_inf, act_dat_in_HBM_inf, ENABLE, act_parameter_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], act_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], act_parameter_in_HBM_inf, group/32);

    // Write command to FPGA
    activate_step_17(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* act_golden_out_bin_inf = get_bin_inf(0, 0,          "./wall_oss/model_layers_0/E0_ACT/output.bin");
    HBM_act_receive_and_compare(cfg_act, c2hx_device[0], "wall_oss_run/model_layers_0", "E0_ACT", act_golden_out_bin_inf);

    // Malloc free
    bin_inf_malloc_free(act_dat_in_bin_inf );
    bin_inf_malloc_free(act_golden_out_bin_inf );
    HBM_bin_inf_malloc_free(act_dat_in_HBM_inf, group);
    HBM_bin_inf_malloc_free(act_parameter_in_HBM_inf, group/32);
#endif

#ifdef STEP_MVMBN2
    // ******************************** STEP19 - MVMBN3 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn3 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 2048, /*Width_out*/ 11008,
        /*DAT_IN_BASE_ADDR*/ runtime0, /*HBM00_WT_BASE_ADDR*/ hbm15, /*BN_BASE_ADDR*/ hbm16, /*DAT_OUT_BASE_ADDR*/ runtime2
    );

    // Input bin_inf
    struct bin_inf* mvmbn3_dat_in_bin_inf = get_bin_inf(0, 22*1*4096,        "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_up_proj/input.bin");
    struct bin_inf* mvmbn3_weight_bin_inf = get_bin_inf(0, 12288*4096,       "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_up_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn3_scales_bin_inf = get_bin_inf(0, 12288*32,         "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_up_proj/scale.bin");
    struct bin_inf* mvmbn3_wt_bin_inf     = get_bin_inf(0, 12288,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn3_bias_bin_inf   = get_bin_inf(0, 12288,            "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn3_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn3_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn3, "wall_oss_run/model_layers_0", "E0_MVMBN3", mvmbn3_weight_bin_inf, mvmbn3_scales_bin_inf, mvmbn3_dat_in_bin_inf, mvmbn3_wt_bin_inf, mvmbn3_bias_bin_inf,
                    mvmbn3_wt_and_scale_in_HBM_inf, ENABLE, mvmbn3_dat_in_HBM_inf, ENABLE, mvmbn3_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_dat_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_wt_and_scale_in_HBM_inf, group);
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn3_bn_wt_and_bias_in_HBM_inf, group);

    // Write command to FPGA
    mvm_f16xi4_step_18(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn3_golden_out_bin_inf = get_bin_inf(0, 22*1*4096, "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_0_up_proj/output.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn3, c2hx_device[0], "wall_oss_run/model_layers_0", "E0_MVMBN3", mvmbn3_golden_out_bin_inf);

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

}