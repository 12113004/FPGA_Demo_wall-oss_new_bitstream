#include "./xdma_lib/fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/read_file.cpp"
#include "./xdma_lib/rt_lib.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

// #define LOAD_WEIGHT
// #define LAN_USE_GODEN
// #define MODEL_NORM_USE_GODEN
// #define MODEL_NORM_COMPARE
// #define COMPARE_GODEN

// #define BLOCK_EMB

#define ViT_BLOCK_0
#define ViT_BLOCK_1
#define ViT_BLOCK_2
#define ViT_BLOCK_3
#define ViT_BLOCK_4
#define ViT_BLOCK_5
#define ViT_BLOCK_6
#define ViT_BLOCK_7
#define ViT_BLOCK_8
#define ViT_BLOCK_9
#define ViT_BLOCK_10
#define ViT_BLOCK_11
#define ViT_BLOCK_12
#define ViT_BLOCK_13
#define ViT_BLOCK_14
#define ViT_BLOCK_15
#define ViT_BLOCK_16
#define ViT_BLOCK_17
#define ViT_BLOCK_18
#define ViT_BLOCK_19
#define ViT_BLOCK_20
#define ViT_BLOCK_21
#define ViT_BLOCK_22
#define ViT_BLOCK_23
#define ViT_BLOCK_24
#define ViT_BLOCK_25
#define ViT_BLOCK_26
#define ViT_BLOCK_27
#define ViT_BLOCK_28
#define ViT_BLOCK_29
#define ViT_BLOCK_30
#define ViT_BLOCK_31

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
#define BLOCK_32
#define BLOCK_33
#define BLOCK_34
#define BLOCK_35

// 全局变量 vit_index
int vit_index;
int vit_end_index;
int index;
int end_index;

