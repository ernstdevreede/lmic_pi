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
//#include "hw.h"
#include <stdlib.h>

// use PB12 (DIP switch 1) as sensor value
void initsensor () {
    return;
//    RCC->AHBENR  |= RCC_AHBENR_GPIOBEN; // clock enable port B
 //   hw_cfg_pin(GPIOB, 12, GPIOCFG_MODE_INP | GPIOCFG_OSPEED_40MHz | GPIOCFG_OTYPE_OPEN); // PB12
}

// read PB12
u2_t readsensor () {
    return random();
//((GPIOB->IDR & (1 << 12)) != 0);
}
