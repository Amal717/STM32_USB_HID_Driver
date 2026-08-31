#ifndef USBD_DRIVER_H_
#define USBD_DRIVER_H_

#include "stm32f407xx.h"
#include "usb_standards.h"

#define USB_OTG_HS_GLOBAL   ((USB_OTG_GlobalTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_HS_DEVICE   ((USB_OTG_DeviceTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_HS_PCGCCTL  ((uint32_t *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_HS_PCGCCTL_PERIPH_BASE))

#define USB_OTG_FS_GLOBAL   ((USB_OTG_GlobalTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_FS_DEVICE   ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_FS_PCGCCTL  ((uint32_t *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FS_PCGCCTL_PERIPH_BASE))

#define ENDPOINT_COUNT      4   /* 1 bi-ctrl endpoint and 3 in and out */
#define EP0_SIZE            64  /* maximum packet size for endpoint 0 */

inline static USB_OTG_INEndpointTypeDef * IN_ENDPOINT(uint8_t endpoint_num)
{
    return (USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE + (endpoint_num * 0x20));
}

inline static USB_OTG_OUTEndpointTypeDef * OUT_ENDPOINT(uint8_t endpoint_num)
{
    return (USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_OUT_ENDPOINT_BASE + (endpoint_num * 0x20));
}

inline static __IO uint32_t *FIFO(uint8_t endpoint_num)
{
    return (__IO uint32_t *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + (endpoint_num * 0x1000));
}

typedef struct {
    void (*usbd_core_init)();
    void (*set_device_addr)(uint8_t addr);
    void (*connect)();
    void (*disconnect)();
    void (*flush_rxfifo)();
    void (*flush_txfifo)(uint8_t endpoint_num);
    void (*configure_in_endpoint)(uint8_t endpoint_num, usb_endpoint_t endpoint_type, uint16_t endpoint_size);
    void (*read_packet)(void const *buffer, uint16_t size);
    void (*write_packet)(uint8_t endpoint_num, void const *buffer, uint16_t size);
    void (*poll)();
} usb_driver_t;

extern const usb_driver_t usb_driver;
extern usb_events_t usb_events;

#endif // !USBD_DRIVER_H_
