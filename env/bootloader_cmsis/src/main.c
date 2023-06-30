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

void st_usbfs_poll(usb_device_t *dev);

int main(void) {
    // usb_request_t req = {};
    while (1) {
        //usb_control_request_dispatch(&usb_device, &req);
		st_usbfs_poll(&usb_device);
    }
}

/* USB_PMA???; USB_PMAADDR - USB_BASE  = 0x6000 - 0x5C00 = ?*/
#define USB_PM_TOP 0x40

/* Each endpoint buffer table addresses */
#define USB_EP0_BTABLE (USB_BASE + 0x50 + 0x00)
#define USB_EP1_BTABLE (USB_BASE + 0x50 + 0x08)
#define USB_EP2_BTABLE (USB_BASE + 0x50 + 0x10)
#define USB_EP3_BTABLE (USB_BASE + 0x50 + 0x18)
#define USB_EP4_BTABLE (USB_BASE + 0x50 + 0x20)
#define USB_EP5_BTABLE (USB_BASE + 0x50 + 0x28)
#define USB_EP6_BTABLE (USB_BASE + 0x50 + 0x30)
#define USB_EP7_BTABLE (USB_BASE + 0x50 + 0x38)

/* Pointer to memory address cast helper */
#define USB_IO(addr) ((__IO uint16_t *)(addr))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* Endpoint related registers structure */
typedef struct {
	__IO uint16_t *EPnR;        // USB_BASE + n*4
	__IO uint16_t *EPnTX_ADDR;  // USB_BASE + 0x50 + n*8 + 0
	__IO uint16_t *EPnTX_COUNT; // USB_BASE + 0x50 + n*8 + 2
	__IO uint16_t *EPnRX_ADDR;  // USB_BASE + 0x50 + n*8 + 4
	__IO uint16_t *EPnRX_COUNT; // USB_BASE + 0x50 + n*8 + 6
} usb_endpoint_t;

/* Helper array with define all endpoints structures */
const usb_endpoint_t USB_EP[8] = {
	{USB_IO(USB_EP0R), USB_IO(USB_EP0_BTABLE), USB_IO(USB_EP0_BTABLE + 2), USB_IO(USB_EP0_BTABLE + 4), USB_IO(USB_EP0_BTABLE + 6)},
	{USB_IO(USB_EP2R), USB_IO(USB_EP2_BTABLE), USB_IO(USB_EP2_BTABLE + 2), USB_IO(USB_EP2_BTABLE + 4), USB_IO(USB_EP2_BTABLE + 6)},
	{USB_IO(USB_EP1R), USB_IO(USB_EP1_BTABLE), USB_IO(USB_EP1_BTABLE + 2), USB_IO(USB_EP1_BTABLE + 4), USB_IO(USB_EP1_BTABLE + 6)},
	{USB_IO(USB_EP3R), USB_IO(USB_EP3_BTABLE), USB_IO(USB_EP3_BTABLE + 2), USB_IO(USB_EP3_BTABLE + 4), USB_IO(USB_EP3_BTABLE + 6)},
	{USB_IO(USB_EP4R), USB_IO(USB_EP4_BTABLE), USB_IO(USB_EP4_BTABLE + 2), USB_IO(USB_EP4_BTABLE + 4), USB_IO(USB_EP4_BTABLE + 6)},
	{USB_IO(USB_EP5R), USB_IO(USB_EP5_BTABLE), USB_IO(USB_EP5_BTABLE + 2), USB_IO(USB_EP5_BTABLE + 4), USB_IO(USB_EP5_BTABLE + 6)},
	{USB_IO(USB_EP6R), USB_IO(USB_EP6_BTABLE), USB_IO(USB_EP6_BTABLE + 2), USB_IO(USB_EP6_BTABLE + 4), USB_IO(USB_EP6_BTABLE + 6)},
	{USB_IO(USB_EP7R), USB_IO(USB_EP7_BTABLE), USB_IO(USB_EP7_BTABLE + 2), USB_IO(USB_EP7_BTABLE + 4), USB_IO(USB_EP7_BTABLE + 6)},
};

uint8_t st_usbfs_force_nak[8] = {0};

