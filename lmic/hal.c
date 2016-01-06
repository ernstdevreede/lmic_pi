#include "config.h"
#include "oslmic.h"
#include "hal.h"
#include "local_hal.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>


int fd;

// -----------------------------------------------------------------------------
// I/O

static void hal_io_init () {
    wiringPiSetup();
    pinMode(pins.nss, OUTPUT);
    pinMode(pins.rxtx, OUTPUT);
    pinMode(pins.rst, OUTPUT);
    pinMode(pins.dio[0], INPUT);
    pinMode(pins.dio[1], INPUT);
    pinMode(pins.dio[2], INPUT);
}

// val == 1  => tx 1
void hal_pin_rxtx (u1_t val) {
    digitalWrite(pins.rxtx, val);
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst (u1_t val) {
    if(val == 0 || val == 1) { // drive pin
        pinMode(pins.rst, OUTPUT);
        digitalWrite(pins.rst, val);
//        digitalWrite(0, val==0?LOW:HIGH);
    } else { // keep pin floating
        pinMode(pins.rst, INPUT);
    }
}

static bool dio_states[NUM_DIO] = {0};

static void hal_io_check() {
    u1_t i;
    for (i = 0; i < NUM_DIO; ++i) {
        if (dio_states[i] != digitalRead(pins.dio[i])) {
            dio_states[i] = !dio_states[i];
            if (dio_states[i]) {
                radio_irq_handler(i);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SPI
//
static int spifd;

static void hal_spi_init () {
    spifd = wiringPiSPISetup(0, 10000000);
}

void hal_pin_nss (u1_t val) {
    digitalWrite(pins.nss, val);
}

// perform SPI transaction with radio
u1_t hal_spi (u1_t out) {
    u1_t res = wiringPiSPIDataRW(0, &out, 1);
    return out;
}


// -----------------------------------------------------------------------------
// TIME

struct timespec tstart={0,0};
static void hal_time_init () {
    int res=clock_gettime(CLOCK_MONOTONIC_RAW, &tstart);
    tstart.tv_nsec=0; //Makes difference calculations in hal_ticks() easier
}

u4_t hal_ticks (void) {
    // LMIC requires ticks to be 15.5μs - 100 μs long
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    ts.tv_sec-=tstart.tv_sec;
    u8_t ticks=ts.tv_sec*(1000000/US_PER_OSTICK)+ts.tv_nsec/(1000*US_PER_OSTICK);
//    fprintf(stderr, "%d hal_ticks()=%d\n", sizeof(time_t), ticks);
    return (u4_t)ticks;
}

// Returns the number of ticks until time.
static u4_t delta_time(u4_t time) {
      u4_t t = hal_ticks( );
      s4_t d = time - t;
      //fprintf(stderr, "deltatime(%d)=%d (%d)\n", time, d, t);
      if (d<=5) { return 0; }
      else {
        return (u4_t)d;
      }
}

void hal_waitUntil (u4_t time) {
    u4_t now=hal_ticks();
    u4_t delta = delta_time(time);
    //fprintf(stderr, "waitUntil(%d) delta=%d\n", time, delta);
    s4_t t=time-now;
    if (delta==0) return;
    if (t>0) { 
      //fprintf(stderr, "delay(%d)\n", t*US_PER_OSTICK/1000);
      delay(t*US_PER_OSTICK/1000);
      return;
    }
}

// check and rewind for target time
u1_t hal_checkTimer (u4_t time) {
    // No need to schedule wakeup, since we're not sleeping
//    fprintf(stderr, "hal_checkTimer(%d):%d (%d)\n", time,  delta_time(time), hal_ticks());
    return delta_time(time) <= 0;
}

static u8_t irqlevel = 0;

void IRQ0(void) {
//  fprintf(stderr, "IRQ0 %d\n", irqlevel);
  if (irqlevel==0) {
    radio_irq_handler(0);
    return;
  }
}

void IRQ1(void) {
  if (irqlevel==0){
    radio_irq_handler(1);
  }
}

void IRQ2(void) {
  if (irqlevel==0){
    radio_irq_handler(2);
  }
}

void hal_disableIRQs () {
//    cli();
    irqlevel++;
//    fprintf(stderr, "disableIRQs(%d)\n", irqlevel);
}

void hal_enableIRQs () {
    if(--irqlevel == 0) {
//      fprintf(stderr, "enableIRQs(%d)\n", irqlevel);
//        sei();

        // Instead of using proper interrupts (which are a bit tricky
        // and/or not available on all pins on AVR), just poll the pin
        // values. Since os_runloop disables and re-enables interrupts,
        // putting this here makes sure we check at least once every
        // loop.
        //
        // As an additional bonus, this prevents the can of worms that
        // we would otherwise get for running SPI transfers inside ISRs
        hal_io_check();
      }
  }

  void hal_sleep () {
      // Not implemented
  }

  void hal_failed (const char *file, u2_t line) {
    fprintf(stderr, "FAILURE\n");
    fprintf(stderr, "%s:%d\n",file, line);
    hal_disableIRQs();
    while(1);
}

void hal_init() {
    fd=wiringPiSetup();
    hal_io_init();
    // configure radio SPI
    hal_spi_init();
    // configure timer and interrupt handler
    hal_time_init();
    wiringPiISR(pins.dio[0], INT_EDGE_RISING, IRQ0);
    wiringPiISR(pins.dio[1], INT_EDGE_RISING, IRQ1);
    wiringPiISR(pins.dio[2], INT_EDGE_RISING, IRQ2);

  
}

