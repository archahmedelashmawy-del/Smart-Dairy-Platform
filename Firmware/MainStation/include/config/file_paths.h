#ifndef FILE_PATHS_H
#define FILE_PATHS_H

/*
====================================================
 Smart Dairy Platform
 File System Layout
====================================================
*/

namespace FilePaths
{
    // Root Directories
    constexpr const char* LOG_DIR       = "/logs";
    constexpr const char* EVENTS_DIR    = "/events";
    constexpr const char* CONFIG_DIR    = "/config";
    constexpr const char* EXPORT_DIR    = "/export";
    constexpr const char* BACKUP_DIR    = "/backup";

    //------------------------------------------------
    // Logs
    //------------------------------------------------

    constexpr const char* MILKING_LOG   = "/logs/milking_log.csv";
    constexpr const char* SYSTEM_LOG    = "/logs/system_log.txt";
    constexpr const char* ERROR_LOG     = "/logs/error_log.txt";

    //------------------------------------------------
    // Veterinary Events
    //------------------------------------------------

    constexpr const char* VET_EVENTS    = "/events/vet_events.csv";

    //------------------------------------------------
    // Configuration
    //------------------------------------------------

    constexpr const char* DEVICE_CONFIG = "/config/device.cfg";

    //------------------------------------------------
    // Export
    //------------------------------------------------

    constexpr const char* EXPORT_FILE   = "/export/export.csv";

}

#endif
