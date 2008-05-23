/***************************************************************************

  Enhanced Graphics Adapter (EGA) section

TODO - Write documentation

"Regular" register on an EGA graphics card:

	3C2 - 7 6 5 4 3 2 1 0 - Misc Output Register - Write Only
	      | | | | | | | |
	      | | | | | | | +-- 3Bx/3Dx I/O port select
	      | | | | | | |     0 = 3Bx for CRTC I/O, 3BA for status reg 1
	      | | | | | | |     1 = 3Dx for CRTC I/O, 3DA for status reg 1
	      | | | | | | +---- enable ram
	      | | | | | |       0 = disable ram from the processor
	      | | | | | |       1 = enable ram to respond to addresses
	      | | | | | |           designated by the Control Data Select
	      | | | | | |           value in the Graphics Controllers.
	      | | | | | +------ clock select bit 0
	      | | | | +-------- clock select bit 1
	      | | | |           00 = 14Mhz from Processor I/O channel
	      | | | |           01 = 16MHz on-bord clock
	      | | | |           10 = External clock from feature connector
	      | | | |           11 = reserved/unused
	      | | | +---------- disable video drivers
	      | | |             0 = activate internal video drivers
	      | | |             1 = disable internal video drivers
	      | | +------------ page bit for odd/even. Selects between 2 pages
	      | |               of 64KB of memory when in odd/even mode.
	      | |               0 = select low page
	      | |               1 = select high page
	      | +-------------- horizontal retrace polarity
	      |                 0 = select positive
	      |                 1 = select negative
	      +---------------- vertical retrace polarity
	                        0 = select positive
	                        1 = select negative


	3C2 - 7 6 5 4 3 2 1 0 - Input Status Reg 0 - Read Only
	      | | | | | | | |
	      | | | | | | | +-- reserved/unused
	      | | | | | | +---- reserved/unused
	      | | | | | +------ reserved/unused
	      | | | | +-------- reserved/unused
	      | | | +---------- switch sense
	      | | |             0 = switch is closed
	      | | |             1 = allows processor to read the 4 config switches
	      | | |                 on the EGA adapter. The setting of CLKSEL determines
	      | | |                 switch to read.
	      | | +------------ input from FEAT0 on the feature connector
	      | +-------------- input from FEAT1 on the feature connector
	      +---------------- CRT Interrupt
	                        0 = vertical retrace if occuring
	                        1 = video is being displayed


	3XA - 7 6 5 4 3 2 1 0 - Feature Control Register - Write Only
	      | | | | | | | |
	      | | | | | | | +-- output to FEAT0 of the feature connector
	      | | | | | | +---- output to FEAT1 of the feature connector
	      | | | | | +------ reserved/unused
	      | | | | +-------- reserved/unused
	      | | | +---------- reserved/unused
	      | | +------------ reserved/unused
	      | +-------------- reserved/unused
	      +---------------- reserved/unused

	3XA - 7 6 5 4 3 2 1 0 - Input Status Reg 1 - Read Only
	      | | | | | | | |
	      | | | | | | | +-- display enable
	      | | | | | | |     0 = indicates the CRT raster is in a horizontal or vertical retrace
	      | | | | | | |     1 = otherwise
	      | | | | | | +---- light pen strobe
	      | | | | | |       0 = light pen trigger has not been set
	      | | | | | |       1 = light pen trigger has been set
	      | | | | | +------ light pen switch
	      | | | | |         0 = switch is closed
	      | | | | |         1 = switch is open
	      | | | | +-------- vertical retrace
	      | | | |           0 = video information is being displayed
	      | | | |           1 = CRT is in vertical retrace
	      | | | +---------- diagnostic usage
	      | | +------------ diagnostic usage
	      | +-------------- reserved/unused
	      +---------------- reserved/unused


The EGA graphics card introduces a lot of new indexed registers to handle the
enhanced graphics. These new indexed registers can be divided into three
groups:
- attribute registers
- sequencer registers
- graphics controllers registers


Attribute Registers AR00 - AR13

The Attribute Registers are all accessed through I/O port 0x3C0. The first
write to I/O port 0x3C0 sets the index register. The next write to I/O port
0x3C0 actually sets the data to the indexed register.

	3C0 - 7 6 5 4 3 2 1 0 - Attribute Access Register
	      | | | | | | | |
	      | | | | | | | +-- index bit 0
	      | | | | | | +---- index bit 1
	      | | | | | +------ index bit 2
	      | | | | +-------- index bit 3
	      | | | +---------- index bit 4
	      | | +------------ palette source
	      | +-------------- reserved/unused
	      +---------------- reserved/unused


	AR00-AR0F - 7 6 5 4 3 2 1 0 - Palette Register #00 - #0F
	            | | | | | | | |
	            | | | | | | | +-- MSB B
	            | | | | | | +---- MSB G
	            | | | | | +------ MSB R
	            | | | | +-------- LSB B
	            | | | +---------- LSB G
	            | | +------------ LSB R
	            | +-------------- reserved/unused
	            +---------------- reserved/unused


	AR10 - 7 6 5 4 3 2 1 0 - Mode Control Register
	       | | | | | | | |
	       | | | | | | | +-- Text/Graphics select
	       | | | | | | +---- Monochrome/Color select
	       | | | | | +------ 9th dot setting
	       | | | | +-------- Blink Enable
	       | | | +---------- reserved/unsued
	       | | +------------ 0 = line compare does not affect pixel output
	       | |               1 = line compare does affect pixel output
	       | +-------------- 0 = pixel changes every dot clock
	       |                 1 = pixel changes every other dot clock
	       +---------------- reserved/unused


	AR11 - 7 6 5 4 3 2 1 0 - Overscan Color Register
	       | | | | | | | |
	       | | | | | | | +-- MSB B
	       | | | | | | +---- MSB G
	       | | | | | +------ MSB R
	       | | | | +-------- LSB B
	       | | | +---------- LSB G
	       | | +------------ LSB R
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	AR12 - 7 6 5 4 3 2 1 0 - Color Plane Enable Register
	       | | | | | | | |
	       | | | | | | | +-- Enable plane 0
	       | | | | | | +---- Enable plane 1
	       | | | | | +------ Enable plane 2
	       | | | | +-------- Enable plane 3
	       | | | +---------- Video Status Mux bit 0
	       | | +------------ Video Status Mux bit 1
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	AR13 - 7 6 5 4 3 2 1 0 - Horizontal Panning Register
	       | | | | | | | |
	       | | | | | | | +-- Pixel left shift bit 0
	       | | | | | | +---- Pixel left shift bit 1
	       | | | | | +------ Pixel left shift bit 2
	       | | | | +-------- Pixel left shift bit 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


Sequencer Registers SR00 - SR04

The Sequencer Registers are accessed through an index register located at I/O
port 0x3C4, and a data register located at I/O port 0x3C5.

	3C4 - 7 6 5 4 3 2 1 0 - Sequencer Index Register - Write Only
	      | | | | | | | |
          | | | | | | | +-- index bit 0
	      | | | | | | +---- index bit 1
	      | | | | | +------ index bit 2
	      | | | | +-------- reserved/unused
	      | | | +---------- reserved/unused
	      | | +------------ reserved/unused
	      | +-------------- reserved/unused
	      +---------------- reserved/unused


	3C5 - 7 6 5 4 3 2 1 0 - Sequencer Data Register - Write Only
	      | | | | | | | |
          | | | | | | | +-- data bit 0
	      | | | | | | +---- data bit 1
	      | | | | | +------ data bit 2
	      | | | | +-------- data bit 3
	      | | | +---------- data bit 4
	      | | +------------ data bit 5
	      | +-------------- data bit 6
	      +---------------- data bit 7


	SR00 - 7 6 5 4 3 2 1 0 - Reset Control Register
	       | | | | | | | |
	       | | | | | | | +-- Must be 1 for normal operation
	       | | | | | | +---- Must be 1 for normal operation
	       | | | | | +------ reserved/unused
	       | | | | +-------- reserved/unused
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	SR01 - 7 6 5 4 3 2 1 0 - Clocking Mode
	       | | | | | | | |
	       | | | | | | | +-- 0 = 9 dots per char, 1 = 8 dots per char
	       | | | | | | +---- clock frequency, 0 = 4 out of 5 memory cycles, 1 = 2 out of 5 memory cycles
	       | | | | | +------ shift load
	       | | | | +-------- 0 = normal dot clock, 1 = master dot clock / 2
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	SR02 - 7 6 5 4 3 2 1 0 - Map Mask
	       | | | | | | | |
	       | | | | | | | +-- 1 = enable map 0
	       | | | | | | +---- 1 = enable map 1
	       | | | | | +------ 1 = enable map 2
	       | | | | +-------- 1 = enable map 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	SR03 - 7 6 5 4 3 2 1 0 - Character Map Select
	       | | | | | | | |
	       | | | | | | | +-- select plane for character map B
	       | | | | | | +---- select plane for character map B
	       | | | | | +------ select plane for character map A
	       | | | | +-------- select plane for character map A
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused
	     Meaning of the plane selection bits:
	     00 - 1st 8K plane 2 bank 0
	     01 - 1st 8K plane 2 bank 1
	     10 - 1st 8K plane 2 bank 2
	     11 - 1st 8K plane 2 bank 3


	SR04 - 7 6 5 4 3 2 1 0 - Memory Mode Register
	       | | | | | | | |
	       | | | | | | | +-- 0 = graphics mode, 1 = text mode
	       | | | | | | +---- 0 = no memory extension, 1 = memory extension
	       | | | | | +------ 0 = odd/even storage, 1 = sequential storage
	       | | | | +-------- reserved/unused
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


Graphics Controller Registers GR00 - GR08

The Graphics Controller Registers are accessed through an index register
located at I/O port 0x3CE, and a data register located at I/O port 0x3CF.

	GR00 - 7 6 5 4 3 2 1 0 - Set/Reset Register
	       | | | | | | | |
	       | | | | | | | +-- set/reset for plane 0
	       | | | | | | +---- set/reset for plane 1
	       | | | | | +------ set/reset for plane 2
	       | | | | +-------- set/reset for plane 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR01 - 7 6 5 4 3 2 1 0 - Enable Set/Reset Register
	       | | | | | | | |
	       | | | | | | | +-- enable set/reset for plane 0
	       | | | | | | +---- enable set/reset for plane 1
	       | | | | | +------ enable set/reset for plane 2
	       | | | | +-------- enable set/reset for plane 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR02 - 7 6 5 4 3 2 1 0 - Color Compare Register
	       | | | | | | | |
	       | | | | | | | +-- color compare 0
	       | | | | | | +---- color compare 1
	       | | | | | +------ color compare 2
	       | | | | +-------- color compare 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR03 - 7 6 5 4 3 2 1 0 - Data Rotate Register
	       | | | | | | | |
	       | | | | | | | +-- number of positions to rotate bit 0
	       | | | | | | +---- number of positions to rotate bit 1
	       | | | | | +------ number of positions to rotate bit 2
	       | | | | +-------- function select bit 0
	       | | | +---------- function select bit 1
	       | | |             00 = data overwrites in specified color
	       | | |             01 = data ANDed with latched data
	       | | |             10 = data ORed with latched data
	       | | |             11 = data XORed with latched data
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR04 - 7 6 5 4 3 2 1 0 - Read Map Select Register
	       | | | | | | | |
	       | | | | | | | +-- plane select bit 0
	       | | | | | | +---- plane select bit 1
	       | | | | | +------ plane select bit 2
	       | | | | +-------- reserved/unused
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR05 - 7 6 5 4 3 2 1 0 - Mode Register
	       | | | | | | | |
	       | | | | | | | +-- write mode bit 0
	       | | | | | | +---- write mode bit 1
	       | | | | | |       00 = write 8 bits of value in set/reset register if enabled,
	       | | | | | |            otherwise write rotated processor data
	       | | | | | |       01 = write with contents of processor latches
	       | | | | | |       10 = memory plane 0-3 filled with 8 bits of value of data bit 0-3
	       | | | | | |       11 = reserved/unused
	       | | | | | +------ test condition
	       | | | | |         0 = normal operation
	       | | | | |         1 = put outputs in high impedance state
	       | | | | +-------- read mode
	       | | | |           0 = read from plane selected by GR04
	       | | | |           1 = do color compare
	       | | | +---------- odd/even addressing mode
	       | | +------------ shift register mode
	       | |               0 = sequential
	       | |               1 = even bits from even maps, odd bits from odd maps
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR06 - 7 6 5 4 3 2 1 0 - Miscellaneous Register
	       | | | | | | | |
	       | | | | | | | +-- 0 = text mode, 1 = graphics mode
	       | | | | | | +---- chain odd maps to even
	       | | | | | +------ memory map bit 0
	       | | | | +-------- memory map bit 1
	       | | | |           00 = 0xA0000, 128KB
	       | | | |           01 = 0xA0000, 64KB
	       | | | |           10 = 0xB0000, 32KB
	       | | | |           11 = 0xB8000, 32KB
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR07 - 7 6 5 4 3 2 1 0 - Color Plane Ignore Register
	       | | | | | | | |
	       | | | | | | | +-- ignore color plane 0
	       | | | | | | +---- ignore color plane 1
	       | | | | | +------ ignore color plane 2
	       | | | | +-------- ignore color plane 3
	       | | | +---------- reserved/unused
	       | | +------------ reserved/unused
	       | +-------------- reserved/unused
	       +---------------- reserved/unused


	GR08 - 7 6 5 4 3 2 1 0 - Bit Mask Register
	       | | | | | | | |
	       | | | | | | | +-- write enable bit 0
	       | | | | | | +---- write enable bit 1
	       | | | | | +------ write enable bit 2
	       | | | | +-------- write enable bit 3
	       | | | +---------- write enable bit 4
	       | | +------------ write enable bit 5
	       | +-------------- write enable bit 6
	       +---------------- write enable bit 7


***************************************************************************/

