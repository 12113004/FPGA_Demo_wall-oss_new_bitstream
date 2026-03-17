
void Receive_data_and_Find_Index_fp16(HANDLE c2hx_device, uint64_t addr, int num)
{
    struct bin_inf* argmax_out_inf = get_bin_inf(addr,  num*2, "./argmax_test.bin");
    c2hx_device_read_bin(c2hx_device, argmax_out_inf);
    printf_fp16_log(argmax_out_inf->bin_data_file, num, "argmax.log");
    
    int *argmax_data = (int*)malloc(sizeof(int)*num);
    if (argmax_data == NULL){printf("fail to malloc argmax_data \n");}
    read_bin(argmax_out_inf ->bin_data_file, argmax_data, num);
    
    uint16_t *argmax_data_fp16 = (uint16_t*)malloc(sizeof(uint16_t)*num);
    if (argmax_data_fp16 == NULL){printf("fail to malloc argmax_data_fp16 \n");}

    for(int i=0;i<num;i++)
        argmax_data_fp16[i] = (uint16_t)argmax_data[i];
    
    half *argmax_data_half = (half*)malloc(sizeof(half)*num);
    if (argmax_data_half == NULL){printf("fail to malloc argmax_data_half \n");}

    uint16_to_half(argmax_data_fp16,  num, argmax_data_half);

    int max_index = 0;
    half max_value = argmax_data_half[0];

    for (int i=1;i<num;i++) 
    {
        if (argmax_data_half[i] > max_value) {
            max_value = argmax_data_half[i];
            max_index = i;
        }
    }

    printf("C code Max Value: %f\n", (float)max_value);
    printf("C code Max Value Index: %d\n", max_index);
    
    free(argmax_data);
    argmax_data = NULL;
}

uint16_t Receive_data_and_Find_Index(HANDLE c2hx, uint64_t addr, int num) {
    uint16_t max_index = 0;
    uint16_t max_value = 0;

    struct bin_inf* argmax_out_inf = get_bin_inf(addr,  num*2, "./argmax_test.bin");
    c2hx_device_read_bin(c2hx, argmax_out_inf);
    
    int *argmax_data = (int*)malloc(sizeof(int)*num);
    if (argmax_data == NULL){printf("fail to malloc argmax_data \n");}
    read_bin(argmax_out_inf ->bin_data_file, argmax_data, num);
    
    uint16_t *argmax_data_fp16 = (uint16_t*)malloc(sizeof(uint16_t)*num);
    if (argmax_data_fp16 == NULL){printf("fail to malloc argmax_data_fp16 \n");}

    for(int i=0;i<num;i++)
    {
        argmax_data_fp16[i] = (uint16_t)argmax_data[i];
        if((argmax_data_fp16[i] >> 15) == 1)
            argmax_data_fp16[i] = 0; 
    }

    max_value = argmax_data_fp16[0];

    for (int i=1;i<num;i++) 
    {
        if (argmax_data_fp16[i] > max_value)
        {
            max_value = argmax_data_fp16[i];
            max_index = i;
        }
    }

//    max_value = (uint16_t)argmax_data[0];
////    printf("max_value: 0x%04x \n", argmax_data[30910]);
//    for (int i=1;i<num;i++) 
//    {
//        argmax_data_fp16[i] = (uint16_t)argmax_data[i];
//        if (((argmax_data_fp16[i] >> 15) == 0) && (argmax_data_fp16[i] > max_value)) 
//        {
//            max_value = argmax_data_fp16[i];
//            max_index = i;
//            printf("11111111111111111111 \n");
//        }
//    }

    free(argmax_data);
    argmax_data = NULL;
    free(argmax_data_fp16);
    argmax_data_fp16 = NULL;

    return max_index;
}