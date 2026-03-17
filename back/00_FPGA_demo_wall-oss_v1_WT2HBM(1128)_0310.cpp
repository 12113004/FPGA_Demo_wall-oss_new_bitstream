#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"

// Read and write command 
// #include "./rw_cmd/1128/wall_oss_2048_wt2hbm_debug_0316_0036.h"
#include "./rw_cmd/1128/wall_oss_2048_wt2hbm_debug_0315_0230.h" // MVM_BN_Res-mask
// #include "./rw_cmd/1128/wall_oss_2048_wt2hbm_debug_0314_2101.h" // elementwise-mask
// #include "./rw_cmd/1128/wall_oss_2048_wt2hbm_debug_0313_1720.h"  // un-mask

// Tests
#include "./tests/HBM_ln_test.cpp"
#include "./tests/HBM_emb_qwen_test.cpp"
#include "./tests/HBM_softmax_test.cpp"
#include "./tests/HBM_act_test.cpp"
#include "./tests/HBM_mvm_bn_test.cpp"
#include "./tests/HBM_mvm_bn_res_test.cpp"
#include "./tests/Argmax.cpp"
#include "./tests/HBM_elementwise_test.cpp"

// RW function
#include "./tests/rw_function.cpp"
// void qwen2_5_vl_all(HANDLE& user_device, HANDLE c2hx_device[NUM_OF_RW_CH], HANDLE h2cx_device[NUM_OF_RW_CH] , int run_token, int last_token) ;

// STEP switch
// #define STEP0_MVMCONV3D
// #define STEP1_LN0
// #define STEP2_MVMBN0_Q
// #define STEP3_EMB_Q
// #define STEP4_MVMBN0_K
// #define STEP5_EMB_K
// #define STEP6_KV2HBM_K
// #define STEP7_TRP
#define STEP7_5_MASK
// #define STEP8_SOFTMAX
// #define STEP9_MVMBN0_V
// #define STEP10_KV2HBM_V
// #define STEP11_F2W
// #define STEP12_MVMBNRES0

// #define STEP13_LN1
// #define STEP14_MVMBN1
// #define STEP15_ACT
// #define STEP16_MVMBNRES1
// #define STEP17_MVMBNRES2

// #define STEP596_LN_Outlayer
// #define STEP597_MVMBN_Argmax

#define ALL_STEP

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

    int embedded_input_token       = 0;
    int BLOCK00_BLOCK26_compare_en = 0;
    int kvcache_mode               = 0;
    int run_token                  = 0;
    int last_token                 = 0;
    int index                      = 0;



    // ******************************** Write 0 to DDR ******************************** //
    // uint32_t size;
    // for(int i=0; i<100; i++)
    //     size = init(h2cx_device[0], ddr_base_addr+20000000*i, 20000000);
    
//     // ******************************** Prepare&Write&Verify Data  ******************************** //
    // qwen2_5_vl_2048_wt2hbm_debug_0309_1033_load_params(user_device, h2cx_device[0], ".");
    // qwen2_5_2048_wt2hbm_debug_0122_1736_load_params(user_device, h2cx_device[0], ".");

//     // ******************************** Config ******************************** //
    run_token    = 648;
    last_token   = 0;
    kvcache_mode = (run_token-last_token == 1)? 1 : 0;

    // qwen2_5_2048_wt2hbm_debug_0106_1423_load_params(user_device, h2cx_device[0], ".");
#ifdef STEP0_MVMCONV3D

    struct FPGA_HBM_MVM_BN_cfg CFG_MVMCONV3D = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1176, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm0,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );
    
    // Input bin_inf
    struct bin_inf* mvmconv3d_dat_in_bin_inf    = get_bin_inf(0, 648*1176,  "./wall_oss/01_CONV3D_visual_patch_embed_proj/input.bin"); 
    struct bin_inf* mvmconv3d_wt_in_bin_inf     = get_bin_inf(0, 1176*1280, "./wall_oss/01_CONV3D_visual_patch_embed_proj/weight_int4.bin"); 
    struct bin_inf* mvmconv3d_scales_in_bin_inf = get_bin_inf(0, 1280*10,   "./wall_oss/01_CONV3D_visual_patch_embed_proj/scale.bin");
    struct bin_inf* mvmconv3d_bn_wt_in_bin_inf  = get_bin_inf(0, 1280,      "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmconv3d_bias_in_bin_inf   = get_bin_inf(0, 1280,      "./rw_data/bn_and_k_bias_0.bin");
    
    // // Output bin_inf
    struct bin_inf* *mvmconv3d_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmconv3d_dat_in_DDR_inf            = (struct bin_inf*) malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmconv3d_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*) malloc(sizeof(struct bin_inf));

    // Transform data to specific folders
    HBM_mvmbn_test( CFG_MVMCONV3D, 
                    "wall_oss_run/01_CONV3D_visual_patch_embed_proj", "MVMCONV3D",  //文件夹，文件名前缀
                    mvmconv3d_wt_in_bin_inf, 
                    mvmconv3d_scales_in_bin_inf, 
                    mvmconv3d_dat_in_bin_inf, 
                    mvmconv3d_bn_wt_in_bin_inf, 
                    mvmconv3d_bias_in_bin_inf,
                    mvmconv3d_wt_and_scale_in_HBM_inf, ENABLE, 
                    &mvmconv3d_dat_in_DDR_inf, ENABLE, 
                    &mvmconv3d_bn_wt_and_bias_in_DDR_inf, ENABLE);
      
    // Write weight data to FPGA  patch_embed_bn_load_param
    patch_embed_wt_load_param(user_device, h2cx_device[0], "."); //把wt数据先写入DDR，然后再搬移到32个HBM memory中
    // patch_embed_bn_load_param(h2cx_device[0], "."); //把wt数据先写入DDR，然后再搬移到32个HBM memory中

    h2cx_device_write_bin(h2cx_device[0], mvmconv3d_dat_in_DDR_inf); //把转换好的input数据通过PCIE端口写入
    verify_data_write(c2hx_device[0], mvmconv3d_dat_in_DDR_inf); 

    h2cx_device_write_bin(h2cx_device[0], mvmconv3d_bn_wt_and_bias_in_DDR_inf); ////把转换好的bn_wt_bias数据通过PCIE端口写入
    verify_data_write(c2hx_device[0], mvmconv3d_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_step_1(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmcon3d_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/01_CONV3D_visual_patch_embed_proj/output.bin");
    HBM_mvmbn_receive_and_compare(CFG_MVMCONV3D, c2hx_device[0], "./wall_oss_run/01_CONV3D_visual_patch_embed_proj", "MVMCONV3D", mvmcon3d_golden_out_bin_inf);
    printf("mvmcon3d debug done!");
#endif


#ifdef STEP1_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    struct FPGA_HBM_LN_cfg CFG_LN0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 1280, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr + runtime1, //data_in = ddr_base_addr + runtime0
        /*LN_WT_BASE_ADDR*/   ddr_base_addr + weight1, //weight0 = 0, 
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr + runtime0
    );

    // Input bin_inf
    struct bin_inf* ln0_dat_in_bin_inf  = get_bin_inf(0, 1*648*1280,    "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/input.bin");
    struct bin_inf* ln0_wt_in_bin_inf   = get_bin_inf(0, 1280,          "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/weight.bin");
    struct bin_inf* ln0_bias_in_bin_inf = get_bin_inf(0, 1280,          "./rw_data/bn_and_k_bias_0.bin");
    
    // Output bin_inf
    struct bin_inf* ln0_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln0_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    
    // Transform data
    HBM_ln_test(CFG_LN0, "wall_oss_run/blocks_0", "LN0", ln0_dat_in_bin_inf, ln0_wt_in_bin_inf, ln0_bias_in_bin_inf, &ln0_dat_in_DDR_inf, ENABLE, &ln0_ln_wt_and_bias_in_DDR_inf, ENABLE);

    // wall_oss_runln1_bias_load_param(h2cx_device[0], ".");

    // Write data to FPGA
    h2cx_device_write_bin(h2cx_device[0], ln0_dat_in_DDR_inf); //把transform好的数据写入
    verify_data_write(c2hx_device[0], ln0_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], ln0_ln_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], ln0_ln_wt_and_bias_in_DDR_inf);
    
    // Write command to FPGA
    layer_norm_step_2(user_device, run_token);

    // Output bin_inf   Read output data from FPGA and compare
    struct bin_inf* ln0_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/02_RMSNORM_visual_blocks_0_norm1/output.bin");
    HBM_ln_receive_and_compare(CFG_LN0, c2hx_device[0], "wall_oss_run/blocks_0", "LN0", ln0_golden_out_bin_inf);

    printf("ln0 debug done!");