#include "driver.h"
#include "video/pc_ega.h"
#include "video/mc6845.h"
#include "video/pc_video.h"
#include "memconv.h"


#define VERBOSE_EGA		1


static struct
{
	const device_config	*mc6845;
	mc6845_update_row_func	update_row;

	/* Registers */
	UINT8	misc_output;
	UINT8	feature_control;

	/* Attribute registers AR00 - AR14 
	*/
	struct {
		UINT8	index;
		UINT8	data[32];
		UINT8	index_write;
	} attribute;

	/* Sequencer registers SR00 - SR04
	*/
	struct {
		UINT8	index;
		UINT8	data[8];
	} sequencer;

	/* Graphics controller registers GR00 - GR08
	*/
	struct {
		UINT8	index;
		UINT8	data[16];
	} graphics_controller;

	UINT8	frame_cnt;
	UINT8	vsync;
	UINT8	hsync;
} ega;


static VIDEO_START( pc_ega );
static VIDEO_UPDATE( mc6845_ega );
static PALETTE_INIT( pc_ega );
static MC6845_UPDATE_ROW( ega_update_row );
static MC6845_ON_HSYNC_CHANGED( ega_hsync_changed );
static MC6845_ON_VSYNC_CHANGED( ega_vsync_changed );
static READ8_HANDLER( pc_ega8_3b0_r );
static WRITE8_HANDLER( pc_ega8_3b0_w );
static READ16_HANDLER( pc_ega16le_3b0_r );
static WRITE16_HANDLER( pc_ega16le_3b0_w );
static READ32_HANDLER( pc_ega32le_3b0_r );
static WRITE32_HANDLER( pc_ega32le_3b0_w );
static READ8_HANDLER( pc_ega8_3c0_r );
static WRITE8_HANDLER( pc_ega8_3c0_w );
static READ16_HANDLER( pc_ega16le_3c0_r );
static WRITE16_HANDLER( pc_ega16le_3c0_w );
static READ32_HANDLER( pc_ega32le_3c0_r );
static WRITE32_HANDLER( pc_ega32le_3c0_w );
static READ8_HANDLER( pc_ega8_3d0_r );
static WRITE8_HANDLER( pc_ega8_3d0_w );
static READ16_HANDLER( pc_ega16le_3d0_r );
static WRITE16_HANDLER( pc_ega16le_3d0_w );
static READ32_HANDLER( pc_ega32le_3d0_r );
static WRITE32_HANDLER( pc_ega32le_3d0_w );


