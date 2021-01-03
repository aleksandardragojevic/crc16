//
// Test the crc routine.
//
// author: aleksandar
//

#include <cstdio>
#include <cstring>

#include "../../crc16.h"

int main(int argc, char** argv) {
    printf("Reflect16(0x%x): 0x%x\n", 0x1021, Reflect(0x1021));
    printf("Reflect16(0x%x): 0x%x\n", 0x8005, Reflect(0x8005));

    const char data[] = "505";

    printf("xmodem(%s): 0x%x\n", data, Crc16Xmodem::Calc(data, strlen(data)));
    printf("kermit(%s): 0x%x\n", data, Crc16Kermit::Calc(data, strlen(data)));
    printf("CCIT-FALSE(%s): 0x%x\n", data, Crc16CcitFalse::Calc(data, strlen(data)));
    printf("Usb(%s): 0x%x\n", data, Crc16Usb::Calc(data, strlen(data)));

    return 0;
}