// 2. 根据宏定义给 vit_index 赋值
void set_vit_block_index(void)
{
    #ifdef ViT_BLOCK_0
    vit_index = 0;
    #elif defined(ViT_BLOCK_1)
    vit_index = 1;
    #elif defined(ViT_BLOCK_2)
    vit_index = 2;
    #elif defined(ViT_BLOCK_3)
    vit_index = 3;
    #elif defined(ViT_BLOCK_4)
    vit_index = 4;
    #elif defined(ViT_BLOCK_5)
    vit_index = 5;
    #elif defined(ViT_BLOCK_6)
    vit_index = 6;
    #elif defined(ViT_BLOCK_7)
    vit_index = 7;
    #elif defined(ViT_BLOCK_8)
    vit_index = 8;
    #elif defined(ViT_BLOCK_9)
    vit_index = 9;
    #elif defined(ViT_BLOCK_10)
    vit_index = 10;
    #elif defined(ViT_BLOCK_11)
    vit_index = 11;
    #elif defined(ViT_BLOCK_12)
    vit_index = 12;
    #elif defined(ViT_BLOCK_13)
    vit_index = 13;
    #elif defined(ViT_BLOCK_14)
    vit_index = 14;
    #elif defined(ViT_BLOCK_15)
    vit_index = 15;
    #elif defined(ViT_BLOCK_16)
    vit_index = 16;
    #elif defined(ViT_BLOCK_17)
    vit_index = 17;
    #elif defined(ViT_BLOCK_18)
    vit_index = 18;
    #elif defined(ViT_BLOCK_19)
    vit_index = 19;
    #elif defined(ViT_BLOCK_20)
    vit_index = 20;
    #elif defined(ViT_BLOCK_21)
    vit_index = 21;
    #elif defined(ViT_BLOCK_22)
    vit_index = 22;
    #elif defined(ViT_BLOCK_23)
    vit_index = 23;
    #elif defined(ViT_BLOCK_24)
    vit_index = 24;
    #elif defined(ViT_BLOCK_25)
    vit_index = 25;
    #elif defined(ViT_BLOCK_26)
    vit_index = 26;
    #elif defined(ViT_BLOCK_27)
    vit_index = 27;
    #elif defined(ViT_BLOCK_28)
    vit_index = 28;
    #elif defined(ViT_BLOCK_29)
    vit_index = 29;
    #elif defined(ViT_BLOCK_30)
    vit_index = 30;
    #elif defined(ViT_BLOCK_31)
    vit_index = 31;
    #else
    // 无任何 BLOCK 定义时的默认值
    vit_index = -1;
    #endif
}
void set_vit_block_end_index(void)
{
    #ifdef ViT_BLOCK_31
    vit_end_index = 31;
    #elif defined(ViT_BLOCK_30)
    vit_end_index = 30;
    #elif defined(ViT_BLOCK_29)
    vit_end_index = 29;
    #elif defined(ViT_BLOCK_28)
    vit_end_index = 28;
    #elif defined(ViT_BLOCK_27)
    vit_end_index = 27;
    #elif defined(ViT_BLOCK_26)
    vit_end_index = 26;
    #elif defined(ViT_BLOCK_25)
    vit_end_index = 25;
    #elif defined(ViT_BLOCK_24)
    vit_end_index = 24;
    #elif defined(ViT_BLOCK_23)
    vit_end_index = 23;
    #elif defined(ViT_BLOCK_22)
    vit_end_index = 22;
    #elif defined(ViT_BLOCK_21)
    vit_end_index = 21;
    #elif defined(ViT_BLOCK_20)
    vit_end_index = 20;
    #elif defined(ViT_BLOCK_19)
    vit_end_index = 19;
    #elif defined(ViT_BLOCK_18)
    vit_end_index = 18;
    #elif defined(ViT_BLOCK_17)
    vit_end_index = 17;
    #elif defined(ViT_BLOCK_16)
    vit_end_index = 16;
    #elif defined(ViT_BLOCK_15)
    vit_end_index = 15;
    #elif defined(ViT_BLOCK_14)
    vit_end_index = 14;
    #elif defined(ViT_BLOCK_13)
    vit_end_index = 13;
    #elif defined(ViT_BLOCK_12)
    vit_end_index = 12;
    #elif defined(ViT_BLOCK_11)
    vit_end_index = 11;
    #elif defined(ViT_BLOCK_10)
    vit_end_index = 10;
    #elif defined(ViT_BLOCK_9)
    vit_end_index = 9;
    #elif defined(ViT_BLOCK_8)
    vit_end_index = 8;
    #elif defined(ViT_BLOCK_7)
    vit_end_index = 7;
    #elif defined(ViT_BLOCK_6)
    vit_end_index = 6;
    #elif defined(ViT_BLOCK_5)
    vit_end_index = 5;
    #elif defined(ViT_BLOCK_4)
    vit_end_index = 4;
    #elif defined(ViT_BLOCK_3)
    vit_end_index = 3;
    #elif defined(ViT_BLOCK_2)
    vit_end_index = 2;
    #elif defined(ViT_BLOCK_1)
    vit_end_index = 1;
    #elif defined(ViT_BLOCK_0)
    vit_end_index = 0;
    #else
    // 无任何 BLOCK 定义时的默认值
    vit_end_index = -1;
    #endif
}
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
    #elif defined(BLOCK_32)
    index = 32;
    #elif defined(BLOCK_33)
    index = 33;
    #elif defined(BLOCK_34)
    index = 34;
    #elif defined(BLOCK_35)
    index = 35;
    #else
    // 无任何 BLOCK 定义时的默认值
    index = -1;
    #endif
}
void set_block_end_index(void)
{
    #ifdef BLOCK_35
    end_index = 35;
    #elif defined(BLOCK_34)
    end_index = 34;
    #elif defined(BLOCK_33)
    end_index = 33;
    #elif defined(BLOCK_32)
    end_index = 32;
    #elif defined(BLOCK_31)
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
#define STEP_Merger_Token
// Lanuage
#define STEP_LN0
// #define STEP_ELEMENTWISE2
// Model Compare
#define BLOCK_Model_Norm
#define STEP_SAVE_bin
#define COMOARE_DIFF

// 直接读取FP16 bin文件为uint16_t数组
std::vector<uint16_t> readBinFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filepath << std::endl;
        exit(1);
    }

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    size_t num_elements = size / 2;
    std::vector<uint16_t> fp16_data(num_elements);
    file.read(reinterpret_cast<char*>(fp16_data.data()), size);
    file.close();

    // std::cout << "  Read " << num_elements << " elements (" << size << " bytes)" << std::endl;
    return fp16_data;
}

