#include <gba/gba.h>
#include <limits.h>

namespace gameboyadvance
{

Cpu::Cpu(GBA &gba) : disp(gba.disp), mem(gba.mem), debug(gba.debug), 
    disass(gba.disass), apu(gba.apu), scheduler(gba.scheduler)
{

}   

void Cpu::init()
{
    // current registers
    memset(regs,0,sizeof(user_regs));

    // backup stores
    memset(user_regs,0,sizeof(user_regs));

    // r8 - r12 banked
    memset(fiq_banked,0,sizeof(fiq_banked)); 

    // regs 13 and 14 banked
    memset(hi_banked,0,sizeof(hi_banked));

    // banked status regs
    memset(status_banked,0,sizeof(status_banked));



    // setup main cpu state
    switch_execution_state(false);  // cpu in arm mode

    write_pc(0x08000000); // cartrige reset vector
    regs[LR] = 0x08000000;
    cpsr = 0x1f;
    regs[SP] = 0x03007f00;
    hi_banked[static_cast<int>(cpu_mode::supervisor)][0] = 0x03007FE0;
    hi_banked[static_cast<int>(cpu_mode::irq)][0] = 0x03007FA0;


    //write_pc(0);
    arm_mode = cpu_mode::system;
    switch_mode(cpu_mode::system);

    // flags
    // combined into cpsr when it is read
    flag_z = false;
    flag_n = false;
    flag_c = false;
    flag_n = false;

    in_bios = false;

    bios_hle_interrupt = false;

    rom_wait_sequential_16 = 1;
    rom_wait_sequential_32 = 1;

    cpu_io.init();
    update_intr_status();
    debug.trace.clear();
}

void Cpu::insert_new_timer_event(int timer)
{
    const auto event_type = static_cast<gba_event>(timer + static_cast<int>(gba_event::timer0));

    // smash off any existing event
    scheduler.remove(event_type);

    // ok here we want to know how many cycles it will take for an overflow
    const auto &r = cpu_io.timers[timer];
    const auto limit = r.cycle_limit[r.scale];
    const auto cycles = (limit * (0x10000 - r.counter)) - r.cycle_count;

    const auto event = scheduler.create_event(cycles,event_type);
    scheduler.insert(event,false);    
}


void Cpu::tick_timer(int t, int cycles)
{
    auto &timer = cpu_io.timers[t];

    // increments on prev timer overflow so we dont care
    // timer is not enabled we dont care
    if(!timer.enable || timer.count_up)
    {
        return;
    }

    timer.cycle_count += cycles;

    const auto limit = timer.cycle_limit[timer.scale];

    // if its above the threshold
    if(timer.cycle_count >= limit)
    {
        //puts("timer inc");

        // timer += how many limits passed
        // / (compilier is not smart enough to use a shift here)
        const auto ticks =  timer.cycle_count >> timer.shift_table[timer.scale];

        // adjust cycle count accordingly
        // % (compilier is not smart enough to use a & here)
        timer.cycle_count &= limit - 1;

        // timer overflowed
        if(timer.counter + ticks > 0xffff)
        {
            timer_overflow(t);
            insert_new_timer_event(t);
        }

        else
        {
            timer.counter += ticks;
        }
    }    
}

void Cpu::timer_overflow(int timer_num)
{
    auto &timer = cpu_io.timers[timer_num];

    // reload the timer with inital value
    timer.counter = timer.reload;

    // if fire irq on timer overflow
    if(timer.irq) 
    {
        request_interrupt(timer.timer_interrupt);
    }


    // do we need to fire it specifically to the dma is actually pointing
    // at what fifo reg?
    // need to test what happens on hardware if dmas fire when they dont point to valid regs
    

    // if the timer num is equal to the dma sound channels dma
    // request a fifo dma if it doesent have 16 bytes
    // then push a fifo byte to the apu
    if(timer_num == apu.apu_io.sound_cnt.timer_num_a)
    {
        if(apu.apu_io.fifo_a.len <= 16)
        {
            mem.dma.handle_dma(dma_type::fifo_a);
        }

        const auto x = apu.apu_io.fifo_a.read();
        //printf("fifo a %x\n",x);
        apu.push_dma_a(x);
    }

    if(timer_num == apu.apu_io.sound_cnt.timer_num_b)
    {
        if(apu.apu_io.fifo_b.len <= 16)
        {
            mem.dma.handle_dma(dma_type::fifo_b);
        }

        const auto x = apu.apu_io.fifo_b.read();
        //printf("fifo b %x\n",x);
        apu.push_dma_b(x);
    }


    // check if the timer above is subject to cascade
    // in what oreder should this happen 
    // should the current timer fire its irq first?
    if(timer_num != 3) // timer 0 cant cascade
    {
        auto &next_timer = cpu_io.timers[timer_num+1];

        if(next_timer.enable && next_timer.count_up)
        {
            // about to converflow
            if(next_timer.counter == 0xffff)
            {
                timer_overflow(timer_num+1);
            }

            else
            {
                next_timer.counter += 1;
            }
        }
    }
}


void Cpu::exec_instr_no_debug_arm()
{
    is_thumb_fetch = false;
    exec_arm();
}

void Cpu::exec_instr_no_debug_thumb()
{    
    is_thumb_fetch = true;
    exec_thumb();
}


void Cpu::exec_instr_no_debug()
{
    if(is_thumb)
    {
        exec_instr_no_debug_thumb();
    }

    else
    {
        exec_instr_no_debug_arm();
    }
}


#ifdef DEBUG
void Cpu::exec_instr_debug()
{
    const uint32_t pc = pc_actual;
    const uint32_t v = pipeline[0];
	if(debug.breakpoint_hit(pc,v,break_type::execute))
	{
		// halt until told otherwhise :)
		write_log(debug,"[DEBUG] execute breakpoint hit ({:x}:{:x})",pc,v);
		debug.halt();
        return;
	}
    exec_instr_no_debug();
}
#endif

// we need to replace this with proper scheduling but its fine for now
void Cpu::handle_power_state()
{
        // check halt and stop here?
    switch(cpu_io.halt_cnt.state)
    {
        // normal (just run as normal)
        case HaltCnt::power_state::normal:
        {
            break;
        }

        case HaltCnt::power_state::halt:
        {
            // debug perf
            //printf("%d:%d\n",disp.get_vcount(),disp.get_cycles());
            //puts("halt");
            
            // need a better check here to prevent the emulator just locking up
            if(!cpu_io.interrupt_enable)
            {
                throw std::runtime_error(fmt::format("[halt] halt locked up at {:08x}",pc_actual));
            }

            // tick cycles until we an interrupt fires
            while(!interrupt_request)
            {
                if(scheduler.size() == 0)
                {
                    throw std::runtime_error("halt infinite loop");
                }
                scheduler.skip_to_event();     
            }
            cpu_io.halt_cnt.state = HaltCnt::power_state::normal;
            break;
        }

        // i think this halts actions until buttons are pressed?
        case HaltCnt::power_state::stop:
        {
            puts("stop unimplemented!");
            cpu_io.halt_cnt.state = HaltCnt::power_state::normal;
            break;
        }
    }
}


// debug register printing
void Cpu::print_regs()
{

    // update current registers
    // so they can be printed
    store_registers(arm_mode);


    debug.print_console("current mode: {}\n",mode_names[static_cast<int>(arm_mode)]);
    debug.print_console("cpu state: {}\n", is_thumb? "thumb" : "arm");

    debug.print_console("USER & SYSTEM REGS\n");

    for(int i = 0; i < 15; i++)
    {
        debug.print_console("{}: {:08x} ",user_regs_names[i],user_regs[i]);
        if((i % 2) == 0)
        {
            debug.print_console("\n");
        }
    }

    debug.print_console("pc: {:08x} ",pc_actual);


    debug.print_console("\n\nFIQ BANKED\n");


    for(int i = 0; i < 5; i++)
    {
        debug.print_console("{}: {:08x} ",fiq_banked_names[i],fiq_banked[i]);
        if((i % 2) == 0)
        {
            debug.print_console("\n");
        }        
    }

    debug.print_console("\nHI BANKED\n");

    for(int i = 0; i < 5; i++)
    {
        debug.print_console("{}: {:08x} {}: {:08x}\n",hi_banked_names[i][0],hi_banked[i][0],
            hi_banked_names[i][1],hi_banked[i][1]);
    }

    debug.print_console("\nSTAUS BANKED\n");

    for(int i = 0; i < 5; i++)
    {
        debug.print_console("{}: {:08x} ",status_banked_names[i],status_banked[i]);
        if((i % 2) == 0)
        {
            debug.print_console("\n");
        }       
    }


    debug.print_console("\ncpsr: {:08x}\n",get_cpsr());

    debug.print_console("FLAGS");

    debug.print_console("Z: {}\n",flag_z? "true" : "false");
    debug.print_console("C: {}\n",flag_c? "true" : "false");
    debug.print_console("N: {}\n",flag_n? "true" : "false");
    debug.print_console("V: {}\n",flag_v? "true" : "false");

}


// set zero flag based on arg
void Cpu::set_zero_flag(uint32_t v)
{
   flag_z =  v == 0;
}


void Cpu::set_negative_flag(uint32_t v)
{
    flag_n = is_set(v,31);
}


// both are set together commonly
// so add a shortcut
void Cpu::set_nz_flag(uint32_t v)
{
    set_zero_flag(v);
    set_negative_flag(v);
}




// set zero flag based on arg
void Cpu::set_zero_flag_long(uint64_t v)
{
    flag_z = v == 0;
}


void Cpu::set_negative_flag_long(uint64_t v)
{
    flag_n = is_set(v,63);  
}


// both are set together commonly
// so add a shortcut
void Cpu::set_nz_flag_long(uint64_t v)
{
    set_zero_flag_long(v);
    set_negative_flag_long(v);
}




void Cpu::switch_mode(cpu_mode new_mode)
{
    // save and load regs
    store_registers(arm_mode);
    load_registers(new_mode);
    arm_mode = new_mode; // finally change modes
    
    // set mode bits in cpsr
    cpsr &= ~0b11111; // mask bottom 5 bits
    cpsr |= get_cpsr_mode_bits(arm_mode);
}


void Cpu::load_registers(cpu_mode mode)
{
    int idx = static_cast<int>(mode);

    switch(mode)
    {

        case cpu_mode::system:
        case cpu_mode::user:
        {
            // load user registers back into registers
            memcpy(regs,user_regs,sizeof(uint32_t) * 15);
            break;
        }


        case cpu_mode::fiq:
        {
            // load bottom 8 user regs
            memcpy(regs,user_regs,sizeof(uint32_t)*8);

            // load fiq banked 
            memcpy(&regs[8],fiq_banked,sizeof(uint32_t)*5);
            regs[SP] = hi_banked[idx][0];
            regs[LR] = hi_banked[idx][1];

            break;
        }

        // they all have the same register layout
        // bar the banked ones
        case cpu_mode::supervisor:
        case cpu_mode::abort:
        case cpu_mode::irq:
        case cpu_mode::undefined:
        {
            // load first 13 user regs back to reg
            memcpy(regs,user_regs,sizeof(uint32_t)*13);

            // load hi regs
            regs[SP] = hi_banked[idx][0];
            regs[LR] = hi_banked[idx][1];


            break;          
        }


        default:
        {
            auto err = fmt::format("[load-regs {:08x}]unhandled mode switch: {:x}\n",pc_actual,idx);
            throw std::runtime_error(err);
        }
    }
}


void Cpu::set_cpsr(uint32_t v)
{
    cpsr = v;

    flag_z = is_set(v,Z_BIT);
    flag_c = is_set(v,C_BIT);
    flag_n = is_set(v,N_BIT);
    flag_v = is_set(v,V_BIT);

    // confirm this?
    is_thumb = is_set(cpsr,5);
    switch_execution_state(is_thumb);
    cpu_mode new_mode = cpu_mode_from_bits(cpsr & 0b11111);
    switch_mode(new_mode);    
}

// store current active registers back into the copies
void Cpu::store_registers(cpu_mode mode)
{
    int idx = static_cast<int>(mode);

    switch(mode)
    {

        case cpu_mode::system:
        case cpu_mode::user:
        {
            // store user registers back into registers
            memcpy(user_regs,regs,sizeof(uint32_t) * 15);
            break;
        }


        case cpu_mode::fiq:
        {
            // store bottom 8 user regs
            memcpy(user_regs,regs,sizeof(uint32_t)*8);


            // store fiq banked 
            memcpy(fiq_banked,&regs[8],sizeof(uint32_t)*5);
            hi_banked[idx][0] = regs[SP];
            hi_banked[idx][1] = regs[LR];

            break;
        }

        // they all have the same register layout
        // bar the banked ones
        case cpu_mode::supervisor:
        case cpu_mode::abort:
        case cpu_mode::irq:
        case cpu_mode::undefined:
        {
            // write back first 13 regs to user
            memcpy(user_regs,regs,sizeof(uint32_t)*13);


            // store hi regs
            hi_banked[idx][0] = regs[SP];
            hi_banked[idx][1] = regs[LR];

            break;          
        }


        default:
        {
            auto err = fmt::format("[store-regs {:08x}]unhandled mode switch: {:x}\n",pc_actual,idx);
            throw std::runtime_error(err);
        }
    }
}


cpu_mode Cpu::cpu_mode_from_bits(uint32_t v)
{
    switch(v)
    {
        case 0b10000: return cpu_mode::user;
        case 0b10001: return cpu_mode::fiq;
        case 0b10010: return cpu_mode::irq;
        case 0b10011: return cpu_mode::supervisor;
        case 0b10111: return cpu_mode::abort;
        case 0b11011: return cpu_mode::undefined;
        case 0b11111: return cpu_mode::system;
    }

    // clearly no program should attempt this 
    // but is their a defined behavior for it?
    debug.trace.print();
    auto err = fmt::format("unknown mode from bits: {:08x}:{:08x}\n",v,pc_actual);
    throw std::runtime_error(err);
}



// common arithmetic and logical operations


/*
thanks yaed for suggesting use of compilier builtins
*/


// this checks if the msb (sign) changes to something it shouldunt
// during arithmetic
template <typename T,typename U, typename X>
inline bool did_overflow(T v1, U v2, X ans) noexcept
{
    return  is_set((v1 ^ ans) & (v2 ^ ans),(sizeof(T)*8)-1); 
}

template <typename T>
inline bool sub_overflow(T v1,T v2) noexcept
{
    if constexpr(std::is_signed<T>())
    {
#ifdef _MSC_VER
        const T ans = v1 - v2;
        // negate 2nd operand so we can pretend
        // this is like an additon
	    return did_overflow(v1,~v2, ans);
#else
        return __builtin_sub_overflow_p(v1,v2,v1);
#endif
    }

    else
    {
        // on arm the the carry flag is set if there is no borrow
        // this is different to x86 so we cant use builtins here
        return v1 >= v2;
    }
}


template <typename T>
inline bool add_overflow(T v1,T v2) noexcept
{
#ifdef _MSC_VER
	const T ans = v1 + v2;
    if constexpr(std::is_signed<T>())
    {
	    return did_overflow(v1, v2, ans);
    }

    else
    {
        return ans < v1;
    }
#else
    return __builtin_add_overflow_p(v1,v2,v1);
#endif
}


uint32_t Cpu::add(uint32_t v1, uint32_t v2, bool s)
{
    const uint32_t ans = v1 + v2;
    if(s)
    {

        flag_v = add_overflow((int32_t)v1,(int32_t)v2);
        flag_c = add_overflow(v1,v2); 

        set_nz_flag(ans);
    }

    return ans;
}


uint32_t Cpu::adc(uint32_t v1, uint32_t v2, bool s)
{

    const uint32_t v3 = flag_c;

    const uint32_t ans = v1 + v2 + v3;

    if(s)
    {

        // ^ as if both operations generate an inproper result we will get an expected sign
        const int32_t ans_signed = v1 + v2;
        flag_v = add_overflow((int32_t)v1,(int32_t)v2) ^ add_overflow(ans_signed,(int32_t)v3);

        // if either operation overflows we need to set the carry
        const uint32_t ans_unsigned = v1 + v2;
        flag_c = add_overflow(v1,v2) || add_overflow(ans_unsigned,v3);

        set_nz_flag(ans);
    }

    return ans;
}


uint32_t Cpu::sub(uint32_t v1, uint32_t v2, bool s)
{
    
    const uint32_t ans = v1 - v2;

    if(s)
    {
        flag_v = sub_overflow((int32_t)v1,(int32_t)v2);
        flag_c = sub_overflow(v1,v2);

        set_nz_flag(ans);
    }


    return ans;
}

uint32_t Cpu::sbc(uint32_t v1, uint32_t v2, bool s)
{
    // subtract one from ans if carry is not set
    const uint32_t v3 = !flag_c;

    const uint32_t ans = v1 - v2 - v3;
    if(s)
    {
        // ^ as if both operations generate an inproper result we will get an expected sign
        const int32_t ans_signed = v1 - v2;
        flag_v = sub_overflow((int32_t)v1,(int32_t)v2) ^ sub_overflow(ans_signed,(int32_t)v3);

        // if both operations overflow we need to set the carry
        const uint32_t ans_unsigned = v1 - v2;
        flag_c = sub_overflow(v1,v2) && sub_overflow(ans_unsigned,v3);

        set_nz_flag(ans);
    }

    return ans;
}


uint32_t Cpu::logical_and(uint32_t v1, uint32_t v2, bool s)
{
    const uint32_t ans = v1 & v2;

    if(s)
    {
        set_nz_flag(ans);
    }

    return ans;
}

uint32_t Cpu::logical_or(uint32_t v1, uint32_t v2, bool s)
{
    const uint32_t ans = v1 | v2;
    if(s)
    {
        set_nz_flag(ans);
    }
    return ans;
}

uint32_t Cpu::bic(uint32_t v1, uint32_t v2, bool s)
{
    const uint32_t ans = v1 & ~v2;
    if(s)
    {
        set_nz_flag(ans);
    }
    return ans;
}

uint32_t Cpu::logical_eor(uint32_t v1, uint32_t v2, bool s)
{
    const uint32_t ans = v1 ^ v2;
    if(s)
    {
        set_nz_flag(ans);
    }
    return ans;
}


/*
https://developer.arm.com/documentation/ddi0210/c/Instruction-Cycle-Timings/Instruction-speed-summary?lang=en
m is:

    1 if bits [31:8] of the multiplier operand are all zero or one, else

    2 if bits [31:16] of the multiplier operand are all zero or one, else

    3 if bits [31:24] of the multiplier operand are all zero or all one, else

    4. 
*/
// all cycles from this are internal
void Cpu::do_mul_cycles(uint32_t mul_operand)
{

    auto cycles = 4;

    if((mul_operand & 0xffffff00) == 0 || (mul_operand & 0xffffff00) == 0xffffff00)
    {
        cycles = 1;
    } 

    else if((mul_operand & 0xffff0000) == 0 || (mul_operand & 0xffff0000) == 0xffff0000)
    {
        cycles = 2;
    } 

    else if((mul_operand & 0xff000000) == 0 || (mul_operand & 0xff000000) == 0xff000000)
    {  
        cycles = 3;
    } 


    for(int i = 0; i < cycles; i++)
    {
        internal_cycle();
    }
}

void Cpu::update_intr_status()
{
    interrupt_request = cpu_io.interrupt_flag & cpu_io.interrupt_enable;
    interrupt_service = interrupt_request && cpu_io.ime;
}

// write the interrupt req bit
void Cpu::request_interrupt(interrupt i)
{
    cpu_io.interrupt_flag = set_bit(cpu_io.interrupt_flag,static_cast<int>(i));   
    update_intr_status();
}

// must decrement before and after for total number of pushed regs
// just like stmfd does
void Cpu::write_stack_fd(uint32_t reg)
{
    mem.write_u32(regs[SP],regs[reg]);
    regs[SP] += ARM_WORD_SIZE;
}

void Cpu::read_stack_fd(uint32_t reg)
{
    regs[reg] = mem.read_u32(regs[SP]);
    regs[SP] += ARM_WORD_SIZE;
}


// the handler will find out what fired for us!
// just check irqs aernt masked
void Cpu::service_interrupt()
{

    const auto idx = static_cast<int>(cpu_mode::irq);

    // spsr for irq = cpsr
    status_banked[idx] = get_cpsr();

    // lr is next instr + 4 for an irq 
    hi_banked[idx][1] = pc_actual + 4;

    // irq mode switch
    switch_mode(cpu_mode::irq);

    
    // switch to arm mode
    switch_execution_state(false); // switch to arm mode
    cpsr = set_bit(cpsr,7); //set the irq bit to mask interrupts

    write_log(debug,"[irq {:08x}] interrupt flag: {:02x} ",pc_actual,cpu_io.interrupt_flag);

    //internal_cycle();

    // go to bios irq handler
    write_pc(0x18); // irq handler
/*
    // HLE routine

    // push r0-r3,r12,lr
    regs[SP] -= 6 * ARM_WORD_SIZE;
    write_stack_fd(R0);
    write_stack_fd(R1);
    write_stack_fd(R2);
    write_stack_fd(R3);
    write_stack_fd(R12);
    write_stack_fd(LR);
    regs[SP] -= 6 * ARM_WORD_SIZE;

    bios_hle_interrupt = true;
    

    // r0 used to load the user irq handler
    // fake lr for bios (hooked in write_pc)
    regs[LR] = 0x138;
    regs[R0] = 0x04000000;

    // jump to user irq handler
    write_pc(mem.read_u32(0x03FFFFFC));
*/
}

void Cpu::write_pc(uint32_t v)
{    
/*
    // return from user irq hanlder
    if(bios_hle_interrupt && v == 0x138)
    {
        bios_hle_interrupt = false;
        pc_actual = 0x138;

        // pop our regs
        read_stack_fd(R0);
        read_stack_fd(R1);
        read_stack_fd(R2);
        read_stack_fd(R3);
        read_stack_fd(R12);
        read_stack_fd(LR);
        
        
        // switch back to previous mode and branch back to where we came from
        v = regs[LR] - 4;
        set_cpsr(status_banked[static_cast<int>(arm_mode)]);
    }
*/
    const auto source = pc_actual - (2 << !is_thumb_fetch); 

    if(is_thumb)
    {
        pc_actual = v & ~1;
        in_bios = pc_actual < 0x4000;
        execute_rom = pc_actual >= 0x08000000 && pc_actual <= 0x0e000000;
        mem.switch_bios(in_bios);
        write_pc_thumb(v);
    }

    else
    {
        pc_actual = v & ~3;
        in_bios = pc_actual < 0x4000;
        mem.switch_bios(in_bios);
        execute_rom = pc_actual >= 0x08000000 && pc_actual <= 0x0e000000;
        write_pc_arm(v);
    } 

    debug.trace.add(source,pc_actual);	
}

}
