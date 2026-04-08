#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

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

int main() {
    // 文件路径
    std::string input_path = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/input_test_time.bin";
    std::string output_path = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/input_test_time.bin_output.bin";

    // 配置
    int total_tokens = 254;    // 总token数
    int hidden_size = 2048;    // 隐藏层维度
    int extract_tokens = 32;   // 要提取的最后token数

    std::cout << "Reading input file..." << std::endl;
    std::vector<uint16_t> input_data = readBinFile(input_path);

    // 验证数据大小
    size_t expected_size = total_tokens * hidden_size;
    if (input_data.size() != expected_size) {
        std::cerr << "Warning: Expected " << expected_size << " elements, got " << input_data.size() << std::endl;
    }

    // 提取后32个token
    // reshape后形状是 (254, 2048)，后32个token即索引 222-253
    size_t start_idx = (total_tokens - extract_tokens) * hidden_size;  // 222 * 2048

    std::cout << "Extracting last " << extract_tokens << " tokens (from index " << (total_tokens - extract_tokens) << ")" << std::endl;
    std::vector<uint16_t> extracted_data(input_data.begin() + start_idx, input_data.end());

    std::cout << "Extracted " << extracted_data.size() << " elements (" << extract_tokens << " x " << hidden_size << ")" << std::endl;

    // 保存
    std::cout << std::endl;
    saveBinFile(output_path, extracted_data);

    std::cout << std::endl << "Verification:" << std::endl;
    std::cout << "  Input shape: (1, " << total_tokens << ", " << hidden_size << ")" << std::endl;
    std::cout << "  Output shape: (1, " << extract_tokens << ", " << hidden_size << ")" << std::endl;
    std::cout << "  Extracted tokens index: " << (total_tokens - extract_tokens) << " - " << (total_tokens - 1) << std::endl;

    return 0;
}
