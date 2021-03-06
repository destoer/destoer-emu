#pragma once
#include <destoer-emu/lib.h>
#include <destoer-emu/debug.h>
#include <gba/forward_def.h>
#include <gba/cpu_io.h>
#include <gba/arm.h>
#include <gba/dma.h>
#include <gba/interrupt.h>
#include <gba/scheduler.h>


namespace gameboyadvance
{

// tests if a cond field in an instr has been met
constexpr bool cond_lut_helper(uint32_t cond, uint32_t flags)
{
    const auto ac = static_cast<arm_cond>(cond);

    const bool flag_z = flags & 1;
    const bool flag_c = flags & 2;
    const bool flag_n = flags & 4;
    const bool flag_v = flags & 8;

    // switch on the cond bits
    // (lower 4)
    switch(ac)
    {
        // z set
        case arm_cond::eq: return flag_z;
        
        // z clear
        case arm_cond::ne: return !flag_z;

        // c set
        case arm_cond::cs: return flag_c;

        // c clear
        case arm_cond::cc: return !flag_c;

        // n set
        case arm_cond::mi: return flag_n;

        // n clear
        case arm_cond::pl: return !flag_n;

        // v set
        case arm_cond::vs: return flag_v; 

        // v clear
        case arm_cond::vc: return !flag_v;

        // c set and z clear
        case arm_cond::hi: return flag_c && !flag_z;

        // c clear or z set
        case arm_cond::ls: return !flag_c || flag_z;

        // n equals v
        case arm_cond::ge: return flag_n == flag_v;

        // n not equal to v
        case arm_cond::lt: return flag_n != flag_v; 

        // z clear and N equals v
        case arm_cond::gt: return !flag_z && flag_n == flag_v;

        // z set or n not equal to v
        case arm_cond::le: return flag_z || flag_n != flag_v;

        // allways
        case arm_cond::al: return true;

        // not valid - see cond_invalid.gba
        case arm_cond::nv: return false;

    }
    return true; // shoud not be reached
}



using ARM_OPCODE_FPTR = void (Cpu::*)(uint32_t opcode);
using ARM_OPCODE_LUT = std::array<ARM_OPCODE_FPTR,4096>;

using THUMB_OPCODE_FPTR = void (Cpu::*)(uint16_t opcode);
using THUMB_OPCODE_LUT = std::array<THUMB_OPCODE_FPTR,256>;

class Cpu final
{
public:
    Cpu(GBA &gba);
    void init();
    void log_regs();

    void update_intr_status();

    void handle_power_state();

    void exec_instr_no_debug();

    bool interrupt_ready() const
    {
        return interrupt_service && !is_set(cpsr,7);    
    }

    void do_interrupts()
    {
        if(interrupt_ready())
        {
            service_interrupt();
        }
    }


    using EXEC_INSTR_FPTR = void (Cpu::*)(void);
#ifdef DEBUG
    

    EXEC_INSTR_FPTR exec_instr_fptr = &Cpu::exec_instr_no_debug;

    inline void exec_instr()
    {
        std::invoke(exec_instr_fptr,this);
    }

    void exec_instr_debug();

#else 

    inline void exec_instr()
    {
        exec_instr_no_debug();
    }

#endif

    void exec_instr_no_debug_thumb();
    void exec_instr_no_debug_arm();


    void switch_execution_state(bool thumb)
    {
        is_thumb = thumb;
        cpsr = is_thumb? set_bit(cpsr,5) : deset_bit(cpsr,5);
    }


    void cycle_tick(int cycles)
    {
        scheduler.delay_tick(cycles);
    }


    void internal_cycle()
    {
        cycle_tick(1);
    }


    void tick_timer(int t, int cycles);
    void insert_new_timer_event(int timer);

    uint32_t get_pipeline_val() const
    {
        return pipeline[1];
    }

    bool is_in_bios() const
    {
        return in_bios;
    }

    uint32_t get_pc() const 
    {
        return pc_actual;
    }
    uint32_t get_user_regs(int idx) const {return user_regs[idx];}
    uint32_t get_current_regs(int idx) const {return regs[idx]; }
    uint32_t get_status_regs(int idx) const {return status_banked[idx]; }
    uint32_t get_fiq_regs(int idx) const {return fiq_banked[idx]; }
    uint32_t get_high_regs(int idx, int idx2) const {return hi_banked[idx][idx2]; }
    uint32_t get_cpsr() const 
    {
        return (cpsr & ~0xf0000000) | (flag_z << Z_BIT) | 
        (flag_c << C_BIT) | (flag_n << N_BIT) | (flag_v << V_BIT);
    } 


