/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stm32f103x6.h> // TODO HOW TO AVOID USE CMSIS DIRECTLY
#include <usbd.h>

/* Private typedef -----------------------------------------------------------*/

/* Endpoint related registers structure */
/** @deprecated */
typedef struct st_usb_endpoint_s {
	__IO uint16_t *REG;      // USB_BASE + n*4
	__IO uint16_t *TX_ADDR;  // USB_BASE + 0x50 + n*8 + 0
	__IO uint16_t *TX_COUNT; // USB_BASE + 0x50 + n*8 + 2
	__IO uint16_t *RX_ADDR;  // USB_BASE + 0x50 + n*8 + 4
	__IO uint16_t *RX_COUNT; // USB_BASE + 0x50 + n*8 + 6
	__IO void *TX_BUFF;
	__IO void *RX_BUFF;
} st_usb_endpoint_t;

/* USB registers (like CMSIS but more usable) */
typedef struct {
	__IO uint32_t EPnR[8];
    __IO uint32_t RESERVED[8];
    __IO uint32_t CNTR;
    __IO uint32_t ISTR;
    __IO uint32_t FNR;
    __IO uint32_t DADDR;
    __IO uint32_t BTABLE;
} st_usb_t;

/* Btable EPn registers */
typedef struct {
	__IO uint32_t TX_ADDR;
    __IO uint32_t TX_COUNT;
    __IO uint32_t RX_ADDR;
    __IO uint32_t RX_COUNT;
} st_usb_epdata_t;

/* Btable all EP */
typedef struct {
	__IO st_usb_epdata_t EP[8];
} st_usb_btable_t;

/* Private define ------------------------------------------------------------*/

/* Top address of USB packed memory */
#define USB_PM_TOP 0x40

#define USB_BTABLE_BASE (USB_BASE + 0x00000050)

/* USB helrer defines */
#define USB_REGS   ((st_usb_t *)(USB_BASE))
#define USB_BTABLE ((st_usb_btable_t *)(USB_BTABLE_BASE))

/* Pre-calculated endpoint related BTABLE addresses */
#define USB_EP0_BTABLE (USB_BASE + 0x50 + 0x00)
#define USB_EP1_BTABLE (USB_BASE + 0x50 + 0x08)
#define USB_EP2_BTABLE (USB_BASE + 0x50 + 0x10)
#define USB_EP3_BTABLE (USB_BASE + 0x50 + 0x18)
#define USB_EP4_BTABLE (USB_BASE + 0x50 + 0x20)
#define USB_EP5_BTABLE (USB_BASE + 0x50 + 0x28)
#define USB_EP6_BTABLE (USB_BASE + 0x50 + 0x30)
#define USB_EP7_BTABLE (USB_BASE + 0x50 + 0x38)

/* Private macro -------------------------------------------------------------*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Private variables ---------------------------------------------------------*/

static bool st_usb_force_nak[8] = {0};

/* Private function prototypes -----------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

/**
 * Set the receive buffer size for a given USB endpoint.
 *
 * @param dev  USB device handle structure
 * @param ep   Index of endpoint to configure.
 * @param size Size in bytes of the RX buffer. Legal sizes : {2,4,6...62}; {64,96,128...992}.
 * @returns Actual size set
 */
static uint16_t _st_usb_set_ep_rx_bufsize(uint8_t ep, uint32_t size)
{
	uint16_t realsize;
	/*
	 * Writes USB_COUNTn_RX reg fields : bits <14:10> are NUM_BLOCK; bit 15 is BL_SIZE
	 * - When (size <= 62), BL_SIZE is set to 0 and NUM_BLOCK set to (size / 2).
	 * - When (size > 62), BL_SIZE is set to 1 and NUM_BLOCK=((size / 32) - 1).
	 *
	 * This algo rounds to the next largest legal buffer size, except 0. Examples:
	 *	size =>	BL_SIZE, NUM_BLOCK	=> Actual bufsize
	 *	0		0		0			??? "Not allowed" according to RM0091, RM0008
	 *	1		0		1			2
	 *	61		0		31			62
	 *	63		1		1			64
	 */
	if (size > 62) {
		/* Round up, div by 32 and sub 1 == (size + 31)/32 - 1 == (size-1)/32)*/
		size = ((size - 1) >> 5) & 0x1F;
		realsize = (size + 1) << 5;
		/* Set BL_SIZE bit (no macro for this) */
		size |= (1<<5);
	} else {
		/* round up and div by 2 */
		size = (size + 1) >> 1;
		realsize = size << 1;
	}

	/* write to the BL_SIZE and NUM_BLOCK fields */
    USB_BTABLE->EP[ep].RX_COUNT = size << 10;

	return realsize;
}

