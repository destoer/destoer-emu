#pragma once
#include <stdint.h>
namespace gameboyadvance
{
// memory constants
constexpr uint32_t IO_MASK = 0x3ff;

// interrupts
constexpr uint32_t IO_IE = 0x04000200 & IO_MASK;
constexpr uint32_t IO_IF = 0x04000202 & IO_MASK;
constexpr uint32_t IO_IME = 0x04000208 & IO_MASK;


constexpr uint32_t IO_WAITCNT = 0x04000204 & IO_MASK;
constexpr uint32_t IO_DISPCNT = 0x04000000 & IO_MASK;
constexpr uint32_t IO_GREENSWAP = 0x04000002 & IO_MASK;
constexpr uint32_t IO_DISPSTAT = 0x04000004 & IO_MASK;
constexpr uint32_t IO_VCOUNT = 0x04000006 & IO_MASK;
constexpr uint32_t IO_BG0CNT = 0x04000008 & IO_MASK;
constexpr uint32_t IO_BG1CNT = 0x0400000a & IO_MASK;
constexpr uint32_t IO_BG2CNT = 0x0400000c & IO_MASK;
constexpr uint32_t IO_BG3CNT = 0x0400000e & IO_MASK;
constexpr uint32_t IO_BG0HOFS = 0x04000010 & IO_MASK; // scroll x for bg0
constexpr uint32_t IO_BG0VOFS = 0x04000012 & IO_MASK; // scroll y for bg0
constexpr uint32_t IO_BG1HOFS = 0x04000014 & IO_MASK; // scroll x for bg1
constexpr uint32_t IO_BG1VOFS = 0x04000016 & IO_MASK; // scroll y for bg1
constexpr uint32_t IO_BG2HOFS = 0x04000018 & IO_MASK; // scroll x for bg2
constexpr uint32_t IO_BG2VOFS = 0x0400001a & IO_MASK; // scroll y for bg2
constexpr uint32_t IO_BG3HOFS = 0x0400001c & IO_MASK; // scroll x for bg3
constexpr uint32_t IO_BG3VOFS = 0x0400001e & IO_MASK; // scroll y for bg3
constexpr uint32_t IO_BG2PA = 0x04000020 & IO_MASK;
constexpr uint32_t IO_BG2PB = 0x04000022 & IO_MASK;
constexpr uint32_t IO_BG2PC = 0x04000024 & IO_MASK;
constexpr uint32_t IO_BG2PD = 0x04000026 & IO_MASK;
constexpr uint32_t IO_BG3PA = 0x04000030 & IO_MASK;
constexpr uint32_t IO_BG3PB = 0x04000032 & IO_MASK;
constexpr uint32_t IO_BG3PC = 0x04000034 & IO_MASK;
constexpr uint32_t IO_BG3PD = 0x04000036 & IO_MASK;
constexpr uint32_t IO_BG2X_L = 0x04000028 & IO_MASK;
constexpr uint32_t IO_BG2X_H = 0x0400002a & IO_MASK;
constexpr uint32_t IO_BG2Y_L = 0x0400002c & IO_MASK;
constexpr uint32_t IO_BG2Y_H = 0x0400002e & IO_MASK;
constexpr uint32_t IO_BG3X_L = 0x04000038 & IO_MASK;
constexpr uint32_t IO_BG3X_H = 0x0400003a & IO_MASK;
constexpr uint32_t IO_BG3Y_L = 0x0400003c & IO_MASK;
constexpr uint32_t IO_BG3Y_H = 0x0400003e & IO_MASK;
constexpr uint32_t IO_WIN0H = 0x04000040 & IO_MASK; // window 0 horizontal dimensions
constexpr uint32_t IO_WIN1H = 0x04000042 & IO_MASK; // window 0 horizontal dimensions
constexpr uint32_t IO_WIN0V = 0x04000044 & IO_MASK; // window 0 horizontal dimensions
constexpr uint32_t IO_WIN1V = 0x04000046 & IO_MASK; // window 0 horizontal dimensions
constexpr uint32_t IO_WININ = 0x04000048 & IO_MASK; // window in
constexpr uint32_t IO_WINOUT = 0x0400004A & IO_MASK; // window out
constexpr uint32_t IO_MOSAIC = 0x0400004c & IO_MASK;

constexpr uint32_t IO_BLDCNT = 0x04000050 & IO_MASK;
constexpr uint32_t IO_BLDALPHA = 0x04000052 & IO_MASK;
constexpr uint32_t IO_BLDY = 0x04000054 & IO_MASK;

// dma 0
constexpr uint32_t IO_DMA0SAD = 0x040000b0 & IO_MASK;
constexpr uint32_t IO_DMA0DAD = 0x040000b4 & IO_MASK;
constexpr uint32_t IO_DMA0CNT_L = 0x040000b8 & IO_MASK;
constexpr uint32_t IO_DMA0CNT_H = 0x040000Ba & IO_MASK;


// dma 1
constexpr uint32_t IO_DMA1SAD = 0x040000bc & IO_MASK;
constexpr uint32_t IO_DMA1DAD = 0x040000c0 & IO_MASK;
constexpr uint32_t IO_DMA1CNT_L = 0x040000c4 & IO_MASK;
constexpr uint32_t IO_DMA1CNT_H = 0x040000c6 & IO_MASK;


// dma 2
constexpr uint32_t IO_DMA2SAD = 0x040000c8 & IO_MASK;
constexpr uint32_t IO_DMA2DAD = 0x040000cc & IO_MASK;
constexpr uint32_t IO_DMA2CNT_L = 0x040000d0 & IO_MASK;
constexpr uint32_t IO_DMA2CNT_H = 0x040000d2 & IO_MASK;

// dma 3
constexpr uint32_t IO_DMA3SAD = 0x040000d4 & IO_MASK;
constexpr uint32_t IO_DMA3DAD = 0x040000d8 & IO_MASK;
constexpr uint32_t IO_DMA3CNT_L = 0x04000dc & IO_MASK;
constexpr uint32_t IO_DMA3CNT_H = 0x040000de & IO_MASK;



// timers
constexpr uint32_t IO_TM0CNT_L = 0x04000100 & IO_MASK;
constexpr uint32_t IO_TM0CNT_H = 0x04000102 & IO_MASK;
constexpr uint32_t IO_TM1CNT_L = 0x04000104 & IO_MASK;
constexpr uint32_t IO_TM1CNT_H = 0x04000106 & IO_MASK;
constexpr uint32_t IO_TM2CNT_L = 0x04000108 & IO_MASK;
constexpr uint32_t IO_TM2CNT_H = 0x0400010a & IO_MASK;
constexpr uint32_t IO_TM3CNT_L = 0x0400010c & IO_MASK;
constexpr uint32_t IO_TM3CNT_H = 0x0400010e & IO_MASK;


constexpr uint32_t IO_KEYINPUT = 0x04000130 & IO_MASK;
constexpr uint32_t IO_KEYCNT = 0x04000132 & IO_MASK;
constexpr uint32_t IO_POSTFLG = 0x040000300 & IO_MASK;

constexpr uint32_t IO_HALTCNT = 0x04000301 & IO_MASK;



constexpr uint32_t IO_SOUNDCNT_H = 0x004000082 & IO_MASK;
constexpr uint32_t IO_SOUNDBIAS = 0x040000088 & IO_MASK;
constexpr uint32_t IO_SOUNDCNT_X = 0x4000084 & IO_MASK;
constexpr uint32_t IO_FIFO_A = 0x040000A0 & IO_MASK;
constexpr uint32_t IO_FIFO_B = 0x040000A4 & IO_MASK;


// serial
constexpr uint32_t SIOCNT = 0x04000128 & IO_MASK;



// gb psgs
constexpr uint32_t IO_NR10 = 0x04000060 & IO_MASK;
constexpr uint32_t IO_NR11 = 0x04000062 & IO_MASK;
constexpr uint32_t IO_NR12 = 0x04000063 & IO_MASK;
constexpr uint32_t IO_NR13 = 0x04000064 & IO_MASK;
constexpr uint32_t IO_NR14 = 0x04000065 & IO_MASK;

constexpr uint32_t IO_NR21 = 0x04000068 & IO_MASK;
constexpr uint32_t IO_NR22 = 0x04000069 & IO_MASK;
constexpr uint32_t IO_NR23 = 0x0400006C & IO_MASK;
constexpr uint32_t IO_NR24 = 0x0400006D & IO_MASK;

constexpr uint32_t IO_NR30 = 0x04000070 & IO_MASK;
constexpr uint32_t IO_NR31 = 0x04000072 & IO_MASK;
constexpr uint32_t IO_NR32 = 0x04000073 & IO_MASK;
constexpr uint32_t IO_NR33 = 0x04000074 & IO_MASK;
constexpr uint32_t IO_NR34 = 0x04000075 & IO_MASK;

constexpr uint32_t IO_NR41 = 0x04000078 & IO_MASK;
constexpr uint32_t IO_NR42 = 0x04000079 & IO_MASK;
constexpr uint32_t IO_NR43 = 0x0400007C & IO_MASK;
constexpr uint32_t IO_NR44 = 0x0400007D & IO_MASK;

constexpr uint32_t IO_NR50 = 0x04000080 & IO_MASK;
constexpr uint32_t IO_NR51 = 0x04000081 & IO_MASK;
constexpr uint32_t IO_NR52 = 0x04000084 & IO_MASK;

}