#endif

#ifdef STEP2_MVMBN0_Q
    // ******************************** STEP2 - MVMBN0_Q ******************************** //
    // struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_Q = GetFPGA_HBM_MVM_BN_cfg(
    //     /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 2048, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
    //     /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime1,
    //     /*HBM00_WT_BASE_ADDR*/ hbm0,
    //     /*BN_BASE_ADDR*/       ddr_base_addr+weight1,
    //     /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    // );

    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_Q = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,      //reg 10
        /*HBM00_WT_BASE_ADDR*/ hbm1,                        //reg 11
        /*BN_BASE_ADDR*/       ddr_base_addr+weight2,       //reg 26
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2       //reg 13
    );

    // Input bin_inf
    // struct bin_inf* mvmbn0_q_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_q_expanded/hidden_states_for_qkv.bin");
    struct bin_inf* mvmbn0_q_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/input.bin");
    struct bin_inf* mvmbn0_q_weight_in_bin_inf = get_bin_inf(0, 2048*1280,  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_q_scales_in_bin_inf = get_bin_inf(0, 2048*10,    "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/scale.bin");
    struct bin_inf* mvmbn0_q_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_q_bias_in_bin_inf   = get_bin_inf(0, 2048,       "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_q_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_q_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_q_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_Q, "wall_oss_run/blocks_0", "MVMBN0_Q", mvmbn0_q_weight_in_bin_inf, mvmbn0_q_scales_in_bin_inf, mvmbn0_q_dat_in_bin_inf, mvmbn0_q_wt_in_bin_inf, mvmbn0_q_bias_in_bin_inf,
                   mvmbn0_q_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_q_dat_in_DDR_inf, ENABLE, &mvmbn0_q_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // Write wt data to FPGA
    // wall_oss_runq_weight_load_param(user_device, h2cx_device[0], "."); 
    
    // // Write input data to FPGA    
    // h2cx_device_write_bin(h2cx_device[0], mvmbn0_q_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbn0_q_dat_in_DDR_inf);
    
    // Write bn_wt_and_bias data to FPGA        
    h2cx_device_write_bin(h2cx_device[0], mvmbn0_q_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn0_q_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_step_3(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn0_q_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_q_proj/output.bin");
    
    HBM_mvmbn_receive_and_compare(CFG_MVMBN0_Q, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_Q", mvmbn0_q_golden_out_bin_inf);
    printf("mvmbn_q debug done!");
#endif


#ifdef STEP3_EMB_Q
    // ******************************** STEP3 - EMB_Q ******************************** //
    struct FPGA_HBM_EMB_cfg CFG_EMB_Q = GetFPGA_HBM_EMB_cfg(
        // /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 2048, /*Pos_in_Num*/ 2048, /*Pos_in_CHin*/ 128,
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in(CHin)*/ 2048, /*Pos_in_Num(MAX TOKEN)*/ 2048, /*Pos_in_CHin*/ 128,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime2,
        /*POS_IN_BASE_ADDR*/  ddr_base_addr+global0,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime3
    );
    // Input bin_inf ./qwen2_5_vl/vision_block00/test_q/vision_block_qkv_input.bin
    //D:\BaiduNetdiskDownload\FPGA_Demo_qwen2v5\qwen2_5_vl\vision_block00\test_q_expanded\q_linear_out_expanded.bin
    struct bin_inf* emb_q_dat_in_bin_inf = get_bin_inf(0, 1*768*2048, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/q_input.bin");
    struct bin_inf* emb_q_pos_in_bin_inf = get_bin_inf(0, 2048*128,   "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/cos_sin.bin"); 
    // Output bin_inf
    struct bin_inf* emb_q_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* emb_q_pos_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    
    // Transform data
    HBM_emb_test(CFG_EMB_Q, "wall_oss_run/blocks_0", "EMB_Q", emb_q_dat_in_bin_inf, emb_q_pos_in_bin_inf, &emb_q_dat_in_DDR_inf, ENABLE, &emb_q_pos_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], emb_q_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], emb_q_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], emb_q_pos_in_DDR_inf);
    verify_data_write(c2hx_device[0], emb_q_pos_in_DDR_inf);

    // Write command to FPGA
    pos_emb_step_4(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    struct bin_inf* emb_q_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/q_output.bin");
    HBM_emb_receive_and_compare(CFG_EMB_Q, c2hx_device[0], "wall_oss_run/blocks_0", "EMB_Q", emb_q_golden_out_bin_inf);
    printf("q_rope debug done!");   
#endif

#ifdef STEP4_MVMBN0_K
    // ******************************** STEP4 - MVMBN0_K ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_K = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm2,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight3,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    );

    // Input bin_inf //
    struct bin_inf* mvmbn0_k_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280,     "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/input.bin");
    struct bin_inf* mvmbn0_k_weight_in_bin_inf = get_bin_inf(0, 2048*1280,      "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_k_scales_in_bin_inf = get_bin_inf(0, 2048*10,        "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/scale.bin");
    struct bin_inf* mvmbn0_k_wt_in_bin_inf     = get_bin_inf(0, 2048,           "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_k_bias_in_bin_inf   = get_bin_inf(0, 2048,           "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_k_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_k_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_k_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_K, "wall_oss_run/blocks_0", "MVMBN0_K", mvmbn0_k_weight_in_bin_inf, mvmbn0_k_scales_in_bin_inf, mvmbn0_k_dat_in_bin_inf, mvmbn0_k_wt_in_bin_inf, mvmbn0_k_bias_in_bin_inf,
                    mvmbn0_k_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_k_dat_in_DDR_inf, ENABLE, &mvmbn0_k_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // wall_oss_runk_weight_load_param(user_device, h2cx_device[0], ".");

    // h2cx_device_write_bin(h2cx_device[0], mvmbn0_k_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbn0_k_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbn0_k_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn0_k_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_step_5(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn0_k_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_k_proj/output.bin");
    HBM_mvmbn_receive_and_compare(CFG_MVMBN0_K, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_K", mvmbn0_k_golden_out_bin_inf);
    printf("k_mvm debug done!");
#endif


#ifdef STEP5_EMB_K
    // ******************************** STEP5 - EMB_K ******************************** //
    struct FPGA_HBM_EMB_cfg CFG_EMB_K = GetFPGA_HBM_EMB_cfg(
        /*Height*/ run_token, /*Head*/ 1, /*Width_in*/ 2048, /*Pos_in_Num*/ 2048, /*Pos_in_CHin*/ 128,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime2,
        /*POS_IN_BASE_ADDR*/  ddr_base_addr+global0,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime4
    );

    // Input bin_inf
    struct bin_inf* emb_k_dat_in_bin_inf = get_bin_inf(0, 1*768*2048,   "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/k_input.bin");
    struct bin_inf* emb_k_pos_in_bin_inf = get_bin_inf(0, 2048*128,     "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/cos_sin.bin"); 
    // Output bin_inf
    struct bin_inf* emb_k_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* emb_k_pos_in_DDR_inf  = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_emb_test(CFG_EMB_K, "wall_oss_run/blocks_0", "EMB_K", emb_k_dat_in_bin_inf, emb_k_pos_in_bin_inf, &emb_k_dat_in_DDR_inf, ENABLE, &emb_k_pos_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], emb_k_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], emb_k_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], emb_k_pos_in_DDR_inf);
    verify_data_write(c2hx_device[0], emb_k_pos_in_DDR_inf);

    // Write command to FPGA
    pos_emb_step_6(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    struct bin_inf* emb_k_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ROPE_visual_blocks_0_attn/k_output.bin");
    HBM_emb_receive_and_compare(CFG_EMB_K, c2hx_device[0], "wall_oss_run/blocks_0", "EMB_K", emb_k_golden_out_bin_inf);
    printf("k_rope debug done!");
#endif

#ifdef STEP6_KV2HBM_K
    // ******************************** STEP6 - KV2HBM_K ******************************** //
    dat_hbm_step_7(user_device, run_token);
#endif

#ifdef STEP7_TRP
    // ******************************** STEP7 - TRP ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_TRP = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 16, /*Width_in*/ run_token, /*Width_out*/ run_token, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime3,
        /*HBM00_WT_BASE_ADDR*/ 0,
        /*BN_BASE_ADDR*/       0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime4
    );
    trp_mvm_step_8(user_device, run_token);
    // Read output data from FPGA and compare //D:\BaiduNetdiskDownload\FPGA_Demo_qwen2v5\qwen2_5_vl\vision_block00\test_qkv_mvm
    struct bin_inf* mvmbn0_trp_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ATTN_MASK/input.bin");
    HBM_mvmbn_receive_and_compare(CFG_MVMBN0_TRP, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_TRP", mvmbn0_trp_golden_out_bin_inf);
    printf("trp_out debug");
#endif

#ifdef STEP7_5_MASK
    // // ******************************** STEP7.5 - ELEMENTWISE ******************************** //
    // struct FPGA_HBM_ELEMENTWISE_cfg CFG_ELEMENTWISE = GetFPGA_HBM_ELEMENTWISE_cfg(
    //     /*Win*/ 648, /*Hin*/ 16, /*CHin*/ 648/*16·64(39补0->64)*/, /*ElementWise_Mode*/ 0,
    //     /*A_DAT_IN_BASE_ADDR*/  ddr_base_addr+mask0,
    //     /*B_DAT_IN_BASE_ADDR*/ ddr_base_addr+runtime4,
    //     /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime3
    // );

    // // Input bin_inf
    // struct bin_inf* elementwise_dat_in_bin_inf = get_bin_inf(0, 16*648*648, "./wall_oss/blocks_0/ATTN_MASK/input.bin");
    // struct bin_inf* elementwise_mask_in_bin_inf = get_bin_inf(0, 16*648*648, "./wall_oss/blocks_0/ATTN_MASK/mask.bin");

    // // Output bin_inf
    // struct bin_inf* elementwise_data_out_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // // Transform data
    // HBM_elementwise_test(CFG_ELEMENTWISE, "wall_oss_run/blocks_0", "ATTN_MASK", elementwise_mask_in_bin_inf, elementwise_dat_in_bin_inf, &elementwise_mask_in_bin_inf, &elementwise_dat_in_bin_inf);
    // // void HBM_elementwise_test(struct FPGA_HBM_ELEMENTWISE_cfg cfg, char* path_name, char* head_name, struct bin_inf* dat_in_A_bin_inf, struct bin_inf* dat_in_B_bin_inf, struct bin_inf* *dat_in_A_DDR_inf, struct bin_inf* *dat_in_B_DDR_inf)

    // // Write wt data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], elementwise_mask_in_bin_inf);
    // verify_data_write(c2hx_device[0], elementwise_mask_in_bin_inf);

    // // Write data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], elementwise_dat_in_bin_inf);
    // verify_data_write(c2hx_device[0], elementwise_dat_in_bin_inf);

    // // Write command to FPGA
    // add_step_9(user_device, run_token);

    // // Read output data from FPGA and compare
    // struct bin_inf* elementwise_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/ATTN_MASK/output.bin");
    // HBM_elementwise_receive_and_compare(CFG_ELEMENTWISE, c2hx_device[0], "wall_oss_run/blocks_0", "ATTN_MASK", elementwise_golden_out_bin_inf);
    // printf("ELEMENTWISE debug");

    // ******************************** STEP7.5 - MASK ******************************** //
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES0 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 16, /*Width_in*/ run_token, /*Width_out*/ run_token, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime4,
        /*HBM00_WT_BASE_ADDR*/ hbm3,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight4,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+weight5,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime3
    );

    // Input bin_inf
    struct bin_inf* mvmbnres0_dat_in_bin_inf         = get_bin_inf(0, 16*648*648,   "./wall_oss/blocks_7/ATTN_MASK/input.bin");
    struct bin_inf* mvmbnres0_weight_in_bin_inf      = get_bin_inf(0, 648*648,      "./wall_oss/blocks_7/ATTN_MASK/weight.bin"); 
    struct bin_inf* mvmbnres0_scales_in_bin_inf      = get_bin_inf(0, 648*6,        "./wall_oss/blocks_7/ATTN_MASK/scale.bin"); 
    struct bin_inf* mvmbnres0_wt_in_bin_inf          = get_bin_inf(0, 648,          "./rw_data/bn_wt_1.bin"); 
    struct bin_inf* mvmbnres0_bias_in_bin_inf        = get_bin_inf(0, 648,          "./rw_data/bn_and_k_bias_0.bin"); 
    struct bin_inf* mvmbnres0_res_add_dat_in_bin_inf = get_bin_inf(0, 16*648*648,   "./wall_oss/blocks_7/ATTN_MASK/mask.bin");
    // Output bin_inf
    struct bin_inf* *mvmbnres0_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres0_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES0, "wall_oss_run/blocks_7", "ATTN_MASK", mvmbnres0_weight_in_bin_inf, mvmbnres0_scales_in_bin_inf, mvmbnres0_dat_in_bin_inf, mvmbnres0_wt_in_bin_inf, mvmbnres0_bias_in_bin_inf, mvmbnres0_res_add_dat_in_bin_inf,
                    mvmbnres0_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres0_dat_in_DDR_inf, ENABLE, &mvmbnres0_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres0_res_add_dat_in_DDR_inf, ENABLE);

    // Write data to FPGA
    wall_oss_runMASK_weight_load_param(user_device, h2cx_device[0], ".");

    h2cx_device_write_bin(h2cx_device[0], mvmbnres0_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres0_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbnres0_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres0_bn_wt_and_bias_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbnres0_res_add_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres0_res_add_dat_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_res_step_9(user_device,run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbnres0_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_7/ATTN_MASK/output.bin");
    HBM_mvmbnres_receive_and_compare(CFG_MVMBNRES0, c2hx_device[0], "wall_oss_run/blocks_7", "ATTN_MASK", mvmbnres0_golden_out_bin_inf);
    printf("MASK debug");
#endif

#ifdef STEP8_SOFTMAX
    // ******************************** STEP8 - SOFTMAX ******************************** //
    struct FPGA_HBM_SOFTMAX_cfg CFG_SOFTMAX = GetFPGA_HBM_SOFTMAX_cfg(
        /*Height*/ run_token, /*Head*/ 16, /*Width_in*/ 648/*16·64(39补0->64)*/, /*DAT_IN_scale*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr+runtime3,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr+runtime4
    );

    // Input bin_inf
    struct bin_inf* softmax_dat_in_bin_inf = get_bin_inf(0, 1*768*768, "./wall_oss/blocks_0/SOFTMAX/input.bin");
    // Output bin_inf
    struct bin_inf* softmax_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_softmax_test(CFG_SOFTMAX, "wall_oss_run/blocks_0", "SOFTMAX", softmax_dat_in_bin_inf, &softmax_dat_in_DDR_inf, ENABLE);

    // Write data to FPGA
    h2cx_device_write_bin(h2cx_device[0], softmax_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], softmax_dat_in_DDR_inf);

    // Write command to FPGA
    softmax_step_10(user_device, run_token, last_token);

    // Read output data from FPGA and compare
    struct bin_inf* softmax_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/SOFTMAX/output.bin");
    HBM_softmax_receive_and_compare(CFG_SOFTMAX, c2hx_device[0], "wall_oss_run/blocks_0", "SOFTMAX", softmax_golden_out_bin_inf);
    printf("softmax debug");
#endif

#ifdef STEP9_MVMBN0_V
    // ******************************** STEP9 - MVMBN0_V ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_V = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
        /*HBM00_WT_BASE_ADDR*/ hbm4,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight6,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    );

    // Input bin_inf
    struct bin_inf* mvmbn0_v_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/input.bin");
    struct bin_inf* mvmbn0_v_weight_in_bin_inf = get_bin_inf(0, 2048*1280,  "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn0_v_scales_in_bin_inf = get_bin_inf(0, 2048*10,    "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/scale.bin");
    struct bin_inf* mvmbn0_v_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn0_v_bias_in_bin_inf   = get_bin_inf(0, 2048,       "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/bias.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn0_v_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn0_v_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn0_v_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN0_V, "wall_oss_run/blocks_0", "MVMBN0_V", mvmbn0_v_weight_in_bin_inf, mvmbn0_v_scales_in_bin_inf, mvmbn0_v_dat_in_bin_inf, mvmbn0_v_wt_in_bin_inf, mvmbn0_v_bias_in_bin_inf,
                    mvmbn0_v_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_v_dat_in_DDR_inf, ENABLE, &mvmbn0_v_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // Write data to FPGA
    wall_oss_runv_weight_load_param(user_device, h2cx_device[0], ".");

    h2cx_device_write_bin(h2cx_device[0], mvmbn0_v_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn0_v_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbn0_v_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn0_v_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_step_11(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn0_v_golden_out_bin_inf = get_bin_inf(0, 0, "./wall_oss/blocks_0/04_LINEAR_visual_blocks_0_attn_qkv_v_proj/output.bin");
    HBM_mvmbn_receive_and_compare(CFG_MVMBN0_V, c2hx_device[0], "wall_oss_run/blocks_0", "MVMBN0_V", mvmbn0_v_golden_out_bin_inf);
    printf("v debug");
#endif

#ifdef STEP10_KV2HBM_V
    // ******************************** STEP10 - KV2HBM_V ******************************** //
    dat_hbm_step_12(user_device, run_token);
    // printf("kv cache debug");
#endif

#ifdef STEP11_F2W
    // ******************************** STEP11 - F2W ******************************** //
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES2 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 648, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime4,
        /*HBM00_WT_BASE_ADDR*/ 0,
        /*BN_BASE_ADDR*/       ddr_base_addr+0,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime2
    );
    f2w_mvm_step_13(user_device, run_token);
    // Read output data from FPGA and compare 
    // struct bin_inf* mvmbnres2_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_o_proj_quant/attn_output_save_expanded_flash.bin");
    // HBM_mvmbnres_receive_and_compare(CFG_MVMBNRES2, c2hx_device[0], "BLOCK_read_data/BLOCK00", "MVMBNRES2", mvmbnres2_golden_out_bin_inf);
    // printf("qkv_mvm_golden_out debug");
#endif

#ifdef STEP12_MVMBNRES0
    // ******************************** STEP12 - MVMBNRES0 ******************************** //
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES0 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 2048, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime2,
        /*HBM00_WT_BASE_ADDR*/ hbm4,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight5,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime1,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime0
    );

    // Input bin_inf
    struct bin_inf* mvmbnres0_dat_in_bin_inf         = get_bin_inf(0, 1*768*2048, "./qwen2_5_vl/vision_block00/test_o_proj_quant/attn_output_save_expanded_flash.bin");
    struct bin_inf* mvmbnres0_weight_in_bin_inf      = get_bin_inf(0, 1280*2048, "./qwen2_5_vl/vision_block00/test_o_proj_quant/block00.self_attn.o_proj.weights.bin"); 
    struct bin_inf* mvmbnres0_scales_in_bin_inf      = get_bin_inf(0, 1280*16, "./qwen2_5_vl/vision_block00/test_o_proj_quant/block00.self_attn.o_proj.scales.bin");
    struct bin_inf* mvmbnres0_wt_in_bin_inf          = get_bin_inf(0, 1280, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres0_bias_in_bin_inf        = get_bin_inf(0, 1280, "./qwen2_5_vl/vision_block00/test_o_proj_quant/block00.self_attn.o_proj.bias.bin");
    struct bin_inf* mvmbnres0_res_add_dat_in_bin_inf = get_bin_inf(0, 1280, "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_residual_0.bin");
    // struct bin_inf* mvmbnres0_res_add_dat_in_bin_inf = get_bin_inf(0, 2048, "./qwen2.5/test_block00/test_LN0/LN0_input.bin");
    // Output bin_inf
    struct bin_inf* *mvmbnres0_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres0_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres0_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES0, "qwen2_5_vl/vision_block00", "MVMBNRES0", mvmbnres0_weight_in_bin_inf, mvmbnres0_scales_in_bin_inf, mvmbnres0_dat_in_bin_inf, mvmbnres0_wt_in_bin_inf, mvmbnres0_bias_in_bin_inf, mvmbnres0_res_add_dat_in_bin_inf,
                    mvmbnres0_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres0_dat_in_DDR_inf, ENABLE, &mvmbnres0_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres0_res_add_dat_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // qwen2_5_vlo_weight_load_param(user_device, h2cx_device[0], ".");

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres0_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres0_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbnres0_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres0_bn_wt_and_bias_in_DDR_inf);

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres0_res_add_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres0_res_add_dat_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_res_step_13(user_device,run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbnres0_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_after_o_proj_0.bin");
    // struct bin_inf* mvmbnres0_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2.5/test_block00/test_MVMBN1/MVMBN1_output.bin");
    HBM_mvmbnres_receive_and_compare(CFG_MVMBNRES0, c2hx_device[0], "BLOCK_read_data/BLOCK00", "MVMBNRES0", mvmbnres0_golden_out_bin_inf);
    printf("o_weight debug");
#endif

#ifdef STEP13_LN1
    // ******************************** STEP13 - LN1 ******************************** //
    struct FPGA_HBM_LN_cfg CFG_LN1 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 1280, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  ddr_base_addr + runtime0,
        /*LN_WT_BASE_ADDR*/   ddr_base_addr + weight6,
        /*DAT_OUT_BASE_ADDR*/ ddr_base_addr + runtime1
    );

    // Input bin_inf
    // struct bin_inf* ln1_dat_in_bin_inf  = get_bin_inf(0, 768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_norm2_input.bin");
    struct bin_inf* ln1_dat_in_bin_inf  = get_bin_inf(0, 768*1280, "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_after_o_proj.bin");
    struct bin_inf* ln1_wt_in_bin_inf   = get_bin_inf(0, 1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_norm2_weight.bin");
    struct bin_inf* ln1_bias_in_bin_inf = get_bin_inf(0, 1280, "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* ln1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln1_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
 
    // Transform data 
    HBM_ln_test(CFG_LN1, "qwen2_5_vl/vision_block00/test_ln1", "LN1", ln1_dat_in_bin_inf, ln1_wt_in_bin_inf, ln1_bias_in_bin_inf, &ln1_dat_in_DDR_inf, ENABLE, &ln1_ln_wt_and_bias_in_DDR_inf, ENABLE);

    qwen2_5_vlln2_bias_load_param(h2cx_device[0], ".");

    // Write data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], ln1_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], ln1_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], ln1_ln_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], ln1_ln_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    layer_norm_step_14(user_device, run_token);

    // Output bin_inf
    struct bin_inf* ln1_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_gate_proj_input.bin");
    
    // Read output data from FPGA and compare
    // HBM_ln_receive_and_compare(CFG_LN1, c2hx_device[0], "BLOCK_read_data/BLOCK00", "LN1", ln1_golden_out_bin_inf);
    // printf("ln2 debug");
#endif

#ifdef STEP14_MVMBN1
    // ******************************** STEP14 - MVMBN1 ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN1 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime1,
        /*HBM00_WT_BASE_ADDR*/ hbm6,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight8,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime3
    );

    // Input bin_inf 
    struct bin_inf* mvmbn1_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_gate_proj_input.bin");
    struct bin_inf* mvmbn1_weight_in_bin_inf = get_bin_inf(0, 3456*1280, "./qwen2_5_vl/vision_block00/test_mlp/gate_proj_weight_expanded.bin"); 
    struct bin_inf* mvmbn1_scales_in_bin_inf = get_bin_inf(0, 3456*10, "./qwen2_5_vl/vision_block00/test_mlp/gate_proj_scales_expanded.bin");
    struct bin_inf* mvmbn1_wt_in_bin_inf     = get_bin_inf(0, 3456, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn1_bias_in_bin_inf   = get_bin_inf(0, 3456, "./qwen2_5_vl/vision_block00/test_mlp/gate_proj_bias_expanded.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn1_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn1_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN1, "qwen2_5_vl/vision_block00", "MVMBN1_gate", mvmbn1_weight_in_bin_inf, mvmbn1_scales_in_bin_inf, mvmbn1_dat_in_bin_inf, mvmbn1_wt_in_bin_inf, mvmbn1_bias_in_bin_inf,
                    mvmbn1_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn1_dat_in_DDR_inf, ENABLE, &mvmbn1_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // qwen2_5_vlmlp_gate_wt_load_param(user_device, h2cx_device[0], ".");
    // h2cx_device_write_bin(h2cx_device[0], mvmbn1_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbn1_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbn1_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn1_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_step_15(user_device, run_token);

    // Read output data from FPGA and compare
    // struct bin_inf* mvmbn1_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_gate_proj_output.bin");
    // HBM_mvmbn_receive_and_compare(CFG_MVMBN1, c2hx_device[0], "BLOCK_read_data/BLOCK00", "GATA", mvmbn1_golden_out_bin_inf);
    // printf("gate debug");
#endif

#ifdef STEP15_ACT
    // ******************************** STEP15 - ACT ******************************** //
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
    HBM_act_test(CFG_ACT, "BLOCK_write_data/BLOCK00", "ACT", act_dat_in_bin_inf, &act_dat_in_DDR_inf, ENABLE, &act_parameter_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // h2cx_device_write_bin(h2cx_device[0], act_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], act_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], act_parameter_in_DDR_inf);
    verify_data_write(c2hx_device[0], act_parameter_in_DDR_inf);

    // Write command to FPGA
    activate_step_16(user_device, run_token);

    // Read output data from FPGA and compare 
    // struct bin_inf* act_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_act_fn_output.bin");
    // HBM_act_receive_and_compare(CFG_ACT, c2hx_device[0], "BLOCK_read_data/BLOCK00", "ACT", act_golden_out_bin_inf);
    // printf("silu debug");
#endif

#ifdef STEP16_MVMBNRES1
    // ******************************** STEP16 - MVMBNRES1 ******************************** //
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES1 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 3456, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime1,
        /*HBM00_WT_BASE_ADDR*/ hbm5,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight7,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime4,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime3
    );

    // Input bin_inf
    struct bin_inf* mvmbnres1_dat_in_bin_inf         = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_norm2/vision_block_gate_proj_input.bin");
    struct bin_inf* mvmbnres1_weight_in_bin_inf      = get_bin_inf(0, 3456*1280, "./qwen2_5_vl/vision_block00/test_mlp/up_proj_weight_expanded.bin"); 
    struct bin_inf* mvmbnres1_scales_in_bin_inf      = get_bin_inf(0, 3456*10, "./qwen2_5_vl/vision_block00/test_mlp/up_proj_scales_expanded.bin");
    struct bin_inf* mvmbnres1_wt_in_bin_inf          = get_bin_inf(0, 3456, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres1_bias_in_bin_inf        = get_bin_inf(0, 3456, "./qwen2_5_vl/vision_block00/test_mlp/up_proj_bias_expanded.bin");
    struct bin_inf* mvmbnres1_res_add_dat_in_bin_inf = get_bin_inf(0, 768*3456, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_act_fn_output.bin"); //
    // Output bin_inf
    struct bin_inf* *mvmbnres1_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres1_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres1_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres1_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES1, "qwen2_5_vl/vision_block00", "MVMBN1_up", mvmbnres1_weight_in_bin_inf, mvmbnres1_scales_in_bin_inf, mvmbnres1_dat_in_bin_inf, mvmbnres1_wt_in_bin_inf, mvmbnres1_bias_in_bin_inf, mvmbnres1_res_add_dat_in_bin_inf,
                    mvmbnres1_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres1_dat_in_DDR_inf, ENABLE, &mvmbnres1_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres1_res_add_dat_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // qwen2_5_vlmlp_up_wt_load_param(user_device, h2cx_device[0], ".");

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres1_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres1_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbnres1_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres1_bn_wt_and_bias_in_DDR_inf);

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres1_res_add_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres1_res_add_dat_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_res_step_17(user_device, run_token);

    // Read output data from FPGA and compare
    // struct bin_inf* mvmbnres1_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_up_mul_output.bin");
    // HBM_mvmbnres_receive_and_compare(CFG_MVMBNRES1, c2hx_device[0], "BLOCK_read_data/BLOCK00", "MVMBNRES1", mvmbnres1_golden_out_bin_inf);
#endif

#ifdef STEP17_MVMBNRES2
    // ******************************** STEP17 - MVMBNRES2 ******************************** //
    struct FPGA_HBM_MVM_BN_RES_cfg CFG_MVMBNRES2 = GetFPGA_HBM_MVM_BN_RES_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 3456, /*Width_out*/ 1280, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime3,
        /*HBM00_WT_BASE_ADDR*/ hbm7,
        /*BN_BASE_ADDR*/       ddr_base_addr+weight10,
        /*Res_Add_BASE_ADDR*/  ddr_base_addr+runtime0,
        /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
    );

    // Input bin_inf
    struct bin_inf* mvmbnres2_dat_in_bin_inf         = get_bin_inf(0, 1*768*3456, "./qwen2_5_vl/vision_block00/test_mlp/vision_block_up_mul_output.bin");
    struct bin_inf* mvmbnres2_weight_in_bin_inf      = get_bin_inf(0, 1280*3456, "./qwen2_5_vl/vision_block00/test_mlp/down_proj_weight_expanded.bin"); 
    struct bin_inf* mvmbnres2_scales_in_bin_inf      = get_bin_inf(0, 1280*27, "./qwen2_5_vl/vision_block00/test_mlp/down_proj_scales.bin");
    struct bin_inf* mvmbnres2_wt_in_bin_inf          = get_bin_inf(0, 1280, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbnres2_bias_in_bin_inf        = get_bin_inf(0, 1280, "./qwen2_5_vl/vision_block00/test_mlp/down_proj_bias.bin");
    struct bin_inf* mvmbnres2_res_add_dat_in_bin_inf = get_bin_inf(0, 1280, "./qwen2_5_vl/vision_block00/test_o_proj_quant/hidden_states_after_o_proj.bin");
    // Output bin_inf
    struct bin_inf* *mvmbnres2_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbnres2_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres2_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbnres2_res_add_dat_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbnres_test(CFG_MVMBNRES2, "qwen2_5_vl/vision_block00", "MVMBNRES2_down", mvmbnres2_weight_in_bin_inf, mvmbnres2_scales_in_bin_inf, mvmbnres2_dat_in_bin_inf, mvmbnres2_wt_in_bin_inf, mvmbnres2_bias_in_bin_inf, mvmbnres2_res_add_dat_in_bin_inf,
                    mvmbnres2_wt_and_scale_in_HBM_inf, ENABLE, &mvmbnres2_dat_in_DDR_inf, ENABLE, &mvmbnres2_bn_wt_and_bias_in_DDR_inf, ENABLE, &mvmbnres2_res_add_dat_in_DDR_inf, ENABLE);

    // Write data to FPGA
    // qwen2_5_vlmlp_down_wt_load_param(user_device, h2cx_device[0], ".");

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres2_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres2_dat_in_DDR_inf);

    h2cx_device_write_bin(h2cx_device[0], mvmbnres2_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbnres2_bn_wt_and_bias_in_DDR_inf);

    // h2cx_device_write_bin(h2cx_device[0], mvmbnres2_res_add_dat_in_DDR_inf);
    // verify_data_write(c2hx_device[0], mvmbnres2_res_add_dat_in_DDR_inf);

    // Write command to FPGA
    mvm_bn_res_step_18(user_device, run_token);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbnres2_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_mlp/vision_block0_output.bin");
    // struct bin_inf* mvmbnres2_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2.5/test_block00/test_MVMBN4/MVMBN4_out.bin");
    HBM_mvmbnres_receive_and_compare(CFG_MVMBNRES2, c2hx_device[0], "BLOCK_read_data/BLOCK00", "MVMBN1RES2", mvmbnres2_golden_out_bin_inf);
    printf("mvmbn1res debug");
#endif

#ifdef STEP596_LN_Outlayer
    // ******************************** STEP596 - LN_Outlayer ******************************** //
    struct FPGA_HBM_LN_cfg CFG_LN_Outlayer = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Width_in*/ 2048, /*RMS_Norm*/ 1, /*Hin*/ 1, /*RELU_EN*/ 0,
        /*DAT_IN_BASE_ADDR*/  NULL,
        /*LN_WT_BASE_ADDR*/   NULL,
        /*DAT_OUT_BASE_ADDR*/ NULL
    );

    // Input bin_inf
    struct bin_inf* ln_Outlayer_dat_in_bin_inf  = get_bin_inf(0, 1*39*2048, ".bin");
    struct bin_inf* ln_Outlayer_wt_in_bin_inf   = get_bin_inf(0, 2048, ".bin");
    struct bin_inf* ln_Outlayer_bias_in_bin_inf = get_bin_inf(0, 2048, ".bin");
    // Output bin_inf
    struct bin_inf* ln_Outlayer_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* ln_Outlayer_ln_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_ln_test(CFG_LN_Outlayer, "BLOCK_write_data/BLOCK00", "LN_Outlayer", ln_Outlayer_dat_in_bin_inf, ln_Outlayer_wt_in_bin_inf, ln_Outlayer_bias_in_bin_inf, &ln_Outlayer_dat_in_DDR_inf, ENABLE, &ln_Outlayer_ln_wt_and_bias_in_DDR_inf, ENABLE);

    // Write data to FPGA
    h2cx_device_write_bin(h2cx_device[0], ln_Outlayer_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], ln_Outlayer_dat_in_DDR_inf);
    h2cx_device_write_bin(h2cx_device[0], ln_Outlayer_ln_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], ln_Outlayer_ln_wt_and_bias_in_DDR_inf);

    // Write command to FPGA

    // Output bin_inf
    struct bin_inf* ln_Outlayer_golden_out_bin_inf = get_bin_inf(0, 0, ".bin");
    
    // Read output data from FPGA and compare
    HBM_ln_receive_and_compare(CFG_LN_Outlayer, c2hx_device[0], "BLOCK_read_data/BLOCK00", "LN_Outlayer", ln_Outlayer_golden_out_bin_inf);
#endif

#ifdef STEP597_MVMBN_Argmax
    // ******************************** STEP597 - MVMBN_Argmax ******************************** //
    struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN_Argmax = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 2048, /*Width_out*/ 151936, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
        /*DAT_IN_BASE_ADDR*/   NULL,
        /*HBM00_WT_BASE_ADDR*/ NULL,
        /*BN_BASE_ADDR*/       NULL,
        /*DAT_OUT_BASE_ADDR*/  NULL
    );

    // Input bin_inf
    struct bin_inf* mvmbn_argmax_dat_in_bin_inf    = get_bin_inf(0, 1*39*2048, ".bin");
    struct bin_inf* mvmbn_argmax_weight_in_bin_inf = get_bin_inf(0, 151936*2048, ".bin"); 
    struct bin_inf* mvmbn_argmax_scales_in_bin_inf = get_bin_inf(0, 151936*16, ".bin");
    struct bin_inf* mvmbn_argmax_wt_in_bin_inf     = get_bin_inf(0, 151936, "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn_argmax_bias_in_bin_inf   = get_bin_inf(0, 151936, ".bin");
    // Output bin_inf
    struct bin_inf* *mvmbn_argmax_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
    struct bin_inf* mvmbn_argmax_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
    struct bin_inf* mvmbn_argmax_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

    // Transform data
    HBM_mvmbn_test(CFG_MVMBN_Argmax, "BLOCK_write_data/BLOCK00", "MVMBN_Argmax", mvmbn_argmax_weight_in_bin_inf, mvmbn_argmax_scales_in_bin_inf, mvmbn_argmax_dat_in_bin_inf, mvmbn_argmax_wt_in_bin_inf, mvmbn_argmax_bias_in_bin_inf,
                    mvmbn_argmax_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn_argmax_dat_in_DDR_inf, ENABLE, &mvmbn_argmax_bn_wt_and_bias_in_DDR_inf, ENABLE);

    // Write data to FPGA
    Output_Layer_wt_load_param(user_device, h2cx_device[0], ".");
    h2cx_device_write_bin(h2cx_device[0], mvmbn_argmax_dat_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn_argmax_dat_in_DDR_inf);
    h2cx_device_write_bin(h2cx_device[0], mvmbn_argmax_bn_wt_and_bias_in_DDR_inf);
    verify_data_write(c2hx_device[0], mvmbn_argmax_bn_wt_and_bias_in_DDR_inf);

    // Write command to FPGA

    // Read ID from FPGA
    printf("Index: %d\n", CSB_Read(user_device, 40));
#endif
//vision_block00_q_weight_load_param(user_device, h2cx_device[0], "."); 
// #ifdef ALL_STEP
//     qwen2_5_vl_all(user_device, c2hx_device, h2cx_device, 768, 0);
// #endif
}

// void qwen2_5_vl_all_load_params(HANDLE& device, HANDLE& h2cx, char* prefix) {
// //   patch_embed_wt_load_param(device, h2cx, prefix);
// //   patch_embed_bn_load_param(h2cx, prefix);
// //   vision_block00_ln1_bias_load_param(h2cx, prefix);
// //   vision_block00_q_weight_load_param(device, h2cx, prefix);
// //   vision_block00_q_bn_load_param(h2cx, prefix);
//   vision_pos_emb_load_param(h2cx, prefix);
// }

// void qwen2_5_vl_all(HANDLE& user_device, HANDLE c2hx_device[NUM_OF_RW_CH], HANDLE h2cx_device[NUM_OF_RW_CH] , int run_token, int last_token) {

    
//     // int run_token    = 768;
//     // int last_token   = 0;
//     // int kvcache_mode = (run_token-last_token == 1)? 1 : 0;

//     struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_Q = GetFPGA_HBM_MVM_BN_cfg(
//         /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
//         /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,      //reg 10
//         /*HBM00_WT_BASE_ADDR*/ hbm1,                        //reg 11
//         /*BN_BASE_ADDR*/       ddr_base_addr+weight2,       //reg 26
//         /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1       //reg 13
//     );

//     // Input bin_inf
//     struct bin_inf* mvmbn0_q_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_q_expanded/hidden_states_for_qkv.bin");
//     struct bin_inf* mvmbn0_q_weight_in_bin_inf = get_bin_inf(0, 2048*1280,  "./qwen2_5_vl/vision_block00/test_q_expanded/proj_attn_w_q_weight_expanded.bin"); 
//     struct bin_inf* mvmbn0_q_scales_in_bin_inf = get_bin_inf(0, 2048*10,    "./qwen2_5_vl/vision_block00/test_q_expanded/proj_attn_w_q_scales_expanded.bin");
//     struct bin_inf* mvmbn0_q_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
//     struct bin_inf* mvmbn0_q_bias_in_bin_inf   = get_bin_inf(0, 2048,       "./qwen2_5_vl/vision_block00/test_q_expanded/proj_attn_w_q_bias_expanded.bin");
//     // Output bin_inf
//     struct bin_inf* *mvmbn0_q_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
//     struct bin_inf* mvmbn0_q_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
//     struct bin_inf* mvmbn0_q_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

//     // Transform data
//     HBM_mvmbn_test(CFG_MVMBN0_Q, "qwen2_5_vl/vision_block00", "MVMBN0_Q", mvmbn0_q_weight_in_bin_inf, mvmbn0_q_scales_in_bin_inf, mvmbn0_q_dat_in_bin_inf, mvmbn0_q_wt_in_bin_inf, mvmbn0_q_bias_in_bin_inf,
//                     mvmbn0_q_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_q_dat_in_DDR_inf, ENABLE, &mvmbn0_q_bn_wt_and_bias_in_DDR_inf, ENABLE);
                    
//     // Write wt data to FPGA
//     vision_block00_q_weight_load_param(user_device, h2cx_device[0], "."); 
//     // Write input data to FPGA    
//     h2cx_device_write_bin(h2cx_device[0], mvmbn0_q_dat_in_DDR_inf);
//     verify_data_write(c2hx_device[0], mvmbn0_q_dat_in_DDR_inf);
//      // Write bn_wt_and_bias data to FPGA        
//     h2cx_device_write_bin(h2cx_device[0], mvmbn0_q_bn_wt_and_bias_in_DDR_inf);
//     verify_data_write(c2hx_device[0], mvmbn0_q_bn_wt_and_bias_in_DDR_inf);

//     // // ******************************** STEP4 - MVMBN0_K ******************************** //
//     // struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_K = GetFPGA_HBM_MVM_BN_cfg(
//     //     /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
//     //     /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
//     //     /*HBM00_WT_BASE_ADDR*/ 0,
//     //     /*BN_BASE_ADDR*/       ddr_base_addr+weight3,
//     //     /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
//     // );

//     // // Input bin_inf
//     // struct bin_inf* mvmbn0_k_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_k_expanded/hidden_states_for_qkv.bin");
//     // struct bin_inf* mvmbn0_k_weight_in_bin_inf = get_bin_inf(0, 2048*1280, "./qwen2_5_vl/vision_block00/test_k_expanded/proj_attn_w_k_weight_expanded.bin"); 
//     // struct bin_inf* mvmbn0_k_scales_in_bin_inf = get_bin_inf(0, 2048*10, "./qwen2_5_vl/vision_block00/test_k_expanded/proj_attn_w_k_scales_expanded.bin");
//     // struct bin_inf* mvmbn0_k_wt_in_bin_inf     = get_bin_inf(0, 2048, "./rw_data/bn_wt_1.bin");
//     // struct bin_inf* mvmbn0_k_bias_in_bin_inf   = get_bin_inf(0, 2048, "./qwen2_5_vl/vision_block00/test_k_expanded/proj_attn_w_k_bias_expanded.bin");
//     // // Output bin_inf
//     // struct bin_inf* *mvmbn0_k_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
//     // struct bin_inf* mvmbn0_k_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
//     // struct bin_inf* mvmbn0_k_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

//     // // Transform data
//     // HBM_mvmbn_test(CFG_MVMBN0_K, "qwen2_5_vl/vision_block00", "MVMBN0_k", mvmbn0_k_weight_in_bin_inf, mvmbn0_k_scales_in_bin_inf, mvmbn0_k_dat_in_bin_inf, mvmbn0_k_wt_in_bin_inf, mvmbn0_k_bias_in_bin_inf,
//     //                 mvmbn0_k_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_k_dat_in_DDR_inf, ENABLE, &mvmbn0_k_bn_wt_and_bias_in_DDR_inf, ENABLE);
//     // // Write data to FPGA
//     // vision_block00_k_weight_load_param(user_device, h2cx_device[0], ".");

//     // h2cx_device_write_bin(h2cx_device[0], mvmbn0_k_dat_in_DDR_inf);
//     // verify_data_write(c2hx_device[0], mvmbn0_k_dat_in_DDR_inf);

//     // h2cx_device_write_bin(h2cx_device[0], mvmbn0_k_bn_wt_and_bias_in_DDR_inf);
//     // verify_data_write(c2hx_device[0], mvmbn0_k_bn_wt_and_bias_in_DDR_inf);
    

//     // // ******************************** STEP9 - MVMBN0_V ******************************** //
//     // struct FPGA_HBM_MVM_BN_cfg CFG_MVMBN0_V = GetFPGA_HBM_MVM_BN_cfg(
//     //     /*Height*/ run_token, /*Win*/ 1, /*Width_in*/ 1280, /*Width_out*/ 2048, /*Skip_Factor*/ 1, /*RELU_EN*/ 0, /*log2_WT_base_addr_Bank_Step*/ 0,
//     //     /*DAT_IN_BASE_ADDR*/   ddr_base_addr+runtime0,
//     //     /*HBM00_WT_BASE_ADDR*/ 0,
//     //     /*BN_BASE_ADDR*/       ddr_base_addr+weight4,
//     //     /*DAT_OUT_BASE_ADDR*/  ddr_base_addr+runtime1
//     // );

//     // // Input bin_inf
//     // struct bin_inf* mvmbn0_v_dat_in_bin_inf    = get_bin_inf(0, 1*768*1280, "./qwen2_5_vl/vision_block00/test_v_expanded/hidden_states_for_qkv.bin");
//     // struct bin_inf* mvmbn0_v_weight_in_bin_inf = get_bin_inf(0, 2048*1280,  "./qwen2_5_vl/vision_block00/test_v_expanded/proj_attn_w_v_weight_expanded.bin"); 
//     // struct bin_inf* mvmbn0_v_scales_in_bin_inf = get_bin_inf(0, 2048*10,    "./qwen2_5_vl/vision_block00/test_v_expanded/proj_attn_w_v_scales_expanded.bin");
//     // struct bin_inf* mvmbn0_v_wt_in_bin_inf     = get_bin_inf(0, 2048,       "./rw_data/bn_wt_1.bin");
//     // struct bin_inf* mvmbn0_v_bias_in_bin_inf   = get_bin_inf(0, 2048,       "./qwen2_5_vl/vision_block00/test_v_expanded/proj_attn_w_v_bias_expanded.bin");
//     // // Output bin_inf
//     // struct bin_inf* *mvmbn0_v_wt_and_scale_in_HBM_inf  = (struct bin_inf**)malloc(sizeof(struct bin_inf*));
//     // struct bin_inf* mvmbn0_v_dat_in_DDR_inf            = (struct bin_inf*)malloc(sizeof(struct bin_inf));
//     // struct bin_inf* mvmbn0_v_bn_wt_and_bias_in_DDR_inf = (struct bin_inf*)malloc(sizeof(struct bin_inf));

//     // // Transform data
//     // HBM_mvmbn_test(CFG_MVMBN0_V, "qwen2_5_vl/vision_block00", "MVMBN0_v", mvmbn0_v_weight_in_bin_inf, mvmbn0_v_scales_in_bin_inf, mvmbn0_v_dat_in_bin_inf, mvmbn0_v_wt_in_bin_inf, mvmbn0_v_bias_in_bin_inf,
//     //                 mvmbn0_v_wt_and_scale_in_HBM_inf, ENABLE, &mvmbn0_v_dat_in_DDR_inf, ENABLE, &mvmbn0_v_bn_wt_and_bias_in_DDR_inf, ENABLE);
//     // // Write data to FPGA
//     // vision_block00_v_weight_load_param(user_device, h2cx_device[0], ".");

//     // h2cx_device_write_bin(h2cx_device[0], mvmbn0_v_dat_in_DDR_inf);
//     // verify_data_write(c2hx_device[0], mvmbn0_v_dat_in_DDR_inf);

//     // h2cx_device_write_bin(h2cx_device[0], mvmbn0_v_bn_wt_and_bias_in_DDR_inf);
//     // verify_data_write(c2hx_device[0], mvmbn0_v_bn_wt_and_bias_in_DDR_inf);

//     // Write command to FPGA
//     // mvm_bn_step_1(device, seq);
//     // layer_norm_step_2(device, seq);
//     mvm_bn_step_3(user_device, run_token);
//     qwen2_5_vl_all_load_params(user_device, h2cx_device[0], ".");


//     pos_emb_step_4(user_device, run_token, last_token);
//     // mvm_bn_step_5(user_device, run_token);
//     // pos_emb_step_6(user_device, run_token, last_token);
//     // dat_hbm_step_7(user_device, run_token, last_token);
//     // trp_mvm_step_8(user_device, run_token, last_token);
//     // softmax_step_9(user_device, run_token, last_token);
//     // mvm_bn_step_10(user_device, run_token);
//     // dat_hbm_step_11(user_device, run_token, last_token);
//     // f2w_mvm_step_12(user_device, run_token, last_token);

//     // Read output data from FPGA and compare
//     struct bin_inf* mvmbn0_q_golden_out_bin_inf = get_bin_inf(0, 0, "./qwen2_5_vl/vision_block00/test_q_rope/q_rope_out_expanded.bin");
//     HBM_mvmbn_receive_and_compare(CFG_MVMBN0_Q, c2hx_device[0], "BLOCK_read_data/BLOCK00", "MVMBN0_Q", mvmbn0_q_golden_out_bin_inf);
//     printf("debug all");

// }