/**
 * Copy a data buffer to packet memory.
 *
 * @param buf Destination pointer for data buffer.
 * @param vPM Source pointer into packet memory.
 * @param len Number of bytes to copy.
 */
static void _st_usb_copy_buf_to_pm(volatile void *vPM, const void *buf, uint16_t len)
{
    /*
	 * This is a bytewise copy, so it always works, even on CM0(+)
	 * that don't support unaligned accesses.
	 */
	const uint8_t *lbuf = buf;//TODO maybe change param types
	volatile uint16_t *PM = vPM;//TODO maybe change param types
	uint32_t i;
	for (i = 0; i < len; i += 2) {
		*PM++ = (uint16_t)lbuf[i+1] << 8 | lbuf[i];
	}
}

/**
 * Copy a data buffer from packet memory.
 *
 * @param buf Destination pointer for data buffer.
 * @param vPM Source pointer into packet memory.
 * @param len Number of bytes to copy.
 */
static void _st_usb_copy_pm_to_buf(void *buf, const volatile void *vPM, uint16_t len)
{
	const volatile uint16_t *PM = vPM;
	uint8_t odd = len & 1;
	len >>= 1;

	if (((uintptr_t) buf) & 0x01) {
		for (; len; PM++, len--) {
			uint16_t value = *PM;
			*(uint8_t *) buf++ = value;
			*(uint8_t *) buf++ = value >> 8;
		}
	} else {
		for (; len; PM++, buf += 2, len--) {
			*(uint16_t *) buf = *PM;
		}
	}

	if (odd) {
		*(uint8_t *) buf = *(uint8_t *) PM;
	}
}

/**
 * Initialize HW
 */
static usb_device_t* st_usb_init(void)
{
    /* Enable USB clock */
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

	/* Force USB reset */
	USB_REGS->CNTR = USB_CNTR_FRES;
	for(uint32_t ctr = 0; ctr < 72000; ++ctr) __NOP(); // wait >1ms

    /* Reset registers */
    USB_REGS->CNTR   = 0;// TODO are this needed
    USB_REGS->BTABLE = 0;
	USB_REGS->DADDR  = 0;
    USB_REGS->ISTR   = 0;

    /* Enable RESET, SUSPEND, RESUME and CTR interrupts. */
    USB_REGS->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;

	/* Enable USB interrupts */
	// NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    return NULL;
}

/**
 * Set an address
 * 
 * @param dev  USB device handle structure
 * @param addr Device assigned address
 */
static void st_usb_set_address(usb_device_t *dev, uint8_t addr)
{
    (void) dev;
    USB_REGS->DADDR = (addr & USB_DADDR_ADD) | USB_DADDR_EF;
}

/**
 * Setup an endpoint
 * 
 * @param dev      USB device handle structure
 * @param ep       Full EP address including direction (e.g. 0x01 or 0x81)
 * @param type     Value for bmAttributes (USB_ENDPOINT_*)
 * @param max_size Endpoint max size
 * @param cb       Callback to execute
 */
