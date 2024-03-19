/*
 * Copyright (c) 2020 Actions Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief USB Actions OTG controller (cuckoo) driver private definitions
 *
 * This file contains the Actions OTG USB controller (cuckoo) driver private
 * definitions.
 */

#ifndef __USB_AOTG_LARK_H__
#define __USB_AOTG_LARK_H__

#include <soc_regs.h>
#include <soc_irq.h>

/* USB IN EP index */
enum usb_aotg_in_ep_idx {
	USB_AOTG_IN_EP_0 = 0,
	USB_AOTG_IN_EP_1,
	USB_AOTG_IN_EP_2,
	USB_AOTG_IN_EP_3,
	USB_AOTG_IN_EP_4,
	USB_AOTG_IN_EP_5,
	USB_AOTG_IN_EP_6,
	USB_AOTG_IN_EP_NUM
};

/* USB OUT EP index */
enum usb_aotg_out_ep_idx {
	USB_AOTG_OUT_EP_0 = 0,
	USB_AOTG_OUT_EP_1,
	USB_AOTG_OUT_EP_2,
	USB_AOTG_OUT_EP_3,
	USB_AOTG_OUT_EP_4,
	USB_AOTG_OUT_EP_5,
	USB_AOTG_OUT_EP_6,
	USB_AOTG_OUT_EP_NUM
};


/*********************************** Common **********************************/

/* FIFO Clock */
#define USB_MEM_CLK	CMU_MEMCLKSRC0
#define USB_RAM_CLKSRC_BIT     23
#define USB_MEM_CLK_EN	CMU_MEMCLKEN0
#define USB_RAM_CLKEN_BIT      23

/* USB AOTG IRQ line */
#define USB_AOTG_IRQ	IRQ_ID_USB


/*********************************** Global **********************************/


/* USB AOTG Base Address */
#define USB_AOTG_BASE	0x40050000

/* USB AVDD Control */
#define UAVDDCTL	(USB_AOTG_BASE+0x428)
#define USB_VDD	UAVDDCTL
#define USB_VDD_EN	6
#define USB_VDD_VOLTAGE_MASK 0x7f
/* defalut output voltage: 1.10V */
#define USB_VDD_VOLTAGE_DEFAULT	(0x1 << 4)

/** Debug */
#define USBDEBUG	(USB_AOTG_BASE + 0x427)
#define USBDEBUG_EN	4
#define USBDEBUG_MODE_MASK	0x0F
/* Testing */
#define BKDOOR	(USB_AOTG_BASE + 0x40D)
#define HS_DISABLE	7


/** Line Status */
#define LINESTATUS	(USB_AOTG_BASE + 0x422)
#define LINE_DM	4
#define LINE_DP	3
#define LINESTATE_MASK	(0x3 << 3)
#define LINESTATE_DM	(1 << LINE_DM)
#define LINESTATE_DP	(1 << LINE_DP)
#define OTGRESET	0


/** DPDM */
#define DPDMCTRL	(USB_AOTG_BASE + 0x421)
/* plug-in (connect/disconnect) */
#define PLUGIN	6
/* Line status detect enable (default: enable) */
#define LSDETEN	4
/* 500K pull-up enable */
#define DM_PULL_UP	3
#define DP_PULL_UP	2
/* 15K pull-down disable */
#define DM_PULL_DOWN	1
#define DP_PULL_DOWN	0
#define DPDM_DEVICE	(0x1F)
#define DPDM_HOST	(0x10)


/** Idpin & Vbus */
#define IDVBUSCTRL	(USB_AOTG_BASE + 0x420)
#define SOFT_IDPIN	3
#define SOFT_IDEN	2
#define IDVBUS_DEVICE	(0x0C)
#define IDVBUS_HOST	(0x04)


/** Auto In mode IN Token timer */
#define AUTOINTIMER	(USB_AOTG_BASE + 0x410)


/** USB Control and Status */
#define USBCS	(USB_AOTG_BASE + 0x1A3)
/* Soft disconnect */
#define USBCS_DISCONN	6
/* Remote wakeup */
#define USBCS_WAKEUP	5
#define USBCS_SPEED	1
#define USBCS_LS	0


/** Device Address */
#define FNADDR	(USB_AOTG_BASE + 0x1A6)


/* USB Clock Gate */
#define USBCLKGATE	(USB_AOTG_BASE + 0x1A7)
#define CLKGATE_SUSPEND	0


/** USB OTG FSM State */
#define USBSTATE	(USB_AOTG_BASE + 0x1BD)
/* Alias */
#define OTGSTATE	USBSTATE
#define OTG_A_IDLE	0x0
#define OTG_A_WAIT_BCON	0x2
#define OTG_A_HOST	0x3
#define OTG_A_SUSPEND	0x4
#define OTG_B_IDLE	0x8
#define OTG_B_PERIPHERAL	0x9


/** USB OTG FSM Control */
#define USBCTRL	(USB_AOTG_BASE + 0x1BE)
/* Alias */
#define OTGCTRL	USBCTRL
#define OTGCTRL_FORCE	7
#define OTGCTRL_BUSREQ	0


/** USB Status */
#define USBSTATUS	(USB_AOTG_BASE + 0x1BF)
/* 0: mini-A; 1: mini-B */
#define USBSTATUS_ID	6
#define USBSTATUS_CONN	1


