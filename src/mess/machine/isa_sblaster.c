/***************************************************************************

  ISA 8 bit Creative Labs Sound Blaster Sound Card

  TODO:
  - DSP type is unknown at current time, and probably has an internal ROM
    that needs decapping;
  - joystick port;
  - implement jumpers DIP-SWs;
  - state machine + read/write members

***************************************************************************/

#include "emu.h"
#include "isa_sblaster.h"
#include "sound/speaker.h"
#include "sound/3812intf.h"
#include "sound/saa1099.h"
#include "machine/pic8259.h"

/*
  adlib (YM3812/OPL2 chip), part of many many soundcards (soundblaster)
  soundblaster: YM3812 also accessible at 0x228/9 (address jumperable)
  soundblaster pro version 1: 2 YM3812 chips
   at 0x388 both accessed,
   at 0x220/1 left?, 0x222/3 right? (jumperable)
  soundblaster pro version 2: 1 OPL3 chip

  pro audio spectrum +: 2 OPL2
  pro audio spectrum 16: 1 OPL3

  2 x saa1099 chips
    also on sound blaster 1.0
    option on sound blaster 1.5

  jumperable? normally 0x220
*/
#define ym3812_StdClock XTAL_3_579545MHz

static struct {
	UINT8 reset_latch;
	UINT8 rbuf_status;
	UINT8 wbuf_status;
	UINT8 fifo[16],fifo_ptr;
	UINT8 fifo_r[16],fifo_r_ptr;
	UINT16 version;
	UINT8 test_reg;
	UINT8 speaker_on;
}m_dsp;


static const int m_cmd_fifo_length[256] =
{
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F        */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 0x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 1x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 2x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 3x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 4x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 5x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 6x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 7x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 8x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* 9x */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* Ax */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* Bx */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1, -1, -1, -1, -1, /* Cx */
	-1,  1, -1,  1, -1, -1, -1, -1,  1, -1, -1,	-1, -1, -1, -1, -1, /* Dx */
	 2,  1, -1, -1,  2, -1, -1, -1,  1, -1, -1,	-1, -1, -1, -1, -1, /* Ex */
	-1, -1,  1, -1, -1, -1, -1, -1,  1, -1, -1,	-1, -1, -1, -1, -1  /* Fx */
};

static const ym3812_interface pc_ym3812_interface =
{
	NULL
};

