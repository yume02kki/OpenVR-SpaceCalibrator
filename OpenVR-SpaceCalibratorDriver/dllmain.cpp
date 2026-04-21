#include "Logging.h"
#include "../Version.h"

__attribute__((constructor)) static void on_load()
{
    OpenLogFile();
    LOG("OpenVR-SpaceCalibratorDriver " SPACECAL_VERSION_STRING " loaded");
}

__attribute__((destructor)) static void on_unload()
{
    LOG("OpenVR-SpaceCalibratorDriver unloaded");
}