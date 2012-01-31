#pragma once

#ifndef __NEXT__
#define __NEXT__

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "imagedev/flopdrv.h"
#include "machine/mccs1850.h"
#include "machine/8530scc.h"
#include "machine/nextkbd.h"
#include "machine/n82077aa.h"
#include "machine/ncr5390.h"
#include "machine/mb8795.h"
#include "machine/nextmo.h"
#include "imagedev/chd_cd.h"
#include "imagedev/harddriv.h"

class next_state : public driver_device
{
public:
	next_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  maincpu(*this, "maincpu"),
		  rtc(*this, "rtc"),
		  scc(*this, "scc"),
		  keyboard(*this, "keyboard"),
		  scsi(*this, "scsi"),
		  net(*this, "net"),
		  mo(*this, "mo"),
		  fdc(*this, "fdc")
	{ }

	required_device<cpu_device> maincpu;
	required_device<mccs1850_device> rtc;
	required_device<scc8530_t> scc;
	required_device<nextkbd_device> keyboard;
	required_device<ncr5390_device> scsi;
	required_device<mb8795_device> net;
	required_device<nextmo_device> mo;
	optional_device<n82077aa_device> fdc; // 040 only

	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	READ8_MEMBER( io_r );
	WRITE8_MEMBER( io_w );
	READ32_MEMBER( rom_map_r );
	READ32_MEMBER( scr2_r );
	WRITE32_MEMBER( scr2_w );
	READ32_MEMBER( scr1_r );
	READ32_MEMBER( irq_status_r );
	READ32_MEMBER( irq_mask_r );
	WRITE32_MEMBER( irq_mask_w );
	READ32_MEMBER( modisk_r );
	READ32_MEMBER( network_r );
	READ32_MEMBER( event_counter_r );
	READ32_MEMBER( dsp_r );
	READ32_MEMBER( fdc_control_r );
	WRITE32_MEMBER( fdc_control_w );
	READ32_MEMBER( dma_ctrl_r );
	WRITE32_MEMBER( dma_ctrl_w );
	WRITE32_MEMBER( dma_040_ctrl_w );
	READ32_MEMBER( dma_regs_r );
	WRITE32_MEMBER( dma_regs_w );
	READ32_MEMBER( scsictrl_r );
	WRITE32_MEMBER( scsictrl_w );
	READ32_MEMBER( phy_r );
	WRITE32_MEMBER( phy_w );
	READ32_MEMBER( timer_data_r );
	WRITE32_MEMBER( timer_data_w );
	READ32_MEMBER( timer_ctrl_r );
	WRITE32_MEMBER( timer_ctrl_w );

	UINT32 scr2;
	UINT32 irq_status;
	UINT32 irq_mask;
	int irq_level;
	UINT32 *vram;
	UINT8 scsictrl, scsistat;

	UINT32 phy[2];

	attotime timer_tbase;
	UINT16 timer_vbase;
	UINT32 timer_data;
	UINT32 timer_ctrl;
	emu_timer *timer_tm;

	UINT32 eventc_latch;

	void scc_irq(int state);
	void keyboard_irq(bool state);

	void scsi_irq(bool state);
	void scsi_drq(bool state);

	void fdc_irq(bool state);
	void fdc_drq(bool state);

	void net_tx_irq(bool state);
	void net_rx_irq(bool state);
	void net_tx_drq(bool state);
	void net_rx_drq(bool state);

	void mo_irq(bool state);
	void mo_drq(bool state);

	static const SCSIConfigTable scsi_devices;
	static const floppy_format_type floppy_formats[];
	static const cdrom_interface cdrom_intf;
	static const harddisk_interface harddisk_intf;

protected:
	struct dma_slot {
		UINT32 start, limit, chain_start, chain_limit, current;
		UINT8 state;
		bool supdate, drq;
	};

	enum {
		// write bits
		DMA_SETENABLE    = 0x01,
		DMA_SETSUPDATE   = 0x02,
		DMA_SETREAD      = 0x04,
		DMA_CLRCOMPLETE  = 0x08,
		DMA_RESET        = 0x10,
		DMA_INITBUF      = 0x20,
		DMA_INITBUFTURBO = 0x40,

		// read bits
		DMA_ENABLE       = 0x01,
		DMA_SUPDATE      = 0x02,
		DMA_READ         = 0x04,
		DMA_COMPLETE     = 0x08,
		DMA_BUSEXC       = 0x10,
	};

	static const char *dma_targets[0x20];
	static const int dma_irqs[0x20];
	static const int scsi_clocks[4];

	dma_slot dma_slots[0x20];
	UINT32 esp;

	virtual void machine_start();
	virtual void machine_reset();

	void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	void timer_start();
	void timer_update();

	void irq_set(int id, bool raise);
	void irq_check();
	const char *dma_name(int slot);
	void dma_do_ctrl_w(int slot, UINT8 data);
	void dma_drq_w(int slot, bool state);
	void dma_read(int slot, UINT8 &val, bool &eof, bool &err);
	void dma_write(int slot, UINT8 val, bool eof, bool &err);
	void dma_check_end(int slot, bool eof);
	void dma_done(int slot);
	void dma_end(int slot);
};

#endif
