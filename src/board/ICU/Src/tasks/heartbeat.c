/*
 * ICU main task. Runs state machine to control all the probe.
 * */
#include "FreeRTOS.h"
#include "task.h"

#include <main.h>


void heartbeat_task(void *pvParameters)
{
	mavlink_get_channel_status(MAVLINK_COMM_0)->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;

	int counter = 0;
	while(1)
	{
		static mavlink_message_t msg;
		static mavlink_heartbeat_t heartbeat =
		{
			.type = MAV_TYPE_FREE_BALLOON,
			.autopilot = MAV_AUTOPILOT_INVALID,
			.base_mode = MAV_MODE_FLAG_TEST_ENABLED,
			.system_status = MAV_STATE_ACTIVE
		};
		mavlink_msg_heartbeat_encode(0, ZIKUSH_ICU, &msg, &heartbeat);

		router_route(&msg, 0);

		counter++;
		if (counter % 5 == 0)
		{
			static mavlink_zikush_icu_stats_t gstats_local;
			taskENTER_CRITICAL();
			gstats_local = global_stats;
			taskEXIT_CRITICAL();

			mavlink_msg_zikush_icu_stats_encode(0, ZIKUSH_ICU, &msg, &gstats_local);
			router_route(&msg, 0);
		}


		vTaskDelay(1000 * portTICK_RATE_MS);
	}

	vTaskDelete(NULL);
}
