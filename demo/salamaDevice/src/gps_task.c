#include <api_os.h>
#include <api_hal_gpio.h>
#include <api_gps.h>

#include "gps_task.h"
#include "gps_parse.h"
#include "gps.h"

HANDLE gpsTaskHandle = NULL;

static char buffer[1024] = "";

void GpsInit(void) {
	GPS_Info_t* gpsInfo = Gps_GetInfo();

	// Open the GPS hardware - UART2
	Trace(1, "GPS init");
	GPS_Init();
	Trace(1, "Gps open");
	GPS_Open(NULL);

	// wait for gps to start up
	while (gpsInfo->rmc.latitude.value == 0)
		OS_Sleep(1000);
	Trace(1, "GPS responds");

	if (!GPS_GetVersion(buffer, 150))
		Trace(1, "Get GPS Version firmware fail");
	else
		Trace(1, "Gps firmware version: %s", buffer);
	if (!GPS_SetFixMode(GPS_FIX_MODE_LOW_SPEED))
		Trace(1, "Set fix mode fail");
	if (!GPS_SetSBASEnable(true))
		Trace(1, "Enable SBAS fail");
	if (!GPS_SetLpMode(GPS_LP_MODE_NORMAL))
		Trace(1, "Set gps lp mode fail");
	if (!GPS_SetOutputInterval(1000))
		Trace(1, "Set nmea output interval fail");
	if (!GPS_SetSearchMode(true, false, true, false))
		Trace(1, "Set search mode fail");
}

void GpsTask(void* pData) {
	// intialize the hardware
	GpsInit();
	
	while (1)
	{
		// sleep
		OS_Sleep(1000);
	}

}

void GpsTaskInit(void) {
	gpsTaskHandle = OS_CreateTask(
		GpsTask,
		NULL,
		NULL,
		GPS_TASK_STACK_SIZE,
		GPS_TASK_PRIORITY,
		0,
		0,
		GPS_TASK_NAME
	);
}