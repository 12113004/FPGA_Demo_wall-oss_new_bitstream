#include "./fp_compare.cpp"
#include <iostream>
using namespace std;
#include <time.h>
#include <math.h>
#include "./xdma_rw.h"
#include "./read_file.cpp"
#include "./rt_lib.h"

int __cdecl main()
{
    HANDLE user_device;
    HANDLE bypass_device;
    HANDLE c2hx_device[NUM_OF_RW_CH];
    HANDLE h2cx_device[NUM_OF_RW_CH];   
    open_device(&user_device, &bypass_device, &c2hx_device[0], &h2cx_device[0]);
    printf("xxxxxxxxxxxxxxxxx");
    BYTE data_1 = 1;
    BYTE data_2;
    c2hx_device_read(c2hx_device[0], 0, 1, &data_2);
    printf("%d", data_2);
    h2cx_device_write(h2cx_device[0], 0, 1, &data_1);
    c2hx_device_read(c2hx_device[0], 0, 1, &data_2);
    printf("%d", data_2);
}