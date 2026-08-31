#include "usbd_driver.h"
#include "Helpers/logger.h"

LogLevel system_log_level = LOG_LEVEL_DEBUG;


static void usbd_core_init(void)
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

    // Unmasks USB global interrupt.
    USB_OTG_FS_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM;
    USB_OTG_FS_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM;
}

static void set_device_addr(uint8_t addr)
{
    USB_OTG_FS_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;
    USB_OTG_FS_DEVICE->DCFG |= addr << USB_OTG_DCFG_DAD_Pos;
}

/**
 * @brief Connects the USB device to the bus.
 **/
static void connect(void)
{
    /* Power up the FS transceiver */
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;

    /* Clear soft disconnect bit -> connect to USB bus */
    USB_OTG_FS_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
}

/**
 *  @brief Disconnect the USB device from the bus.
 **/
static void disconnect(void)
{
    /* Set soft disconnect bit */
    USB_OTG_FS_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;

    /* Power down FS transceiver */
    USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;
}

/**
 *  @brief Pops data from the RxFIFO and stores it in the buffer.
 *  @param buffer - pointer to the buffer, in which the popped data 
 *                  will be stored.
 *  @param size - Count of bytes to be popped from the dedicated RxFIFO
 **/
static void read_packet(void const *buffer, uint16_t size)
{
    // Note: There is only one RxFIFO.
    uint32_t *fifo = FIFO(0);

    for (; size >= 4; size -= 4, buffer += 4) {
        // Pops one 32-bit word of data (until there is less than one word remaining).
        uint32_t data = *fifo;
        // Stores the data in the buffer.
        *((uint32_t *)buffer) = data;
    }

    if (size > 0) {
        // Pops the last remaining bytes (which are less than one word).
        uint32_t data = *fifo;

        for (; size > 0; size--, buffer++, data >>= 8) {
            // Stores the data in the buffer with the correct alignment.
            *((uint32_t *)buffer) = data & 0xFF;
        }
    }
}

/**
 *  @brief Pushes a packet into the TxFIFO of an IN endpoint.
 *  @param endpoint_num - The number of the endpoint, to which the data
 *                        will be written.
 *  @param buffer - Pointer to the buffer contains the data to be written
 *                        to the endpoint.
 *  @param size - The size of data to be written in bytes.
 **/
static void write_packet(uint8_t endpoint_num,  void const *buffer, uint16_t size)
{
    uint32_t *fifo = FIFO(endpoint_num);
    USB_OTG_INEndpointTypeDef *in_endpoint = IN_ENDPOINT(endpoint_num);
    
    // Configures the transmission (1 packet that has 'size' bytes).
    in_endpoint->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_XFRSIZ);
    in_endpoint->DIEPTSIZ |=  ( 1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos) |
                              ( size << USB_OTG_DIEPTSIZ_XFRSIZ_Pos);

    // Enables the transmission after clearing both STALL and NAK of the endpoint. 
    in_endpoint->DIEPCTL &= ~(USB_OTG_DIEPCTL_STALL);
    in_endpoint->DIEPCTL |= USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA;

    // Gets the size in term of 32-bit words (to avoid integer overflow in the loop).
    size = (size + 3) / 4;

    for (; size > 0; size--, buffer += 4) {
        // Pushes the data to the TxFIFO.
        *fifo = *((uint32_t *)buffer);
    }
}

/**
 *  @brief Updates the start addresses of all FIFOs according to the 
 *         size of each FIFO.
 **/        