static const mc6845_interface mc6845_ega_intf =
{
	EGA_SCREEN_NAME,	/* screen number */
	16257000/8,			/* clock */
	8,					/* numbers of pixels per video memory address */
	NULL,				/* begin_update */
	ega_update_row,		/* update_row */
	NULL,				/* end_update */
	NULL,				/* on_de_chaged */
	ega_hsync_changed,	/* on_hsync_changed */
	ega_vsync_changed	/* on_vsync_changed */
};


MACHINE_DRIVER_START( pcvideo_ega )
	MDRV_SCREEN_ADD(EGA_SCREEN_NAME, RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(XTAL_14_31818MHz,912,0,640,262,0,200)
	MDRV_PALETTE_LENGTH( 64 )

	MDRV_PALETTE_INIT(pc_ega)

	MDRV_DEVICE_ADD(EGA_MC6845_NAME, MC6845)
	MDRV_DEVICE_CONFIG( mc6845_ega_intf )

	MDRV_VIDEO_START( pc_ega )
	MDRV_VIDEO_UPDATE( mc6845_ega )
MACHINE_DRIVER_END


static PALETTE_INIT( pc_ega )
{
	int i;

	for ( i = 0; i < 64; i++ )
	{
		UINT8 r = ( ( i & 0x04 ) ? 0xAA : 0x00 ) + ( ( i & 0x20 ) ? 0x55 : 0x00 );
		UINT8 g = ( ( i & 0x02 ) ? 0xAA : 0x00 ) + ( ( i & 0x10 ) ? 0x55 : 0x00 );
		UINT8 b = ( ( i & 0x01 ) ? 0xAA : 0x00 ) + ( ( i & 0x08 ) ? 0x55 : 0x00 );

		palette_set_color_rgb( machine, i, r, g, b );
	}
}


static VIDEO_START( pc_ega )
{
	int buswidth;

	buswidth = cputype_databus_width(machine->config->cpu[0].type, ADDRESS_SPACE_PROGRAM);
	switch(buswidth)
	{
		case 8:
			memory_install_read8_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, SMH_BANK11 );
			memory_install_write8_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, pc_video_videoram_w );
			memory_install_read8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega8_3b0_r );
			memory_install_write8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega8_3b0_w );
			memory_install_read8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega8_3c0_r );
			memory_install_write8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega8_3c0_w );
			memory_install_read8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega8_3d0_r );
			memory_install_write8_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega8_3d0_w );
			break;

		case 16:
			memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, SMH_BANK11 );
			memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, pc_video_videoram16le_w );
			memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega16le_3b0_r );
			memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega16le_3b0_w );
			memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega16le_3c0_r );
			memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega16le_3c0_w );
			memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega16le_3d0_r );
			memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega16le_3d0_w );
			break;

		case 32:
			memory_install_read32_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, SMH_BANK11 );
			memory_install_write32_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, pc_video_videoram32_w );
			memory_install_read32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega32le_3b0_r );
			memory_install_write32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bb, 0, 0, pc_ega32le_3b0_w );
			memory_install_read32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega32le_3c0_r );
			memory_install_write32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, pc_ega32le_3c0_w );
			memory_install_read32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega32le_3d0_r );
			memory_install_write32_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3db, 0, 0, pc_ega32le_3d0_w );
			break;

		default:
			fatalerror("EGA:  Bus width %d not supported\n", buswidth);
			break;
	}

	/* 256KB Video ram max on an EGA card */
	videoram_size = 0x40000;

	videoram = auto_malloc(videoram_size);

	memory_set_bankptr(11, videoram);

	ega.mc6845 = device_list_find_by_tag(machine->config->devicelist, MC6845, EGA_MC6845_NAME);
	ega.update_row = NULL;
	ega.misc_output = 0;
	ega.attribute.index_write = 1;

	/* Set up default palette */
	ega.attribute.data[0] = 0;
	ega.attribute.data[1] = 1;
	ega.attribute.data[2] = 2;
	ega.attribute.data[3] = 3;
	ega.attribute.data[4] = 4;
	ega.attribute.data[5] = 5;
	ega.attribute.data[6] = 0x14;
	ega.attribute.data[7] = 7;
	ega.attribute.data[8] = 0x38;
	ega.attribute.data[9] = 0x39;
	ega.attribute.data[10] = 0x3A;
	ega.attribute.data[11] = 0x3B;
	ega.attribute.data[12] = 0x3C;
	ega.attribute.data[13] = 0x3D;
	ega.attribute.data[14] = 0x3E;
	ega.attribute.data[15] = 0x3F;
}


