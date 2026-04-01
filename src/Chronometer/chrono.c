#include "chrono.h"

#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(chrono_mod, LOG_LEVEL_INF);

// Variables modifiées par le thread et lues par le timer LVGL
static volatile int hours;    
static volatile int minutes;  
static volatile int seconds;  
static volatile bool running; 

#define STACK_SIZE 512
#define PRIORITY 5

K_THREAD_STACK_DEFINE(chrono_stack, STACK_SIZE);
static struct k_thread chrono_thread;
static k_tid_t chrono_tid;

static void chrono_thread_fn(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    while (1) {
        k_sleep(K_SECONDS(1));

        if (!running) {
            continue;
        }

        seconds++;

        if (seconds >= 60) {
            seconds = 0;
            minutes++;
        }

        if (minutes >= 60) {
            minutes = 0;
            hours++;
        }
    }
}

void chrono_init(void)
{
    hours = 0;
    minutes = 0;
    seconds = 0;
    running = false;

    chrono_tid = k_thread_create(&chrono_thread, chrono_stack,
                                  K_THREAD_STACK_SIZEOF(chrono_stack),
                                  chrono_thread_fn,
                                  NULL, NULL, NULL,
                                  PRIORITY, 0, K_NO_WAIT);

    LOG_INF("Chrono initialise: %02d:%02d:%02d", hours, minutes, seconds);
}

void chrono_start(void)
{
    if (running) {
        return;
    }

    running = true;
    LOG_INF("Chrono demarre");
}

void chrono_stop(void)
{
    if (!running) {
        return;
    }

    running = false;
    LOG_INF("Chrono arrete a %02d:%02d:%02d", hours, minutes, seconds);
}

void chrono_reset(void)
{
    running = false;
    hours = 0;
    minutes = 0;
    seconds = 0;

    LOG_INF("Chrono remis a zero: %02d:%02d:%02d", hours, minutes, seconds);
}

void chrono_get_time(int *h, int *m, int *s)
{
    *h = hours;
    *m = minutes;
    *s = seconds;
}