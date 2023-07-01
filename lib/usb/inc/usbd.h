#ifndef __USBD_H__
#define __USBD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
//TODO do not include descriptors if not need
#include "usb_descr.h"

/* Exported types ------------------------------------------------------------*/

/* Operation result types */
typedef enum usb_result_e {
    USB_RESULT_NOTSUPP = 0,
    USB_RESULT_HANDLED = 1,
    USB_RESULT_NEXT_CALLBACK = 2,
} usb_result_t;

/* USB control states */
typedef enum usb_state_e {
    USB_STATE_IDLE,
    USB_STATE_STALLED,
    USB_STATE_DATA_IN,
    USB_STATE_LAST_DATA_IN,
    USB_STATE_DATA_OUT,
    USB_STATE_LAST_DATA_OUT,
    USB_STATE_STATUS_IN,
    USB_STATE_STATUS_OUT,
} usb_state_t;

/* USB transactiontype, used in endpoints callback map */
typedef enum usbd_transaction_e {
	USB_TRANSACTION_IN,
	USB_TRANSACTION_OUT,
	USB_TRANSACTION_SETUP,
} usbd_transaction_t;

typedef struct usb_device_s usb_device_t;

/**
 * Data request callback
 * 
 * @see usb_control_in
 * @see usb_control_out
 * @param dev USB device handle structure
 * @param req USB data request
 * @param ptr Additional data poiter
 */
typedef void (*usb_cb_control_complete_t)(usb_device_t *dev, usb_request_t *req, void *ptr);

/**
 * Control request callback
 * 
 * @see usb_control_request_dispatch
 * @param dev USB device handle structure
 * @param req USB data request
 * @param buf Pointer to data buffer
 * @param len Size of buffer
 * @param ptr Additional data poiter
 */
typedef usb_result_t (*usb_cb_control_t)(
    usb_device_t *dev,
    usb_request_t *req,
    uint8_t **buf,
    uint16_t *len,
    usb_cb_control_complete_t *complete_cb,
    void *ptr
);

/**
 * Set cinfiguration request callback
 * 
 * @see usb_set_configuration
 * @param dev USB device handle structure
 * @param wValue USB SET_CONFIGURATION request wValue field
 */
typedef void (*usb_cb_set_configuration_t)(usb_device_t *dev, uint16_t wValue);

/**
 * Set interface request callback
 * 
 * @see usb_set_interface
 * @param dev USB device handle structure
 * @param wIndex USB SET_INTERFACE request wIndex field
 * @param wValue USB SET_INTERFACE request wValue field
 */
typedef void (*usb_cb_set_interface_t)(usb_device_t *dev, uint16_t wIndex, uint16_t wValue);

/**
 * Endpoint callback
 * 
 * @param dev USB device handle structure
 * @param ep USB endpoint number
 */
typedef void (*usb_cb_endpoint_t)(usb_device_t *dev, uint8_t ep);

/* Exported constants --------------------------------------------------------*/
#ifndef USB_MAX_CB_CONTROL
#define USB_MAX_CB_CONTROL 4
#endif

#ifndef USB_MAX_CB_SET_CONFIGURATION
#define USB_MAX_CB_SET_CONFIGURATION 4
#endif

/* Private types -------------------------------------------------------------*/

//TODO move to private, maybe
typedef struct usb_control_s {
    usb_state_t state;
    usb_request_t req __attribute__((aligned(4)));
    uint8_t *ctrl_buf;
    uint16_t ctrl_len;
    usb_cb_control_complete_t complete_cb;
    void *complete_arg;//TODO are this needed???
    bool needs_zlp;
} usb_control_t;