static VIDEO_UPDATE( mc6845_ega )
{
	mc6845_update( ega.mc6845, bitmap, cliprect);
	return 0;
}


static MC6845_UPDATE_ROW( ega_update_row )
{
	if ( ega.update_row )
	{
		ega.update_row( device, bitmap, cliprect, ma, ra, y, x_count, cursor_x, param );
	}
}


static MC6845_ON_HSYNC_CHANGED( ega_hsync_changed )
{
	ega.hsync = hsync ? 1 : 0;
}


static MC6845_ON_VSYNC_CHANGED( ega_vsync_changed )
{
	ega.vsync = vsync ? 8 : 0;
	if ( vsync )
	{
		ega.frame_cnt++;
	}
}


static READ8_HANDLER( pc_ega8_3X0_r )
{
	int data = 0xff;

	if ( VERBOSE_EGA )
	{
		logerror("pc_ega_3X0_r: offset = %02x\n", offset );
	}

	switch ( offset )
	{
	/* CRT Controller - address register */
	case 0: case 2: case 4: case 6:
		/* return last written mc6845 address value here? */
		break;

	/* CRT Controller - data register */
	case 1: case 3: case 5: case 7:
		data = mc6845_register_r( ega.mc6845, offset );
		break;

	/* Input Status Register 1 */
	case 10:
		data = ega.vsync | ega.hsync;

		/* Reset the attirubte writing flip flop to let the next write go to the index reigster */
		ega.attribute.index_write = 1;
		break;
	}

	return data;
}


