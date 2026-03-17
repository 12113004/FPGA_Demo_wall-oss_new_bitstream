void General_Map_Feature_Data(struct FPGA_HBM_TRP_cfg cfg, int Height, int Width, int CH, int *in, int *mem[]);
void General_Map_Wt_Data(struct FPGA_HBM_TRP_cfg cfg, int *in, int *mem[]);
void General_DeMap_Feature_Data(struct FPGA_HBM_TRP_cfg cfg, int Height, int Width, int CH, uint16_t *mem[], uint16_t *out);

#include "General_Map_trp.cpp"