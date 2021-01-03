//
// Calculating crc16.
//
// If CRC calculation is reversed, the polynomial passed in needs to be
// be reversed by the code defining the Calc16 variant.
//
// We can define a constexpr function to calculate the correct polynomial
// at compile time, but I was a bit concerned about using older compilers.
// Instead, we expose a util function that reflects a passed in value. Its
// purpose is to be used by the code defining the Crc16 variant.
//
// author: aleksandar.dragojevic@gmail.com
//

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#elif defined(ARDUINO)
  #include "WProgram.h"
#else
  #include <cstdint>
#endif

template<
    typename IntType,
    size_t Bits = 16>
IntType Reflect(IntType val) {
    IntType res = 0;
    IntType check_mask = 1 << (Bits - 1);
    IntType set_mask = 1;

    while(check_mask != 0) {
        if(check_mask & val) {
            res |= set_mask;
        }

        check_mask >>= 1;
        set_mask <<= 1;
    }

    return res;
}

template<
    uint16_t Poly,
    uint16_t Init,
    uint16_t XorOut,
    bool Reverse,
    typename BufferSizeType = uint8_t>
class Crc16 {
public:
    static uint16_t Calc(const void* buf, size_t bytes) {
        return Calc(buf, static_cast<BufferSizeType>(bytes));
    }

    static uint16_t Calc(const void* buf, BufferSizeType bytes) {
        uint16_t crc = Init;

        for(BufferSizeType i = 0;i < bytes;i++) {
            auto next = static_cast<const uint8_t *>(buf)[i];

            if(Reverse) {
                crc = ProcessOneReverse(next, crc);
            } else {
                crc = ProcessOne(next, crc);
            }
        }

        crc ^= XorOut;

        return crc;
    }

private:
    static constexpr uint8_t Lsb = 1;

    static constexpr uint16_t Msb8 = 0x80;
    static constexpr uint16_t Msb16 = 0x8000;

    static constexpr uint16_t Shift1 = 1;
    static constexpr uint16_t Shift8 = 8;

    static uint16_t ProcessOne(uint8_t val, uint16_t crc) {
        for(uint8_t i = 0; i < 8; i++) {
            if(((crc & Msb16) >> Shift8) ^ (val & Msb8)) {
                crc = (crc << 1) ^ Poly;
            } else {
                crc <<= 1;
            }

            val <<= 1;
        }

        return crc;
    }

    static uint16_t ProcessOneReverse(uint8_t val, uint16_t crc) {
        crc ^= val;

        for(uint8_t i = Msb8;i != 0;i >>= Shift1) {
            if(crc & Lsb) {
                crc = (crc >> Shift1) ^ Poly;
            } else {
                crc = crc >> Shift1;
            }
        }

        return crc;
    }
};

using Crc16Xmodem = Crc16<0x1021, 0, 0, false>;
using Crc16Kermit = Crc16<0x8408, 0, 0, true>;
using Crc16CcitFalse = Crc16<0x1021, 0xFFFF, 0, false>;
using Crc16Usb = Crc16<0xA001, 0xFFFF, 0xFFFF, true>;
