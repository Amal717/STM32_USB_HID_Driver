#ifndef USB_STANDARDS_H_
#define USB_STANDARDS_H_

#include <stdint.h>

typedef enum {
    USB_ENDPOINT_TYPE_CONTROL,
    USB_ENDPOINT_TYPE_ISOCHRONOUS,
    USB_ENDPOINT_TYPE_BULK,
    USB_ENDPOINT_TYPE_INTERRUPT
} usb_endpoint_t;

typedef struct {
    void (*on_usb_reset_received)(void);
    void (*on_setup_data_received)(uint8_t endpoint_num, uint16_t bcnt);
    void (*on_out_data_received)(uint8_t endpoint_num, uint16_t bcnt);
    void (*on_in_transfer_completed)(uint8_t endpoint_num);
    void (*on_out_transfer_completed)(uint8_t endpoint_num);
    void (*on_usb_polled)(void);
} usb_events_t;

typedef enum {
    USB_DEVICE_STATE_DEFAULT,
    USB_DEVICE_STATE_ADDRESSED,
    USB_DEVICE_STATE_CONFIGURED,
    USB_DEVICE_STATE_SUSPENDED
} usb_device_state_t;

typedef enum {
    USB_CONTROL_STAGE_SETUP, // can also be called USB_CONTROL_STAGE_IDLE
    USB_CONTROL_STAGE_DATA_OUT,
    USB_CONTROL_STAGE_DATA_IN,
    USB_CONTROL_STAGE_DATA_IN_IDLE,
    USB_CONTROL_STAGE_DATA_IN_ZERO,
    USB_CONTROL_STAGE_STATUS_OUT,
    USB_CONTROL_STAGE_STATUS_IN
}usb_control_transfer_stage_t;

#endif // !USB_STANDARD_H_
