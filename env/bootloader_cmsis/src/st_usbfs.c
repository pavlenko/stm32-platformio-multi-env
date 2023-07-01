/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stm32f103x6.h> // TODO HOW TO AVOID USE CMSIS DIRECTLY
#include <usbd.h>

/* Private typedef -----------------------------------------------------------*/

/* Endpoint related registers structure */
typedef struct {
	__IO uint16_t *REG;      // USB_BASE + n*4
	__IO uint16_t *TX_ADDR;  // USB_BASE + 0x50 + n*8 + 0
	__IO uint16_t *TX_COUNT; // USB_BASE + 0x50 + n*8 + 2
	__IO uint16_t *RX_ADDR;  // USB_BASE + 0x50 + n*8 + 4
	__IO uint16_t *RX_COUNT; // USB_BASE + 0x50 + n*8 + 6
	__IO void *EPnTX_BUFF;
	__IO void *EPnRX_BUFF;
} st_usb_endpoint_t;

/* Private define ------------------------------------------------------------*/

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

#define USB_IO(addr) ((__IO uint16_t *)(addr))

/* Private variables ---------------------------------------------------------*/

/* Helper array with define all endpoints structures */
static const st_usb_endpoint_t USB_EP[8] = {
	{USB_IO(USB_EP0R), USB_IO(USB_EP0_BTABLE), USB_IO(USB_EP0_BTABLE + 2), USB_IO(USB_EP0_BTABLE + 4), USB_IO(USB_EP0_BTABLE + 6)},
	{USB_IO(USB_EP2R), USB_IO(USB_EP2_BTABLE), USB_IO(USB_EP2_BTABLE + 2), USB_IO(USB_EP2_BTABLE + 4), USB_IO(USB_EP2_BTABLE + 6)},
	{USB_IO(USB_EP1R), USB_IO(USB_EP1_BTABLE), USB_IO(USB_EP1_BTABLE + 2), USB_IO(USB_EP1_BTABLE + 4), USB_IO(USB_EP1_BTABLE + 6)},
	{USB_IO(USB_EP3R), USB_IO(USB_EP3_BTABLE), USB_IO(USB_EP3_BTABLE + 2), USB_IO(USB_EP3_BTABLE + 4), USB_IO(USB_EP3_BTABLE + 6)},
	{USB_IO(USB_EP4R), USB_IO(USB_EP4_BTABLE), USB_IO(USB_EP4_BTABLE + 2), USB_IO(USB_EP4_BTABLE + 4), USB_IO(USB_EP4_BTABLE + 6)},
	{USB_IO(USB_EP5R), USB_IO(USB_EP5_BTABLE), USB_IO(USB_EP5_BTABLE + 2), USB_IO(USB_EP5_BTABLE + 4), USB_IO(USB_EP5_BTABLE + 6)},
	{USB_IO(USB_EP6R), USB_IO(USB_EP6_BTABLE), USB_IO(USB_EP6_BTABLE + 2), USB_IO(USB_EP6_BTABLE + 4), USB_IO(USB_EP6_BTABLE + 6)},
	{USB_IO(USB_EP7R), USB_IO(USB_EP7_BTABLE), USB_IO(USB_EP7_BTABLE + 2), USB_IO(USB_EP7_BTABLE + 4), USB_IO(USB_EP7_BTABLE + 6)},
};

/* Private function prototypes -----------------------------------------------*/
/* External function prototypes ----------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Function definitions ------------------------------------------------------*/

/**
 * Set the receive buffer size for a given USB endpoint.
 *
 * @param dev the usb device handle returned from @ref usbd_init
 * @param ep Index of endpoint to configure.
 * @param size Size in bytes of the RX buffer. Legal sizes : {2,4,6...62}; {64,96,128...992}.
 * @returns (uint16) Actual size set
 */
uint16_t _st_usb_set_ep_rx_bufsize(uint8_t ep, uint32_t size)
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
    *(USB_EP[ep].RX_COUNT) = size << 10;

	return realsize;
}

usb_device_t* st_usb_init(void)
{
    /* Enable USB clock */
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    /* Reset registers */
    USB->CNTR   = 0;// TODO are this needed
    USB->BTABLE = 0;
    USB->ISTR   = 0;

    /* Enable RESET, SUSPEND, RESUME and CTR interrupts. */
    USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;
    return NULL;
}

void st_usb_set_address(usb_device_t *dev, uint8_t addr)
{
    (void) dev;
    USB->DADDR = (addr & USB_DADDR_ADD) | USB_DADDR_EF;
}

void st_usb_ep_setup(usb_device_t *dev, uint8_t addr, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb)
{
	uint8_t dir = addr & 0x80;
	addr &= 0x7F;

    *(USB_EP[addr].REG) = (*(USB_EP[addr].REG) & (USB_EPREG_MASK & ~USB_EPADDR_FIELD)) | addr;
    *(USB_EP[addr].REG) = (*(USB_EP[addr].REG) & (USB_EPREG_MASK & ~USB_EP_T_FIELD)) | (type << USB_EP_T_FIELD_Pos);
	
	if (dir || addr == 0) {
		*(USB_EP[addr].TX_ADDR) = dev->pm_top;

		if (cb) {
			dev->cb_endpoint[addr][USB_TRANSACTION_IN] = (void *) cb;
		}

		*(USB_EP[addr].REG) = *(USB_EP[addr].REG) & (USB_EPREG_MASK | USB_EPTX_DTOG1 | USB_EPTX_DTOG2);
		*(USB_EP[addr].REG) = (*(USB_EP[addr].REG) & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_NAK;
		dev->pm_top += max_size;
	}

	if (!dir) {
		uint16_t realsize = 0;
		*(USB_EP[addr].RX_ADDR) = dev->pm_top;
		//TODO realsize = st_usbfs_set_ep_rx_bufsize(dev, address, max_size);
		
        if (cb) {
			dev->cb_endpoint[addr][USB_TRANSACTION_OUT] = (void *) cb;
		}

		*(USB_EP[addr].REG) = *(USB_EP[addr].REG) & (USB_EPREG_MASK | USB_EPRX_DTOG1 | USB_EPRX_DTOG2);
		*(USB_EP[addr].REG) = (*(USB_EP[addr].REG) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
		dev->pm_top += realsize;
	}
}

usb_driver_t st_usb_driver = {
    .init        = st_usb_init,
    .set_address = st_usb_set_address,
    .ep_setup    = st_usb_ep_setup,
};