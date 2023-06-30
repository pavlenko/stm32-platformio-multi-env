#include <stdint.h>
#include <stm32f103x6.h>

#include "usbd.h"
#include "usb_dfu.h"

dfu_functional_descriptor_t dfu_functional_descriptor = {
    .bLength         = sizeof(dfu_functional_descriptor_t),
    .bDescriptorType = DFU_DESCRIPTOR_TYPE_FUNCTIONAL,
    .bmAttributes    = DFU_ATTR_CAN_UPLOAD | DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_WILL_DETACH,
    .wDetachTimeout  = 0xFF,
    .wTransferSize   = 0x0400, // 0x0400 = 1024
    .bcdDFUVersion   = 0x011A,
};

usb_interface_descriptor_t dfu_interface_descriptor = {
    .bLength            = USB_DESCRIPTOR_SIZE_INTERFACE,
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber   = 0x00,
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0xFE, // DFU Class
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = 0x02,
    .iInterface         = 0x04,

    .extra_ptr = &dfu_functional_descriptor,
    .extra_len = sizeof(dfu_functional_descriptor),
};

usb_interface_t interfaces[] = {
    {.altsettings = &dfu_interface_descriptor, .num_altsetting = 1}
};

usb_config_descriptor_t usb_config_descriptor = {
    .bLength             = USB_DESCRIPTOR_SIZE_CONFIG,
    .bDescriptorType     = USB_DESCRIPTOR_TYPE_CONFIG,
    .wTotalLength        = 0x00,// calculated on fly
    .bNumInterfaces      = 0x01,
    .bConfigurationValue = 0x01,
    .iConfiguration      = 0x00,
    .bmAttributes        = USB_CONFIG_ATTR_DEFAULT | USB_CONFIG_ATTR_SELF_POWERED,
    .bMaxPower           = 0x32,// 0x32 = 50, milliampers

    .interfaces = interfaces,
};

usb_device_descriptor_t usb_device_descriptor = {
    .bLength            = USB_DESCRIPTOR_SIZE_DEVICE,
    .bDescriptorType    = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = 0x40,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0200,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01,
};

static const char *usb_strings[] = {
    "MASTER",
    "HexaPod Mark II Bootloader",
    "HexaPod Mark II",
    "@Internal Flash   /0x08000000/8*001Ka,56*001Kg",
};

uint8_t usb_buffer[1024];
uint8_t dfu_buffer[1024];

//TODO create init function
dfu_t dfu = {
    .buf   = dfu_buffer,
    .state = DFU_STATE_DFU_IDLE,
    .descr = &dfu_functional_descriptor,
};

//TODO create init function
usb_device_t usb_device = {
    .device_descr = &usb_device_descriptor,
    .config_descr = &usb_config_descriptor,
    .strings = usb_strings,
    .num_strings = 4,
    .cb_control = {
        //{.cb = dfu_cb_control, .ptr = &dfu},
        dfu_cb_control,
    },
};

void dfu_system_reset()
{}

void dfu_memory_unlock()
{}

void dfu_memory_lock()
{}

void dfu_memory_erase_page(uint32_t address)
{
    (void) address;
}

void dfu_memory_write_uint16(uint32_t address, uint16_t data)
{
    (void) address;
    (void) data;
}

int main(void) {
    usb_request_t req = {};
    while (1) {
        usb_control_request_dispatch(&usb_device, &req);
    }
}

/* Get register content. */
#define GET_REG(REG)		((uint16_t) *(REG))

/* Set register content. */
#define SET_REG(REG, VAL)	(*(REG) = (uint16_t)(VAL))

/* Clear register bit. */
#define CLR_REG_BIT(REG, BIT)	SET_REG((REG), (~(BIT)))

/* USB EP register */
#define MMIO32 (*(volatile uint32_t *)(addr))
#define USB_EP_REG(EP)		(&MMIO32(USB_DEV_FS_BASE) + (EP))

#define USB_EP_REG(EP)		(&(*(volatile uint32_t *)(USB_DEV_FS_BASE)) + (EP))
#define USB_EP_REG(EP)		(volatile uint32_t *)(USB_DEV_FS_BASE) + (EP)

#define USB_EP_RX_CTR		0x8000 /* Correct transfer RX */
#define USB_EP_RX_DTOG		0x4000 /* Data toggle RX */
#define USB_EP_RX_STAT		0x3000 /* Endpoint status for RX */