    cpu_mode get_mode() const { return arm_mode; }

    bool is_cpu_thumb() const { return is_thumb; }

    // print all registers for debugging
    // if we go with a graphical debugger
    // we need to make ones for each arrayc
    // and return a std::string
    void print_regs();
    
    void execute_arm_opcode(uint32_t instr);
    void execute_thumb_opcode(uint16_t instr);
    
    void request_interrupt(interrupt i);

#ifdef DEBUG
    void change_breakpoint_enable(bool enabled) noexcept
    {
        if(enabled)
        {
            exec_instr_fptr = &Cpu::exec_instr_debug;         
        }

        else
        {
            exec_instr_fptr = &Cpu::exec_instr_no_debug; 
        }
    }
#endif


    // cpu io memory
    CpuIo cpu_io;

    uint32_t rom_wait_sequential_16 = 1;
    uint32_t rom_wait_sequential_32 = 1;
private:


    void exec_thumb();
    void exec_arm();

    uint32_t fetch_arm_opcode();
    uint16_t fetch_thumb_opcode();

    void arm_fill_pipeline();
    void thumb_fill_pipeline(); 
    void write_pc_arm(uint32_t v);
    void write_pc_thumb(uint32_t v);
    void write_pc(uint32_t v);


public:

    //arm cpu instructions
    void arm_unknown(uint32_t opcode);
    
    template<const bool L>
    void arm_branch(uint32_t opcode);

    template<const bool S,const bool I, const int OP>
    void arm_data_processing(uint32_t opcode);

    template<const bool MSR, const bool SPSR, const bool I>
    void arm_psr(uint32_t opcode);

    template<const bool L, const bool W, const bool P, const bool I>
    void arm_single_data_transfer(uint32_t opcode);


    void arm_branch_and_exchange(uint32_t opcode);

    template<const bool P, const bool U, const bool I, const bool L, const bool W>
    void arm_hds_data_transfer(uint32_t opcode);

    template<const bool S, const bool P, const bool U, const bool W, const bool L>
    void arm_block_data_transfer(uint32_t opcode);

    template<const bool B>
    void arm_swap(uint32_t opcode);

    template<const bool S, const bool A>
    void arm_mul(uint32_t opcode);

    template<bool S, bool A, bool U>
    void arm_mull(uint32_t opcode);

    void arm_swi(uint32_t opcode);



    // thumb cpu instructions
    void thumb_unknown(uint16_t opcode);

    template<const int RD>
    void thumb_ldr_pc(uint16_t opcode);

    template<const int TYPE>
    void thumb_mov_reg_shift(uint16_t opcode);

    template<const int COND>
    void thumb_cond_branch(uint16_t opcode);

    template<const int OP, const int RD>
    void thumb_mcas_imm(uint16_t opcode);

    template<const bool FIRST>
    void thumb_long_bl(uint16_t opcode);

    void thumb_alu(uint16_t opcode);

    template<const int OP>
    void thumb_add_sub(uint16_t opcode);

    template<const int RB, const bool L>
    void thumb_multiple_load_store(uint16_t opcode);

    template<const int OP>
    void thumb_hi_reg_ops(uint16_t opcode);

    template<const int OP>
    void thumb_ldst_imm(uint16_t opcode);

    template<const bool POP, const bool IS_LR>
    void thumb_push_pop(uint16_t opcode);

    template<const int L>
    void thumb_load_store_half(uint16_t opcode);

    void thumb_branch(uint16_t opcode);

    template<const int RD, const bool IS_PC>
    void thumb_get_rel_addr(uint16_t opcode);

    template<const int OP>
    void thumb_load_store_reg(uint16_t opcode);

    template<const int OP>
    void thumb_load_store_sbh(uint16_t opcode);

    void thumb_swi(uint16_t opcode);

    void thumb_sp_add(uint16_t opcode);

    template<const int RD, const bool L>
    void thumb_load_store_sp(uint16_t opcode);





