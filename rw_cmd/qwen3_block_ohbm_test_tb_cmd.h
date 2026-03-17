// hbm storage define
uint64_t hbm0 = 0x000000000;
uint64_t hbm1 = 0x000000140;
uint64_t hbm2 = 0x00002ad40;
uint64_t hbm3 = 0x00002af80;
uint64_t hbm4 = 0x000001300;
uint64_t hbm5 = 0x000059b80;
uint64_t hbm6 = 0x000059dc0;
uint64_t hbm7 = 0x0000849c0;
uint64_t hbm8 = 0x000084c00;
uint64_t hbm9 = 0x0000aef00;
// runtime storage define
uint64_t runtime0 = 0x0000af040;
uint64_t runtime1 = 0x000000000;
uint64_t runtime2 = 0x00019cbe0;
uint64_t runtime3 = 0x0001c8680;

uint64_t input = runtime0;

uint64_t output = runtime2;

void mvm_f16xi4_step_q(HANDLE& device)
{
    CSB_Write(device, 2,  4096);
    CSB_Write(device, 3,  19);
    CSB_Write(device, 4,  1);
    CSB_Write(device, 5,  19);
    CSB_Write(device, 6,  1);
    CSB_Write(device, 7,  1024);
    CSB_Write(device, 8,  0);
    CSB_Write(device, 9,  0);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 4864);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 275712);
    CSB_Write(device, 14, 0);
    CSB_Write(device, 15, 0);
    CSB_Write(device, 16, 4);
    CSB_Write(device, 17, 1);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 65792);
    CSB_Write(device, 20, 65792);
    CSB_Write(device, 21, 0);
    CSB_Write(device, 22, 11536384);
    CSB_Write(device, 23, 7340160);
    CSB_Write(device, 24, 2048);
    CSB_Write(device, 25, 275200);
    CSB_Write(device, 26, 0);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 608);
    CSB_Write(device, 30, 608);
    CSB_Write(device, 31, 608);
    CSB_Write(device, 32, 608);
    CSB_Write(device, 34, 31);

    CSB_Write(device, 2,  4096);
    CSB_Write(device, 3,  19);
    CSB_Write(device, 4,  1);
    CSB_Write(device, 5,  19);
    CSB_Write(device, 6,  1);
    CSB_Write(device, 7,  1024);
    CSB_Write(device, 8,  1024);
    CSB_Write(device, 9,  0);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 72448);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 276928);
    CSB_Write(device, 14, 0);
    CSB_Write(device, 15, 0);
    CSB_Write(device, 16, 6);
    CSB_Write(device, 17, 1);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 65792);
    CSB_Write(device, 20, 65792);
    CSB_Write(device, 21, 0);
    CSB_Write(device, 22, 11536384);
    CSB_Write(device, 23, 7340160);
    CSB_Write(device, 24, 2048);
    CSB_Write(device, 25, 275328);
    CSB_Write(device, 26, 0);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 608);
    CSB_Write(device, 30, 608);
    CSB_Write(device, 31, 608);
    CSB_Write(device, 32, 608);
    CSB_Write(device, 34, 31);

    CSB_Write(device, 2,  4096);
    CSB_Write(device, 3,  19);
    CSB_Write(device, 4,  1);
    CSB_Write(device, 5,  19);
    CSB_Write(device, 6,  1);
    CSB_Write(device, 7,  1024);
    CSB_Write(device, 8,  2048);
    CSB_Write(device, 9,  0);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 140032);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 278144);
    CSB_Write(device, 14, 0);
    CSB_Write(device, 15, 0);
    CSB_Write(device, 16, 6);
    CSB_Write(device, 17, 1);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 65792);
    CSB_Write(device, 20, 65792);
    CSB_Write(device, 21, 0);
    CSB_Write(device, 22, 11536384);
    CSB_Write(device, 23, 7340160);
    CSB_Write(device, 24, 2048);
    CSB_Write(device, 25, 275456);
    CSB_Write(device, 26, 0);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 608);
    CSB_Write(device, 30, 608);
    CSB_Write(device, 31, 608);
    CSB_Write(device, 32, 608);
    CSB_Write(device, 34, 31);

    CSB_Write(device, 2,  4096);
    CSB_Write(device, 3,  19);
    CSB_Write(device, 4,  1);
    CSB_Write(device, 5,  19);
    CSB_Write(device, 6,  1);
    CSB_Write(device, 7,  1024);
    CSB_Write(device, 8,  3072);
    CSB_Write(device, 9,  0);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 207616);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 279360);
    CSB_Write(device, 14, 0);
    CSB_Write(device, 15, 0);
    CSB_Write(device, 16, 6);
    CSB_Write(device, 17, 1);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 65792);
    CSB_Write(device, 20, 65792);
    CSB_Write(device, 21, 0);
    CSB_Write(device, 22, 11536384);
    CSB_Write(device, 23, 7340160);
    CSB_Write(device, 24, 2048);
    CSB_Write(device, 25, 275584);
    CSB_Write(device, 26, 0);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 608);
    CSB_Write(device, 30, 608);
    CSB_Write(device, 31, 608);
    CSB_Write(device, 32, 608);
    CSB_Write(device, 34, 31);
    while (CSB_Read(device, 1) != 1) {}
}

void mvm_f16xi4_step_kv(HANDLE& device)
{
    CSB_Write(device, 2,  4096);
    CSB_Write(device, 3,  19);
    CSB_Write(device, 4,  1);
    CSB_Write(device, 5,  19);
    CSB_Write(device, 6,  1);
    CSB_Write(device, 7,  1024);
    CSB_Write(device, 8,  0);
    CSB_Write(device, 9,  0);
    CSB_Write(device, 10, 0);
    CSB_Write(device, 11, 4864);
    CSB_Write(device, 12, 16896);
    CSB_Write(device, 13, 72576);
    CSB_Write(device, 14, 0);
    CSB_Write(device, 15, 0);
    CSB_Write(device, 16, 4);
    CSB_Write(device, 17, 1);
    CSB_Write(device, 18, 0);
    CSB_Write(device, 19, 65792);
    CSB_Write(device, 20, 65792);
    CSB_Write(device, 21, 0);
    CSB_Write(device, 22, 11536384);
    CSB_Write(device, 23, 7340160);
    CSB_Write(device, 24, 2048);
    CSB_Write(device, 25, 72448);
    CSB_Write(device, 26, 0);
    CSB_Write(device, 27, 0);
    CSB_Write(device, 28, 0);
    CSB_Write(device, 29, 608);
    CSB_Write(device, 30, 608);
    CSB_Write(device, 31, 608);
    CSB_Write(device, 32, 608);
    CSB_Write(device, 34, 31);
    while (CSB_Read(device, 1) != 1) {}
}

void activate_step_2(HANDLE& device)
{
    CSB_Write(device, 130, 29184);
    CSB_Write(device, 131, 0);
    CSB_Write(device, 132, 608);
    CSB_Write(device, 133, 608);
    CSB_Write(device, 134, 14592);
    CSB_Write(device, 135, 608);
    CSB_Write(device, 136, 608);
    CSB_Write(device, 137, 12288);
    CSB_Write(device, 138, 1);
    CSB_Write(device, 139, 19);
    CSB_Write(device, 140, 1);
    CSB_Write(device, 141, 0);
    CSB_Write(device, 142, 0);
    CSB_Write(device, 143, 0);
    CSB_Write(device, 144, 0);
    CSB_Write(device, 145, 16);
    while (CSB_Read(device, 129) != 1) {}
}