static void refresh_fifo_start_addr()
{
    // The first changeable start address begins after the region of RxFIFO.
    uint16_t start_addr = ((USB_OTG_FS->GRXFSIZ & USB_OTG_GRXFSIZ_RXFD) >> 
                            USB_OTG_GRXFSIZ_RXFD_Pos) * 4;

    // Updates the start address of the TxFIFO0.
    USB_OTG_FS->DIEPTXF0_HNPTXFSIZ &= ~USB_OTG_TX0FSA;
    USB_OTG_FS->DIEPTXF0_HNPTXFSIZ |= start_addr << USB_OTG_TX0FSA_Pos;

    // The next start address is after where the last TxFIFO ends.
    start_addr += ((USB_OTG_FS->DIEPTXF0_HNPTXFSIZ & USB_OTG_TX0FD) >>
                    USB_OTG_TX0FD_Pos) * 4;

    // Updates the start addresses of the rest TxFIFOs.
    for (uint8_t txfifo_num = 0; txfifo_num < ENDPOINT_COUNT - 1; txfifo_num++) {
        
        USB_OTG_FS->DIEPTXF[txfifo_num] &= ~USB_OTG_NPTXFSA;
        USB_OTG_FS->DIEPTXF[txfifo_num] |= start_addr << USB_OTG_NPTXFSA_Pos;

        start_addr += ((USB_OTG_FS->DIEPTXF[txfifo_num] & USB_OTG_NPTXFD) >>
                        USB_OTG_NPTXFD_Pos) * 4;
    }
}

/**
 *  @brief Configures the RxFIFO of all OUT endpoints.
 *  @param size - The size of the largest OUT endpoints in bytes.
 *  @note - The RxFIFO is shared between all OUT endpoints.
 **/
static void configure_rxfifo_size(uint16_t size)
{
    // Considers the space required to save status packets in RxFIFO
    // and gets the size in term of 32-bit words.
    size = 10 + (2 * ((size / 4) + 1));

    // Configures the depth of the FIFO.
    USB_OTG_FS->GRXFSIZ &= ~USB_OTG_GRXFSIZ_RXFD;
    USB_OTG_FS->GRXFSIZ |= size << USB_OTG_GRXFSIZ_RXFD_Pos;

    refresh_fifo_start_addr();
}

/**
 *  @brief Configures the TxFIFO of an IN endpoint.
 *  @param endpoint_num - The number of the IN endpoint we want to configure its TxFIFO
 *  @param size - The size of the IN endpoint in bytes.
 *  @note - Any change on any FIFO will update the registers of all TxFIFOs to adapt the
 *          start offsets in the FIFO dedicate memory.
 **/
static void configure_txfifo_size(uint8_t endpoint_num, uint16_t size)
{
    // Gets the FIFO size in term of 32-bit words.
    size = (size + 3) / 4;

    // Configures the depth of the TxFIFO.
    if (endpoint_num == 0) {

        USB_OTG_FS->DIEPTXF0_HNPTXFSIZ &= ~USB_OTG_TX0FD;
        USB_OTG_FS->DIEPTXF0_HNPTXFSIZ |=  size << USB_OTG_TX0FD_Pos;
    } else {
        USB_OTG_FS->DIEPTXF[endpoint_num - 1] &= ~USB_OTG_NPTXFD;
        USB_OTG_FS->DIEPTXF[endpoint_num - 1] |= size << USB_OTG_NPTXFD_Pos;
    }

    refresh_fifo_start_addr();
}

/**
 *  @brief Flushes the RxFIFO of all OUT endpoints.
 **/
static void flush_rxfifo()
{
    USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
}

/**
 *  @brief Flushes the TxFIFO of an IN endpoint.
 *  @param endpoint_num - The number of an IN endpoint to flush its TxFIFO.
 **/
static void flush_txfifo(uint8_t endpoint_num)
{
    USB_OTG_HS->GRSTCTL &= ~USB_OTG_GRSTCTL_TXFNUM;
    USB_OTG_HS->GRSTCTL |= (endpoint_num << USB_OTG_GRSTCTL_TXFNUM_Pos) |
                            USB_OTG_GRSTCTL_TXFFLSH;
}