    // tests if a cond field in an instr has been met
    template<const int COND>
    bool cond_met_constexpr()
    {
        const auto ac = static_cast<arm_cond>(COND);

        if constexpr(ac == arm_cond::eq) { return flag_z; }
        else if constexpr(ac ==  arm_cond::ne) { return !flag_z; }
        else if constexpr(ac ==  arm_cond::cs) { return flag_c; }
        else if constexpr(ac ==  arm_cond::cc) { return !flag_c; }
        else if constexpr(ac ==  arm_cond::mi) { return flag_n; }
        else if constexpr(ac ==  arm_cond::pl) { return !flag_n; }
        else if constexpr(ac ==  arm_cond::vs) { return flag_v; }
        else if constexpr(ac ==  arm_cond::vc) { return !flag_v; }
        else if constexpr(ac ==  arm_cond::hi) { return flag_c && !flag_z; }
        else if constexpr(ac ==  arm_cond::ls) { return !flag_c || flag_z; }
        else if constexpr(ac ==  arm_cond::ge) { return flag_n == flag_v; }
        else if constexpr(ac ==  arm_cond::lt) { return flag_n != flag_v; }
        else if constexpr(ac ==  arm_cond::gt) { return !flag_z && flag_n == flag_v; }
        else if constexpr(ac ==  arm_cond::le) { return flag_z || flag_n != flag_v; }
        else if constexpr(ac ==  arm_cond::al) { return true; }
        else if constexpr(ac ==  arm_cond::nv) { return false; }
    }




private:

    // cpu operations eg adds
    uint32_t add(uint32_t v1, uint32_t v2, bool s);
    uint32_t adc(uint32_t v1, uint32_t v2, bool s);
    uint32_t bic(uint32_t v1, uint32_t v2, bool s);
    uint32_t sub(uint32_t v1, uint32_t v2, bool s);
    uint32_t sbc(uint32_t v1, uint32_t v2, bool s);
    uint32_t logical_and(uint32_t v1, uint32_t v2, bool s);
    uint32_t logical_or(uint32_t v1, uint32_t v2, bool s);
    uint32_t logical_eor(uint32_t v1, uint32_t v2, bool s);
    void do_mul_cycles(uint32_t mul_operand);

    bool cond_met(uint32_t cond)
    {
        const auto flags = flag_z | flag_c << 1 | flag_n << 2 | flag_v << 3;

        return is_set(cond_lut[cond],flags);
    }

    void service_interrupt();

    void write_stack_fd(uint32_t reg);
    void read_stack_fd(uint32_t reg);


    void swi(uint32_t function);

    // timers
    void timer_overflow(int timer);

    // mode switching
    void switch_mode(cpu_mode new_mode);
    void store_registers(cpu_mode mode);
    void load_registers(cpu_mode mode);
    void set_cpsr(uint32_t v);
    cpu_mode cpu_mode_from_bits(uint32_t v);

    //flag helpers
    void set_negative_flag(uint32_t v);
    void set_zero_flag(uint32_t v);
    void set_nz_flag(uint32_t v);

    void set_negative_flag_long(uint64_t v);
    void set_zero_flag_long(uint64_t v);
    void set_nz_flag_long(uint64_t v);

    Display &disp;
    Mem &mem;
    Debug &debug;
    Disass &disass;
    Apu &apu;
    GBAScheduler &scheduler;

    // underlying registers

    // registers in the current mode
    // swapped out with backups when a mode switch occurs
    uint32_t regs[16] = {0};

    // flags
    // combined into cpsr when it is read
    bool flag_z;
    bool flag_n;
    bool flag_c;
    bool flag_v;

    bool interrupt_request;
    bool interrupt_service;


    bool bios_hle_interrupt;


    // backup stores
    uint32_t user_regs[16] = {0};
    uint32_t pc_actual = 0;
    uint32_t cpsr = 0; // status reg

    // r8 - r12 banked
    uint32_t fiq_banked[5] = {0};

    // regs 13 and 14 banked
    uint32_t hi_banked[5][2] = {0};

    // banked status regs
    uint32_t status_banked[5] = {0};

    // in arm or thumb mode?
    bool is_thumb = false;

    bool execute_rom = false;

    // instruction state when the currently executed instruction is fetched
    // required to correctly log branches
    bool is_thumb_fetch = false;

    bool dma_in_progress = false;

    // what context is the arm cpu in
    cpu_mode arm_mode;

    // rather than 16 by 16 bool array
    // store are a bitset
    using COND_LUT = std::array<uint16_t,16>;

    constexpr COND_LUT gen_cond_lut()
    {
        COND_LUT arr{};
        for(uint32_t c = 0; c < 16; c++)
        {
            for(uint32_t f = 0; f < 16; f++)
            {
                if(cond_lut_helper(c,f))
                {
                    arr[c] = set_bit(arr[c],f);
                }
            }
        }

        return arr;
    }

    const COND_LUT cond_lut = gen_cond_lut();

    bool in_bios;

    // cpu pipeline
    uint32_t pipeline[2] = {0};
};

}