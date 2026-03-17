
void General_Map_Feature_Data(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int Height, int Width, int CH, int *in, int *mem[]);
void General_Map_Res_Add_Data(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int Height, int Width, int CH, int *in, int *mem[]);
void General_DeMap_Feature_Data(struct FPGA_HBM_MVM_BN_RES_cfg cfg, int Height, int Width, int CH, uint16_t *mem[], uint16_t *out);

#include "General_Map_mvm_bn_res.cpp"