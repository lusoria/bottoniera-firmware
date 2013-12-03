/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"

#include "usbcfg.h"

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

	(void) arg;
	chRegSetThreadName("blinker");
	while (TRUE) {
		palClearPad(LED1_GPIO, LED1_PIN);
		if (palReadPad(GPIOE, 5))
			palClearPad(LED2_GPIO, LED2_PIN);
		chThdSleepMilliseconds(500); palSetPad(LED1_GPIO, LED1_PIN);
		if (palReadPad(GPIOE, 5))
			palSetPad(LED2_GPIO, LED2_PIN);
		chThdSleepMilliseconds(500);
	}

	return RDY_OK;
}

/*
 * Application entry point.
 */
int main(void) {

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
//	sduObjectInit(&SDU1);
//	sduStart(&SDU1, &serusbcfg);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(hidcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(hidcfg.usbp, &usbcfg);
	usbConnectBus(hidcfg.usbp);

	/*
	 * Activates the serial driver 2 using the driver default configuration.
	 */
//  sdStart(&SD1, NULL);
	/*
	 * Creates the blinker thread.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (TRUE) {
//		chprintf((BaseSequentialStream *) &SDU1, "diocane\r\n");
		chThdSleepMilliseconds(500);
	}
}