static void st_usb_ep_setup(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb)
{
	uint8_t dir = ep & 0x80;
	ep &= 0x7F;

    USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPADDR_FIELD)) | ep;
    USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EP_T_FIELD)) | (type << USB_EP_T_FIELD_Pos);
	
	if (dir || ep == 0) {
		USB_BTABLE->EP[ep].TX_ADDR = dev->pm_top;

		if (cb) {
			dev->cb_endpoint[ep][USB_TRANSACTION_IN] = (void *) cb;
		}

		USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EPTX_DTOG1 | USB_EPTX_DTOG2);
		USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_NAK;
		dev->pm_top += max_size;
	}

	if (!dir) {
		uint16_t realsize = 0;
		USB_BTABLE->EP[ep].RX_ADDR = dev->pm_top;
		realsize = _st_usb_set_ep_rx_bufsize(ep, max_size);
		
        if (cb) {
			dev->cb_endpoint[ep][USB_TRANSACTION_OUT] = (void *) cb;
		}

		USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EPRX_DTOG1 | USB_EPRX_DTOG2);
		USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
		dev->pm_top += realsize;
	}
}

/**
 * Reset all endpoints
 * 
 * @param dev  USB device handle structure
 */
static void st_usb_ep_reset(usb_device_t *dev)
{
    (void) dev;
	uint8_t i;

	for (i = 1; i < 8; i++) {
		USB_REGS->EPnR[i] = (USB_REGS->EPnR[i] & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_DIS;
		USB_REGS->EPnR[i] = (USB_REGS->EPnR[i] & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_DIS;
	}

	dev->pm_top = USB_PM_TOP + (2 * dev->device_descr->bMaxPacketSize0);
}

/**
 * Get STALL status of an endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  Full EP address (with direction bit)
 * @return Non-zero if endpoint is stalled 
 */
static bool st_usb_ep_stall_get(usb_device_t *dev, uint8_t ep)
{
    (void) dev;
	if (ep & 0x80) {
		return (USB_REGS->EPnR[ep & 0x7F] & USB_EPTX_STAT) == USB_EP_TX_STALL;
	} else {
		return (USB_REGS->EPnR[ep] & USB_EPRX_STAT) == USB_EP_RX_STALL;
	}
}

/**
 * Set/clr STALL condition on an endpoint
 * 
 * @param dev   USB device handle structure
 * @param ep    Full EP address (with direction bit)
 * @param stall If 0, clear STALL, else set stall.
 */
static void st_usb_ep_stall_set(usb_device_t *dev, uint8_t ep, bool stall)
{
    (void) dev;
	if (ep == 0) {
		USB_REGS->EPnR[ep]
            = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | (stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);
	}

	if (ep & 0x80) {
		ep &= 0x7F;
		USB_REGS->EPnR[ep]
            = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | (stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);

		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EPTX_DTOG1 | USB_EPTX_DTOG2);
		}
	} else {
		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EPRX_DTOG1 | USB_EPRX_DTOG2);
		}

		USB_REGS->EPnR[ep]
            = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | (stall ? USB_EP_RX_STALL : USB_EP_RX_NAK);
	}
}

/**
 * Set an Out endpoint to NAK
 * 
 * @param dev USB device handle structure
 * @param ep  EP address
 * @param nak If non-zero, set NAK
 */
static void st_usb_ep_nak_set(usb_device_t *dev, uint8_t ep, bool nak)
{
    (void) dev;
	if (ep & 0x80) {
		return;
	}

	st_usb_force_nak[ep] = nak;

    USB_REGS->EPnR[ep]
        = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | (nak ? USB_EP_RX_NAK : USB_EP_RX_VALID);
}

/**
 * Write a packet to endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  EP address (direction is ignored)
 * @param buf pointer to user data to write
 * @param len # of bytes
 * @return Actual # of bytes read
 */
static uint16_t st_usb_ep_wr_packet(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len)
{
    (void) dev;
	ep &= 0x7F;

	if ((USB_REGS->EPnR[ep] & USB_EPTX_STAT) == USB_EP_TX_VALID) {
		return 0;
	}

	//TODO check valid ptr
	_st_usb_copy_buf_to_pm((__IO uint16_t *)(USB_BTABLE_BASE + USB_BTABLE->EP[ep].TX_ADDR * 2), buf, len);
	
	USB_BTABLE->EP[ep].TX_COUNT = len;
	USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_VALID;

	return len;
}

