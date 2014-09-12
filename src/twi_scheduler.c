#include "twi_scheduler.h"
#include "app_scheduler.h"
#include "app_timer.h"

/** Maximum size of scheduler events. Note that scheduler BLE stack events do not contain
    any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t)

/** Maximum number of events in the scheduler queue. */
#define SCHED_QUEUE_SIZE                10

void scheduler_init()
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