// 保存FP16 bin文件
void saveBinFile(const std::string& filepath, const std::vector<uint16_t>& data) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create: " << filepath << std::endl;
        exit(1);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size() * 2);
    file.close();

    std::cout << "Saved: " << filepath << " (" << data.size() * 2 << " bytes)" << std::endl;
}

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

    int Lang_run_token    = 254;
    int Lang_last_token   = 0;
    int Lang_hidden_dim   = 2048;

    char** filename = (char**)malloc(sizeof(char*)*4);
    for(int i=0;i<4;i++)
        filename[i] = (char*)malloc(sizeof(char)*200);
    set_vit_block_index();
    set_vit_block_end_index();
    set_block_index();
    set_block_end_index();

#ifdef LOAD_WEIGHT
    test_load_params(h2cx_device[0], ".");
    llm_test_load_params(h2cx_device[0], ".");
#endif

#ifdef STEP_ViT_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_vit_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ hidden_dim,
        /*DAT_IN_BASE_ADDR*/ runtime1, /*LN_WT_BASE_ADDR*/ hbm2, /*DAT_OUT_BASE_ADDR*/ runtime0
    );
    // Input bin_inf
    sprintf(filename[0],  "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/input.bin", vit_index, vit_index);
    printf("%s\n", filename[0]);
    struct bin_inf* vit_ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  filename[0]);
    struct bin_inf* vit_ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/blocks_0/RMSNORM_visual_blocks_0_norm1/weight.bin");
    struct bin_inf* vit_ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *vit_ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *vit_ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_vit_ln0, "wall_oss_run/blocks_0", "LN0", vit_ln0_dat_in_bin_inf, vit_ln0_weight_bin_inf, vit_ln0_bias_bin_inf, vit_ln0_dat_in_HBM_inf, ENABLE, vit_ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], vit_ln0_dat_in_HBM_inf, group);

    test_ViT(user_device, run_token, last_token);

#endif

#ifdef STEP_ViT_ELEMENTWISE2
    // ******************************** STEP22 ELEMENTWISE2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ run_token, /*Hin*/ 1, /*Width_in*/ 1280,
        /*DAT_IN_A_BASE_ADDR*/ runtime0, /*DAT_IN_B_BASE_ADDR*/ runtime4, /*DAT_OUT_BASE_ADDR*/ runtime1
    );
    sprintf(filename[0],  "./wall_oss/blocks_%d/RMSNORM_visual_blocks_%d_norm1/input.bin", vit_end_index+1, vit_end_index+1);
    printf("%s\n", filename[0]);
    // Read output data from FPGA and compare
    #ifdef ViT_BLOCK_31
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
    #ifndef ViT_BLOCK_31
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
    HBM_mvmbn_receive_and_compare(cfg_mvmbn3, c2hx_device[0], "wall_oss_run/Merger", "MVMBN3", mvmbn3_golden_out_bin_inf, TRUE);

    // Malloc free
    bin_inf_malloc_free(mvmbn3_golden_out_bin_inf );
#endif

#ifdef STEP_Merger_Token
    // 完整的文件路径
    // std::string image_path     = "./wall_oss/LINEAR_visual_merger_mlp_2/output.bin";
    std::string image_path     = "./wall_oss_run/Merger/MVM_BN_FPGA_out_bin/MVMBN3_dat_out_demaped.bin";
    std::string text_0_path    = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/text_0.bin";
    std::string proprio_path   = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/proprio.bin";
    std::string text_1_path    = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/text_1.bin";
    std::string action_path    = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/action.bin";
    std::string output_path    = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/input_test_time.bin";

    // 配置
    int img_tokens = 81;       // 每张图片的token数
    int hidden_size = 2048;    // 隐藏层维度

    // 预分配结果缓冲区 (254 * 2048)
    std::vector<uint16_t> merged_data;
    merged_data.reserve(254 * hidden_size);

    // 1. 读取text_0 (22 tokens)
    std::vector<uint16_t> text_0 = readBinFile(text_0_path);
    merged_data.insert(merged_data.end(), text_0.begin(), text_0.end());

    // 2. 读取image.bin并拆分 - 先取img_0部分 (81 tokens)
    std::vector<uint16_t> image = readBinFile(image_path);
    size_t img_elements = img_tokens * hidden_size;  // 81 * 2048
    merged_data.insert(merged_data.end(), image.begin(), image.begin() + img_elements);

    // 3. 读取proprio (7 tokens)
    std::vector<uint16_t> proprio = readBinFile(proprio_path);
    merged_data.insert(merged_data.end(), proprio.begin(), proprio.end());

    // 4. 取image.bin的img_1部分 (81 tokens)
    merged_data.insert(merged_data.end(), image.begin() + img_elements, image.end());

    // 5. 读取text_1 (31 tokens)
    std::vector<uint16_t> text_1 = readBinFile(text_1_path);
    merged_data.insert(merged_data.end(), text_1.begin(), text_1.end());

    // 6. 读取action (32 tokens)
    std::vector<uint16_t> action = readBinFile(action_path);
    merged_data.insert(merged_data.end(), action.begin(), action.end());

    // 保存合并后的文件
    std::cout << std::endl;
    saveBinFile(output_path, merged_data);

    // 验证信息
    // std::cout << std::endl << "Verification:" << std::endl;
    // std::cout << "  Shape: (1, 254, 2048)" << std::endl;
    // std::cout << "  Total elements: " << merged_data.size() << std::endl;
    // std::cout << "  Total bytes: " << merged_data.size() * 2 << std::endl;
    // std::cout << "  Order: text_0(22) -> img_0(81) -> proprio(7) -> img_1(81) -> text_1(31) -> action(32)" << std::endl;