/************************************ IRQ ************************************/


/** USB Core */
#define USBIEN	(USB_AOTG_BASE + 0x198)
#define USBIEN_NTR	6
#define USBIEN_HS	5
#define USBIEN_RESET	4
#define USBIEN_SUSPEND	3
#define USBIEN_TOKEN	2
#define USBIEN_SOF	1
#define USBIEN_SETUP	0

#define USBIRQ	(USB_AOTG_BASE + 0x18C)
#define USBIRQ_NTR	6
/* High-speed */
#define USBIRQ_HS	5
/* USB Bus Reset */
#define USBIRQ_RESET	4
/* Suspend Signal */
#define USBIRQ_SUSPEND	3
/* SETUP Token */
#define USBIRQ_TOKEN	2
/* SOF Packet */
#define USBIRQ_SOF	1
/* SETUP Data */
#define USBIRQ_SETUP	0


/** OTG FSM */
#define OTGIEN	(USB_AOTG_BASE + 0x1C0)
/* a_peripheral/b_peripheral */
#define OTGIEN_PERIPHERAL	4
/* a_host/b_host */
#define OTGIEN_LOCSOF	2
/* a_idle/b_idle */
#define OTGIEN_IDLE	0

#define OTGIRQ	(USB_AOTG_BASE + 0x1BC)
#define OTGIRQ_PERIPHERAL	4
#define OTGIRQ_LOCSOF	2
#define OTGIRQ_IDLE	0


/** Specific OTG FSM IRQ */
#define OTGSTATE_IEN	(USB_AOTG_BASE + 0x405)
#define OTGSTATE_IRQ	(USB_AOTG_BASE + 0x404)
#define OTGSTATE_B_WAIT_ACON	2
#define OTGSTATE_A_WAIT_BCON	1
#define OTGSTATE_A_SUSPEND	0


/** USB External Interrupt request */
#define USBEIRQ	(USB_AOTG_BASE + 0x400)
/* Alias: same register */
#define USBEIEN	USBEIRQ
/* External */
#define USBEIRQ_EXTERN	7
/* Wakeup */
#define USBEIRQ_WAKEUP	6
/* Resume */
#define USBEIRQ_RESUME	5
/* Connnect/disconnect */
#define USBEIRQ_CONDISC	4
/* External */
#define USBEIEN_EXTERN	3
/* Wakeup */
#define USBEIEN_WAKEUP	2
/* Resume */
#define USBEIEN_RESUME	1
/* Connnect/disconnect */
#define USBEIEN_CONDISC	0
#define USBEIEN_MASK	(0x0f)
#define USBEIRQ_MASK	(0xf0)


/** Interrupt Vector */
#define IVECT	(USB_AOTG_BASE + 0x1A0)
/* Device */
/* Setup Data */
#define UIV_SUDAV	0x00
#define UIV_SOF	0x04
/* Setup Token */
#define UIV_SUTOK	0x08
#define UIV_SUSPEND	0x0c
#define UIV_USBRST	0x10
#define UIV_HSPEED	0x14
/* Endpoint */
#define UIV_HCOUT0ERR	0x16
#define UIV_EP0IN	0x18
#define UIV_HCIN0ERR	0x1a
#define UIV_EP0OUT	0x1c
#define UIV_EP0PING	0x20
#define UIV_HCOUT1ERR	0x22
#define UIV_EP1IN	0x24
#define UIV_HCIN1ERR	0x26
#define UIV_EP1OUT	0x28
#define UIV_EP1PING	0x2c
#define UIV_HCOUT2ERR	0x2e
#define UIV_EP2IN	0x30
#define UIV_HCIN2ERR	0x32
#define UIV_EP2OUT	0x34
#define UIV_EP2PING	0x38
#define UIV_HCOUT3ERR	0x3a
#define UIV_EP3IN	0x3c
#define UIV_HCIN3ERR	0x3e
#define UIV_EP3OUT	0x40
#define UIV_EP3PING	0x44
#define UIV_HCOUT4ERR	0x46
#define UIV_EP4IN	0x48
#define UIV_HCIN4ERR	0x4A
#define UIV_EP4OUT	0x4C
#define UIV_EP4PING	0x50
/* IN Token */
#define UIV_INTOKEN	0xe0
/* OUT Token */
#define UIV_OUTTOKEN	0xe4
#define UIV_OTGIRQ	0xd8

/* Controller */
#define UIV_RESUME	0xe6
#define UIV_CONDISC	0xe8
/* OTG */
#define UIV_OTGSTATE	0xea
/* EP OUT Short Packet */
#define UIV_EPOUTSHTPKT	0xec
#define UIV_HCINEND	0xee
#define UIV_HCOUTEMPTY	0xf0
#define UIV_NTR	0xf2

#define UIV_EPIN_VEC2ADDR(vector)	((vector - UIV_EP0IN) / 12)
#define UIV_EPOUT_VEC2ADDR(vector)	((vector - UIV_EP0OUT) / 12)
#define UIV_HCINERR_VEC2ADDR(vector)	((vector - UIV_HCIN0ERR) / 12)
#define UIV_HCOUTERR_VEC2ADDR(vector)	((vector - UIV_HCOUT0ERR) / 12)

/** Endpoint Interrupt request */

