#include <gb/apu.h>



namespace gameboy_psg
{
void Channel::chan_save_state(std::ofstream &fp)
{
    file_write_var(fp,lengthc);
    file_write_var(fp,length_enabled);
	file_write_var(fp,output);
}

void FreqReg::freq_save_state(std::ofstream &fp)
{
	file_write_var(fp,freq);
	file_write_var(fp,period);
    file_write_var(fp,duty_idx);
}

void Envelope::env_save_state(std::ofstream &fp)
{
	file_write_var(fp,env_period); 
	file_write_var(fp,env_load); 
	file_write_var(fp,volume);
	file_write_var(fp,volume_load);
	file_write_var(fp,env_enabled);
	file_write_var(fp,env_up);
}


void Square::save_state(std::ofstream &fp)
{
    file_write_var(fp,cur_duty);
}

void Wave::save_state(std::ofstream &fp)
{
	file_write_var(fp,volume);
	file_write_var(fp,volume_load);
	file_write_arr(fp,wave_table,sizeof(wave_table));
}


void Noise::save_state(std::ofstream &fp)
{
	file_write_var(fp,clock_shift);
	file_write_var(fp,counter_width);
	file_write_var(fp,divisor_idx); 
	file_write_var(fp,shift_reg); 
	file_write_var(fp,period);
}

void Sweep::sweep_save_state(std::ofstream &fp)
{
	file_write_var(fp,sweep_enabled);
	file_write_var(fp,sweep_shadow);
	file_write_var(fp,sweep_period);
	file_write_var(fp,sweep_timer);
	file_write_var(fp,sweep_calced);
	file_write_var(fp,sweep_reg);
}


void Channel::chan_load_state(std::ifstream &fp)
{
    file_read_var(fp,lengthc);
    file_read_var(fp,length_enabled);
	file_read_var(fp,output);
}

void FreqReg::freq_load_state(std::ifstream &fp)
{
	file_read_var(fp,freq);
	file_read_var(fp,period);
    file_read_var(fp,duty_idx);
    duty_idx &= 7;
}

void Envelope::env_load_state(std::ifstream &fp)
{
	file_read_var(fp,env_period); 
	file_read_var(fp,env_load); 
	file_read_var(fp,volume);
	file_read_var(fp,volume_load);
	file_read_var(fp,env_enabled);
	file_read_var(fp,env_up);
}


void Square::load_state(std::ifstream &fp)
{
    file_read_var(fp,cur_duty);
    cur_duty &= 3;
}

void Wave::load_state(std::ifstream &fp)
{
	file_read_var(fp,volume);
	file_read_var(fp,volume_load);
	file_read_arr(fp,wave_table,sizeof(wave_table));
}


void Noise::load_state(std::ifstream &fp)
{
	file_read_var(fp,clock_shift);
	file_read_var(fp,counter_width);
	file_read_var(fp,divisor_idx); 
	file_read_var(fp,shift_reg); 
	file_read_var(fp,period);
    divisor_idx &= 7;
}

void Sweep::sweep_load_state(std::ifstream &fp)
{
	file_read_var(fp,sweep_enabled);
	file_read_var(fp,sweep_shadow);
	file_read_var(fp,sweep_period);
	file_read_var(fp,sweep_timer);
	file_read_var(fp,sweep_calced);
	file_read_var(fp,sweep_reg);
}

}


namespace gameboy
{

void Apu::load_state(std::ifstream &fp)
{
    psg.c1.load_state(fp); psg.c1.sweep_load_state(fp);
    psg.c2.load_state(fp);
    psg.c3.load_state(fp);
    psg.c4.load_state(fp);


	file_read_var(fp,down_sample_cnt);
	psg.load_state(fp);
}


void Apu::save_state(std::ofstream &fp)
{
    psg.c1.save_state(fp); psg.c1.sweep_save_state(fp);
    psg.c2.save_state(fp);
    psg.c3.save_state(fp);
    psg.c4.save_state(fp);

	file_write_var(fp,down_sample_cnt);
	psg.save_state(fp);
}

}