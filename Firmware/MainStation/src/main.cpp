#include <Arduino.h>
#include "core/logger.h"
#include "core/event_bus.h"
#include "drivers/communication/espnow_driver.h"
#include "services/communication/communication_service.h"

// System Instances
ESPNowDriver espDriver;
CommunicationService commService(espDriver);

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Logger::info("Initializing Smart Dairy Firmware System...");

    // Initialize Communication Service
    if (commService.begin() == ErrorCode::OK)
    {
        Logger::info("Communication Service initialized successfully.");
    }
    else
    {
        Logger::error("Failed to initialize Communication Service!");
    }
}

void loop()
{
    // Update Communication Loop Queue processing
    commService.update();

    // Periodic Diagnostics output every 10 seconds
    static uint32_t lastDiagnostics = 0;
    if (millis() - lastDiagnostics >= 10000)
    {
        commService.printDiagnostics(Serial);
        lastDiagnostics = millis();
    }

    delay(10);
}