/* Hardware specific driver */
typedef struct usb_driver_s {
    /**
     * Initialize HW
     */
    usb_device_t *(*init)(void);

    /**
     * Set an address
     * 
     * @param dev  USB device handle structure
     * @param addr Device assigned address
     */
	void (*set_address)(usb_device_t *dev, uint8_t addr);

    /**
     * Setup an endpoint
     * 
     * @param dev      USB device handle structure
     * @param ep       Full EP address including direction (e.g. 0x01 or 0x81)
     * @param type     Value for bmAttributes (USB_ENDPOINT_*)
     * @param max_size Endpoint max size
     * @param cb       Callback to execute
     */
	void (*ep_setup)(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb);

    /**
     * Reset all endpoints
     * 
     * @param dev  USB device handle structure
     */
	void (*ep_reset)(usb_device_t *dev);

    /**
     * Get STALL status of an endpoint
     * 
     * @param dev USB device handle structure
     * @param ep  Full EP address (with direction bit)
     * @return Non-zero if endpoint is stalled 
     */
    bool (*ep_stall_get)(usb_device_t *dev, uint8_t ep);

    /**
     * Set/clr STALL condition on an endpoint
     * 
     * @param dev   USB device handle structure
     * @param ep    Full EP address (with direction bit)
     * @param stall If 0, clear STALL, else set stall.
     */
	void (*ep_stall_set)(usb_device_t *dev, uint8_t ep, bool stall);

    /**
     * Set an Out endpoint to NAK
     * 
     * @param dev USB device handle structure
     * @param ep  EP address
     * @param nak If non-zero, set NAK
     */
	void (*ep_nak_set)(usb_device_t *dev, uint8_t ep, bool nak);

    /**
     * Write a packet to endpoint
     * 
     * @param dev USB device handle structure
     * @param ep  EP address (direction is ignored)
     * @param buf pointer to user data to write
     * @param len # of bytes
     * @return Actual # of bytes read
     */
	uint16_t (*ep_wr_packet)(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len);

    /**
     * Read a packet from endpoint
     * 
     * @param dev USB device handle structure
     * @param ep  EP address (direction is ignored)
     * @param buf user buffer that will receive data
     * @param len # of bytes
     * @return Actual # of bytes read
     */
	uint16_t (*ep_rd_packet)(usb_device_t *dev, uint8_t ep, void *buf, uint16_t len);

    /**
     * Dispatch interrupts
     * 
     * @param dev USB device handle structure
     */
	void (*poll)(usb_device_t *dev);

    /**
     * Disconnect device handler
     * 
     * @param dev USB device handle structure
     * @param disconnected Disconnected flag
     */
	void (*disconnect)(usb_device_t *dev, bool disconnected);
} usb_driver_t;

//TODO move to private, maybe
typedef struct usb_device_s {
    const usb_driver_t *driver;
    const usb_device_descriptor_t *device_descr;
    const usb_config_descriptor_t *config_descr;
    //const usb_string_descriptor_t *string_descr; //TODO try descriptors
    const char * const *strings;
    const uint8_t num_strings;

    uint8_t current_address;
    uint8_t current_config;

    uint16_t pm_top;    /**< Top of allocated endpoint buffer memory */

    uint8_t *ctrl_buf;  /**< Internal buffer used for control transfers */
    uint16_t ctrl_len;

    usb_control_t control;

    usb_cb_endpoint_t cb_endpoint[8][3];
    void (*cb_reset)(void);
	void (*cb_suspend)(void);
	void (*cb_resume)(void);
	void (*cb_sof)(void);
    usb_cb_control_t cb_control[USB_MAX_CB_CONTROL];
    usb_cb_set_configuration_t cb_set_configuration[USB_MAX_CB_SET_CONFIGURATION];
} usb_device_t;


/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void usbd_init(usb_device_t *dev);

/**
 * Register a custom set configuration request callback
 * 
 * @param dev USB device handle structure
 * @param cb  Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_set_configuration(usb_device_t *dev, usb_cb_set_configuration_t cb);

/**
 * Register a custom set interface request callback
 * 
 * @param dev USB device handle structure
 * @param cb  Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_set_interface(usb_device_t *dev, usb_cb_set_interface_t cb);

/**
 * Register a custom control request callback
 * 
 * @param dev  USB device handle structure
 * @param type Handled request type
 * @param mask Mask to match type
 * @param cb   Callback to execute
 * @return true(1) on success, else false(0) on failure
 */
bool usb_register_cb_control(usb_device_t *dev, uint8_t type, uint8_t mask, usb_cb_control_t cb);

void usb_reset(usb_device_t *dev);//TODO private

#ifdef __cplusplus
}
#endif

#endif // __USBD_H__