#define USB_EP_SETUP		0x0800 /* Setup transaction completed */
#define USB_EP_TYPE		0x0600 /* Endpoint type */
#define USB_EP_KIND		0x0100 /* Endpoint kind.
				* When set and type=bulk    -> double buffer
				* When set and type=control -> status out
				*/

#define USB_EP_TX_CTR		0x0080 /* Correct transfer TX */
#define USB_EP_TX_DTOG		0x0040 /* Data toggle TX */
#define USB_EP_TX_STAT		0x0030 /* Endpoint status for TX */
#define USB_EP_ADDR		0x000F /* Endpoint Address */
#define USB_SET_EP_ADDR(EP, ADDR) \
	SET_REG(USB_EP_REG(EP), ((GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK & (~USB_EP_ADDR))) | ADDR))

#define USB_SET_EP_TYPE(EP, TYPE) \
	SET_REG(USB_EP_REG(EP), (GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK & (~USB_EP_TYPE))) | TYPE)

#define USB_SET_EP_KIND(EP) \
	SET_REG(USB_EP_REG(EP), (GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK & (~USB_EP_KIND))) | USB_EP_KIND)

#define USB_CLR_EP_KIND(EP) \
	SET_REG(USB_EP_REG(EP), (GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK & (~USB_EP_KIND))))

#define USB_SET_EP_STAT_OUT(EP)	USB_SET_EP_KIND(EP)
#define USB_CLR_EP_STAT_OUT(EP)	USB_CLR_EP_KIND(EP)

#define USB_SET_EP_ADDR(EP, ADDR) \
	SET_REG(USB_EP_REG(EP), ((GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK & (~USB_EP_ADDR))) | ADDR))

/* Macros for clearing DTOG bits */
#define USB_CLR_EP_TX_DTOG(EP) \
	SET_REG(USB_EP_REG(EP), GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK | USB_EP_TX_DTOG))

#define USB_CLR_EP_RX_DTOG(EP) \
	SET_REG(USB_EP_REG(EP), GET_REG(USB_EP_REG(EP)) & (USB_EP_NTOGGLE_MSK | USB_EP_RX_DTOG))

#define USB_EP_ADDR_FIELD 0x000F // TODO leave
#define USB_EP_ADDR_MASK (USB_EPREG_MASK & ~USB_EPADDR_FIELD) // TODO leave

#define USB_EP_TYPE_BULK      0x0000
#define USB_EP_TYPE_CONTROL   0x0200 // 00 10 00 00
#define USB_EP_TYPE_ISO       0x0400 // 01 00 00 00
#define USB_EP_TYPE_INTERRUPT 0x0600 // 01 10 00 00

/* --- USB BTABLE registers ------------------------------------------------ */

#define USB_EP_TX_ADDR(ep) \
	((uint16_t *)(USB_PMA_BASE + (USB_GET_BTABLE + (ep) * 8 + 0) * 1))

#define USB_EP_TX_COUNT(ep) \
	((uint16_t *)(USB_PMA_BASE + (USB_GET_BTABLE + (ep) * 8 + 2) * 1))

#define USB_EP_RX_ADDR(ep) \
	((uint16_t *)(USB_PMA_BASE + (USB_GET_BTABLE + (ep) * 8 + 4) * 1))

#define USB_EP_RX_COUNT(ep) \
	((uint16_t *)(USB_PMA_BASE + (USB_GET_BTABLE + (ep) * 8 + 6) * 1))

/* --- USB BTABLE manipulators --------------------------------------------- */

#define USB_GET_EP_TX_BUFF(ep) \
	(USB_PMA_BASE + (uint8_t *)(USB_GET_EP_TX_ADDR(ep) * 1))

#define USB_GET_EP_RX_BUFF(ep) \
	(USB_PMA_BASE + (uint8_t *)(USB_GET_EP_RX_ADDR(ep) * 1))

#define USB_GET_EP_TX_ADDR(EP)		GET_REG(USB_EP_TX_ADDR(EP))
#define USB_GET_EP_TX_COUNT(EP)		GET_REG(USB_EP_TX_COUNT(EP))
#define USB_GET_EP_RX_ADDR(EP)		GET_REG(USB_EP_RX_ADDR(EP))
#define USB_GET_EP_RX_COUNT(EP)		GET_REG(USB_EP_RX_COUNT(EP))
#define USB_SET_EP_TX_ADDR(EP, ADDR)	SET_REG(USB_EP_TX_ADDR(EP), ADDR)
#define USB_SET_EP_TX_COUNT(EP, COUNT)	SET_REG(USB_EP_TX_COUNT(EP), COUNT)
#define USB_SET_EP_RX_ADDR(EP, ADDR)	SET_REG(USB_EP_RX_ADDR(EP), ADDR)
#define USB_SET_EP_RX_COUNT(EP, COUNT)	SET_REG(USB_EP_RX_COUNT(EP), COUNT)

