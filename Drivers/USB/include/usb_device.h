#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usb_standards.h"

typedef struct {
    /* @brief The current USB device state. */
    usb_device_state_t device_state;
    /* @brief The current control transfer stage (for endpoint0). */
    usb_control_transfer_stage_t control_transfer_stage;
    /* @brief The selected USB configuration. */
    uint8_t configuration_value;

    /** @defgroup usb_device_out_in_buffer_pointers 
     * @{*/
    const void *out_buffer;
    uint32_t out_data_size;
    const void *in_buffer;
    uint32_t in_data_size;
    /** @}*/
} usb_device_t ;

#endif // !USB_DEVICE_H_

