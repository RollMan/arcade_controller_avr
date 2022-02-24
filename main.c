/* Name: main.c
 * Project: hid-mouse, a very simple HID example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-07
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.

We use VID/PID 0x046D/0xC00E which is taken from a Logitech mouse. Don't
publish any hardware using these IDs! This is for demonstration only!
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM const char usbDescriptorHidReport[56] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x39,                    //   USAGE (Hat switch)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x07,                    //   LOGICAL_MAXIMUM (7)
    0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
    0x46, 0x3b, 0x01,              //   PHYSICAL_MAXIMUM (315)
    0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x42,                    //   INPUT (Data,Var,Abs,Null)
    0x65, 0x00,                    //   UNIT (None)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x0a,                    //   USAGE_MAXIMUM (Button 10)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x95, 0x0a,                    //   REPORT_COUNT (10)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};
/*
 * |   7|    6|    5|    4|    3|    2|    1|    0|
 * |----|-----|-----|-----|-----|-----|-----|-----|
 * |   x|    x|    x|    x|    S|    N|    E|    W|
 * |  b7|   b6|   b5|   b4|   b3|   b2|   b1|   b0|
 * |    |   xx|   xx|   xx|   xx|   xx|   b9|   b8|
 */

typedef struct{
    uint8_t rot;
    uint8_t button_lower;
    uint8_t button_upper;
}report_t;

typedef enum DESCRIPTOR_PADSWITCH {
  DPAD_N        = 0x00,
  DPAD_NE       = 0x01,
  DPAD_E        = 0x02,
  DPAD_SE       = 0x03,
  DPAD_S        = 0x04,
  DPAD_SW       = 0x05,
  DPAD_W        = 0x06,
  DPAD_NW       = 0x07,
  DPAD_RELEASED = 0x08,
} PADSWITCH_BITS;

static report_t reportBuffer;
static uchar    idleRate;   /* repeat rate for keyboards, never used for mice */

static char parseStick(uint8_t port){
  uint8_t res = 0;
  uint8_t X_DOWN = !(port & (1 << 0));
  uint8_t X_UP = !(port & (1 << 1));
  uint8_t Y_UP = !(port & (1 << 2));
  uint8_t Y_DOWN = !(port & (1 << 3));

  if (X_UP){
    if (Y_UP){
      res = DPAD_NE;
    }else if (Y_DOWN){
      res = DPAD_SE;
    }else{
      res = DPAD_E;
    }
  }else if (X_DOWN){
    if (Y_UP){
      res = DPAD_NW;
    }else if (Y_DOWN){
      res = DPAD_SW;
    }else{
      res = DPAD_W;
    }
  }else if (Y_UP){
    res = DPAD_N;
  }else if (Y_DOWN){
    res = DPAD_S;
  }else{
    res = DPAD_RELEASED;
  }


  return res;
}

static void pollButtons(void){
    /*
     * |      | 7  | 6   | 5  | 4  | 3  | 2  | 1  | 0  |
     * |------|----|-----|----|----|----|----|----|----|
     * | PINB | .  | .   | B9 | B8 | -Y | +Y | -X | +X |
     * | PINC | x  | RST | B5 | B4 | B3 | B2 | B1 | B0 |
     * | PIND | B7 | B6  |  x | D- |  x | D+ |  x |  x |
     */


     reportBuffer.rot = parseStick(PINB);
     reportBuffer.button_lower = ~((0xC0 & PIND)  | (0x3f & PINC));
     reportBuffer.button_upper = ~(((0x30 & PINB) >> 4));
     /*
      * TODO:
      *   PINC7 (Button 7) は存在せず、PINB4 (Button 8), PINB5 (Button 9) は SPI 書き込み用に予約、PINC6 はリセットに予約なので
      *   PIND (現在 USB バスとして使用中) を代わりに使えないか検討。
      */
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    /* The following requests are never used. But since they are required by
     * the specification, we implement them in this example.
     */
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        DBG1(0x50, &rq->bRequest, 1);   /* debug output: print our request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            usbMsgPtr = (void *)&reportBuffer;
            return sizeof(reportBuffer);
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = &idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        /* no vendor specific requests implemented */
    }
    return 0;   /* default for not implemented requests: return no data back to host */
}

/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
uchar   i;

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    odDebugInit();
    DBG1(0x00, 0, 0);       /* debug output: main starts */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    DBG1(0x01, 0, 0);       /* debug output: main loop starts */

    DDRB = 0x00;
    DDRC = 0x00;
    DDRD = (DDRD & 0x3f);
    PORTD = (PORTD | 0xC0);

    DDRB = 0x00;
    DDRC = 0x00;
    PORTB = 0xff;
    PORTC = 0xff;
    for(;;){                /* main event loop */
        DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
        wdt_reset();
        usbPoll();
        if(usbInterruptIsReady()){
            /* called after every poll of the interrupt endpoint */
            pollButtons();
            DBG1(0x03, 0, 0);   /* debug output: interrupt report prepared */
            usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
        }
    }
}

/* ------------------------------------------------------------------------- */
