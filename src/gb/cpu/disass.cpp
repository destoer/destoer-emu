#include <gb/disass.h>
#include <gb/memory.h>
#include <destoer-emu/lib.h>


namespace gameboy
{

void Disass::init(Memory *m)
{
    mem = m;
}



enum class disass_type
{
    op_u16=0, op_u8=1, op_i8=2, op_none=3
};


constexpr int disass_type_size[] = 
{
    3, // opcode and u16 oper (op_u16)
    2, // opcode and u8 oper (op_u8)
    2, // ditto (op_i8)
    1 // opcode only (op_none)
};

struct Disass_entry
{
    const char *fmt_str;
    disass_type type;
};

// tables auto generated with a script
constexpr Disass_entry opcode_table[256] = 
{
    {"nop",disass_type::op_none},
    {"ld bc,{:x}",disass_type::op_u16},
    {"ld (bc),a",disass_type::op_none},
    {"inc bc",disass_type::op_none},
    {"inc b",disass_type::op_none},
    {"dec b",disass_type::op_none},
    {"ld b,{:x}",disass_type::op_u8},
    {"rlca",disass_type::op_none},
    {"ld ({:x}),sp",disass_type::op_u16},
    {"add hl,bc",disass_type::op_none},
    {"ld a,(bc)",disass_type::op_none},
    {"dec bc",disass_type::op_none},
    {"inc c",disass_type::op_none},
    {"dec c",disass_type::op_none},
    {"ld c,{:x}",disass_type::op_u8},
    {"rrca",disass_type::op_none},
    {"stop",disass_type::op_none},
    {"ld de,{:x}",disass_type::op_u16},
    {"ld (de),a",disass_type::op_none},
    {"inc de",disass_type::op_none},
    {"inc d",disass_type::op_none},
    {"dec d",disass_type::op_none},
    {"ld d,{:x}",disass_type::op_u8},
    {"rla",disass_type::op_none},
    {"jr {:x}",disass_type::op_i8},
    {"add hl,de",disass_type::op_none},
    {"ld a,(de)",disass_type::op_none},
    {"dec de",disass_type::op_none},
    {"inc e",disass_type::op_none},
    {"dec e",disass_type::op_none},
    {"ld e,{:x}",disass_type::op_u8},
    {"rra",disass_type::op_none},
    {"jr nz,{:x}",disass_type::op_i8},
    {"ld hl,{:x}",disass_type::op_u16},
    {"ld (hl+),a",disass_type::op_none},
    {"inc hl",disass_type::op_none},
    {"inc h",disass_type::op_none},
    {"dec h",disass_type::op_none},
    {"ld h,{:x}",disass_type::op_u8},
    {"daa",disass_type::op_none},
    {"jr z,{:x}",disass_type::op_i8},
    {"add hl,hl",disass_type::op_none},
    {"ld a,(hl+)",disass_type::op_none},
    {"dec hl",disass_type::op_none},
    {"inc l",disass_type::op_none},
    {"dec l",disass_type::op_none},
    {"ld l,{:x}",disass_type::op_u8},
    {"cpl",disass_type::op_none},
    {"jr nc,{:x}",disass_type::op_i8},
    {"ld sp,{:x}",disass_type::op_u16},
    {"ld (hl-),a",disass_type::op_none},
    {"inc sp",disass_type::op_none},
    {"inc (hl)",disass_type::op_none},
    {"dec (hl)",disass_type::op_none},
    {"ld (hl),{:x}",disass_type::op_u8},
    {"scf",disass_type::op_none},
    {"jr c,{:x}",disass_type::op_i8},
    {"add hl,sp",disass_type::op_none},
    {"ld a,(hl-)",disass_type::op_none},
    {"dec sp",disass_type::op_none},
    {"inc a",disass_type::op_none},
    {"dec a",disass_type::op_none},
    {"ld a,{:x}",disass_type::op_u8},
    {"ccf",disass_type::op_none},
    {"ld b,b",disass_type::op_none},
    {"ld b,c",disass_type::op_none},
    {"ld b,d",disass_type::op_none},
    {"ld b,e",disass_type::op_none},
    {"ld b,h",disass_type::op_none},
    {"ld b,l",disass_type::op_none},
    {"ld b,(hl)",disass_type::op_none},
    {"ld b,a",disass_type::op_none},
    {"ld c,b",disass_type::op_none},
    {"ld c,c",disass_type::op_none},
    {"ld c,d",disass_type::op_none},
    {"ld c,e",disass_type::op_none},
    {"ld c,h",disass_type::op_none},
    {"ld c,l",disass_type::op_none},
    {"ld c,(hl)",disass_type::op_none},
    {"ld c,a",disass_type::op_none},
    {"ld d,b",disass_type::op_none},
    {"ld d,c",disass_type::op_none},
    {"ld d,d",disass_type::op_none},
    {"ld d,e",disass_type::op_none},
    {"ld d,h",disass_type::op_none},
    {"ld d,l",disass_type::op_none},
    {"ld d,(hl)",disass_type::op_none},
    {"ld d,a",disass_type::op_none},
    {"ld e,b",disass_type::op_none},
    {"ld e,c",disass_type::op_none},
    {"ld e,d",disass_type::op_none},
    {"ld e,e",disass_type::op_none},
    {"ld e,h",disass_type::op_none},
    {"ld e,l",disass_type::op_none},
    {"ld e,(hl)",disass_type::op_none},
    {"ld e,a",disass_type::op_none},
    {"ld h,b",disass_type::op_none},
    {"ld h,c",disass_type::op_none},
    {"ld h,d",disass_type::op_none},
    {"ld h,e",disass_type::op_none},
    {"ld h,h",disass_type::op_none},
    {"ld h,l",disass_type::op_none},
    {"ld h,(hl)",disass_type::op_none},
    {"ld h,a",disass_type::op_none},
    {"ld l,b",disass_type::op_none},
    {"ld l,c",disass_type::op_none},
    {"ld l,d",disass_type::op_none},
    {"ld l,e",disass_type::op_none},
    {"ld l,h",disass_type::op_none},
    {"ld l,l",disass_type::op_none},
    {"ld l,(hl)",disass_type::op_none},
    {"ld l,a",disass_type::op_none},
    {"ld (hl),b",disass_type::op_none},
    {"ld (hl),c",disass_type::op_none},
    {"ld (hl),d",disass_type::op_none},
    {"ld (hl),e",disass_type::op_none},
    {"ld (hl),h",disass_type::op_none},
    {"ld (hl),l",disass_type::op_none},
    {"halt",disass_type::op_none},
    {"ld (hl),a",disass_type::op_none},
    {"ld a,b",disass_type::op_none},
    {"ld a,c",disass_type::op_none},
    {"ld a,d",disass_type::op_none},
    {"ld a,e",disass_type::op_none},
    {"ld a,h",disass_type::op_none},
    {"ld a,l",disass_type::op_none},
    {"ld a,(hl)",disass_type::op_none},
    {"ld a,a",disass_type::op_none},
    {"add a,b",disass_type::op_none},
    {"add a,c",disass_type::op_none},
    {"add a,d",disass_type::op_none},
    {"add a,e",disass_type::op_none},
    {"add a,h",disass_type::op_none},
    {"add a,l",disass_type::op_none},
    {"add a,(hl)",disass_type::op_none},
    {"add a,a",disass_type::op_none},
    {"adc a,b",disass_type::op_none},
    {"adc a,c",disass_type::op_none},
    {"adc a,d",disass_type::op_none},
    {"adc a,e",disass_type::op_none},
    {"adc a,h",disass_type::op_none},
    {"adc a,l",disass_type::op_none},
    {"adc a,(hl)",disass_type::op_none},
    {"adc a,a",disass_type::op_none},
    {"sub a,b",disass_type::op_none},
    {"sub a,c",disass_type::op_none},
    {"sub a,d",disass_type::op_none},
    {"sub a,e",disass_type::op_none},
    {"sub a,h",disass_type::op_none},
    {"sub a,l",disass_type::op_none},
    {"sub a,(hl)",disass_type::op_none},
    {"sub a,a",disass_type::op_none},
    {"sbc a,b",disass_type::op_none},
    {"sbc a,c",disass_type::op_none},
    {"sbc a,d",disass_type::op_none},
    {"sbc a,e",disass_type::op_none},
    {"sbc a,h",disass_type::op_none},
    {"sbc a,l",disass_type::op_none},
    {"sbc a,(hl)",disass_type::op_none},
    {"sbc a,a",disass_type::op_none},
    {"and a,b",disass_type::op_none},
    {"and a,c",disass_type::op_none},
    {"and a,d",disass_type::op_none},
    {"and a,e",disass_type::op_none},
    {"and a,h",disass_type::op_none},
    {"and a,l",disass_type::op_none},
    {"and a,(hl)",disass_type::op_none},
    {"and a,a",disass_type::op_none},
    {"xor a,b",disass_type::op_none},
    {"xor a,c",disass_type::op_none},
    {"xor a,d",disass_type::op_none},
    {"xor a,e",disass_type::op_none},
    {"xor a,h",disass_type::op_none},
    {"xor a,l",disass_type::op_none},
    {"xor a,(hl)",disass_type::op_none},
    {"xor a,a",disass_type::op_none},
    {"or a,b",disass_type::op_none},
    {"or a,c",disass_type::op_none},
    {"or a,d",disass_type::op_none},
    {"or a,e",disass_type::op_none},
    {"or a,h",disass_type::op_none},
    {"or a,l",disass_type::op_none},
    {"or a,(hl)",disass_type::op_none},
    {"or a,a",disass_type::op_none},
    {"cp a,b",disass_type::op_none},
    {"cp a,c",disass_type::op_none},
    {"cp a,d",disass_type::op_none},
    {"cp a,e",disass_type::op_none},
    {"cp a,h",disass_type::op_none},
    {"cp a,l",disass_type::op_none},
    {"cp a,(hl)",disass_type::op_none},
    {"cp a,a",disass_type::op_none},
    {"ret nz",disass_type::op_none},
    {"pop bc",disass_type::op_none},
    {"jp nz,{:x}",disass_type::op_u16},
    {"jp {:x}",disass_type::op_u16},
    {"call nz,{:x}",disass_type::op_u16},
    {"push bc",disass_type::op_none},
    {"add a,{:x}",disass_type::op_u8},
    {"rst 00h",disass_type::op_none},
    {"ret z",disass_type::op_none},
    {"ret",disass_type::op_none},
    {"jp z,{:x}",disass_type::op_u16},
    {"prefix cb",disass_type::op_none},
    {"call z,{:x}",disass_type::op_u16},
    {"call {:x}",disass_type::op_u16},
    {"adc a,{:x}",disass_type::op_u8},
    {"rst 08h",disass_type::op_none},
    {"ret nc",disass_type::op_none},
    {"pop de",disass_type::op_none},
    {"jp nc,{:x}",disass_type::op_u16},
    {"unknown opcode",disass_type::op_none},
    {"call nc,{:x}",disass_type::op_u16},
    {"push de",disass_type::op_none},
    {"sub a,{:x}",disass_type::op_u8},
    {"rst 10h",disass_type::op_none},
    {"ret c",disass_type::op_none},
    {"reti",disass_type::op_none},
    {"jp c,{:x}",disass_type::op_u16},
    {"unknown opcode",disass_type::op_none},
    {"call c,{:x}",disass_type::op_u16},
    {"unknown opcode",disass_type::op_none},
    {"sbc a,{:x}",disass_type::op_u8},
    {"rst 18h",disass_type::op_none},
    {"ld (ff00+{:x}),a",disass_type::op_u8},
    {"pop hl",disass_type::op_none},
    {"ld (ff00+c),a",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"push hl",disass_type::op_none},
    {"and a,{:x}",disass_type::op_u8},
    {"rst 20h",disass_type::op_none},
    {"add sp,{:x}",disass_type::op_i8},
    {"jp hl",disass_type::op_none},
    {"ld ({:x}),a",disass_type::op_u16},
    {"unknown opcode",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"xor a,{:x}",disass_type::op_u8},
    {"rst 28h",disass_type::op_none},
    {"ld a,(ff00+{:x})",disass_type::op_u8},
    {"pop af",disass_type::op_none},
    {"ld a,(ff00+c)",disass_type::op_none},
    {"di",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"push af",disass_type::op_none},
    {"or a,{:x}",disass_type::op_u8},
    {"rst 30h",disass_type::op_none},
    {"ld hl,sp+{:x}",disass_type::op_i8},
    {"ld sp,hl",disass_type::op_none},
    {"ld a,({:x})",disass_type::op_u16},
    {"ei",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"unknown opcode",disass_type::op_none},
    {"cp a,{:x}",disass_type::op_u8},
    {"rst 38h",disass_type::op_none}
};


constexpr Disass_entry cb_opcode_table[256] = 
{
    {"rlc b",disass_type::op_none},
    {"rlc c",disass_type::op_none},
    {"rlc d",disass_type::op_none},
    {"rlc e",disass_type::op_none},
    {"rlc h",disass_type::op_none},
    {"rlc l",disass_type::op_none},
    {"rlc (hl)",disass_type::op_none},
    {"rlc a",disass_type::op_none},
    {"rrc b",disass_type::op_none},
    {"rrc c",disass_type::op_none},
    {"rrc d",disass_type::op_none},
    {"rrc e",disass_type::op_none},
    {"rrc h",disass_type::op_none},
    {"rrc l",disass_type::op_none},
    {"rrc (hl)",disass_type::op_none},
    {"rrc a",disass_type::op_none},
    {"rl b",disass_type::op_none},
    {"rl c",disass_type::op_none},
    {"rl d",disass_type::op_none},
    {"rl e",disass_type::op_none},
    {"rl h",disass_type::op_none},
    {"rl l",disass_type::op_none},
    {"rl (hl)",disass_type::op_none},
    {"rl a",disass_type::op_none},
    {"rr b",disass_type::op_none},
    {"rr c",disass_type::op_none},
    {"rr d",disass_type::op_none},
    {"rr e",disass_type::op_none},
    {"rr h",disass_type::op_none},
    {"rr l",disass_type::op_none},
    {"rr (hl)",disass_type::op_none},
    {"rr a",disass_type::op_none},
    {"sla b",disass_type::op_none},
    {"sla c",disass_type::op_none},
    {"sla d",disass_type::op_none},
    {"sla e",disass_type::op_none},
    {"sla h",disass_type::op_none},
    {"sla l",disass_type::op_none},
    {"sla (hl)",disass_type::op_none},
    {"sla a",disass_type::op_none},
    {"sra b",disass_type::op_none},
    {"sra c",disass_type::op_none},
    {"sra d",disass_type::op_none},
    {"sra e",disass_type::op_none},
    {"sra h",disass_type::op_none},
    {"sra l",disass_type::op_none},
    {"sra (hl)",disass_type::op_none},
    {"sra a",disass_type::op_none},
    {"swap b",disass_type::op_none},
    {"swap c",disass_type::op_none},
    {"swap d",disass_type::op_none},
    {"swap e",disass_type::op_none},
    {"swap h",disass_type::op_none},
    {"swap l",disass_type::op_none},
    {"swap (hl)",disass_type::op_none},
    {"swap a",disass_type::op_none},
    {"srl b",disass_type::op_none},
    {"srl c",disass_type::op_none},
    {"srl d",disass_type::op_none},
    {"srl e",disass_type::op_none},
    {"srl h",disass_type::op_none},
    {"srl l",disass_type::op_none},
    {"srl (hl)",disass_type::op_none},
    {"srl a",disass_type::op_none},
    {"bit 0,b",disass_type::op_none},
    {"bit 0,c",disass_type::op_none},
    {"bit 0,d",disass_type::op_none},
    {"bit 0,e",disass_type::op_none},
    {"bit 0,h",disass_type::op_none},
    {"bit 0,l",disass_type::op_none},
    {"bit 0,(hl)",disass_type::op_none},
    {"bit 0,a",disass_type::op_none},
    {"bit 1,b",disass_type::op_none},
    {"bit 1,c",disass_type::op_none},
    {"bit 1,d",disass_type::op_none},
    {"bit 1,e",disass_type::op_none},
    {"bit 1,h",disass_type::op_none},
    {"bit 1,l",disass_type::op_none},
    {"bit 1,(hl)",disass_type::op_none},
    {"bit 1,a",disass_type::op_none},
    {"bit 2,b",disass_type::op_none},
    {"bit 2,c",disass_type::op_none},
    {"bit 2,d",disass_type::op_none},
    {"bit 2,e",disass_type::op_none},
    {"bit 2,h",disass_type::op_none},
    {"bit 2,l",disass_type::op_none},
    {"bit 2,(hl)",disass_type::op_none},
    {"bit 2,a",disass_type::op_none},
    {"bit 3,b",disass_type::op_none},
    {"bit 3,c",disass_type::op_none},
    {"bit 3,d",disass_type::op_none},
    {"bit 3,e",disass_type::op_none},
    {"bit 3,h",disass_type::op_none},
    {"bit 3,l",disass_type::op_none},
    {"bit 3,(hl)",disass_type::op_none},
    {"bit 3,a",disass_type::op_none},
    {"bit 4,b",disass_type::op_none},
    {"bit 4,c",disass_type::op_none},
    {"bit 4,d",disass_type::op_none},
    {"bit 4,e",disass_type::op_none},
    {"bit 4,h",disass_type::op_none},
    {"bit 4,l",disass_type::op_none},
    {"bit 4,(hl)",disass_type::op_none},
    {"bit 4,a",disass_type::op_none},
    {"bit 5,b",disass_type::op_none},
    {"bit 5,c",disass_type::op_none},
    {"bit 5,d",disass_type::op_none},
    {"bit 5,e",disass_type::op_none},
    {"bit 5,h",disass_type::op_none},
    {"bit 5,l",disass_type::op_none},
    {"bit 5,(hl)",disass_type::op_none},
    {"bit 5,a",disass_type::op_none},
    {"bit 6,b",disass_type::op_none},
    {"bit 6,c",disass_type::op_none},
    {"bit 6,d",disass_type::op_none},
    {"bit 6,e",disass_type::op_none},
    {"bit 6,h",disass_type::op_none},
    {"bit 6,l",disass_type::op_none},
    {"bit 6,(hl)",disass_type::op_none},
    {"bit 6,a",disass_type::op_none},
    {"bit 7,b",disass_type::op_none},
    {"bit 7,c",disass_type::op_none},
    {"bit 7,d",disass_type::op_none},
    {"bit 7,e",disass_type::op_none},
    {"bit 7,h",disass_type::op_none},
    {"bit 7,l",disass_type::op_none},
    {"bit 7,(hl)",disass_type::op_none},
    {"bit 7,a",disass_type::op_none},
    {"res 0,b",disass_type::op_none},
    {"res 0,c",disass_type::op_none},
    {"res 0,d",disass_type::op_none},
    {"res 0,e",disass_type::op_none},
    {"res 0,h",disass_type::op_none},
    {"res 0,l",disass_type::op_none},
    {"res 0,(hl)",disass_type::op_none},
    {"res 0,a",disass_type::op_none},
    {"res 1,b",disass_type::op_none},
    {"res 1,c",disass_type::op_none},
    {"res 1,d",disass_type::op_none},
    {"res 1,e",disass_type::op_none},
    {"res 1,h",disass_type::op_none},
    {"res 1,l",disass_type::op_none},
    {"res 1,(hl)",disass_type::op_none},
    {"res 1,a",disass_type::op_none},
    {"res 2,b",disass_type::op_none},
    {"res 2,c",disass_type::op_none},
    {"res 2,d",disass_type::op_none},
    {"res 2,e",disass_type::op_none},
    {"res 2,h",disass_type::op_none},
    {"res 2,l",disass_type::op_none},
    {"res 2,(hl)",disass_type::op_none},
    {"res 2,a",disass_type::op_none},
    {"res 3,b",disass_type::op_none},
    {"res 3,c",disass_type::op_none},
    {"res 3,d",disass_type::op_none},
    {"res 3,e",disass_type::op_none},
    {"res 3,h",disass_type::op_none},
    {"res 3,l",disass_type::op_none},
    {"res 3,(hl)",disass_type::op_none},
    {"res 3,a",disass_type::op_none},
    {"res 4,b",disass_type::op_none},
    {"res 4,c",disass_type::op_none},
    {"res 4,d",disass_type::op_none},
    {"res 4,e",disass_type::op_none},
    {"res 4,h",disass_type::op_none},
    {"res 4,l",disass_type::op_none},
    {"res 4,(hl)",disass_type::op_none},
    {"res 4,a",disass_type::op_none},
    {"res 5,b",disass_type::op_none},
    {"res 5,c",disass_type::op_none},
    {"res 5,d",disass_type::op_none},
    {"res 5,e",disass_type::op_none},
    {"res 5,h",disass_type::op_none},
    {"res 5,l",disass_type::op_none},
    {"res 5,(hl)",disass_type::op_none},
    {"res 5,a",disass_type::op_none},
    {"res 6,b",disass_type::op_none},
    {"res 6,c",disass_type::op_none},
    {"res 6,d",disass_type::op_none},
    {"res 6,e",disass_type::op_none},
    {"res 6,h",disass_type::op_none},
    {"res 6,l",disass_type::op_none},
    {"res 6,(hl)",disass_type::op_none},
    {"res 6,a",disass_type::op_none},
    {"res 7,b",disass_type::op_none},
    {"res 7,c",disass_type::op_none},
    {"res 7,d",disass_type::op_none},
    {"res 7,e",disass_type::op_none},
    {"res 7,h",disass_type::op_none},
    {"res 7,l",disass_type::op_none},
    {"res 7,(hl)",disass_type::op_none},
    {"res 7,a",disass_type::op_none},
    {"set 0,b",disass_type::op_none},
    {"set 0,c",disass_type::op_none},
    {"set 0,d",disass_type::op_none},
    {"set 0,e",disass_type::op_none},
    {"set 0,h",disass_type::op_none},
    {"set 0,l",disass_type::op_none},
    {"set 0,(hl)",disass_type::op_none},
    {"set 0,a",disass_type::op_none},
    {"set 1,b",disass_type::op_none},
    {"set 1,c",disass_type::op_none},
    {"set 1,d",disass_type::op_none},
    {"set 1,e",disass_type::op_none},
    {"set 1,h",disass_type::op_none},
    {"set 1,l",disass_type::op_none},
    {"set 1,(hl)",disass_type::op_none},
    {"set 1,a",disass_type::op_none},
    {"set 2,b",disass_type::op_none},
    {"set 2,c",disass_type::op_none},
    {"set 2,d",disass_type::op_none},
    {"set 2,e",disass_type::op_none},
    {"set 2,h",disass_type::op_none},
    {"set 2,l",disass_type::op_none},
    {"set 2,(hl)",disass_type::op_none},
    {"set 2,a",disass_type::op_none},
    {"set 3,b",disass_type::op_none},
    {"set 3,c",disass_type::op_none},
    {"set 3,d",disass_type::op_none},
    {"set 3,e",disass_type::op_none},
    {"set 3,h",disass_type::op_none},
    {"set 3,l",disass_type::op_none},
    {"set 3,(hl)",disass_type::op_none},
    {"set 3,a",disass_type::op_none},
    {"set 4,b",disass_type::op_none},
    {"set 4,c",disass_type::op_none},
    {"set 4,d",disass_type::op_none},
    {"set 4,e",disass_type::op_none},
    {"set 4,h",disass_type::op_none},
    {"set 4,l",disass_type::op_none},
    {"set 4,(hl)",disass_type::op_none},
    {"set 4,a",disass_type::op_none},
    {"set 5,b",disass_type::op_none},
    {"set 5,c",disass_type::op_none},
    {"set 5,d",disass_type::op_none},
    {"set 5,e",disass_type::op_none},
    {"set 5,h",disass_type::op_none},
    {"set 5,l",disass_type::op_none},
    {"set 5,(hl)",disass_type::op_none},
    {"set 5,a",disass_type::op_none},
    {"set 6,b",disass_type::op_none},
    {"set 6,c",disass_type::op_none},
    {"set 6,d",disass_type::op_none},
    {"set 6,e",disass_type::op_none},
    {"set 6,h",disass_type::op_none},
    {"set 6,l",disass_type::op_none},
    {"set 6,(hl)",disass_type::op_none},
    {"set 6,a",disass_type::op_none},
    {"set 7,b",disass_type::op_none},
    {"set 7,c",disass_type::op_none},
    {"set 7,d",disass_type::op_none},
    {"set 7,e",disass_type::op_none},
    {"set 7,h",disass_type::op_none},
    {"set 7,l",disass_type::op_none},
    {"set 7,(hl)",disass_type::op_none},
    {"set 7,a",disass_type::op_none}    
};

// not sure if its worth just adding a size field
// and having a bunch of extra bytes in the exe
uint32_t Disass::get_op_sz(uint16_t addr)
{
    uint8_t opcode = mem->read_mem(addr++);

    bool is_cb = opcode == 0xcb;

    disass_type type = is_cb ? cb_opcode_table[mem->read_mem(addr)].type : opcode_table[opcode].type;

    int size = is_cb?  1 : 0; // 1 byte prefix for cb

    return size + disass_type_size[static_cast<int>(type)];
}



std::string Disass::disass_op(uint16_t addr)
{

    uint8_t opcode = mem->read_mem(addr++);

    
    if(opcode == 0xcb)
    {
        opcode = mem->read_mem(addr);
        Disass_entry entry = cb_opcode_table[opcode];
        return std::string(entry.fmt_str);
    }

    else
    {
        Disass_entry entry = opcode_table[opcode];


        switch(entry.type)
        {
            // eg jp
            case disass_type::op_u16:
            {
                return fmt::format(entry.fmt_str,mem->read_word(addr));
            }

            // eg ld a, 0xff
            case disass_type::op_u8:
            {
                return fmt::format(entry.fmt_str,mem->read_mem(addr));
            }

            // eg jr
            case disass_type::op_i8:
            {
                int8_t operand = mem->read_mem(addr++);
                return fmt::format(entry.fmt_str,addr+operand);
            }

            // eg ld a, b
            case disass_type::op_none:
            {
                return std::string(entry.fmt_str);
            }
        }
    }
    return "disass_failed!?"; // should not be  reached
}

}