static void configure_endpoint0(uint8_t endpoint_size)
{
    USB_OTG_FS_DEVICE->DAINTMSK |= 1 << USB_OTG_DAINTMSK_IEPM_Pos |
                                   1 << USB_OTG_DAINTMSK_OEPM_Pos;

    // Configure the maximum packet size, activates the edpoint, and NAK the endpoint (cannot send data yet).
    IN_ENDPOINT(0)->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;
    IN_ENDPOINT(0)->DIEPCTL |= USB_OTG_DIEPCTL_USBAEP;

    IN_ENDPOINT(0)->DIEPCTL |= (endpoint_size << USB_OTG_DIEPCTL_MPSIZ_Pos);
    IN_ENDPOINT(0)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;

    // Clears NAK, and enables endpoint data transmission.
    OUT_ENDPOINT(0)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;

    configure_rxfifo_size(EP0_SIZE);
    configure_txfifo_size(0, endpoint_size);
}

static void configure_in_endpoint(uint8_t endpoint_num, usb_endpoint_t endpoint_type, uint16_t endpoint_size)
{
    // Unmask all interrupts of the targeted IN endpoint.
    USB_OTG_FS_DEVICE->DAINTMSK |= 1 << endpoint_num;

    // Activates the endpoint, sets endpoint handshake to NAK (not ready to send data),
    // sets DATA0 packet identifier,configures its type, its max packet size,
    // and assigns it a TXFIFO.
    IN_ENDPOINT(endpoint_num)->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ |
                                           USB_OTG_DIEPCTL_EPTYP |
                                           USB_OTG_DIEPCTL_TXFNUM);

    IN_ENDPOINT(endpoint_num)->DIEPCTL |= USB_OTG_DIEPCTL_USBAEP;
    IN_ENDPOINT(endpoint_num)->DIEPCTL |= endpoint_size << USB_OTG_DIEPCTL_MPSIZ_Pos;
    IN_ENDPOINT(endpoint_num)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
    IN_ENDPOINT(endpoint_num)->DIEPCTL |= endpoint_type << USB_OTG_DIEPCTL_EPTYP_Pos;
    IN_ENDPOINT(endpoint_num)->DIEPCTL |= endpoint_num << USB_OTG_DIEPCTL_TXFNUM_Pos;
    IN_ENDPOINT(endpoint_num)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;

    configure_txfifo_size(endpoint_num, endpoint_size);
}

static void configure_out_endpoint(uint8_t endpoint_num, usb_endpoint_t endpoint_type, uint16_t endpoint_size)
{
    // Unmask all interrupts of the targeted OUT endpoint
    USB_OTG_FS_DEVICE->DAINTMSK |= 1 << (endpoint_num + USB_OTG_DAINTMSK_OEPM_Pos);

    // Configure max packet size and endpoint type.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL &= ~(USB_OTG_DOEPCTL_MPSIZ |
                                             USB_OTG_DOEPCTL_EPTYP);

    // Activate the endpoint.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL |= USB_OTG_DOEPCTL_USBAEP;
    // Set maximum packet size.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL |= endpoint_size << USB_OTG_DOEPCTL_MPSIZ_Pos;
    // Set endpoint handshake to NAK.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    // configure endpoint type.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL |= endpoint_type << USB_OTG_DOEPCTL_EPTYP_Pos;
    // Set DATA0 PID / even frame.
    OUT_ENDPOINT(endpoint_num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
}

/**
 *  @brief Deconfigures IN and OUT endpoint of a specific endpoint number.
 *  @param endpoint_num - The number of the IN and OUT endpoint to deconfigure.
 **/
