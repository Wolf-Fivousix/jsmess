/***************************************************************************

        MZ80 driver by Miodrag Milanovic

        22/11/2008 Preliminary driver.

****************************************************************************/

#include "driver.h"
#include "machine/8255ppi.h"
#include "machine/pit8253.h"
#include "includes/mz80.h"

gfx_layout mz80k_charlayout =
{
	8, 8,				/* 8x8 characters */
	256,				/* 256 characters */
	1,				  /* 1 bits per pixel */
	{0},				/* no bitplanes; 1 bit per pixel */
	{0, 1, 2, 3, 4, 5, 6, 7},
	{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
	8*8					/* size of one char */
};

gfx_layout mz80kj_charlayout =
{
	8, 8,				/* 8x8 characters */
	256,				/* 256 characters */
	1,				  /* 1 bits per pixel */
	{0},				/* no bitplanes; 1 bit per pixel */
	{7, 6, 5, 4, 3, 2, 1, 0},
	{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
	8*8					/* size of one char */
};

/* Video hardware */
VIDEO_START( mz80k )
{
}

VIDEO_UPDATE( mz80k )
{
	int x,y;
	const address_space *space = cpu_get_address_space(screen->machine->cpu[0], ADDRESS_SPACE_PROGRAM);
	mz80k_vertical = mz80k_vertical ? 0 : 1;	
	mz80k_cursor_cnt++;
	if (mz80k_cursor_cnt==64) mz80k_cursor_cnt = 0;
		
	for(y = 0; y < 25; y++ )
	{
		for(x = 0; x < 40; x++ )
		{
			int code = memory_read_byte(space,0xD000 + x + y*40);		
			drawgfx(bitmap, screen->machine->gfx[0],  code , 0, 0,0, x*8,y*8,
				NULL, TRANSPARENCY_NONE, 0);
		}
	}
	return 0;
}