/* OUT IRQ */
#define OUT04IEN	(USB_AOTG_BASE + 0x196)
#define OUTIEN	OUT04IEN
#define OUT04IRQ	(USB_AOTG_BASE + 0x18A)
#define OUTIRQ	OUT04IRQ
#define IRQ_EP0OUT	0
#define IRQ_EP1OUT	1
#define IRQ_EP2OUT	2
#define IRQ_EP3OUT	3
#define IRQ_EPxOUT(ep)	(ep)

/** OUT Endpoints Short Packet Control */
#define OUT_SHTPKT	(USB_AOTG_BASE + 0x401)
#define IRQ_EP4OUT_SHTPKT	4
#define IRQ_EP3OUT_SHTPKT	3
#define IRQ_EP2OUT_SHTPKT	2
#define IRQ_EP1OUT_SHTPKT	1

/* IN IRQ */
#define IN04IEN	(USB_AOTG_BASE + 0x194)
#define INIEN	IN04IEN
#define IN04IRQ	(USB_AOTG_BASE + 0x188)
#define INIRQ	IN04IRQ
#define IRQ_EP0IN	0
#define IRQ_EP1IN	1
#define IRQ_EP2IN	2
#define IRQ_EP3IN	3
#define IRQ_EP4IN	3
#define IRQ_EPxIN(ep)	(ep)

#define INEMPTY_IRQ	(USB_AOTG_BASE + 0x402)
#define INEMPTY_IEN	(USB_AOTG_BASE + 0x403)
#define IRQ_IN4EMPTY	4
#define IRQ_IN3EMPTY	3
#define IRQ_IN2EMPTY	2
#define IRQ_IN1EMPTY	1
#define IEN_IN4EMPTY	4
#define IEN_IN3EMPTY	3
#define IEN_IN2EMPTY	2
#define IEN_IN1EMPTY	1

#define IRQ_INxEMPTY(ep)	(ep)
#define IEN_INxEMPTY(ep)	(ep)
#define INEMPTY_IRQ_MASK	(0x1e)
#define INEMPTY_IEN_MASK	(0x1e)
#define INEMPTY_IRQ_SHIFT	1
#define INEMPTY_IEN_SHIFT	1
/* shift to base 0 */
#define INEMPTY_IRQ2ADDR(bit)	(bit + 1)
#define INEMPTY_IEN2ADDR(bit)	(bit + 1)

/** IN Token IRQ */
#define IN04TOKIEN	(USB_AOTG_BASE + 0x19C)
#define IN_TOKIEN	IN04TOKIEN
#define IN04TOKIRQ	(USB_AOTG_BASE + 0x190)
#define IN_TOKIRQ	IN04TOKIRQ
#define EP0IN_TOKEN	0
#define EP1IN_TOKEN	1
#define EP2IN_TOKEN	2
#define EP3IN_TOKEN	3
#define EP4IN_TOKEN	4
#define EPIN_TOKEN_NUM	5
#define EPIN_TOKEN_MASK	0x1F

/** OUT Token IRQ */
#define OUT04TOKIEN	(USB_AOTG_BASE + 0x19D)
#define OUT_TOKIEN	OUT04TOKIEN
#define OUT04TOKIRQ	(USB_AOTG_BASE + 0x191)
#define OUT_TOKIRQ	OUT04TOKIRQ
#define EP0OUT_TOKEN	0
#define EP1OUT_TOKEN	1
#define EP2OUT_TOKEN	2
#define EP3OUT_TOKEN	3
#define EP4OUT_TOKEN	4
#define EPOUT_TOKEN_NUM	5
#define EPOUT_TOKEN_MASK	0x1F


/************************************ FIFO ***********************************/

/*
 * AOTG USB Controller FIFO layout (add by order...)
 * Luckily, we don't need to take care of ep0 FIFO.
 *
 * URAM0 FIFO (only for ep1-in dma): 2KB [0x0, 0x800)
 * URAM1 FIFO (only for ep2-out dma): 2KB [0x800, 0x1000)
 * URAM2 FIFO (for ep1-out, ep2-in, ep3-in/out ep4-in/out cpu): 4KB [0x1000, 0x1FFF)
 * URAM3 FIFO (only for ep0): 128-byte
 */
/* FIFO start address */
#define URAM0_FIFO_START	0x0
#define URAM1_FIFO_START	0x800
#define URAM2_FIFO_START	0x1000
#define NORMAL_FIFO_START	URAM2_FIFO_START
/* Arbitrary single independent FIFO block size */
#define NORMAL_FIFO_BLOCK	256

/*
 * Arbitrary allocaction for NORMAL_FIFO_START, need to optimize!
 */
#define EP1IN_FIFO_START	URAM0_FIFO_START
#define EP2OUT_FIFO_START	URAM1_FIFO_START
#define EP1OUT_FIFO_START	NORMAL_FIFO_START
#define EP2IN_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 4)
#define EP3OUT_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 8)
#define EP3IN_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 9)
#define EP4OUT_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 10)
#define EP4IN_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 11)
#define EP5OUT_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 12)
#define EP5IN_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 13)
#define EP6OUT_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 14)
#define EP6IN_FIFO_START	(NORMAL_FIFO_START + NORMAL_FIFO_BLOCK * 15)

