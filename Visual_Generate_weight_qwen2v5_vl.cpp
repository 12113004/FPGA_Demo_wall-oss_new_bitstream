#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

// Read and write command 
// #include "./rw_cmd/1128/glm1128_MAXTOKEN2048_wt2hbm_longsqe8.h"
#include "./Visual_test_2048_wt2hbm_0128_1114.h"
// #include "./rw_cmd/1128/qwen2_2048_lite_wt2hbm_for_1128_bitstream_run_new_addr_new_cfg_final_liuhang.h"
// #include "./rw_cmd/1128/qwen2_2048_wt2hbm_0106_0947.h"

// Tests
#include "./tests/HBM_ln_test.cpp"
#include "./tests/HBM_emb_qwen_test.cpp"
#include "./tests/HBM_softmax_test.cpp"
#include "./tests/HBM_act_test.cpp"
#include "./tests/HBM_mvm_bn_test.cpp"
#include "./tests/HBM_mvm_bn_res_test.cpp"
#include "./tests/Argmax.cpp"

// RW function
#include "./tests/rw_function.cpp"

// STEP switch
#define STEP1_LN0
#define STEP2_MVMBN0_Q
#define STEP3_EMB_Q
#define STEP4_MVMBN0_K
#define STEP5_EMB_K
#define STEP6_KV2HBM_K
#define STEP7_TRP
#define STEP8_SOFTMAX
#define STEP9_MVMBN0_V
#define STEP10_KV2HBM_V
#define STEP11_F2W
#define STEP12_MVMBNRES0

#define STEP13_LN1
#define STEP14_MVMBN1
#define STEP15_ACT
#define STEP16_MVMBNRES1
#define STEP17_MVMBNRES2
// #define STEP596_LN_Outlayer
// #define STEP597_MVMBN_Argmax

// ******************************** Config ******************************** //
int run_token    = 768;
int last_token   = 0;
int kvcache_mode = (run_token-last_token == 1)? 1 : 0;

