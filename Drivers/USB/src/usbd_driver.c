#include "usbd_driver.h"

void usbd_core_init(void)
{
    /* Enable clock for USB OTG FS */
    RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;

    /*
     * Configure USB core:
     *
     * - Force device mode
     * - Set turnaround time
     */

    USB_OTG_FS->GUSBCFG &=
        ~(USB_OTG_GUSBCFG_FDMOD |
          USB_OTG_GUSBCFG_TRDT);

    /* Force device mode */
    USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;

    /* Set turnaround time to 9 */
    USB_OTG_FS->GUSBCFG |=
        (9U << USB_OTG_GUSBCFG_TRDT_Pos);


    /*
     * Configure device speed.
     *
     * DSPD = 0b11
     * Full-speed device using internal FS PHY.
     */
    USB_OTG_FS_DEVICE->DCFG &= ~USB_OTG_DCFG_DSPD;

    USB_OTG_FS_DEVICE->DCFG |=
        (0x03U << USB_OTG_DCFG_DSPD_Pos);


    /*
     * Enable VBUS sensing.
     *
     * PA9 is connected to VBUS_FS on the Discovery board.
     */
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;


    /* Unmask required USB interrupts */
    USB_OTG_FS->GINTMSK |=
        USB_OTG_GINTMSK_USBRST   |  /* USB reset */
        USB_OTG_GINTMSK_ENUMDNEM |  /* Enumeration done */
        USB_OTG_GINTMSK_SOFM     |  /* Start of frame */
        USB_OTG_GINTMSK_USBSUSPM |  /* USB suspend */
        USB_OTG_GINTMSK_WUIM     |  /* Wakeup */
        USB_OTG_GINTMSK_IEPINT   |  /* IN endpoint interrupt */
        USB_OTG_GINTMSK_OEPINT   |  /* OUT endpoint interrupt */
        USB_OTG_GINTMSK_RXFLVLM;    /* RX FIFO not empty */


    /* Clear pending USB interrupts */
    USB_OTG_FS->GINTSTS = 0xFFFFFFFFU;


    /* Enable global USB interrupts */
    USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}


void connect(void)
{
    /* Power up the FS transceiver */
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;

    /* Clear soft disconnect bit -> connect to USB bus */
    USB_OTG_FS_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
}


void disconnect(void)
{
    /* Set soft disconnect bit */
    USB_OTG_FS_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;

    /* Power down FS transceiver */
    USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;
}