/** FIFO Control */
#define FIFOCTRL	(USB_AOTG_BASE + 0x1A8)
/* FIFO Auto */
#define FIFOCTRL_AUTO	5
/* FIFO Direction */
#define FIFOCTRL_IO_IN	4
/* Endpoint address */
#define FIFOCTRL_NUM_MASK	0x0F

/** FIFO Data */
#define FIFO1DAT	(USB_AOTG_BASE + 0x084)
#define FIFO2DAT	(USB_AOTG_BASE + 0x088)
#define FIFO3DAT	(USB_AOTG_BASE + 0x08C)
#define FIFO4DAT	(USB_AOTG_BASE + 0x090)
#define FIFOxDAT(ep)	(FIFO1DAT + (ep - 1) * 4)

/** FIFO OUT Address */
#define EP1OUT_STADDR	(USB_AOTG_BASE + 0x304)
#define EP2OUT_STADDR	(USB_AOTG_BASE + 0x308)
#define EP3OUT_STADDR	(USB_AOTG_BASE + 0x30C)
#define EP4OUT_STADDR	(USB_AOTG_BASE + 0x310)
#define EP5OUT_STADDR	(USB_AOTG_BASE + 0x314)
#define EP6OUT_STADDR	(USB_AOTG_BASE + 0x318)
#define EPxOUT_STADDR(ep)	(EP1OUT_STADDR + (ep - 1) * 4)

/** FIFO IN Address */
#define EP1IN_STADDR	(USB_AOTG_BASE + 0x344)
#define EP2IN_STADDR	(USB_AOTG_BASE + 0x348)
#define EP3IN_STADDR	(USB_AOTG_BASE + 0x34C)
#define EP4IN_STADDR	(USB_AOTG_BASE + 0x350)
#define EP5IN_STADDR	(USB_AOTG_BASE + 0x354)
#define EP6IN_STADDR	(USB_AOTG_BASE + 0x358)

#define EPxIN_STADDR(ep)	(EP1IN_STADDR + (ep - 1) * 4)

/** Ep0 In Data */
#define EP0INDATA	(USB_AOTG_BASE + 0x100)
#define EP0IN_FIFO	EP0INDATA
/** Ep0 Out Data */
#define EP0OUTDATA	(USB_AOTG_BASE + 0x140)
#define EP0OUT_FIFO	EP0OUTDATA
/** Setup Data */
#define SETUPDAT0	(USB_AOTG_BASE + 0x180)
#define SETUPDAT1	(USB_AOTG_BASE + 0x181)
#define SETUPDAT2	(USB_AOTG_BASE + 0x182)
#define SETUPDAT3	(USB_AOTG_BASE + 0x183)
#define SETUPDAT4	(USB_AOTG_BASE + 0x184)
#define SETUPDAT5	(USB_AOTG_BASE + 0x185)
#define SETUPDAT6	(USB_AOTG_BASE + 0x186)
#define SETUPDAT7	(USB_AOTG_BASE + 0x187)
#define SETUP_FIFO	SETUPDAT0

/** Byte Counter */
#define OUT0BC	(USB_AOTG_BASE + 0x000)
#define IN0BC	(USB_AOTG_BASE + 0x001)

/* Out Endpoint */
#define OUT1BCL	(USB_AOTG_BASE + 0x008)
#define OUT1BCH	(USB_AOTG_BASE + 0x009)
#define OUT1BC	OUT1BCL
#define OUT2BCL	(USB_AOTG_BASE + 0x010)
#define OUT2BCH	(USB_AOTG_BASE + 0x011)
#define OUT2BC	OUT2BCL
#define OUT3BCL	(USB_AOTG_BASE + 0x018)
#define OUT3BCH	(USB_AOTG_BASE + 0x019)
#define OUT3BC	OUT3BCL
#define OUT4BCL	(USB_AOTG_BASE + 0x020)
#define OUT4BCH	(USB_AOTG_BASE + 0x021)
#define OUT4BC	OUT4BCL
#define OUT5BCL	(USB_AOTG_BASE + 0x028)
#define OUT5BCH	(USB_AOTG_BASE + 0x029)
#define OUT5BC	OUT5BCL
#define OUT6BCL	(USB_AOTG_BASE + 0x030)
#define OUT6BCH	(USB_AOTG_BASE + 0x031)
#define OUT6BC	OUT6BCL
#define OUTxBC(ep)	(OUT1BC + (ep - 1) * 8)

/* In Endpoint */
#define IN1BCL	(USB_AOTG_BASE + 0x00C)
#define IN1BCH	(USB_AOTG_BASE + 0x00D)
#define IN1BC	IN1BCL
#define IN2BCL	(USB_AOTG_BASE + 0x014)
#define IN2BCH	(USB_AOTG_BASE + 0x015)
#define IN2BC	IN2BCL
#define IN3BCL	(USB_AOTG_BASE + 0x01C)
#define IN3BCH	(USB_AOTG_BASE + 0x01D)
#define IN3BC	IN3BCL
#define IN4BCL	(USB_AOTG_BASE + 0x024)
#define IN4BCH	(USB_AOTG_BASE + 0x025)
#define IN4BC	IN4BCL
#define IN5BCL	(USB_AOTG_BASE + 0x02C)
#define IN5BCH	(USB_AOTG_BASE + 0x02D)
#define IN5BC	IN5BCL
#define IN6BCL	(USB_AOTG_BASE + 0x034)
#define IN6BCH	(USB_AOTG_BASE + 0x035)
#define IN6BC	IN6BCL