static MACHINE_CONFIG_FRAGMENT( sblaster1_0_config )
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("ym3812", YM3812, ym3812_StdClock)
	MCFG_SOUND_CONFIG(pc_ym3812_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
	MCFG_SOUND_ADD("saa1099.1", SAA1099, 4772720)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
	MCFG_SOUND_ADD("saa1099.2", SAA1099, 4772720)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END

static MACHINE_CONFIG_FRAGMENT( sblaster1_5_config )
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("ym3812", YM3812, ym3812_StdClock)
	MCFG_SOUND_CONFIG(pc_ym3812_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
	/* no CM/S support (empty sockets) */

MACHINE_CONFIG_END


static READ8_DEVICE_HANDLER( ym3812_16_r )
{
	UINT8 retVal = 0xff;
	switch(offset)
	{
		case 0 : retVal = ym3812_status_port_r( device, offset ); break;
	}
	return retVal;
}

static WRITE8_DEVICE_HANDLER( ym3812_16_w )
{
	switch(offset)
	{
		case 0 : ym3812_control_port_w( device, offset, data ); break;
		case 1 : ym3812_write_port_w( device, offset, data ); break;
	}
}

static READ8_DEVICE_HANDLER( saa1099_16_r )
{
	return 0xff;
}

static WRITE8_DEVICE_HANDLER( saa1099_16_w )
{
	switch(offset)
	{
		case 0 : saa1099_control_w( device, offset, data ); break;
		case 1 : saa1099_data_w( device, offset, data ); break;
	}
}

static void queue(UINT8 data)
{
	if (m_dsp.fifo_ptr < 15)
	{
		m_dsp.fifo[m_dsp.fifo_ptr] = data;

		m_dsp.fifo_ptr++;
	}
	else
	{
		// FIFO gets to la-la-land
		//logerror("FIFO?\n");
	}
}

static void queue_r(UINT8 data)
{
	m_dsp.rbuf_status |= 0x80;

	if (m_dsp.fifo_r_ptr < 15)
	{
		m_dsp.fifo_r[m_dsp.fifo_r_ptr] = data;

		m_dsp.fifo_r_ptr++;
	}
	else
	{
		// FIFO gets to la-la-land
		//logerror("FIFO?\n");
	}
}

static UINT8 dequeue_r()
{
	UINT8 data = m_dsp.fifo_r[0];

	if (m_dsp.fifo_r_ptr > 0)
	{
		for (int i = 0; i < 15; i++)
			m_dsp.fifo_r[i] = m_dsp.fifo_r[i + 1];

		m_dsp.fifo_r[15] = 0;

		m_dsp.fifo_r_ptr--;
	}

	if(m_dsp.fifo_r_ptr == 0)
		m_dsp.rbuf_status &= ~0x80;

	return data;
}


static READ8_DEVICE_HANDLER( dsp_reset_r )
{
	if(offset)
		return 0xff;
	logerror("Soundblaster DSP Reset port undocumented read\n");
	return 0xff;
}

static WRITE8_DEVICE_HANDLER( dsp_reset_w )
{
	if(offset)
		return;

	if(data == 0 && m_dsp.reset_latch == 1)
	{
		// reset routine
		queue_r(0xaa); // reset OK ID
	}

	m_dsp.reset_latch = data;

	//printf("%02x\n",data);
}

static READ8_DEVICE_HANDLER( dsp_data_r )
{
	if(offset)
		return 0xff;

	return dequeue_r();
}

static WRITE8_DEVICE_HANDLER( dsp_data_w )
{
	if(offset)
		return;
	logerror("Soundblaster DSP data port undocumented write\n");
}

static READ8_DEVICE_HANDLER(dsp_rbuf_status_r)
{
	if(offset)
		return 0xff;

	return m_dsp.rbuf_status;
}

static READ8_DEVICE_HANDLER(dsp_wbuf_status_r)
{
	if(offset)
		return 0xff;

	return m_dsp.wbuf_status;
}

static WRITE8_DEVICE_HANDLER(dsp_rbuf_status_w)
{
	if(offset)
		return;

	logerror("Soundblaster DSP Read Buffer status undocumented write\n");
}

static void process_fifo(running_machine &machine, UINT8 cmd)
{
	if (m_cmd_fifo_length[cmd] == -1)
	{
		printf("unemulated or undefined fifo command %02x\n",cmd);
	}
	else if(m_dsp.fifo_ptr == m_cmd_fifo_length[cmd])
	{
		/* get FIFO params */
		switch(cmd)
		{
			case 0xd1: // speaker on
				// ...
				m_dsp.speaker_on = 1;
				break;
			case 0xd3: // speaker off
				// ...
				m_dsp.speaker_on = 0;
				break;
			case 0xd8: // speaker status
				queue_r(m_dsp.speaker_on ? 0xff : 0x00);
				break;
			case 0xe0: // get DSP identification
				queue_r(m_dsp.fifo[1] ^ 0xff);
				break;
			case 0xe1: // get DSP version
				queue_r(m_dsp.version >> 8);
				queue_r(m_dsp.version & 0xff);
				break;
			case 0xe4: // write test register
				m_dsp.test_reg = m_dsp.fifo[1];
				break;
			case 0xe8: // read test register
				queue_r(m_dsp.test_reg);
				break;
			case 0xf2: // send PIC irq
				//m_isa->irq5_w(1);
				pic8259_ir5_w(machine.device("pic8259_master"), 1);
				break;
			case 0xf8: // ???
				logerror("SB: Unknown command write 0xf8");
				queue_r(0);
				break;
		}

		m_dsp.fifo_ptr = 0;
	}
}

static WRITE8_DEVICE_HANDLER(dsp_cmd_w)
{
	if(offset)
		return;

	queue(data);

	process_fifo(device->machine(),m_dsp.fifo[0]);
}

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type ISA8_SOUND_BLASTER_1_0 = &device_creator<isa8_sblaster1_0_device>;
const device_type ISA8_SOUND_BLASTER_1_5 = &device_creator<isa8_sblaster1_5_device>;

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor isa8_sblaster1_0_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( sblaster1_0_config );
}

machine_config_constructor isa8_sblaster1_5_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( sblaster1_5_config );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  isa8_sblaster_device - constructor
//-------------------------------------------------

isa8_sblaster1_0_device::isa8_sblaster1_0_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
        device_t(mconfig, ISA8_SOUND_BLASTER_1_0, "ISA8_SOUND_BLASTER_1_0", tag, owner, clock),
		device_isa8_card_interface(mconfig, *this)
{
}

isa8_sblaster1_5_device::isa8_sblaster1_5_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
        device_t(mconfig, ISA8_SOUND_BLASTER_1_5, "ISA8_SOUND_BLASTER_1_5", tag, owner, clock),
		device_isa8_card_interface(mconfig, *this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void isa8_sblaster1_0_device::device_start()
{
    set_isa_device();
	m_isa->install_device(subdevice("saa1099.1"), 0x0220, 0x0221, 0, 0, FUNC(saa1099_16_r), FUNC(saa1099_16_w) );
	m_isa->install_device(subdevice("saa1099.2"), 0x0222, 0x0223, 0, 0, FUNC(saa1099_16_r), FUNC(saa1099_16_w) );
	m_isa->install_device(this,                   0x0226, 0x0227, 0, 0, FUNC(dsp_reset_r), FUNC(dsp_reset_w) );
	m_isa->install_device(subdevice("ym3812"),    0x0228, 0x0229, 0, 0, FUNC(ym3812_16_r), FUNC(ym3812_16_w) );
	m_isa->install_device(this,                   0x022a, 0x022b, 0, 0, FUNC(dsp_data_r),  FUNC(dsp_data_w) );
	m_isa->install_device(this,                   0x022c, 0x022d, 0, 0, FUNC(dsp_wbuf_status_r),  FUNC(dsp_cmd_w) );
	m_isa->install_device(this,                   0x022e, 0x022f, 0, 0, FUNC(dsp_rbuf_status_r),  FUNC(dsp_rbuf_status_w) );
	m_isa->install_device(subdevice("ym3812"),    0x0388, 0x0389, 0, 0, FUNC(ym3812_16_r), FUNC(ym3812_16_w) );
	m_dsp.version = 0x0105;
}

void isa8_sblaster1_5_device::device_start()
{
    set_isa_device();
	/* 1.5 ditches CM/S support (empty sockets) */
	m_isa->install_device(this,                   0x0226, 0x0227, 0, 0, FUNC(dsp_reset_r), FUNC(dsp_reset_w) );
	m_isa->install_device(subdevice("ym3812"),    0x0228, 0x0229, 0, 0, FUNC(ym3812_16_r), FUNC(ym3812_16_w) );
	m_isa->install_device(this,                   0x022a, 0x022b, 0, 0, FUNC(dsp_data_r),  FUNC(dsp_data_w) );
	m_isa->install_device(this,                   0x022c, 0x022d, 0, 0, FUNC(dsp_wbuf_status_r),  FUNC(dsp_cmd_w) );
	m_isa->install_device(this,                   0x022e, 0x022f, 0, 0, FUNC(dsp_rbuf_status_r),  FUNC(dsp_rbuf_status_w) );
	m_isa->install_device(subdevice("ym3812"),    0x0388, 0x0389, 0, 0, FUNC(ym3812_16_r), FUNC(ym3812_16_w) );
	m_dsp.version = 0x0200;
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void isa8_sblaster1_0_device::device_reset()
{
//	...
}

void isa8_sblaster1_5_device::device_reset()
{
// ...
}