void usb_set_address(usb_device_t *dev, uint8_t address)
{
	/* Set device address and enable. */
	// SET_REG(USB_DADDR_REG, (addr & USB_DADDR_ADDR) | USB_DADDR_EF);
	USB->DADDR = (address & USB_DADDR_ADD) | USB_DADDR_EF;
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
    (void) dev;
    (void) address;
    (void) type;
    (void) max_size;
    (void) cb;
    /* Translate USB standard type codes to STM32. */
    // const uint16_t typelookup[] = {
	// 	[USB_ENDPOINT_TRANSFER_TYPE_CONTROL]     = USB_EP_TYPE_CONTROL,
	// 	[USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS] = USB_EP_TYPE_ISO,
	// 	[USB_ENDPOINT_TRANSFER_TYPE_BULK]        = USB_EP_TYPE_BULK,
	// 	[USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT]   = USB_EP_TYPE_INTERRUPT,
	// };
	uint8_t dir = address & 0x80;
	address &= 0x7F;

	/* Assign address. */
    // USB_SET_EP_ADDR(addr, addr);
    *(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPADDR_FIELD)) | address;
	
	// USB_SET_EP_TYPE(addr, typelookup[type]);
    *(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EP_T_FIELD)) | (type << USB_EP_T_FIELD_Pos);
	
	if (dir || (address == 0)) {
		// USB_SET_EP_TX_ADDR(address, dev->pm_top);
		*(USB_EP[address].EPnTX_ADDR) = dev->pm_top;

		if (cb) {
			//dev->user_callback_ctr[address][USB_TRANSACTION_IN] = (void *)cb;
		}
        // USB_CLR_EP_TX_DTOG(address);
		*(USB_EP[address].EPnR) = *(USB_EP[address].EPnR) & (USB_EPREG_MASK | USB_EPTX_DTOG1 | USB_EPTX_DTOG2);
		// USB_SET_EP_TX_STAT(address, USB_EP_TX_STAT_NAK);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_NAK;
		dev->pm_top += max_size;
	}

	if (!dir) {
		uint16_t realsize = 0;
		// USB_SET_EP_RX_ADDR(address, dev->pm_top);
		*(USB_EP[address].EPnRX_ADDR) = dev->pm_top;
		//realsize = st_usbfs_set_ep_rx_bufsize(dev, address, max_size);
		if (cb) {
			//dev->user_callback_ctr[address][USB_TRANSACTION_OUT] = (void *)cb;
		}
		// USB_CLR_EP_RX_DTOG(address);
		*(USB_EP[address].EPnR) = *(USB_EP[address].EPnR) & (USB_EPREG_MASK | USB_EPRX_DTOG1 | USB_EPRX_DTOG2);
		// USB_SET_EP_RX_STAT(address, USB_EP_RX_STAT_VALID);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
		dev->pm_top += realsize;
	}
}

void usb_ep_reset(usb_device_t *dev)
{
    (void) dev;
	uint8_t i;

	/* Reset all endpoints. */
	for (i = 1; i < 8; i++) {//DO not loop, direct call all endpoints
		//USB_SET_EP_TX_STAT(i, USB_EP_TX_STAT_DISABLED);
		*(USB_EP[i].EPnR) = (*(USB_EP[i].EPnR) & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_DIS;
		//USB_SET_EP_RX_STAT(i, USB_EP_RX_STAT_DISABLED);
		*(USB_EP[i].EPnR) = (*(USB_EP[i].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_DIS;
	}
	dev->pm_top = USB_PM_TOP + (2 * dev->device_descr->bMaxPacketSize0);
}

uint16_t usb_ep_read_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
	// if ((*USB_EP_REG(addr) & USB_EP_RX_STAT) == USB_EP_RX_STAT_VALID) {
	// 	return 0;
	// }
	if ((*(USB_EP[address].EPnR) & USB_EPRX_STAT) == USB_EP_RX_VALID) {
		return 0;
	}

	// len = MIN(USB_GET_EP_RX_COUNT(addr) & 0x3ff, len);
	len = MIN(*(USB_EP[address].EPnRX_COUNT) & 0x3FF, len);

	// st_usbfs_copy_from_pm(buf, USB_GET_EP_RX_BUFF(addr), len);
	// USB_CLR_EP_RX_CTR(addr);
	*(USB_EP[address].EPnR) = *(USB_EP[address].EPnR) & (USB_EPREG_MASK | USB_EP_CTR_RX);

	if (!st_usbfs_force_nak[address]) {
		// USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_VALID);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
	}

	return len;
}

uint16_t usb_ep_write_packet(usb_device_t *dev, uint8_t address, const void *buf, uint16_t len)
{
    (void) dev;
    (void) address;
    (void) buf;
    (void) len;
	address &= 0x7F;

	// if ((*USB_EP_REG(addr) & USB_EP_TX_STAT) == USB_EP_TX_STAT_VALID) {
	// 	return 0;
	// }
	if ((*(USB_EP[address].EPnR) & USB_EPTX_STAT) == USB_EP_TX_VALID) {
		return 0;
	}

	// st_usbfs_copy_to_pm(USB_GET_EP_TX_BUFF(addr), buf, len);
	// USB_SET_EP_TX_COUNT(addr, len);
	*(USB_EP[address].EPnTX_COUNT) = len;
	// USB_SET_EP_TX_STAT(addr, USB_EP_TX_STAT_VALID);
	*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPTX_STAT)) | USB_EP_TX_VALID;

	return len;
}

