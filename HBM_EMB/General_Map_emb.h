
void General_Map_Feature_Data(struct FPGA_HBM_EMB_cfg cfg, int Height, int Width, int CH, int *in, int *mem[]);
void General_Map_Pos_Data(struct FPGA_HBM_EMB_cfg cfg, int *in, int *mem[]);
void General_DeMap_Feature_Data(struct FPGA_HBM_EMB_cfg cfg, int Height, int Width, int CH, uint16_t *mem[], uint16_t *out);

#include "General_Map_emb.cpp"