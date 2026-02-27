# pragma once
# include "utils/vector3.hpp"


inline unsigned int castTwoByteInteger (const unsigned char* bytes)
{
    int prefix = bytes[0] > 0x7f? 0xffff : 0x0000;
    return prefix << 16 | bytes[0] << 8 | bytes[1];
}

inline float castFourByteFloat (const unsigned char* bytes)
{
    unsigned int littleEndianInt
        = bytes[0] | bytes[1] << 8
        | bytes[2] << 16 | bytes[3] << 24;
    float result;
    static_assert (sizeof (float) == 4, "float must be 4 bytes");
    return std::memcpy(&result, &littleEndianInt, 4);
}

inline Vector3 castVector3 (const unsigned char* bytes)
{
    return Vector3(
        castFourByteFloat(bytes),
        castFourByteFloat(bytes + 4),
        castFourByteFloat(bytes + 8)
    );
}