#endif

#ifdef STEP_LN0
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg cfg_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ Lang_run_token, /*Hin*/ 1, /*Width_in*/ Lang_hidden_dim,
        /*DAT_IN_BASE_ADDR*/ llm_runtime0, /*LN_WT_BASE_ADDR*/ llm_hbm0, /*DAT_OUT_BASE_ADDR*/ llm_runtime1
    );
    // Input bin_inf
    #ifdef LAN_USE_GODEN
        sprintf(filename[0],  "./wall_oss/model_layers_%d/RMSNORM_model_layers_%d_input_layernorm/input.bin", index, index);
        printf("%s\n", filename[0]);
        struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  filename[0]);
    #else
        struct bin_inf* ln0_dat_in_bin_inf   = get_bin_inf(0, Lang_run_token*1*Lang_hidden_dim,     "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/input_test_time.bin");
    #endif
    
    struct bin_inf* ln0_weight_bin_inf   = get_bin_inf(0, 1*Lang_hidden_dim,                    "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/weight.bin");
    struct bin_inf* ln0_bias_bin_inf     = get_bin_inf(0, Lang_hidden_dim,                      "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(cfg_ln0, "wall_oss_run/model_layers_0", "LN0", ln0_dat_in_bin_inf, ln0_weight_bin_inf, ln0_bias_bin_inf, ln0_dat_in_HBM_inf, ENABLE, ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], ln0_ln_wt_and_bias_HBM_inf, group);

    // Write command to FPGA
    llm_test(user_device, Lang_run_token, last_token);

#endif

#ifdef STEP_ELEMENTWISE2
    // ******************************** STEP22 ELEMENTWISE2 ******************************** //
    // Parameter Config
    struct FPGA_HBM_ELEMENTWISE_cfg cfg_elementwise2 = GetFPGA_HBM_ELEMENTWISE_cfg(
        /*Height*/ Lang_run_token, /*Hin*/ 1, /*Width_in*/ Lang_hidden_dim,
        /*DAT_IN_A_BASE_ADDR*/ llm_runtime4, /*DAT_IN_B_BASE_ADDR*/ llm_runtime1, /*DAT_OUT_BASE_ADDR*/ llm_runtime0
    );
    // Read output data from FPGA and compare
    #ifndef BLOCK_35
        sprintf(filename[0],  "./wall_oss/model_layers_%d/RMSNORM_model_layers_%d_input_layernorm/input.bin", end_index+1, end_index+1);
        printf("%s\n", filename[0]);
        struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0,         filename[0]);
        HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "wall_oss_run/model_layers_0", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);
    #else
        sprintf(filename[0],  "./wall_oss/RMSNORM_model_norm/input.bin");
        printf("%s\n", filename[0]);
        struct bin_inf* elementwise2_golden_out_bin_inf = get_bin_inf(0, 0,         filename[0]);
        HBM_elementwise_receive_and_compare(cfg_elementwise2, c2hx_device[0], "wall_oss_run/model_layers_0", "ELEMENTWISE2", elementwise2_golden_out_bin_inf);
    #endif

    // Malloc free
    // bin_inf_malloc_free(elementwise2_dat_in_A_bin_inf);
    // bin_inf_malloc_free(elementwise2_dat_in_B_bin_inf);
    bin_inf_malloc_free(elementwise2_golden_out_bin_inf);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_A_HBM_inf, group);
    // HBM_bin_inf_malloc_free(elementwise2_dat_in_B_HBM_inf, group);