/* Each endpoint buffer table addresses */
#define USB_EP0_BTABLE (USB_BASE + 0x50 + 0x00)
#define USB_EP1_BTABLE (USB_BASE + 0x50 + 0x08)
#define USB_EP2_BTABLE (USB_BASE + 0x50 + 0x10)
#define USB_EP3_BTABLE (USB_BASE + 0x50 + 0x18)
#define USB_EP4_BTABLE (USB_BASE + 0x50 + 0x20)
#define USB_EP5_BTABLE (USB_BASE + 0x50 + 0x28)
#define USB_EP6_BTABLE (USB_BASE + 0x50 + 0x30)
#define USB_EP7_BTABLE (USB_BASE + 0x50 + 0x38)

/* Endpoint related registers structure */
typedef struct {
	__IO uint16_t EPnR;        // USB_BASE + n*4
	__IO uint16_t EPnTX_ADDR;  // USB_BASE + 0x50 + n*8 + 0
	__IO uint16_t EPnTX_COUNT; // USB_BASE + 0x50 + n*8 + 2
	__IO uint16_t EPnRX_ADDR;  // USB_BASE + 0x50 + n*8 + 4
	__IO uint16_t EPnRX_COUNT; // USB_BASE + 0x50 + n*8 + 6
} usb_endpoint_t;

/* Helper array with define all endpoints structures */
const usb_endpoint_t USB_EP[1] = {
	{USB_EP0R, USB_EP0_BTABLE, USB_EP0_BTABLE + 2, USB_EP0_BTABLE + 4, USB_EP0_BTABLE + 6},
	{USB_EP1R, USB_EP1_BTABLE, USB_EP1_BTABLE + 2, USB_EP1_BTABLE + 4, USB_EP1_BTABLE + 6},
	{USB_EP2R, USB_EP2_BTABLE, USB_EP2_BTABLE + 2, USB_EP2_BTABLE + 4, USB_EP2_BTABLE + 6},
	{USB_EP3R, USB_EP3_BTABLE, USB_EP3_BTABLE + 2, USB_EP3_BTABLE + 4, USB_EP3_BTABLE + 6},
	{USB_EP4R, USB_EP4_BTABLE, USB_EP4_BTABLE + 2, USB_EP4_BTABLE + 4, USB_EP4_BTABLE + 6},
	{USB_EP5R, USB_EP5_BTABLE, USB_EP5_BTABLE + 2, USB_EP5_BTABLE + 4, USB_EP5_BTABLE + 6},
	{USB_EP6R, USB_EP6_BTABLE, USB_EP6_BTABLE + 2, USB_EP6_BTABLE + 4, USB_EP6_BTABLE + 6},
	{USB_EP7R, USB_EP7_BTABLE, USB_EP7_BTABLE + 2, USB_EP7_BTABLE + 4, USB_EP7_BTABLE + 6},
}

/**
 * Setup an endpoint
 * 
 * @param dev 
 * @param address  Full EP address including direction (e.g. 0x01 or 0x81)
 * @param type     Value for bmAttributes (USB_ENDPOINT_*)
 * @param max_size Endpoint max size
 * @param cb       Callback to execute
 */