void generate_wt_emb_and_act()
{
//    // ******************************** STEP4 - EMBQ ******************************** //
    // Parameter Config
    struct FPGA_HBM_EMB_cfg CFG_EMB_Q = GetFPGA_HBM_EMB_cfg(
        // /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 2048, /*Pos_in_Num*/ 2048, /*Pos_in_CHin*/ 128,
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in(CHin)*/ 2048, /*Pos_in_Num(MAX TOKEN)*/ 2048, /*Pos_in_CHin*/ 128,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime2,
        /*POS_IN_BASE_ADDR*/  ddr_base_addr+global0,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime3
    );

    // Input bin_inf
    struct bin_inf* emb_q_dat_in_bin_inf = get_bin_inf(0, 1*768*2048, "./qwen2_5_vl/vision_block00/test_q_rope_expand2/q_linear_out_expanded.bin");
    struct bin_inf* emb_q_pos_in_bin_inf = get_bin_inf(0, 2048*128,   "./qwen2_5_vl/vision_block00/cos_sin/cos_sin.bin"); 
    // Output bin_inf
    struct bin_inf* emb_q_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* emb_q_pos_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_emb_test(CFG_EMB_Q, "BLOCK_write_data_qwen2v5_vl", "EMB", emb_q_dat_in_bin_inf, emb_q_pos_in_bin_inf, &emb_q_dat_in_DDR_inf, DISABLE, &emb_q_pos_in_DDR_inf, ENABLE);

    // Malloc free
    bin_inf_malloc_free(emb_q_dat_in_bin_inf);
    bin_inf_malloc_free(emb_q_pos_in_bin_inf);
    
//    // ******************************** STEP15 - ACT ******************************** //
    // Parameter Config
    struct FPGA_HBM_ACT_cfg CFG_ACT = GetFPGA_HBM_ACT_cfg(
        /*Height*/ run_token, /*Width_in*/ 3456, /*Hin*/ 1,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime3,
        /*WT_BASE_ADDR*/      ddr_base_addr+weight9,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime4
    );

    // Input bin_inf
    struct bin_inf* act_dat_in_bin_inf = get_bin_inf(0, 1*768*3456, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_gate_proj_output.bin");
    // Output bin_inf
    struct bin_inf* act_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* act_parameter_in_DDR_inf  = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_act_test(CFG_ACT, "BLOCK_write_data_qwen2v5_vl", "ACT", act_dat_in_bin_inf, &act_dat_in_DDR_inf, ENABLE, &act_parameter_in_DDR_inf, ENABLE);

    // Malloc free
    bin_inf_malloc_free(act_dat_in_bin_inf );
    bin_inf_malloc_free(act_parameter_in_DDR_inf);

    // ******************************** STEP0 - MVMCONV3D ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMCONV3D = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1176, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm0,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );
    
    // Input bin_inf
    struct bin_inf* mvmconv3d_dat_in_bin_inf    = get_bin_inf(0, 768*1176, "./qwen2_5_vl/test_MVMCONV3D/MVMCONV3D_read_data_bin2/conv3d_input.bin"); 
    struct bin_inf* mvmconv3d_wt_in_bin_inf     = get_bin_inf(0, 1280*1176, "./qwen2_5_vl/test_MVMCONV3D/MVMCONV3D_read_data_bin2/conv3d.weights.bin"); 
    struct bin_inf* mvmconv3d_scales_in_bin_inf = get_bin_inf(0, 1280*10,   "./qwen2_5_vl/test_MVMCONV3D/MVMCONV3D_read_data_bin2/conv3d.scales.bin");
    struct bin_inf* mvmconv3d_bn_wt_in_bin_inf  = get_bin_inf(0, 1280,      "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmconv3d_bias_in_bin_inf   = get_bin_inf(0, 1280,      "./rw_data/bn_and_k_bias_0.bin");
    
    // // Output bin_inf
    struct bin_inf* *mvmconv3d_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmconv3d_dat_in_DDR_inf            = (struct bin_inf*) malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmconv3d_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*) malloc(sizeof(struct bin_inf));

    // Transform data to specific folders
    HBM_mvmbn_test( CFG_MVMCONV3D, 
                    "BLOCK_write_data_qwen2v5_vl/VL_MVMCONV3D", "MVMCONV3D",  //文件夹，文件名前缀
                    mvmconv3d_wt_in_bin_inf, 
                    mvmconv3d_scales_in_bin_inf, 
                    mvmconv3d_dat_in_bin_inf, 
                    mvmconv3d_bn_wt_in_bin_inf, 
                    mvmconv3d_bias_in_bin_inf,
                    mvmconv3d_wt_and_scale_in_HBM_inf, ENABLE, 
                    &mvmconv3d_dat_in_DDR_inf, ENABLE, 
                    &mvmconv3d_bn_wt_and_bias_in_DDR_inf, ENABLE);
}

void generate_wt_BLOCK(char* name1, char* name2, char* name3, int index)
{
    char** filename = (char**)malloc(sizeof(char*)*4);
    for(int i=0;i<4;i++)
        filename[i] = (char*)malloc(sizeof(char)*200);
    sprintf(filename[0], "%s%s", name2, name3);

    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg CFG_LN0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 1280, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr + runtime1, //data_in = ddr_base_addr + runtime0
        /*LN_WT_BASE_ADDR*/   ddr_base_addr + weight1, //weight0 = 0, 
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr + runtime0
    );
   
    sprintf(filename[1], "./rw_data/%s%s/block%02d.rms_norm1.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* ln0_dat_in_bin_inf  = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_mlp/vision_block0_output.bin");
    // struct bin_inf* ln0_dat_in_bin_inf  = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/test_MVMCONV3D/MVMCONV3D_read_data_bin/conv3d_block_output.bin");
    struct bin_inf* ln0_wt_in_bin_inf   = get_bin_inf(0, 1280, filename[1]);
    struct bin_inf* ln0_bias_in_bin_inf = get_bin_inf(0, 1280, "./rw_data/bn_and_k_bias_0.bin");
    
    // Output bin_inf
    struct bin_inf* ln0_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln0_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    
    // Transform data
    HBM_ln_test(CFG_LN0, filename[0], "LN0", ln0_dat_in_bin_inf, ln0_wt_in_bin_inf, ln0_bias_in_bin_inf, &ln0_dat_in_DDR_inf, DISABLE, &ln0_ln_wt_and_bias_in_DDR_inf, ENABLE);

    // ******************************** STEP2 - MVMBN0_Q ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_Q = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,      //reg 10
        /*HBM00_WT_BASE_ADDR*/ hbm1,                        //reg 11
        /*BN_BASE_ADDR*/       ddr_base_addr+weight2,       //reg 26
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2       //reg 13
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.self_attn.q_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.self_attn.q_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.self_attn.q_proj.bias.bin", name1, name3, index);
    
    // Input bin_inf
    struct bin_inf* mvmbn0_q_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/test_block00/test_LN0/vision_block_qkv_input.bin");
    struct bin_inf* mvmbn0_q_weight_in_bin_inf = get_bin_inf(0, 2048*1280,  filename[1]); 
    struct bin_inf* mvmbn0_q_scales_in_bin_inf = get_bin_inf(0, 2048*10,    filename[2]);
    struct bin_inf* mvmbn0_q_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_q_bias_in_bin_inf   = get_bin_inf(0, 2048,       filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_q_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_q_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_q_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_Q, filename[0], "MVMBN0_q", mvmbn0_q_weight_in_bin_inf, mvmbn0_q_scales_in_bin_inf, mvmbn0_q_dat_in_bin_inf, mvmbn0_q_wt_in_bin_inf, mvmbn0_q_bias_in_bin_inf,
        mvmbn0_q_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_q_dat_in_DDR_inf, DISABLE, &mvmbn0_q_bn_wt_and_bias_in_DDR_inf, ENABLE);

//    // ******************************** STEP4 - MVMBN0_K ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_K = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm2,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight3,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    );
    
    sprintf(filename[1], "./rw_data/%s%s/block%02d.self_attn.k_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.self_attn.k_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.self_attn.k_proj.bias.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* mvmbn0_k_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/test_block00/test_LN0/vision_block_qkv_input.bin");
    struct bin_inf* mvmbn0_k_weight_in_bin_inf = get_bin_inf(0, 2048*1280,   filename[1]); 
    struct bin_inf* mvmbn0_k_scales_in_bin_inf = get_bin_inf(0, 2048*10,     filename[2]);
    struct bin_inf* mvmbn0_k_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_k_bias_in_bin_inf   = get_bin_inf(0, 2048,        filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_k_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_k_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_k_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_K, filename[0], "MVMBN0_k", mvmbn0_k_weight_in_bin_inf, mvmbn0_k_scales_in_bin_inf, mvmbn0_k_dat_in_bin_inf, mvmbn0_k_wt_in_bin_inf, mvmbn0_k_bias_in_bin_inf,
                    mvmbn0_k_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_k_dat_in_DDR_inf, DISABLE, &mvmbn0_k_bn_wt_and_bias_in_DDR_inf, ENABLE);

//    // ******************************** STEP9 - MVMBN0_V ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_V = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm3,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight4,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.self_attn.v_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.self_attn.v_proj.scales.bin",  name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.self_attn.v_proj.bias.bin",    name1, name3, index);
    // Input bin_inf
    struct bin_inf* mvmbn0_v_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/test_block00/test_LN0/vision_block_qkv_input.bin");
    struct bin_inf* mvmbn0_v_weight_in_bin_inf = get_bin_inf(0, 2048*1280,   filename[1]); 
    struct bin_inf* mvmbn0_v_scales_in_bin_inf = get_bin_inf(0, 2048*10,     filename[2]);
    struct bin_inf* mvmbn0_v_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_v_bias_in_bin_inf   = get_bin_inf(0, 2048,        filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn0_v_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_v_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_v_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_V, filename[0], "MVMBN0_v", mvmbn0_v_weight_in_bin_inf, mvmbn0_v_scales_in_bin_inf, mvmbn0_v_dat_in_bin_inf, mvmbn0_v_wt_in_bin_inf, mvmbn0_v_bias_in_bin_inf,
                    mvmbn0_v_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_v_dat_in_DDR_inf, DISABLE, &mvmbn0_v_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // ******************************** STEP12 - MVMBNRES0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES0 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 2048, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime2,
        /*HBM00_WT_BASE_ADDR*/ hbm4,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight5,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime1,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime0
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.self_attn.o_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.self_attn.o_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.self_attn.o_proj.bias.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* mvmbnres0_dat_in_bin_inf         = get_bin_inf(0, 1*768*2048, "./qwen2_5_vl/vision_block00/test_o_proj_quant/attn_output_save_expanded.bin");
    struct bin_inf* mvmbnres0_weight_in_bin_inf      = get_bin_inf(0, 1280*2048,  filename[1]); 
    struct bin_inf* mvmbnres0_scales_in_bin_inf      = get_bin_inf(0, 1280*16,    filename[2]);
    struct bin_inf* mvmbnres0_wt_in_bin_inf          = get_bin_inf(0, 1280,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres0_bias_in_bin_inf        = get_bin_inf(0, 1280,       filename[3]);
    struct bin_inf* mvmbnres0_res_add_dat_in_bin_inf = get_bin_inf(0, 1280,       "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_residual1.bin");
    // Output bin_inf
    struct bin_inf* *mvmbnres0_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres0_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES0, filename[0], "MVMBNRES0_o", mvmbnres0_weight_in_bin_inf, mvmbnres0_scales_in_bin_inf, mvmbnres0_dat_in_bin_inf, mvmbnres0_wt_in_bin_inf, mvmbnres0_bias_in_bin_inf, mvmbnres0_res_add_dat_in_bin_inf,
                    mvmbnres0_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres0_dat_in_DDR_inf, DISABLE, &mvmbnres0_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres0_res_add_dat_in_DDR_inf, DISABLE);

//    // ******************************** STEP13 - LN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg CFG_LN1 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 1280, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr + runtime0,
        /*LN_WT_BASE_ADDR*/   ddr_base_addr + weight6,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr + runtime1
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.post_attention_layernorm.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* ln1_dat_in_bin_inf  = get_bin_inf(0, 768*1280, "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_after_o_proj.bin");
    struct bin_inf* ln1_wt_in_bin_inf   = get_bin_inf(0, 1280,     filename[1]);
    struct bin_inf* ln1_bias_in_bin_inf = get_bin_inf(0, 1280,     "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* ln1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln1_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_ln_test(CFG_LN1, filename[0], "LN1", ln1_dat_in_bin_inf, ln1_wt_in_bin_inf, ln1_bias_in_bin_inf, &ln1_dat_in_DDR_inf, DISABLE, &ln1_ln_wt_and_bias_in_DDR_inf, ENABLE);

//    // ******************************** STEP14 - MVMBN1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN1 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime1,
        /*HBM00_WT_BASE_ADDR*/ hbm6,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight8,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime3
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.mlp.gate_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.mlp.gate_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.mlp.gate_proj.bias.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* mvmbn1_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_gate_proj_input.bin");
    struct bin_inf* mvmbn1_weight_in_bin_inf = get_bin_inf(0, 3456*1280, filename[1]); 
    struct bin_inf* mvmbn1_scales_in_bin_inf = get_bin_inf(0, 3456*10,   filename[2]);
    struct bin_inf* mvmbn1_wt_in_bin_inf     = get_bin_inf(0, 3456,      "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn1_bias_in_bin_inf   = get_bin_inf(0, 3456,      filename[3]);
    // Output bin_inf
    struct bin_inf* *mvmbn1_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn1_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN1, filename[0], "MVMBN1_gate", mvmbn1_weight_in_bin_inf, mvmbn1_scales_in_bin_inf, mvmbn1_dat_in_bin_inf, mvmbn1_wt_in_bin_inf, mvmbn1_bias_in_bin_inf,
                    mvmbn1_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn1_dat_in_DDR_inf, DISABLE, &mvmbn1_bn_wt_and_bias_in_DDR_inf, ENABLE);

//    // ******************************** STEP16 - MVMBNRES1 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES1 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime1,
        /*HBM00_WT_BASE_ADDR*/ hbm5,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight7,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime4,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime3
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.mlp.up_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.mlp.up_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.mlp.up_proj.bias.bin", name1, name3, index);

    // Input bin_inf
    struct bin_inf* mvmbnres1_dat_in_bin_inf         = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_gate_proj_input.bin");
    struct bin_inf* mvmbnres1_weight_in_bin_inf      = get_bin_inf(0, 3456*1280,   filename[1]); 
    struct bin_inf* mvmbnres1_scales_in_bin_inf      = get_bin_inf(0, 3456*10,     filename[2]);
    struct bin_inf* mvmbnres1_wt_in_bin_inf          = get_bin_inf(0, 3456,        "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres1_bias_in_bin_inf        = get_bin_inf(0, 3456,        filename[3]);
    struct bin_inf* mvmbnres1_res_add_dat_in_bin_inf = get_bin_inf(0, 768*3456,   "./qwen2_5_vl/vision_block00/test_mlp/vision_block_act_fn_output.bin"); //
    // Output bin_inf
    struct bin_inf* *mvmbnres1_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres1_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres1_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES1, filename[0], "MVMBNRES1_up", mvmbnres1_weight_in_bin_inf, mvmbnres1_scales_in_bin_inf, mvmbnres1_dat_in_bin_inf, mvmbnres1_wt_in_bin_inf, mvmbnres1_bias_in_bin_inf, mvmbnres1_res_add_dat_in_bin_inf,
                mvmbnres1_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres1_dat_in_DDR_inf, DISABLE, &mvmbnres1_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres1_res_add_dat_in_DDR_inf, DISABLE);

   // ******************************** STEP17 - MVMBNRES2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES2 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 3456, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime3,
        /*HBM00_WT_BASE_ADDR*/ hbm7,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight10,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );

    sprintf(filename[1], "./rw_data/%s%s/block%02d.mlp.down_proj.weights.bin", name1, name3, index);
    sprintf(filename[2], "./rw_data/%s%s/block%02d.mlp.down_proj.scales.bin", name1, name3, index);
    sprintf(filename[3], "./rw_data/%s%s/block%02d.mlp.down_proj.bias.bin", name1, name3, index);
    // Input bin_inf
    struct bin_inf* mvmbnres2_dat_in_bin_inf         = get_bin_inf(0, 1*768*3456, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_up_mul_output.bin");
    struct bin_inf* mvmbnres2_weight_in_bin_inf      = get_bin_inf(0, 1280*3456,   filename[1]); 
    struct bin_inf* mvmbnres2_scales_in_bin_inf      = get_bin_inf(0, 1280*27,     filename[2]);
    struct bin_inf* mvmbnres2_wt_in_bin_inf          = get_bin_inf(0, 1280,        "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres2_bias_in_bin_inf        = get_bin_inf(0, 1280,        filename[3]);
    struct bin_inf* mvmbnres2_res_add_dat_in_bin_inf = get_bin_inf(0, 1280,         "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_after_o_proj.bin");
    // Output bin_inf
    struct bin_inf* *mvmbnres2_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf*  mvmbnres2_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf*  mvmbnres2_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf*  mvmbnres2_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES2, filename[0], "MVMBNRES2_down",  mvmbnres2_weight_in_bin_inf, mvmbnres2_scales_in_bin_inf, mvmbnres2_dat_in_bin_inf, mvmbnres2_wt_in_bin_inf, mvmbnres2_bias_in_bin_inf, mvmbnres2_res_add_dat_in_bin_inf,
                mvmbnres2_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres2_dat_in_DDR_inf, DISABLE, &mvmbnres2_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres2_res_add_dat_in_DDR_inf, DISABLE);
   
}

void generate_wt_merger()
{
    // ******************************** STEP18_LN2 ******************************** //
    struct FPGA_HBM_LN_cfg CFG_LN1 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 1280, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr + runtime1,
        /*LN_WT_BASE_ADDR*/   ddr_base_addr + weight321,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr + runtime0
    );

    // Input bin_inf
    // struct bin_inf* ln1_dat_in_bin_inf  = get_bin_inf(0, 768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_norm2_input.bin");
    struct bin_inf* ln1_dat_in_bin_inf  = get_bin_inf(0, 768*1280, "./qwen2_5_vl/vision_block00/test_merge/merge_input.bin");
    struct bin_inf* ln1_wt_in_bin_inf   = get_bin_inf(0, 1280,      "./qwen2_5_vl/vision_block00/test_merge/merge_norm_weight.bin");
    struct bin_inf* ln1_bias_in_bin_inf = get_bin_inf(0, 1280,      "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* ln1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln1_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
 
    // Transform data 
    HBM_ln_test(CFG_LN1, "BLOCK_write_data_qwen2v5_vl/Merge", "LN2", ln1_dat_in_bin_inf, ln1_wt_in_bin_inf, ln1_bias_in_bin_inf, &ln1_dat_in_DDR_inf, ENABLE, &ln1_ln_wt_and_bias_in_DDR_inf, ENABLE);
    // ******************************** STEP19_MVMMERGE1 ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN_MERGER = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token/4, /*Win*/ 1, /*Width_in*/ 1280*4, /*Width_out*/ 5120, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm225,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight322,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );

    // Input bin_inf 
    struct bin_inf* mvmbn_m1_dat_in_bin_inf    = get_bin_inf(0, 1*192*5120, "./BLOCK_read_data/BLOCK00/LN_FPGA_out_bin/LN2_dat_out_demaped.bin");
    // struct bin_inf* mvmbn_m1_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_merge/merge_norm_out.bin");
    struct bin_inf* mvmbn_m1_weight_in_bin_inf = get_bin_inf(0, 5120*5120,  "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc1.weights.bin"); 
    struct bin_inf* mvmbn_m1_scales_in_bin_inf = get_bin_inf(0, 5120*40,      "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc1.scales.bin");
    struct bin_inf* mvmbn_m1_wt_in_bin_inf     = get_bin_inf(0, 5120,         "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn_m1_bias_in_bin_inf   = get_bin_inf(0, 5120,         "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc1.bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn_m1_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn_m1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn_m1_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN_MERGER, "BLOCK_write_data_qwen2v5_vl/Merge", "MVMBN0_m1", mvmbn_m1_weight_in_bin_inf, mvmbn_m1_scales_in_bin_inf, mvmbn_m1_dat_in_bin_inf, mvmbn_m1_wt_in_bin_inf, mvmbn_m1_bias_in_bin_inf,
                    mvmbn_m1_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn_m1_dat_in_DDR_inf, ENABLE, &mvmbn_m1_bn_wt_and_bias_in_DDR_inf, ENABLE);
    // ******************************** STEP20_GELU ******************************** //
    struct FPGA_HBM_ACT_cfg CFG_ACT_GELU = GetFPGA_HBM_ACT_cfg(
        /*Height*/ run_token/4, /*Width_in*/ 5120, /*Hin*/ 1,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime1,
        /*WT_BASE_ADDR*/      ddr_base_addr+global1,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime0
    );

    // Input bin_inf
    struct bin_inf* gelu_dat_in_bin_inf = get_bin_inf(0, 1*768*5120, "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc1_out.bin");
    // Output bin_inf
    struct bin_inf* gelu_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* gelu_parameter_in_DDR_inf  = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_act_test(CFG_ACT_GELU, "BLOCK_write_data_qwen2v5_vl", "GELU", gelu_dat_in_bin_inf, &gelu_dat_in_DDR_inf, ENABLE, &gelu_parameter_in_DDR_inf, ENABLE);
    // ******************************** STEP21_MVMMERGE2 ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN_MERGER2 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token/4, /*Win*/ 1, /*Width_in*/ 5120, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm226,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight323,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );

    // Input bin_inf 
    struct bin_inf* mvmbn_m2_dat_in_bin_inf    = get_bin_inf(0, 1*192*5120, "./qwen2_5_vl/vision_block00/test_merge/merge_gelu_out.bin");
    struct bin_inf* mvmbn_m2_weight_in_bin_inf = get_bin_inf(0, 2048*5120,  "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc2.weights.bin"); 
    struct bin_inf* mvmbn_m2_scales_in_bin_inf = get_bin_inf(0, 2048*40,      "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc2.scales.bin");
    struct bin_inf* mvmbn_m2_wt_in_bin_inf     = get_bin_inf(0, 2048,         "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn_m2_bias_in_bin_inf   = get_bin_inf(0, 2048,         "./qwen2_5_vl/vision_block00/test_merge/merge_mlp_fc2.bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn_m2_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn_m2_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn_m2_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN_MERGER2, "BLOCK_write_data_qwen2v5_vl/Merge", "MVMBN0_m2", mvmbn_m2_weight_in_bin_inf, mvmbn_m2_scales_in_bin_inf, mvmbn_m2_dat_in_bin_inf, mvmbn_m2_wt_in_bin_inf, mvmbn_m2_bias_in_bin_inf,
                    mvmbn_m2_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn_m2_dat_in_DDR_inf, ENABLE, &mvmbn_m2_bn_wt_and_bias_in_DDR_inf, ENABLE);

}

int __cdecl main()
{
    int  model_part      = 0;
    char *read_filename  = "qwen2v5_vl_all_weights/";
    char *write_filename = "BLOCK_write_data_qwen2v5_vl/";
    char BLOCK_name[20];

    switch(model_part)
    {
        case 0:
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
            generate_wt_merger();
        break;
        default:;
        break;
    }
}