#endif 

#ifdef BLOCK_Model_Norm
    // ******************************** STEP1 - LN0 ******************************** //
    // Parameter Config
    struct FPGA_HBM_LN_cfg model_cfg_ln0 = GetFPGA_HBM_LN_cfg(
        /*Height*/ Lang_run_token, /*Hin*/ 1, /*Width_in*/ Lang_hidden_dim,
        /*DAT_IN_BASE_ADDR*/ llm_runtime0, /*LN_WT_BASE_ADDR*/ llm_hbm902, /*DAT_OUT_BASE_ADDR*/ llm_runtime1
    );

    #ifdef MODEL_NORM_USE_GODEN
    // Input bin_inf
    struct bin_inf* model_ln0_dat_in_bin_inf   = get_bin_inf(0, run_token*1*hidden_dim,  "./wall_oss/RMSNORM_model_norm/input.bin");
    struct bin_inf* model_ln0_weight_bin_inf   = get_bin_inf(0, 1*hidden_dim,            "./wall_oss/RMSNORM_model_norm/weight.bin");
    struct bin_inf* model_ln0_bias_bin_inf     = get_bin_inf(0, hidden_dim,              "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *model_ln0_dat_in_HBM_inf         = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *model_ln0_ln_wt_and_bias_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_ln_test(model_cfg_ln0, "wall_oss_run/model_norm", "LN_Outlayer", model_ln0_dat_in_bin_inf, model_ln0_weight_bin_inf, model_ln0_bias_bin_inf, model_ln0_dat_in_HBM_inf, ENABLE, model_ln0_ln_wt_and_bias_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], model_ln0_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], model_ln0_ln_wt_and_bias_HBM_inf, group);
    #endif

    // Write command to FPGA
    model_normmodel_norm(user_device, Lang_run_token, last_token);

    #ifdef MODEL_NORM_COMPARE
    // Read output data from FPGA and compare
    struct bin_inf* model_ln0_golden_out_bin_inf = get_bin_inf(0, run_token*1*hidden_dim, "./wall_oss/RMSNORM_model_norm/output.bin");
    HBM_ln_receive_and_compare(model_cfg_ln0, c2hx_device[0], "wall_oss_run/model_norm", "LN_Outlayer", model_ln0_golden_out_bin_inf);

    // Malloc free
    // bin_inf_malloc_free(model_ln0_dat_in_bin_inf);
    // bin_inf_malloc_free(model_ln0_weight_bin_inf);
    // bin_inf_malloc_free(model_ln0_bias_bin_inf);
    // // bin_inf_malloc_free(model_ln0_golden_out_bin_inf );
    // HBM_bin_inf_malloc_free(model_ln0_dat_in_HBM_inf, group);
    // HBM_bin_inf_malloc_free(model_ln0_ln_wt_and_bias_HBM_inf, group); 
    #endif
#endif

#ifdef STEP_SAVE_bin
    // 文件路径
    #ifdef COMPARE_GODEN
    std::string compare_input_path = "./wall_oss/RMSNORM_model_norm/output.bin";
    #else
    std::string compare_input_path = "./wall_oss_run/model_norm/LN_FPGA_out_bin/LN_Outlayer_dat_out_demaped.bin";
    #endif
    
    std::string compare_output_path = "./wall_oss/LINEAR_action_preprocessor_action_proj_back/input_test_time.bin";

    // 配置
    int compare_total_tokens = Lang_run_token;    // 总token数
    int compare_hidden_size = Lang_hidden_dim;    // 隐藏层维度
    int extract_tokens = 32;   // 要提取的最后token数

    // std::cout << "Reading input file..." << std::endl;
    std::vector<uint16_t> input_data = readBinFile(compare_input_path);

    // 验证数据大小
    size_t expected_size = compare_total_tokens * compare_hidden_size;
    if (input_data.size() != expected_size) {
        std::cerr << "Warning: Expected " << expected_size << " elements, got " << input_data.size() << std::endl;
    }

    // 提取后32个token
    // reshape后形状是 (254, 2048)，后32个token即索引 222-253
    size_t start_idx = (compare_total_tokens - extract_tokens) * compare_hidden_size;  // 222 * 2048

    // std::cout << "Extracting last " << extract_tokens << " tokens (from index " << (compare_total_tokens - extract_tokens) << ")" << std::endl;
    std::vector<uint16_t> extracted_data(input_data.begin() + start_idx, input_data.end());

    // std::cout << "Extracted " << extracted_data.size() << " elements (" << extract_tokens << " x " << compare_hidden_size << ")" << std::endl;

    // 保存
    std::cout << std::endl;
    saveBinFile(compare_output_path, extracted_data);

    // std::cout << std::endl << "Verification:" << std::endl;
    // std::cout << "  Input shape: (1, " << compare_total_tokens << ", " << compare_hidden_size << ")" << std::endl;
    // std::cout << "  Output shape: (1, " << extract_tokens << ", " << compare_hidden_size << ")" << std::endl;
    // std::cout << "  Extracted tokens index: " << (compare_total_tokens - extract_tokens) << " - " << (compare_total_tokens - 1) << std::endl;
#endif 

#ifdef COMOARE_DIFF
    // ******************************** STEP21 - MVMBN4 ******************************** //
    // Parameter Config
    struct FPGA_HBM_MVM_BN_cfg cfg_mvmbn4 = GetFPGA_HBM_MVM_BN_cfg(
        /*Height*/ 32, /*Hin*/ 1, /*Width_in*/ 2048, /*Width_out*/ 2048,
        /*DAT_IN_BASE_ADDR*/ llm_runtime0, /*HBM00_WT_BASE_ADDR*/ hbm24, /*BN_BASE_ADDR*/ hbm25, /*DAT_OUT_BASE_ADDR*/ llm_runtime0
    );

    // Input bin_inf
    struct bin_inf* mvmbn4_dat_in_bin_inf = get_bin_inf(0, run_token*3456,  "./wall_oss/LINEAR_action_preprocessor_action_proj_back/input_test_time.bin");
    struct bin_inf* mvmbn4_weight_bin_inf = get_bin_inf(0, 3456*1280,       "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_1_down_proj/weight_int4.bin"); 
    struct bin_inf* mvmbn4_scales_bin_inf = get_bin_inf(0, 1280*27,         "./wall_oss/model_layers_0/LINEAR_model_layers_0_moe_experts_1_down_proj/scale.bin");
    struct bin_inf* mvmbn4_wt_bin_inf     = get_bin_inf(0, 1280,            "./rw_data/bn_wt_1.bin");
    struct bin_inf* mvmbn4_bias_bin_inf   = get_bin_inf(0, 1280,            "./rw_data/bn_and_k_bias_0.bin");
    // Output bin_inf
    struct bin_inf* *mvmbn4_wt_and_scale_in_HBM_inf   = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_dat_in_HBM_inf            = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);
    struct bin_inf* *mvmbn4_bn_wt_and_bias_in_HBM_inf = (struct bin_inf**)malloc(sizeof(struct bin_inf)*group);

    // Transform data
    HBM_mvmbn_test(cfg_mvmbn4, "wall_oss_run/model_norm", "MVM_COMPARE", mvmbn4_weight_bin_inf, mvmbn4_scales_bin_inf, mvmbn4_dat_in_bin_inf, mvmbn4_wt_bin_inf, mvmbn4_bias_bin_inf,
                    mvmbn4_wt_and_scale_in_HBM_inf, ENABLE, mvmbn4_dat_in_HBM_inf, ENABLE, mvmbn4_bn_wt_and_bias_in_HBM_inf, ENABLE);

    // Write data to FPGA
    HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_dat_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_wt_and_scale_in_HBM_inf, group);
    // HBM_bin_write_and_verify(h2cx_device[0], c2hx_device[0], mvmbn4_bn_wt_and_bias_in_HBM_inf, group);

    // Read output data from FPGA and compare
    struct bin_inf* mvmbn4_golden_out_bin_inf = get_bin_inf(0, 0,       "./wall_oss/LINEAR_action_preprocessor_action_proj_back/input.bin");
    HBM_mvmbn_receive_and_compare(cfg_mvmbn4, c2hx_device[0], "wall_oss_run/model_norm", "MVM_COMPARE", mvmbn4_golden_out_bin_inf);

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

    cout << "test_end" << endl;

}