#define INxBC(ep)	(IN1BC + (ep - 1) * 8)
#define INxBCL(ep)	(IN1BCL + (ep - 1) * 8)
#define INxBCH(ep)	(IN1BCH + (ep - 1) * 8)


/************************************ HCD ***********************************/

/** Host Endpoint Control */
#define HCEP0CTRL	(USB_AOTG_BASE + 0x0C0)

#define HCOUT0CTRL	HCEP0CTRL
#define HCOUT1CTRL	(USB_AOTG_BASE + 0x0C4)
#define HCOUT2CTRL	(USB_AOTG_BASE + 0x0C8)
#define HCOUT3CTRL	(USB_AOTG_BASE + 0x0CC)
#define HCOUT4CTRL	(USB_AOTG_BASE + 0x0D0)
#define HCOUTxCTRL(ep)	(HCEP0CTRL + ep * 4)

#define HCIN0CTRL	HCEP0CTRL
#define HCIN1CTRL	(USB_AOTG_BASE + 0x0C6)
#define HCIN2CTRL	(USB_AOTG_BASE + 0x0CA)
#define HCIN3CTRL	(USB_AOTG_BASE + 0x0CE)
#define HCIN4CTRL	(USB_AOTG_BASE + 0x0D2)
#define HCINxCTRL(ep)	((ep == 0) ? HCIN0CTRL : (HCIN1CTRL + (ep - 1) * 4))

/* default: 0 */
#define HCEPCTRL_EP_ADDR(ep)	(ep & 0x0F)


/** Host Endpoint Error */
#define HCOUT0ERR	(USB_AOTG_BASE + 0x0C1)
#define HCOUT1ERR	(USB_AOTG_BASE + 0x0C5)
#define HCOUT2ERR	(USB_AOTG_BASE + 0x0C9)
#define HCOUT3ERR	(USB_AOTG_BASE + 0x0CD)
#define HCOUT4ERR	(USB_AOTG_BASE + 0x0D1)
#define HCOUTxERR(ep)	(HCOUT0ERR + ep * 4)

#define HCIN0ERR	(USB_AOTG_BASE + 0x0C3)
#define HCIN1ERR	(USB_AOTG_BASE + 0x0C7)
#define HCIN2ERR	(USB_AOTG_BASE + 0x0CB)
#define HCIN3ERR	(USB_AOTG_BASE + 0x0CF)
#define HCIN4ERR	(USB_AOTG_BASE + 0x0D3)
#define HCINxERR(ep)	(HCIN0ERR + ep * 4)

/* HCEPERR_DOPING: only for ep-out */
#define HCEPERR_DOPING	6
#define HCEPERR_RESEND	5
#define HCEPERR_TYPE_MASK	(0x7 << 2)
#define HCEPERR_CNT_MASK	0x3
/* Error types */
#define NO_ERR	(0x0 << 2)
#define ERR_CRC	(0x1 << 2)
#define ERR_TOGGLE	(0x2 << 2)
#define ERR_STALL	(0x3 << 2)
#define ERR_TIMEOUT	(0x4 << 2)
#define ERR_PID	(0x5 << 2)
#define ERR_OVERRUN	(0x6 << 2)
#define ERR_UNDERRUN	(0x7 << 2)

/* Error count maximum */
#define ERR_COUNT_MAX	10

#define HCIN03ERRIRQ	(USB_AOTG_BASE + 0x1B4)
#define HCINEPERRIRQ	HCIN03ERRIRQ

#define HCOUT03ERRIRQ	(USB_AOTG_BASE + 0x1B6)
#define HCOUTEPERRIRQ	HCOUT03ERRIRQ

#define HCEPxERRIRQ(ep)	(ep)

#define HCIN03ERRIEN	(USB_AOTG_BASE + 0x1B8)
#define HCINEPERRIEN	HCIN03ERRIEN

#define HCOUT03ERRIEN	(USB_AOTG_BASE + 0x1BA)
#define HCOUTEPERRIEN	HCOUT03ERRIEN

#define HCEPxERRIEN(ep)	(ep)


/** Host IN Endpoint Control */
#define HCINCTRL	(USB_AOTG_BASE + 0x411)
#define HCINx_START(ep)	(ep)
/* set: stop IN token when a short packet received */
#define HCINx_SHORT(ep)	((ep - 1) * 2)


/** Host IN End interrupt */
#define HCINENDINT	(USB_AOTG_BASE + 0x403)
#define IRQ_HCIN4END	4
#define IRQ_HCIN3END	3
#define IRQ_HCIN2END	2
#define IRQ_HCIN1END	1
#define IEN_HCIN4END	4
#define IEN_HCIN3END	3
#define IEN_HCIN2END	2
#define IEN_HCIN1END	1


