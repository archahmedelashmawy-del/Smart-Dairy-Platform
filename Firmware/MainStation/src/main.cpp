#include <Arduino.h>
#include "core/logger.h"
#include "core/event_bus.h"
#include "drivers/communication/espnow_driver.h"
#include "services/communication/communication_service.h"

// إنشاء الكائنات الأساسية للنظام (System Singletons/Instances)
ESPNowDriver espDriver;
CommunicationService commService(espDriver);

void setup()
{
    // تهيئة الاتصال التسلسلي (Serial) للـ Diagnostics
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Logger::info("==========================================");
    Logger::info("   Smart Dairy Platform Firmware v1.0     ");
    Logger::info("==========================================");

    // تهيئة خدمة الاتصالات
    ErrorCode initStatus = commService.begin();
    if (initStatus == ErrorCode::OK)
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
    // معالجة طابور الحزم المتبقية والاستجابة للأحداث الشبكية
    commService.update();

    // طباعة التشخيص والإحصائيات كل 10 ثوانٍ
    static uint32_t lastDiagnosticsTime = 0;
    if (millis() - lastDiagnosticsTime >= 10000)
    {
        commService.printDiagnostics(Serial);
        lastDiagnosticsTime = millis();
    }

    // تأخير بسيط لمنع استهلاك الـ CPU بدون داعٍ
    delay(10);
}