static WRITE8_HANDLER( pc_ega8_3X0_w )
{
	if ( VERBOSE_EGA )
	{
		logerror("pc_ega_3X0_w: offset = %02x, data = %02x\n", offset, data );
	}

	switch ( offset )
	{
	/* CRT Controller - address register */
	case 0: case 2: case 4: case 6:
		mc6845_address_w( ega.mc6845, offset, data );
		break;

	/* CRT Controller - data register */
	case 1: case 3: case 5: case 7:
		mc6845_register_w( ega.mc6845, offset, data );
		break;

	/* Set Light Pen Flip Flop */
	case 9:
		break;

	/* Feature Control */
	case 10:
		ega.feature_control = data;
		break;

	/* Clear Light Pen Flip Flop */
	case 11:
		break;
	}
}


static READ8_HANDLER( pc_ega8_3b0_r )
{
	return ( ega.misc_output & 0x01 ) ? 0xFF : pc_ega8_3X0_r( machine, offset );
}


static READ8_HANDLER( pc_ega8_3d0_r )
{
	return ( ega.misc_output & 0x01 ) ? pc_ega8_3X0_r( machine, offset ) : 0xFF;
}


static WRITE8_HANDLER( pc_ega8_3b0_w )
{
	if ( ! ( ega.misc_output & 0x01 ) )
	{
		pc_ega8_3X0_w( machine, offset, data );
	}
}


