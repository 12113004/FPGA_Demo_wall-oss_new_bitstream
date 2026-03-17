// hbm storage define
uint64_t hbm0 = 0x000000000;
uint64_t hbm1 = 0x000000140;
uint64_t hbm2 = 0x00002ad40;
uint64_t hbm3 = 178848;
uint64_t hbm4 = 0x00002ef80;
uint64_t hbm5 = 0x000059b80;
uint64_t hbm6 = 0x000059dc0;
uint64_t hbm7 = 0x0000849c0;
uint64_t hbm8 = 0x000084c00;
uint64_t hbm9 = 0x0000aef00;
// runtime storage define
uint64_t runtime0 = 0;
uint64_t runtime1 = 0x0001847c0;
uint64_t runtime2 = 244384;
uint64_t runtime3 = 0x0001c8680;

void rope_step_3(HANDLE& device, int last_token)
{
    CSB_Write(130, 178848);
    CSB_Write(131, 0);
    CSB_Write(132, 19872);
    CSB_Write(133, 19872);
    CSB_Write(134, 244384);
    CSB_Write(135, 19872);
    CSB_Write(136, 19872);
    CSB_Write(137, 128);
    CSB_Write(138, 113);
    CSB_Write(139, 0);
    CSB_Write(140, 36);
    CSB_Write(141, 65536);
    CSB_Write(142, 65536);
    CSB_Write(143, 2308);
    CSB_Write(144, 0);
    CSB_Write(145, 4);
}

PosEmb_wdma done!
reg_addr=130, reg_data=182464
reg_addr=131, reg_data=3616
reg_addr=132, reg_data=19872
reg_addr=133, reg_data=19872
reg_addr=134, reg_data=248000
reg_addr=135, reg_data=19872
reg_addr=136, reg_data=19872
reg_addr=137, reg_data=128
reg_addr=138, reg_data=113
reg_addr=139, reg_data=0
reg_addr=140, reg_data=36
reg_addr=141, reg_data=65536
reg_addr=142, reg_data=65536
reg_addr=143, reg_data=2308
reg_addr=144, reg_data=0
reg_addr=145, reg_data=4
PosEmb_wdma done!
reg_addr=130, reg_data=186080
reg_addr=131, reg_data=7232
reg_addr=132, reg_data=19872
reg_addr=133, reg_data=19872
reg_addr=134, reg_data=251616
reg_addr=135, reg_data=19872
reg_addr=136, reg_data=19872
reg_addr=137, reg_data=128
reg_addr=138, reg_data=113
reg_addr=139, reg_data=0
reg_addr=140, reg_data=36
reg_addr=141, reg_data=65536
reg_addr=142, reg_data=65536
reg_addr=143, reg_data=2308
reg_addr=144, reg_data=0
reg_addr=145, reg_data=4
PosEmb_wdma done!
reg_addr=130, reg_data=189696
reg_addr=131, reg_data=10848
reg_addr=132, reg_data=19872
reg_addr=133, reg_data=19872
reg_addr=134, reg_data=255232
reg_addr=135, reg_data=19872
reg_addr=136, reg_data=19872
reg_addr=137, reg_data=128
reg_addr=138, reg_data=113
reg_addr=139, reg_data=0
reg_addr=140, reg_data=36
reg_addr=141, reg_data=65536
reg_addr=142, reg_data=65536
reg_addr=143, reg_data=2308
reg_addr=144, reg_data=0
reg_addr=145, reg_data=4
PosEmb_wdma done!
reg_addr=130, reg_data=193312
reg_addr=131, reg_data=14464
reg_addr=132, reg_data=19872
reg_addr=133, reg_data=19872
reg_addr=134, reg_data=258848
reg_addr=135, reg_data=19872
reg_addr=136, reg_data=19872
reg_addr=137, reg_data=128
reg_addr=138, reg_data=113
reg_addr=139, reg_data=0
reg_addr=140, reg_data=36
reg_addr=141, reg_data=65536
reg_addr=142, reg_data=65536
reg_addr=143, reg_data=2308
reg_addr=144, reg_data=0
reg_addr=145, reg_data=4
PosEmb_wdma done!
reg_addr=130, reg_data=196928
reg_addr=131, reg_data=18080
reg_addr=132, reg_data=19872
reg_addr=133, reg_data=19872
reg_addr=134, reg_data=262464
reg_addr=135, reg_data=19872
reg_addr=136, reg_data=19872
reg_addr=137, reg_data=128
reg_addr=138, reg_data=56
reg_addr=139, reg_data=0
reg_addr=140, reg_data=36
reg_addr=141, reg_data=65536
reg_addr=142, reg_data=65536
reg_addr=143, reg_data=2308
reg_addr=144, reg_data=0
reg_addr=145, reg_data=4
PosEmb_wdma done!
Run PosEmb Finish!