void usb_ep_setup(usb_device_t *dev, uint8_t address, uint8_t type, uint16_t max_size, usb_cb_endpoint cb)
{
    //TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
    (void) dev;
    (void) address;
    (void) type;
    (void) max_size;
    (void) cb;
    /* Translate USB standard type codes to STM32. */
    const uint16_t typelookup[] = {
		[USB_ENDPOINT_TRANSFER_TYPE_CONTROL]     = USB_EP_TYPE_CONTROL,
		[USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS] = USB_EP_TYPE_ISO,
		[USB_ENDPOINT_TRANSFER_TYPE_BULK]        = USB_EP_TYPE_BULK,
		[USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT]   = USB_EP_TYPE_INTERRUPT,
	};
	uint8_t dir = address & 0x80;
	address &= 0x7F;

	/* Assign address. */
    // Get concrete endpoint register address
    __IO uint32_t USB_EPxR = *(&USB->EP0R + address);//TODO check

    // USB_SET_EP_ADDR(addr, addr);
    USB_EPxR = (USB_EPxR & (USB_EPREG_MASK & ~USB_EPADDR_FIELD)) | address;
    // USB_SET_EP_TYPE(addr, typelookup[type]);
    USB_EPxR = (USB_EPxR & (USB_EPREG_MASK & ~USB_EP_T_FIELD)) | (type << USB_EP_T_FIELD_Pos);

	if (dir || (address == 0)) {
		USB_SET_EP_TX_ADDR(address, dev->pm_top);// ->
        // SET_REG(USB_EP_TX_ADDR(address), dev->pm_top);//->
        // SET_REG((uint16_t *)(USB_PMA_BASE + (USB_GET_BTABLE + (address) * 8 + 0) * 1), dev->pm_top);
        // reg = (uint16_t *)(USB_PMAADDR + USB->BTABLE + address * 8)

		if (cb) {
			dev->user_callback_ctr[address][USB_TRANSACTION_IN] = (void *)cb;
		}
        USB_CLR_EP_TX_DTOG(address);
		USB_SET_EP_TX_STAT(address, USB_EP_TX_STAT_NAK);
		dev->pm_top += max_size;
	}

	if (!dir) {
		uint16_t realsize;
		USB_SET_EP_RX_ADDR(address, dev->pm_top);
		realsize = st_usbfs_set_ep_rx_bufsize(dev, address, max_size);
		if (cb) {
			dev->user_callback_ctr[address][USB_TRANSACTION_OUT] = (void *)cb;
		}
		USB_CLR_EP_RX_DTOG(address);
		USB_SET_EP_RX_STAT(address, USB_EP_RX_STAT_VALID);
		dev->pm_top += realsize;
	}
}

#define USB_EP_REG(EP)		(&MMIO32(USB_DEV_FS_BASE) + (EP))

/* Masking all toggle bits */
#define USB_EP_NTOGGLE_MSK	(USB_EP_RX_CTR | \
				 USB_EP_SETUP | \
				 USB_EP_TYPE | \
				 USB_EP_KIND | \
				 USB_EP_TX_CTR | \
				 USB_EP_ADDR)

/* All non toggle bits plus EP_RX toggle bits */
#define USB_EP_RX_STAT_TOG_MSK	(USB_EP_RX_STAT | USB_EP_NTOGGLE_MSK)
/* All non toggle bits plus EP_TX toggle bits */
#define USB_EP_TX_STAT_TOG_MSK	(USB_EP_TX_STAT | USB_EP_NTOGGLE_MSK)

#define TOG_SET_REG_BIT_MSK_AND_SET(REG, MSK, BIT, EXTRA_BITS)		\
do {									\
	register uint16_t toggle_mask = GET_REG(REG) & (MSK);		\
	toggle_mask ^= BIT;						\
	SET_REG((REG), toggle_mask | (EXTRA_BITS));			\
} while (0)

#define USB_SET_EP_RX_STAT(EP, STAT) \
	TOG_SET_REG_BIT_MSK_AND_SET(USB_EP_REG(EP), USB_EP_RX_STAT_TOG_MSK, STAT, USB_EP_RX_CTR | USB_EP_TX_CTR)

#define USB_SET_EP_TX_STAT(EP, STAT) \
	TOG_SET_REG_BIT_MSK_AND_SET(USB_EP_REG(EP), USB_EP_TX_STAT_TOG_MSK, STAT, USB_EP_RX_CTR | USB_EP_TX_CTR)

