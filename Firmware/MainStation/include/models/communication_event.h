#ifndef COMMUNICATION_EVENT_H
#define COMMUNICATION_EVENT_H

#include "drivers/communication/espnow_driver.h"

/*
==========================================================
 Smart Dairy Platform
 Communication Event Layer (Clean Architecture)
----------------------------------------------------------
 Decouples upper-level services from lower-level drivers.
==========================================================
*/

using CommunicationEvent = ReceivedPacket;

#endif // COMMUNICATION_EVENT_H
