#include "usbd_framework.h"
#include "usbd_driver.h"
#include "Helpers/logger.h"

static usb_device_t *usbd_handle;

void usbd_init(usb_device_t *usb_device)
{
    usbd_handle = usb_device;
    usb_driver.usbd_core_init();
    usb_driver.connect();
}

void usdb_poll()
{
    usb_driver.poll();
}

static void usb_reset_received_handler()
{
    usbd_handle->in_data_size = 0;
    usbd_handle->out_data_size = 0;
    usbd_handle->configuration_value = 0;
    usbd_handle->device_state = USB_DEVICE_STATE_DEFAULT;
    usbd_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
    usb_driver.set_device_addr(0);
}

static void setup_data_received_handler(uint8_t endpoint_num, uint16_t byte_count)
{
    usb_driver.read_packet(usbd_handle->out_buffer, byte_count);
}

usb_events_t usb_events = {
    .on_usb_reset_received = usb_reset_received_handler,
    .on_setup_data_received = setup_data_received_handler
};