void usb_ep_reset(usb_device_t *dev)
{
    (void) dev;

    //TODO check doc
    USB->EP0R = (USB->EP0R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP1R = (USB->EP1R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP2R = (USB->EP2R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP3R = (USB->EP3R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP4R = (USB->EP4R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP5R = (USB->EP5R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP6R = (USB->EP6R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;
    USB->EP7R = (USB->EP7R & ~(USB_EPTX_STAT | USB_EPRX_STAT)) | USB_EP_CTR_RX | USB_EP_CTR_TX;

	/* Reset all endpoints. */
	for (i = 1; i < 8; i++) {
        //TODO
		//USB_SET_EP_TX_STAT(i, 0);
		//USB_SET_EP_RX_STAT(i, 0);
	}

	//dev->pm_top = USBD_PM_TOP + (2 * dev->desc->bMaxPacketSize0);
}

uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
    return 0;
    if ((*USB_EP_REG(addr) & USB_EP_RX_STAT) == USB_EP_RX_STAT_VALID) {
		return 0;
	}

	len = MIN(USB_GET_EP_RX_COUNT(addr) & 0x3ff, len);
	st_usbfs_copy_from_pm(buf, USB_GET_EP_RX_BUFF(addr), len);
	USB_CLR_EP_RX_CTR(addr);

	if (!st_usbfs_force_nak[addr]) {
		USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_VALID);
	}

	return len;
}

void usb_ep_write_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
    addr &= 0x7F;

	if ((*USB_EP_REG(addr) & USB_EP_TX_STAT) == USB_EP_TX_STAT_VALID) {
		return 0;
	}

	st_usbfs_copy_to_pm(USB_GET_EP_TX_BUFF(addr), buf, len);
	USB_SET_EP_TX_COUNT(addr, len);
	USB_SET_EP_TX_STAT(addr, USB_EP_TX_STAT_VALID);

	return len;
}

void usb_ep_stall_set(usb_device_t *dev, uint8_t address, uint8_t stall)
{
    (void) dev;
	if (addr == 0) {
		USB_SET_EP_TX_STAT(addr, stall ? USB_EP_TX_STAT_STALL :
				   USB_EP_TX_STAT_NAK);
	}

	if (addr & 0x80) {
		addr &= 0x7F;

		USB_SET_EP_TX_STAT(addr, stall ? USB_EP_TX_STAT_STALL :
				   USB_EP_TX_STAT_NAK);

		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			USB_CLR_EP_TX_DTOG(addr);
		}
	} else {
		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			USB_CLR_EP_RX_DTOG(addr);
		}

		USB_SET_EP_RX_STAT(addr, stall ? USB_EP_RX_STAT_STALL :
				   USB_EP_RX_STAT_VALID);
	}
}

uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t address)
{
    (void) dev;
    (void) address;
    return 0;
    if (addr & 0x80) {
		if ((*USB_EP_REG(addr & 0x7F) & USB_EP_TX_STAT) ==
		    USB_EP_TX_STAT_STALL) {
			return 1;
		}
	} else {
		if ((*USB_EP_REG(addr) & USB_EP_RX_STAT) ==
		    USB_EP_RX_STAT_STALL) {
			return 1;
		}
	}
}

void usb_ep_nak_set(usb_device_t *dev, uint8_t address, uint8_t nak)
{
    (void) dev;
    /* It does not make sense to force NAK on IN endpoints. */
	if (addr & 0x80) {
		return;
	}

	st_usbfs_force_nak[addr] = nak;

	if (nak) {
		USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_NAK);
	} else {
		USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_VALID);
	}
}

void st_usbfs_poll(usbd_device *dev)
{
	uint16_t istr = *USB_ISTR_REG;

	if (istr & USB_ISTR_RESET) {
		USB_CLR_ISTR_RESET();
		dev->pm_top = USBD_PM_TOP;
		_usbd_reset(dev);
		return;
	}

	if (istr & USB_ISTR_CTR) {
		uint8_t ep = istr & USB_ISTR_EP_ID;
		uint8_t type;

		if (istr & USB_ISTR_DIR) {
			/* OUT or SETUP? */
			if (*USB_EP_REG(ep) & USB_EP_SETUP) {
				type = USB_TRANSACTION_SETUP;
				st_usbfs_ep_read_packet(dev, ep, &dev->control_state.req, 8);
			} else {
				type = USB_TRANSACTION_OUT;
			}
		} else {
			type = USB_TRANSACTION_IN;
			USB_CLR_EP_TX_CTR(ep);
		}

		if (dev->user_callback_ctr[ep][type]) {
			dev->user_callback_ctr[ep][type] (dev, ep);
		} else {
			USB_CLR_EP_RX_CTR(ep);
		}
	}

	if (istr & USB_ISTR_SUSP) {
		USB_CLR_ISTR_SUSP();
		if (dev->user_callback_suspend) {
			dev->user_callback_suspend();
		}
	}

	if (istr & USB_ISTR_WKUP) {
		USB_CLR_ISTR_WKUP();
		if (dev->user_callback_resume) {
			dev->user_callback_resume();
		}
	}

	if (istr & USB_ISTR_SOF) {
		USB_CLR_ISTR_SOF();
		if (dev->user_callback_sof) {
			dev->user_callback_sof();
		}
	}

	if (dev->user_callback_sof) {
		*USB_CNTR_REG |= USB_CNTR_SOFM;
	} else {
		*USB_CNTR_REG &= ~USB_CNTR_SOFM;
	}
}