static WRITE8_HANDLER( pc_ega8_3d0_w )
{
	if ( ega.misc_output & 0x01 )
	{
		pc_ega8_3X0_w( machine, offset, data );
	}
}


static READ8_HANDLER( pc_ega8_3c0_r )
{
	int data = 0xff;

	if ( VERBOSE_EGA )
	{
		logerror("pc_ega_3c0_r: offset = %02x\n", offset );
	}

	switch ( offset )
	{
	/* Attributes Controller */
	case 0:
		break;

	/* Feature Read */
	case 2:
		data = ( data & 0x0F );
		data |= ( ( ega.feature_control & 0x03 ) << 5 );
		data |= ( ega.vsync ? 0x00 : 0x80 );
		break;

	/* Sequencer */
	case 4:
		break;
	case 5:
		break;

	/* Graphics Controller */
	case 14:
		break;
	case 15:
		break;
	}
	return data;
}


static WRITE8_HANDLER( pc_ega8_3c0_w )
{
	if ( VERBOSE_EGA )
	{
		logerror("pc_ega_3c0_w: offset = %02x, data = %02x\n", offset, data );
	}

	switch ( offset )
	{
	/* Attributes Controller */
	case 0:
		if ( ega.attribute.index_write )
		{
			ega.attribute.index = data;
		}
		else
		{
			ega.attribute.data[ ega.attribute.index & 0x1F ] = data;
		}
		ega.attribute.index_write ^= 0x01;
		break;

	/* Misccellaneous Output */
	case 2:
		ega.misc_output = data;
		break;

	/* Sequencer */
	case 4:
		ega.sequencer.index = data;
		break;
	case 5:
//logerror("SR%02x = 0x%02x\n", ega.graphics_controller.index & 0x07, data );
		ega.sequencer.data[ ega.sequencer.index & 0x07 ] = data;
		break;

	/* Graphics Controller */
	case 14:
		ega.graphics_controller.index = data;
		break;
	case 15:
//logoerror("GR%02x = 0x%02x\n", ega.graphics_controller.index & 0x0F, data );
		ega.graphics_controller.data[ ega.graphics_controller.index & 0x0F ] = data;
		break;
	}
}

