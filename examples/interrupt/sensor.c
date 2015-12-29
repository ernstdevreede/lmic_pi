/*******************************************************************************
 * Copyright (c) 2014-2015 IBM Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    IBM Zurich Research Lab - initial API, implementation and documentation
 *******************************************************************************/

#include "lmic.h"
#include "hw.h"

// use PB12 (DIP switch 1)
#define INP_PORT 1
#define INP_PIN 12

static osjob_t irqjob;

// use DIP1 as sensor value
void initsensor (osjobcb_t callback) {
    // configure input
    RCC->AHBENR  |= RCC_AHBENR_GPIOBEN; // clock enable port B
    hw_cfg_pin(GPIOx(INP_PORT), INP_PIN, GPIOCFG_MODE_INP | GPIOCFG_OSPEED_40MHz | GPIOCFG_OTYPE_OPEN);
    hw_cfg_extirq(INP_PORT, INP_PIN, GPIO_IRQ_CHANGE);
    // save application callback
    irqjob.func = callback;
}

// read PB12
u2_t readsensor () {
    return ((GPIOB->IDR & (1 << INP_PIN)) != 0);
}

// called by EXTI_IRQHandler
// (set preprocessor option CFG_EXTI_IRQ_HANDLER=sensorirq)
void sensorirq () {
    if((EXTI->PR & (1<<INP_PIN)) != 0) { // pending
	EXTI->PR = (1<<INP_PIN); // clear irq
        // run application callback function in 50ms (debounce)
        os_setTimedCallback(&irqjob, os_getTime()+ms2osticks(50), irqjob.func);
    }
}
