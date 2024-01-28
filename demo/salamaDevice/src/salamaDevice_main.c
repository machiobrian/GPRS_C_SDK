#include <string.h>
#include <stdio.h>
#include <api_os.h>
#include <api_call.h>
#include <api_gps.h>
#include <api_event.h>
#include <api_hal_uart.h>
#include <api_debug.h>
#include "buffer.h"
#include "gps_parse.h"
#include "math.h"
#include "gps.h"
#include "api_hal_pm.h"
#include "api_mqtt.h"
#include "api_key.h"
#include "time.h"
#include "api_info.h"
#include "assert.h"
#include "api_socket.h"
#include "api_network.h"
#include "api_hal_gpio.h"
#include "api_hal_adc.h"
#include "api_hal_pm.h"
#include "api_hal_watchdog.h"

#include "main_task.h"
#include "gps_task.h"
#include "mqtt_task.h"
#include "secret.h"

HANDLE mainTaskHandle = NULL;

GPIO_config_t stateLed = {
	.mode = GPIO_MODE_OUTPUT,
	.pin = GPIO_PIN28,
	.defaultLevel = GPIO_LEVEL_HIGH
};

bool AttachActivate() {

}

void EventDispatch(API_Event_t* pEvent) {

}

/*
Init HW and run MQTT task.
@param pData Parameter is passed when this function is called
*/
void app_MainTask(void, *pData) {
	API_Event_t* event = NULL;
	Trace(1, "Main Task Started");
	
	TIME_SetIsAutoUpdateRtcTime(true); // localtime will be synced with GPS
	GPIO_Init(stateLed);
	// create MQTT Task
	MqttTaskInit();
	GpsTaskInit();

	// Wait & Process system events
	while (1) {
		if (OS_WaitEvent(mainTaskHandle, (void**)&event, OS_TIME_OUT_WAIT_FOREVER)) {
			EventDispatch(event);
			OS_Free(event->pParam1);
			OS_Free(event->pParam2);
			OS_Free(event);
		}
	}

}

/*
Entry point of the application.
e.g folder -> first
			first_Main(){
			}
therefore: 
	folder -> salamaDevice
		salamaDevice_Main(){
		}
*/


void salamaDevice_Main(void) {
	mainTaskHandle = OS_CreateTask(
		app_MainTask,
		NULL,
		NULL,
		MAIN_TASK_STACK_SIZE,
		MAIN_TASK_PRIORITY,
		0,
		0,
		MAIN_TASK_NAME
	);
	OS_SetUserMainHandle(&mainTaskHandle);
	
}