static READ16_HANDLER( pc_ega16le_3b0_r ) { return read16le_with_read8_handler(pc_ega8_3b0_r,machine,  offset, mem_mask); }
static WRITE16_HANDLER( pc_ega16le_3b0_w ) { write16le_with_write8_handler(pc_ega8_3b0_w, machine, offset, data, mem_mask); }
static READ32_HANDLER( pc_ega32le_3b0_r ) { return read32le_with_read8_handler(pc_ega8_3b0_r, machine, offset, mem_mask); }
static WRITE32_HANDLER( pc_ega32le_3b0_w ) { write32le_with_write8_handler(pc_ega8_3b0_w, machine, offset, data, mem_mask); }

static READ16_HANDLER( pc_ega16le_3c0_r ) { return read16le_with_read8_handler(pc_ega8_3c0_r,machine,  offset, mem_mask); }
static WRITE16_HANDLER( pc_ega16le_3c0_w ) { write16le_with_write8_handler(pc_ega8_3c0_w, machine, offset, data, mem_mask); }
static READ32_HANDLER( pc_ega32le_3c0_r ) { return read32le_with_read8_handler(pc_ega8_3c0_r, machine, offset, mem_mask); }
static WRITE32_HANDLER( pc_ega32le_3c0_w ) { write32le_with_write8_handler(pc_ega8_3c0_w, machine, offset, data, mem_mask); }

static READ16_HANDLER( pc_ega16le_3d0_r ) { return read16le_with_read8_handler(pc_ega8_3d0_r,machine,  offset, mem_mask); }
static WRITE16_HANDLER( pc_ega16le_3d0_w ) { write16le_with_write8_handler(pc_ega8_3d0_w, machine, offset, data, mem_mask); }
static READ32_HANDLER( pc_ega32le_3d0_r ) { return read32le_with_read8_handler(pc_ega8_3d0_r, machine, offset, mem_mask); }
static WRITE32_HANDLER( pc_ega32le_3d0_w ) { write32le_with_write8_handler(pc_ega8_3d0_w, machine, offset, data, mem_mask); }

