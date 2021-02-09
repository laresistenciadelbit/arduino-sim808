# SIM8xx

This is a fork of the original SIM808 library for Arduino.
It was modified to work with the BK-SIM808 module along with another modules of the Sim8xx series.
There were added some functions to retrieve information from the SIM module like the internal clock and simple battery status.
There were added some delays to make the circuit more energy spikeless.

# --------

This library allows to access some of the features of the [SIM808](https://simcom.ee/documents/?dir=SIM808) GPS & GPRS module. It requires only the `RESET` pin to work and a TTL Serial. `STATUS` pin can be wired to enhance the module power status detection, while wiring the `PWRKEY` adds the ability to turn the module on & off.

The library tries to reduces memory consumption as much as possible, but nonetheless use a 64 bytes buffer to communicate with the SIM808 module. When available, SIM808 responses are parsed to ensure that commands are correctly executed by the module. Commands timeouts are also set according to SIMCOM documentation.  

> No default instance is created when the library is included

## Features
 * Fine control over the module power management
 * Sending SMS
 * Sending GET and POST [HTTP(s)](#a-note-about-https) requests
 * Acquiring GPS positions, with access to individual fields
 * Reading of the device states (battery, gps, network)

## Why another library ?
There is a number of libraries out there which support this modem ([Adafruit's FONA](https://github.com/adafruit/Adafruit_FONA), [TinyGSM](https://github.com/vshymanskyy/TinyGSM) for instance), so why build another one ? None fit the needs I had for a project. FONA is more a giant example for testing commands individually and I was getting unreliable results with it. TinyGSM seems great but what it gains in chips support it lacks in fine grained control over each modules, which I needed.

This library is then greatly inspired by FONA, which served as the reference implementation, but mostly only support the features I needed for my project and has been tested thoroughly and successfully in that configuration. It also tries to reduce the final HEX size as this was a real problem for the project it was built for.

It does *not* have the pretention to become the new SIM808 standard library, but can be useful to others as a source of inspiration or documentation to understand how AT commands works.

## Debugging
 If you need to debug the communication with the SIM808 module, you can either define `_DEBUG` to `1`, or directly change `_SIM808_DEBUG` to `1` in [SIMComAT.h](/src/SIMComAT.h).
 > Be aware that it will increase the final hex size as debug strings are stored in flash.

 ## Usage
 No default instance is created when the library is included. It's up to you to create one with the appropriate parameters.

 ```cpp
#include <SIM8xx.h>
#include <SoftwareSerial.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM8xx_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX);
SIM8xx SIM8xx = SIM8xx(SIM_RST, SIM_PWR, SIM_STATUS);
// SIM8xx sim = SIM8xx(SIM_RST); // if you only have the RESET pin wired
// SIM8xx sim = SIM8xx(SIM_RST, SIM_PWR); // if you only have the RESET and PWRKEY pins wired

void setup() {
    simSerial.begin(SIM8xx_BAUDRATE);
    sim.begin(simSerial);

    sim.powerOnOff(true);    //power on the SIM808. Unavailable without the PWRKEY pin wired
    sim.init();
}

void loop() {
    // whatever you need to do
}
 ```
See examples for further usage.