/** Host IN packet count */
#define HCIN1CNTL	(USB_AOTG_BASE + 0x412)
#define HCIN1CNTH	(USB_AOTG_BASE + 0x413)
#define HCIN1CNT	HCIN1CNTL
#define HCIN2CNTL	(USB_AOTG_BASE + 0x414)
#define HCIN2CNTH	(USB_AOTG_BASE + 0x415)
#define HCIN2CNT	HCIN2CNTL
#define HCIN3CNTL	(USB_AOTG_BASE + 0x416)
#define HCIN3CNTH	(USB_AOTG_BASE + 0x417)
#define HCIN3CNT	HCIN3CNTL
#define HCIN4CNTL	(USB_AOTG_BASE + 0x418)
#define HCIN4CNTH	(USB_AOTG_BASE + 0x419)
#define HCIN4CNT	HCIN4CNTL
#define HCINxCNT(ep)	(HCIN1CNT + (ep - 1) * 2)
#define HCINxCNTL(ep)	(HCIN1CNTL + (ep - 1) * 2)
#define HCINxCNTH(ep)	(HCIN1CNTH + (ep - 1) * 2)


/** Host Port Control */
#define HCPORTCTRL	(USB_AOTG_BASE + 0x1AB)
/* default: RST_55MS */
#define RST_10MS	(0x0 << 6)
#define RST_55MS	(0x1 << 6)
#define RST_1_6MS	(0x2 << 6)
/* Port Reset */
#define PORTRST	5
/* Test_J */
#define PORTTST_J	(0x1 << 0)
/* Test_K */
#define PORTTST_K	(0x2 << 0)
/* Test_SE0_NAK */
#define PORTTST_SE0	(0x4 << 0)
/* Test_Packet */
#define PORTTST_PKT	(0x8 << 0)
/* Test_Force_Enable */
#define PORTTST_FE	(0x10 << 0)


/** Host Frame */
#define HCFRMNL	(USB_AOTG_BASE + 0x1AC)
#define HCFRMNH	(USB_AOTG_BASE + 0x1AD)
#define HCFRMN	HCFRMNL
#define HCFRMREMAINL	(USB_AOTG_BASE + 0x1AE)
#define HCFRMREMAINH	(USB_AOTG_BASE + 0x1AF)
#define HCFRMREMAIN	HCFRMREMAINL


/************************************ Endp ***********************************/

/** Endpoint 0 Control and Status */
#define EP0CS	(USB_AOTG_BASE + 0x002)
#define EP0CS_SETTOGGLE	6
#define EP0CS_CLRTOGGLE	5
#define EP0CS_HCSET	4
#define EP0CS_OUTBUSY	3
#define EP0CS_INBUSY	2
/* Endpoint 0 NAK */
#define EP0CS_NAK	1
/* Endpoint 0 STALL */
#define EP0CS_STALL	0


/** Endpoint Control and Status */
#define OUT1CS	(USB_AOTG_BASE + 0x00B)
#define OUT2CS	(USB_AOTG_BASE + 0x013)
#define OUT3CS	(USB_AOTG_BASE + 0x01B)
#define OUT4CS	(USB_AOTG_BASE + 0x023)
#define OUTxCS(ep)	(OUT1CS + (ep - 1) * 8)

#define IN1CS	(USB_AOTG_BASE + 0x00F)
#define IN2CS	(USB_AOTG_BASE + 0x017)
#define IN3CS	(USB_AOTG_BASE + 0x01F)
#define IN4CS	(USB_AOTG_BASE + 0x027)
#define INxCS(ep)	(IN1CS + (ep - 1) * 8)

#define EPCS_AUTO	4
#define EPCS_NPAK_MASK	0x0C
#define EPCS_NPAK_00	(0x0 < 2)
#define EPCS_NPAK_01	(0x1 < 2)
#define EPCS_NPAK_10	(0x2 < 2)
#define EPCS_NPAK_11	(0x3 < 2)
#define EPCS_NPAK	2
#define EPCS_BUSY	1
#define EPCS_ERR	0


/** Endpoint Control */
#define OUT1CTRL	(USB_AOTG_BASE + 0x00A)
#define OUT2CTRL	(USB_AOTG_BASE + 0x012)
#define OUT3CTRL	(USB_AOTG_BASE + 0x01A)
#define OUT4CTRL	(USB_AOTG_BASE + 0x022)
#define OUTxCTRL(ep)	(OUT1CTRL + (ep - 1) * 8)

#define IN1CTRL	(USB_AOTG_BASE + 0x00E)
#define IN2CTRL	(USB_AOTG_BASE + 0x016)
#define IN3CTRL	(USB_AOTG_BASE + 0x01E)
#define IN4CTRL	(USB_AOTG_BASE + 0x026)
#define INxCTRL(ep)	(IN1CTRL + (ep - 1) * 8)

#define EPCTRL_VALID	7
#define EPCTRL_STALL	6
#define EPCTRL_TYPE	0x0C
#define EPCTRL_BUF	0x03
/* 3 ISO packets per microframe */
#define EPCTRL_ISOC_3	5
/* 2 ISO packets per microframe */
#define EPCTRL_ISOC_2	4


/** NAK Control for OUT Endpoints */
#define NAKOUTCTRL	(USB_AOTG_BASE + 0x40C)
#define EP3_NAK	3
#define EP2_NAK	2
#define EP1_NAK	1


/** Endpoint Reset */
#define EPRST	(USB_AOTG_BASE + 0x1A2)
/* Endpoint FIFO Reset */
#define EPRST_FIFORST	6
/* Endpoint Toggle Reset */
#define EPRST_TOGRST	5
/* Endpoint Direction */
#define EPRST_IO_IN	4
/* Endpoint address */
#define EPRST_NUM_MASK	0x0F


/** Ep max packet size */