void usb_ep_stall_set(usb_device_t *dev, uint8_t address, uint8_t stall)
{
    (void) dev;
	if (address == 0) {
		// USB_SET_EP_TX_STAT(addr, stall ? USB_EP_TX_STAT_STALL : USB_EP_TX_STAT_NAK);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPTX_STAT))
		                        | (stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);
	}

	if (address & 0x80) {
		address &= 0x7F;
		// USB_SET_EP_TX_STAT(addr, stall ? USB_EP_TX_STAT_STALL : USB_EP_TX_STAT_NAK);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPTX_STAT))
		                        | (stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);

		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			// USB_CLR_EP_TX_DTOG(addr);
			*(USB_EP[address].EPnR) = *(USB_EP[address].EPnR) & (USB_EPREG_MASK | USB_EPTX_DTOG1 | USB_EPTX_DTOG2);
		}
	} else {
		/* Reset to DATA0 if clearing stall condition. */
		if (!stall) {
			// USB_CLR_EP_RX_DTOG(addr);
			*(USB_EP[address].EPnR) = *(USB_EP[address].EPnR) & (USB_EPREG_MASK | USB_EPRX_DTOG1 | USB_EPRX_DTOG2);
		}

		// USB_SET_EP_RX_STAT(addr, stall ? USB_EP_RX_STAT_STALL : USB_EP_RX_STAT_VALID);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT))
		                        | (stall ? USB_EP_RX_STALL : USB_EP_RX_NAK);
	}
}

uint8_t usb_ep_stall_get(usb_device_t *dev, uint8_t address)
{
    (void) dev;
    (void) address;
	if (address & 0x80) {
		return (*(USB_EP[address & 0x7F].EPnR) & USB_EPTX_STAT) == USB_EP_TX_STALL;
		// if ((*USB_EP_REG(addr & 0x7F) & USB_EP_TX_STAT) == USB_EP_TX_STAT_STALL) {
		// 	return 1;
		// }
	} else {
		return (*(USB_EP[address].EPnR) & USB_EPRX_STAT) == USB_EP_RX_STALL;
		// if ((*USB_EP_REG(addr) & USB_EP_RX_STAT) == USB_EP_RX_STAT_STALL) {
		// 	return 1;
		// }
	}
    // return 0;
}

void usb_ep_nak_set(usb_device_t *dev, uint8_t address, uint8_t nak)
{
    (void) dev;
	if (address & 0x80) {
		return;
	}

	st_usbfs_force_nak[address] = nak;

	if (nak) {
		// USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_NAK);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_NAK;
	} else {
		// USB_SET_EP_RX_STAT(addr, USB_EP_RX_STAT_VALID);
		*(USB_EP[address].EPnR) = (*(USB_EP[address].EPnR) & (USB_EPREG_MASK & ~USB_EPRX_STAT)) | USB_EP_RX_VALID;
	}
}

void st_usbfs_poll(usb_device_t *dev)
{
	(void) dev;
	uint16_t istr = USB->ISTR;//*USB_ISTR_REG;

	if (istr & USB_ISTR_RESET) {
		// USB_CLR_ISTR_RESET();
		USB->ISTR &= ~USB_ISTR_RESET;
		dev->pm_top = USB_PM_TOP;
		usb_reset(dev);
		return;
	}

	if (istr & USB_ISTR_CTR) {
		uint8_t ep = istr & USB_ISTR_EP_ID;
		// uint8_t type;

		if (istr & USB_ISTR_DIR) {
			/* OUT or SETUP? */
			if (*(USB_EP[ep].EPnR) & USB_EP_SETUP) {
				// type = USB_TRANSACTION_SETUP;
				usb_ep_read_packet(dev, ep, &dev->control.req, 8);
				usb_control_setup(dev, ep);
			} else {
				// type = USB_TRANSACTION_OUT;
				usb_control_out(dev, ep);
			}
		} else {
			// type = USB_TRANSACTION_IN;
			// USB_CLR_EP_TX_CTR(ep);
			*(USB_EP[ep].EPnR) = *(USB_EP[ep].EPnR) & (USB_EPREG_MASK | USB_EP_CTR_TX);
			usb_control_in(dev, ep);
		}

		//TODO need callback map for endpoints
		// if (dev->user_callback_ctr[ep][type]) {
			// dev->user_callback_ctr[ep][type] (dev, ep);
		// } else {
			// USB_CLR_EP_RX_CTR(ep);
			// *(USB_EP[ep].EPnR) = *(USB_EP[ep].EPnR) & (USB_EPREG_MASK | USB_EP_CTR_RX);
		// }
	}

	if (istr & USB_ISTR_SUSP) {
		// USB_CLR_ISTR_SUSP();
		USB->ISTR &= ~USB_ISTR_SUSP;
		// if (dev->user_callback_suspend) {
		// 	dev->user_callback_suspend();
		// }
	}

	if (istr & USB_ISTR_WKUP) {
		// USB_CLR_ISTR_WKUP();
		USB->ISTR &= ~USB_ISTR_WKUP;
		// if (dev->user_callback_resume) {
		// 	dev->user_callback_resume();
		// }
	}

	if (istr & USB_ISTR_SOF) {
		// USB_CLR_ISTR_SOF();
		USB->ISTR &= ~USB_ISTR_SOF;
		// if (dev->user_callback_sof) {
		// 	dev->user_callback_sof();
		// }
	}

	// if (dev->user_callback_sof) {
		// *USB_CNTR_REG |= USB_CNTR_SOFM;
	// } else {
		// *USB_CNTR_REG &= ~USB_CNTR_SOFM;
		USB->CNTR &= ~USB_CNTR_SOFM;
	// }
}
