#pragma once
#include <n64/forward_def.h>
#include <destoer-emu/lib.h>


namespace nintendo64
{


using DISASS_FUNC = std::string (*)(u32 opcode, u64 pc);


std::string disass_opcode(u32 opcode, u64 pc);

std::string disass_lui(u32 opcode, u64 pc);
std::string disass_addiu(u32 opcode, u64 pc);

std::string disass_lw(u32 opcode, u64 pc);


std::string disass_bne(u32 opcode, u64 pc);

std::string disass_unknown(u32 opcode, u64 pc);
std::string disass_unknown_cop0(u32 opcode, u64 pc);

std::string disass_cop0(u32 opcode, u64 pc);
std::string disass_mtc0(u32 opcode, u64 pc); 

}