/* Ep0 max packet size: not configure */
#define OUT0MAXPKT	(USB_AOTG_BASE + 0x1E0)
#define EP0MAXPKT	OUT0MAXPKT

/* Ep-out max packet size */
#define OUT1MAXPKTL	(USB_AOTG_BASE + 0x1E2)
#define OUT1MAXPKTH	(USB_AOTG_BASE + 0x1E3)
#define OUT1MAXPKT	OUT1MAXPKTL
#define OUT2MAXPKTL	(USB_AOTG_BASE + 0x1E4)
#define OUT2MAXPKTH	(USB_AOTG_BASE + 0x1E5)
#define OUT2MAXPKT	OUT2MAXPKTL
#define OUT3MAXPKTL	(USB_AOTG_BASE + 0x1E6)
#define OUT3MAXPKTH	(USB_AOTG_BASE + 0x1E7)
#define OUT3MAXPKT	OUT3MAXPKTL
#define OUT4MAXPKTL	(USB_AOTG_BASE + 0x1E8)
#define OUT4MAXPKTH	(USB_AOTG_BASE + 0x1E9)
#define OUT4MAXPKT	OUT4MAXPKTL
#define OUTxMAXPKT(ep)	(OUT1MAXPKT + (ep - 1) * 2)
#define OUTxMAXPKTL(ep)	(OUT1MAXPKTL + (ep - 1) * 2)
#define OUTxMAXPKTH(ep)	(OUT1MAXPKTH + (ep - 1) * 2)

/* Ep-in max packet size */
#define IN1MAXPKTL	(USB_AOTG_BASE + 0x3E2)
#define IN1MAXPKTH	(USB_AOTG_BASE + 0x3E3)
#define IN1MAXPKT	IN1MAXPKTL
#define IN2MAXPKTL	(USB_AOTG_BASE + 0x3E4)
#define IN2MAXPKTH	(USB_AOTG_BASE + 0x3E5)
#define IN2MAXPKT	IN2MAXPKTL
#define IN3MAXPKTL	(USB_AOTG_BASE + 0x3E6)
#define IN3MAXPKTH	(USB_AOTG_BASE + 0x3E7)
#define IN3MAXPKT	IN3MAXPKTL
#define IN4MAXPKTL	(USB_AOTG_BASE + 0x3E8)
#define IN4MAXPKTH	(USB_AOTG_BASE + 0x3E9)
#define IN4MAXPKT	IN4MAXPKTL

#define INxMAXPKT(ep)	(IN1MAXPKT + (ep - 1) * 2)


/************************************ PHY ************************************/


#define USBPHYCTRL	(USB_AOTG_BASE + 0x423)
#define PHY_PLLEN	7
#define PHY_DALLUALLEN	6
#define USBPHYCTRL_ENABLE	0xC0
#define USBPHYCTRL_DISABLE	0x0


#define VDCTRL	(USB_AOTG_BASE + 0x424)
#define VDSTATE	(USB_AOTG_BASE + 0x425)
#define USBEFUSEREF	(USB_AOTG_BASE + 0x426)


/************************************ DMA ************************************/


/* IN Endpoint DMA */
#define IN1_DMACTL	(USB_AOTG_BASE + 0x430)

#define IN1_DMALEN1L	(USB_AOTG_BASE + 0x434)
#define IN1_DMALEN1M	(USB_AOTG_BASE + 0x435)
#define IN1_DMALEN1H	(USB_AOTG_BASE + 0x436)
#define IN1_DMALEN2L	(USB_AOTG_BASE + 0x438)
#define IN1_DMALEN2M	(USB_AOTG_BASE + 0x439)
#define IN1_DMALEN2H	(USB_AOTG_BASE + 0x43A)

#define IN1_DMAREMAIN1L	(USB_AOTG_BASE + 0x43C)
#define IN1_DMAREMAIN1M	(USB_AOTG_BASE + 0x43D)
#define IN1_DMAREMAIN1H	(USB_AOTG_BASE + 0x43E)
#define IN1_DMAREMAIN2L	(USB_AOTG_BASE + 0x440)
#define IN1_DMAREMAIN2M	(USB_AOTG_BASE + 0x441)
#define IN1_DMAREMAIN2H	(USB_AOTG_BASE + 0x442)

/* OUT Endpoint DMA */
#define OUT2_DMACTL	(USB_AOTG_BASE + 0x444)

#define OUT2_DMALENL	(USB_AOTG_BASE + 0x448)
#define OUT2_DMALENM	(USB_AOTG_BASE + 0x449)
#define OUT2_DMALENH	(USB_AOTG_BASE + 0x44A)

#define OUT2_DMAREMAINL	(USB_AOTG_BASE + 0x44C)
#define OUT2_DMAREMAINM	(USB_AOTG_BASE + 0x44D)
#define OUT2_DMAREMAINH	(USB_AOTG_BASE + 0x44E)

/* 0: normal; 1: reset */
#define DMACTL_FIFORST	7
/* 0: burst8; 1: single */
#define DMACTL_MODE	4
/* 0: disabled; 1: enabled */
#define DMACTL_AUTOLOAD	3
/* 0: disabled; 1: enabled */
#define DMACTL_START	0


/********************************* Functions *********************************/


/*
 * Turn USB controller power on
 */
static inline void usb_aotg_power_on(void)
{

}

/*
 * Turn USB controller power off
 */
static inline void usb_aotg_power_off(void)
{

}

