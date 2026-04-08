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
    // 完整的文件路径
    std::string text_0_path    = "./wall_oss/model_layers_0/RMSNORM_model_layers_0_input_layernorm/text_0.bin";
    std::string image_path     = "./wall_oss/LINEAR_visual_merger_mlp_2/output.bin";
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
    std::cout << "Reading text_0.bin (22 tokens)" << std::endl;
    std::vector<uint16_t> text_0 = readBinFile(text_0_path);
    merged_data.insert(merged_data.end(), text_0.begin(), text_0.end());

    // 2. 读取image.bin并拆分 - 先取img_0部分 (81 tokens)
    std::cout << "Reading image.bin - img_0 part (81 tokens)" << std::endl;
    std::vector<uint16_t> image = readBinFile(image_path);
    size_t img_elements = img_tokens * hidden_size;  // 81 * 2048
    merged_data.insert(merged_data.end(), image.begin(), image.begin() + img_elements);

    // 3. 读取proprio (7 tokens)
    std::cout << "Reading proprio.bin (7 tokens)" << std::endl;
    std::vector<uint16_t> proprio = readBinFile(proprio_path);
    merged_data.insert(merged_data.end(), proprio.begin(), proprio.end());

    // 4. 取image.bin的img_1部分 (81 tokens)
    std::cout << "Reading image.bin - img_1 part (81 tokens)" << std::endl;
    merged_data.insert(merged_data.end(), image.begin() + img_elements, image.end());

    // 5. 读取text_1 (31 tokens)
    std::cout << "Reading text_1.bin (31 tokens)" << std::endl;
    std::vector<uint16_t> text_1 = readBinFile(text_1_path);
    merged_data.insert(merged_data.end(), text_1.begin(), text_1.end());

    // 6. 读取action (32 tokens)
    std::cout << "Reading action.bin (32 tokens)" << std::endl;
    std::vector<uint16_t> action = readBinFile(action_path);
    merged_data.insert(merged_data.end(), action.begin(), action.end());

    // 保存合并后的文件
    std::cout << std::endl;
    saveBinFile(output_path, merged_data);

    // 验证信息
    std::cout << std::endl << "Verification:" << std::endl;
    std::cout << "  Shape: (1, 254, 2048)" << std::endl;
    std::cout << "  Total elements: " << merged_data.size() << std::endl;
    std::cout << "  Total bytes: " << merged_data.size() * 2 << std::endl;
    std::cout << "  Order: text_0(22) -> img_0(81) -> proprio(7) -> img_1(81) -> text_1(31) -> action(32)" << std::endl;

    return 0;
}
