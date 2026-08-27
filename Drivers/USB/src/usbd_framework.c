#include "usbd_framework.h"

void usbd_init()
{
    usbd_core_init();
    connect();
}

