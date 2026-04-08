#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cmath>

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

    std::cout << "  Read " << num_elements << " elements (" << size << " bytes)" << std::endl;
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

// FP16转FP32辅助函数
float fp16_to_fp32(uint16_t fp16_val) {
    // 简单的FP16转FP32实现
    uint32_t sign = (fp16_val >> 15) & 0x1;
    uint32_t exp = (fp16_val >> 10) & 0x1F;
    uint32_t mant = fp16_val & 0x3FF;

    if (exp == 0) {
        // 零或次正规数
        if (mant == 0) return sign ? -0.0f : 0.0f;
        // 次正规数处理
        float val = mant / 1024.0f * powf(2, -14);
        return sign ? -val : val;
    } else if (exp == 31) {
        // 无穷或NaN
        return mant ? NAN : (sign ? -INFINITY : INFINITY);
    }

    // 正规数
    float val = (1.0f + mant / 1024.0f) * powf(2, exp - 15);
    return sign ? -val : val;
}

// 对比两个bin文件
void compareBins(const std::string& filepath1, const std::string& filepath2, int tokens, int hidden_size) {
    std::cout << "\n=== Comparing Bins ===" << std::endl;
    std::cout << "File1: " << filepath1 << std::endl;
    std::cout << "File2: " << filepath2 << std::endl;

    std::vector<uint16_t> data1 = readBinFile(filepath1);
    std::vector<uint16_t> data2 = readBinFile(filepath2);

    size_t expected_size = tokens * hidden_size;
    if (data1.size() != expected_size || data2.size() != expected_size) {
        std::cerr << "Error: Size mismatch. Expected " << expected_size
                  << ", got " << data1.size() << " and " << data2.size() << std::endl;
        return;
    }

    int total_diff = 0;
    float max_error = 0.0f;
    float sum_error = 0.0f;
    int max_error_idx = -1;

    // 逐元素对比
    for (size_t i = 0; i < expected_size; i++) {
        if (data1[i] != data2[i]) {
            total_diff++;
            float val1 = fp16_to_fp32(data1[i]);
            float val2 = fp16_to_fp32(data2[i]);
            float error = fabsf(val1 - val2);
            sum_error += error;
            if (error > max_error) {
                max_error = error;
                max_error_idx = i;
            }
        }
    }

    // 输出对比结果
    std::cout << "\nComparison Result:" << std::endl;
    std::cout << "  Total elements: " << expected_size << std::endl;
    std::cout << "  Different elements: " << total_diff << std::endl;
    std::cout << "  Match rate: " << (100.0f * (expected_size - total_diff) / expected_size) << "%" << std::endl;

    if (total_diff > 0) {
        std::cout << "  Max error: " << max_error << std::endl;
        std::cout << "  Average error: " << (sum_error / total_diff) << std::endl;
        if (max_error_idx >= 0) {
            int token_idx = max_error_idx / hidden_size;
            int hidden_idx = max_error_idx % hidden_size;
            std::cout << "  Max error at: token " << token_idx << ", hidden " << hidden_idx << std::endl;
            std::cout << "    File1: " << fp16_to_fp32(data1[max_error_idx]) << " (0x" << std::hex << data1[max_error_idx] << std::dec << ")" << std::endl;
            std::cout << "    File2: " << fp16_to_fp32(data2[max_error_idx]) << " (0x" << std::hex << data2[max_error_idx] << std::dec << ")" << std::endl;
        }

        // 显示前10个不同的位置
        std::cout << "\n  First 10 differences:" << std::endl;
        int shown = 0;
        for (size_t i = 0; i < expected_size && shown < 10; i++) {
            if (data1[i] != data2[i]) {
                int token_idx = i / hidden_size;
                int hidden_idx = i % hidden_size;
                std::cout << "    [" << token_idx << "," << hidden_idx << "] ";
                std::cout << "file1=" << fp16_to_fp32(data1[i]) << " ";
                std::cout << "file2=" << fp16_to_fp32(data2[i]) << std::endl;
                shown++;
            }
        }
    } else {
        std::cout << "  Result: PASS - Files are identical!" << std::endl;
    }
}

int main() {
    // 文件路径
    std::string input_path = "./wall_oss_run/model_layers_0/ELEMENTWISE_FPGA_out_bin/ELEMENTWISE2_dat_out_demaped.bin";
    std::string output_path = "./wall_oss/LINEAR_action_preprocessor_action_proj_back/input_test_time.bin";
    std::string compare_path = "./wall_oss/LINEAR_action_preprocessor_action_proj_back/golden_32tokens.bin";  // 用于对比的文件

    // 配置
    int total_tokens = 254;        // 总token数 (根据实际修改，或用变量Lang_run_token)
    int hidden_size = 2048;        // 隐藏层维度 (根据实际修改，或用变量Lang_hidden_dim)
    int extract_tokens = 32;       // 要提取的最后token数

    std::cout << "Reading input file..." << std::endl;
    std::vector<uint16_t> input_data = readBinFile(input_path);

    // 验证数据大小
    size_t expected_size = total_tokens * hidden_size;
    if (input_data.size() != expected_size) {
        std::cerr << "Warning: Expected " << expected_size << " elements, got " << input_data.size() << std::endl;
    }

    // 提取后32个token
    size_t start_idx = (total_tokens - extract_tokens) * hidden_size;

    std::cout << "\nExtracting last " << extract_tokens << " tokens (from index " << (total_tokens - extract_tokens) << ")" << std::endl;
    std::vector<uint16_t> extracted_data(input_data.begin() + start_idx, input_data.end());

    std::cout << "Extracted " << extracted_data.size() << " elements (" << extract_tokens << " x " << hidden_size << ")" << std::endl;

    // 保存
    std::cout << std::endl;
    saveBinFile(output_path, extracted_data);

    // 如果有对比文件，进行数值对比
    // 如果不需要对比，可以注释掉下面这行或传入空路径
    if (!compare_path.empty()) {
        compareBins(output_path, compare_path, extract_tokens, hidden_size);
    }

    return 0;
}