/**
 * Read a packet from endpoint
 * 
 * @param dev USB device handle structure
 * @param ep  EP address (direction is ignored)
 * @param buf user buffer that will receive data
 * @param len # of bytes
 * @return Actual # of bytes read
 */
static uint16_t st_usb_ep_rd_packet(usb_device_t *dev, uint8_t ep, void *buf, uint16_t len)
{
    (void) dev;
	if ((USB_REGS->EPnR[ep] & USB_EPRX_STAT) == USB_EP_RX_VALID) {
		return 0;
	}

	len = MIN(USB_BTABLE->EP[ep].RX_COUNT & 0x3FF, len);

	_st_usb_copy_pm_to_buf(buf, (__IO uint16_t *)(USB_BTABLE_BASE + USB_BTABLE->EP[ep].RX_ADDR * 2), len);
	
	USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EP_CTR_RX);

	if (!st_usb_force_nak[ep]) {
		USB_REGS->EPnR[ep] = (USB_REGS->EPnR[ep] & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
	}

	return len;
}

/**
 * Dispatch interrupts
 * 
 * @param dev USB device handle structure
 */
static void st_usb_poll(usb_device_t *dev)
{
	(void) dev;
	uint16_t istr = USB_REGS->ISTR;

	if (istr & USB_ISTR_RESET) {
		USB_REGS->ISTR &= ~USB_ISTR_RESET;
		dev->pm_top = USB_PM_TOP;
		usb_reset(dev);
		return;
	}

	if (istr & USB_ISTR_CTR) {
		uint8_t ep = istr & USB_ISTR_EP_ID;
		uint8_t type;

		if (istr & USB_ISTR_DIR) {
			if (USB_REGS->EPnR[ep] & USB_EP_SETUP) {
				type = USB_TRANSACTION_SETUP;
				st_usb_ep_rd_packet(dev, ep, &dev->control.req, 8);
			} else {
				type = USB_TRANSACTION_OUT;
			}
		} else {
			type = USB_TRANSACTION_IN;
			USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EP_CTR_TX);
		}

		if (dev->cb_endpoint[ep][type]) {
			dev->cb_endpoint[ep][type] (dev, ep);
		} else {
			USB_REGS->EPnR[ep] = USB_REGS->EPnR[ep] & (USB_EPREG_MASK | USB_EP_CTR_RX);
		}
	}

	if (istr & USB_ISTR_SUSP) {
		USB_REGS->ISTR &= ~USB_ISTR_SUSP;
		if (dev->cb_suspend) {
			dev->cb_suspend();
		}
	}

	if (istr & USB_ISTR_WKUP) {
		USB_REGS->ISTR &= ~USB_ISTR_WKUP;
		if (dev->cb_resume) {
			dev->cb_resume();
		}
	}

	if (istr & USB_ISTR_SOF) {
		USB_REGS->ISTR &= ~USB_ISTR_SOF;
		if (dev->cb_sof) {
			dev->cb_sof();
		}
	}

	if (dev->cb_sof) {
		USB_REGS->CNTR |= USB_CNTR_SOFM;
	} else {
		USB_REGS->CNTR &= ~USB_CNTR_SOFM;
	}
}

usb_driver_t st_usb_driver = {
    .init         = st_usb_init,
    .set_address  = st_usb_set_address,
    .ep_setup     = st_usb_ep_setup,
    .ep_reset     = st_usb_ep_reset,
    .ep_stall_get = st_usb_ep_stall_get,
    .ep_stall_set = st_usb_ep_stall_set,
    .ep_nak_set   = st_usb_ep_nak_set,
    .ep_wr_packet = st_usb_ep_wr_packet,
    .ep_rd_packet = st_usb_ep_rd_packet,
    .poll         = st_usb_poll,
    .disconnect   = NULL, // Not supported for now
};