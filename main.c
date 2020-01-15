/*
 * Toggles LED on pin 1 on port N
 * Turns off LED on pin 0 on port N if button (USR_SW1 on port J pin 0) is pressed
 *
 * Already set build options of this project (no need to change anything):
 * Stack size set to 4096 byte
 * Heap size disabled - No malloc() available
 * No code optimization - Easier debugging
 * Strict floating point interrupt behavior
 * Minimal printf() and friends support - No floating point - reduces footprint
 * Hardware floating point unit activated
 */

#define TARGET_IS_TM4C129_RA2 /* Tells rom.h the version of the silicon */
#include <stdint.h> /* C99 header for uint*_t types */
#include <stdbool.h> /* Driverlib headers require stdbool.h to be included first */
#include <driverlib/gpio.h> /* Supplies GPIO* functions and GPIO_PIN_x */
#include <driverlib/sysctl.h> /* Supplies SysCtl* functions and SYSCTL_* macros */
#include <driverlib/rom.h> /* Supplies ROM_* variations of functions */
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */

#include "local_inc/bluetooth.h"

/* Controller is initially clocked with 16 MHz (via PIOSC) */
/* !!! Changing this macro does not change clock speed !!! */
#define F_CPU (16000000)

int main(void)
{
    uint32_t ui32Strength;
    uint32_t ui32PinType;
    uint8_t ui8button;
    uint8_t ui8toggle = (1<<1);

    //Clock auf 120MHZ setzen
    uint32_t clkFreq = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    /* Activate GPIO ports */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Set pin 0 of GPIO port J to digital input */
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);


    /* Enable pull-up for button on pin 0 of port J
     * Note the API difference between the *Get and *Set functions. */
    GPIOPadConfigGet(GPIO_PORTJ_BASE, 0, &ui32Strength, &ui32PinType);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, ui32Strength, GPIO_PIN_TYPE_STD_WPU);

    /* Set pins 0 & 1 of GPIO port N to digital output */
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);

    //enable UART peripheral
    setup_UART(clkFreq);


    while(1) {
        /* Toggle bit 1 in variable ui8toggle */
        ui8toggle = ui8toggle ^ (1<<1);
        /* Write value of ui8toggle to pin 1 of port N */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ui8toggle);

        /* Read value of pin 0 on port j into variable */
        ui8button = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        /* Set pin 0 of port N accordingly */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, ui8button);

        /* Delay for *approximately* 0.5 s.
         * 16M CPU cycles per second / 2 / 3 (SysCtlDelay factor) = 2666667
         * The actual delay can be longer due to interrupts! This is not suitable for exact timing. */
        ROM_SysCtlDelay(F_CPU/2/3);

    }
}