static void deconfigure_endpoint(uint8_t endpoint_num)
{
    USB_OTG_INEndpointTypeDef *in_endpoint = IN_ENDPOINT(endpoint_num);
    USB_OTG_OUTEndpointTypeDef *out_endpoint = OUT_ENDPOINT(endpoint_num);

    // Masks all interrupts of the targeted IN and OUT endpoints.
    USB_OTG_FS_DEVICE->DAINTMSK &= ~((1 << endpoint_num) |
                                     (1 << (endpoint_num + USB_OTG_DAINTMSK_OEPM_Pos)));

    // Clears all interrupts of the endpoint
    in_endpoint->DIEPINT = 0x29FF;
    out_endpoint->DOEPINT = 0x715F;

    // Disables the endpoints if possible.
    if (in_endpoint->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
        // Disables endpoint transmission.
        in_endpoint->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS;
    }

    // Deactivates the endpoint
    in_endpoint->DIEPCTL &= ~USB_OTG_DIEPCTL_USBAEP;

    if (endpoint_num != 0) {
        if (out_endpoint->DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
            // Disables endpoint transmission
            out_endpoint->DOEPCTL |= USB_OTG_DOEPCTL_EPDIS;
        }

        // Deactivates the endpoint
        out_endpoint->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;
    }

    // Flushes the FIFOs.
    flush_txfifo(endpoint_num);
    flush_rxfifo();
}



static void usbrst_handler()
{
    log_info("USB reset signal was detected.");

    for (uint8_t i = 0; i <= ENDPOINT_COUNT; i++)
        deconfigure_endpoint(i);
}

static void enumdone_handler()
{
    log_info("USB device speed enumeration done.");
    configure_endpoint0(8);
}

static void rxflvl_handler()
{
    // Pops the status information word from the RxFIFO.
    uint32_t receive_status = USB_OTG_HS_GLOBAL->GRXSTSP;

    // The endpoint that received the data.
    uint8_t endpoint_num = (receive_status & USB_OTG_GRXSTSP_EPNUM) >>
                            USB_OTG_GRXSTSP_EPNUM_Pos;
    // The count of bytes in the received packet.
    uint16_t byte_count = (receive_status & USB_OTG_GRXSTSP_BCNT) >>
                            USB_OTG_GRXSTSP_BCNT_Pos;
    // The status of the received packet.
    uint16_t pktst = (receive_status & USB_OTG_GRXSTSP_PKTSTS) >>
                      USB_OTG_GRXSTSP_PKTSTS_Pos;
    
    switch (pktst) {
        case 0x06:  // SETUP packet (includes data).
            usb_events.on_setup_data_received(endpoint_num, byte_count);
            break;

        case 0x02:  // OUT packet (includes data).

            break;

        case 0x04:  // SETUP stage has completed.
                    // Re-enables the transmission on the endpoint.
            OUT_ENDPOINT(endpoint_num)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK |
                USB_OTG_DOEPCTL_EPENA;
            break;

        case 0x03:  // OUT transfer has completed.
                    // Re-enables the transmission on the endpoint,
            OUT_ENDPOINT(endpoint_num)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK |
                USB_OTG_DOEPCTL_EPENA;
            break;
    }
}

static void gintsts_handler()
{
    volatile uint32_t gintsts = USB_OTG_FS_GLOBAL->GINTSTS;

    if (gintsts & USB_OTG_GINTSTS_USBRST) {

        usbrst_handler();
        // clear the interrupt
        USB_OTG_FS_GLOBAL->GINTSTS = USB_OTG_GINTSTS_USBRST;

    } 
    else if (gintsts & USB_OTG_GINTSTS_ENUMDNE) {

        enumdone_handler();
        // Clear the interrupt.
        USB_OTG_FS_GLOBAL->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;

    }
    else if (gintsts & USB_OTG_GINTSTS_RXFLVL) {

        rxflvl_handler();
        // clear the interrupt.
        USB_OTG_FS_GLOBAL->GINTSTS |= USB_OTG_GINTSTS_RXFLVL;

    }
    else if (gintsts & USB_OTG_GINTSTS_IEPINT) {



    }
    else if (gintsts & USB_OTG_GINTSTS_OEPINT) {


    }
}

const usb_driver_t usb_driver = {
    .usbd_core_init = usbd_core_init,
    .set_device_addr = set_device_addr,
    .connect = connect,
    .disconnect = disconnect,
    .flush_rxfifo = flush_rxfifo,
    .flush_txfifo = flush_txfifo,
    .configure_in_endpoint= configure_in_endpoint,
    .read_packet = read_packet,
    .write_packet = write_packet,
    .poll = gintsts_handler,
};