/*
 * USB PHY configuration
 */
static void usb_phy_setting(uint8_t reg, uint8_t value)
{
}


static inline void usb_aotg_reset_specific(void)
{
}

static inline void usb_aotg_disable_specific(void)
{
}

static inline void aotg_dc_phy_init(void)
{
}

static inline int aotg_dc_epout_alloc_fifo_specific(uint8_t ep_idx)
{
	switch (ep_idx) {
	case USB_AOTG_OUT_EP_1:
		usb_write32(EP1OUT_STADDR, EP1OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_2:
		usb_write32(EP2OUT_STADDR, EP2OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_3:
		usb_write32(EP3OUT_STADDR, EP3OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_4:
		usb_write32(EP4OUT_STADDR, EP4OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_5:
		usb_write32(EP5OUT_STADDR, EP5OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_6:
		usb_write32(EP6OUT_STADDR, EP6OUT_FIFO_START);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static inline int aotg_dc_epin_alloc_fifo_specific(uint8_t ep_idx)
{
	switch (ep_idx) {
	case USB_AOTG_IN_EP_1:
		usb_write32(EP1IN_STADDR, EP1IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_2:
		usb_write32(EP2IN_STADDR, EP2IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_3:
		usb_write32(EP3IN_STADDR, EP3IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_4:
		usb_write32(EP4IN_STADDR, EP4IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_5:
		usb_write32(EP5IN_STADDR, EP5IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_6:
		usb_write32(EP6IN_STADDR, EP6IN_FIFO_START);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/*
 * Switch FIFO clock to make sure it is available for AOTG
 */
static inline int aotg_dc_fifo_enable(void)
{
	usb_set_bit32(SPLL_CTL, 0);
	usb_set_bit32(USB_MEM_CLK, USB_RAM_CLKSRC_BIT);
	usb_set_bit32(USB_MEM_CLK_EN, USB_RAM_CLKEN_BIT);

	acts_clock_peripheral_enable(CLOCK_ID_USB2);

	return 0;
}

static inline int aotg_dc_fifo_disable(void)
{
	acts_clock_peripheral_disable(CLOCK_ID_USB2);
	usb_clear_bit32(USB_MEM_CLK, USB_RAM_CLKSRC_BIT);
	usb_clear_bit32(USB_MEM_CLK_EN, USB_RAM_CLKEN_BIT);

	usb_clear_bit32(SPLL_CTL, 0);

	return 0;
}

static inline void aotg_dc_force_fs(void)
{
	usb_set_bit8(BKDOOR, HS_DISABLE);
}

static inline int aotg_hc_epout_alloc_fifo_specific(uint8_t ep_idx)
{
	switch (ep_idx) {
	case USB_AOTG_OUT_EP_1:
		usb_write32(EP1OUT_STADDR, EP1OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_2:
		usb_write32(EP2OUT_STADDR, EP2OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_3:
		usb_write32(EP3OUT_STADDR, EP3OUT_FIFO_START);
		break;
	case USB_AOTG_OUT_EP_4:
		usb_write32(EP4OUT_STADDR, EP4OUT_FIFO_START);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static inline int aotg_hc_epin_alloc_fifo_specific(uint8_t ep_idx)
{
	switch (ep_idx) {
	case USB_AOTG_IN_EP_1:
		usb_write32(EP1IN_STADDR, EP1IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_2:
		usb_write32(EP2IN_STADDR, EP2IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_3:
		usb_write32(EP3IN_STADDR, EP3IN_FIFO_START);
		break;
	case USB_AOTG_IN_EP_4:
		usb_write32(EP4IN_STADDR, EP4IN_FIFO_START);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static inline void aotg_hc_phy_init(void)
{
	usb_phy_setting(0xe0, 0xa3);
	usb_phy_setting(0x84, 0x1a);
	usb_phy_setting(0xe7, 0x0b);
	usb_phy_setting(0xe7, 0x0f);
	usb_phy_setting(0xe3, 0x1e);
	usb_phy_setting(0x87, 0x94);
	usb_phy_setting(0x91, 0x08);
}

/*
 * Switch FIFO clock to make sure it is available for AOTG
 */
static inline int aotg_hc_fifo_enable(void)
{
	uint32_t value = usb_read32(USB_MEM_CLK);
	value |= (1<<8);
	usb_write32(USB_MEM_CLK, value);

	value = usb_read32(USB_MEM_CLK_EN);
	value |= (1<<8);
	usb_write32(USB_MEM_CLK_EN, value);

	return 0;
}

static inline int aotg_hc_fifo_disable(void)
{
	uint32_t value = usb_read32(USB_MEM_CLK);
	value |= ~(1<<8);
	usb_write32(USB_MEM_CLK, value);

	value = usb_read32(USB_MEM_CLK_EN);
	value |= ~(1<<8);
	usb_write32(USB_MEM_CLK_EN, value);

	return 0;
}

static inline int usb_aotg_dpdm_init(void)
{
	/* gpio 1&2 set mfp dp dm */
	usb_write32(GPIO_REG_BASE + 0x8, 0x200f);
	usb_write32(GPIO_REG_BASE + 0xc, 0x200f);

	return 0;
}

static inline int usb_aotg_dpdm_exit(void)
{
	return 0;
}

#endif /* __USB_AOTG_LARK_H__ */

