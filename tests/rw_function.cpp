#include "./xdma_lib/xdma_rw.h"
#include "./xdma_lib/bin_compare.cpp"

#define ENABLE  1
#define DISABLE 0

void HBM_bin_write_and_verify(HANDLE h2cx_device, HANDLE c2hx_device, struct bin_inf* write_to_HBM_inf[], int binfile_num)
{
    for(int i=0;i<binfile_num;i++)
    {
        h2cx_device_write_bin(h2cx_device, write_to_HBM_inf[i]);
        verify_data_write(c2hx_device, write_to_HBM_inf[i]);
    }
}

void HBM00_bin_write_and_verify(HANDLE h2cx_device, HANDLE c2hx_device, struct bin_inf* write_to_HBM_inf)
{
    h2cx_device_write_bin(h2cx_device, write_to_HBM_inf);
    verify_data_write(c2hx_device, write_to_HBM_inf);
}

void bin_inf_malloc_free(struct bin_inf* bin_inf)
{
    free(bin_inf);
    bin_inf = NULL;
}

void HBM_bin_inf_malloc_free(struct bin_inf* *write_to_HBM_inf, int binfile_num)
{
    for(int i=0;i<binfile_num;i++)
    {
        free(write_to_HBM_inf[i]->bin_data_file);
        write_to_HBM_inf[i]->bin_data_file = NULL;
        free(write_to_HBM_inf[i]);
        write_to_HBM_inf[i] = NULL;
    }
    free(write_to_HBM_inf);
    write_to_HBM_inf = NULL;
}