/* Raiden 2 Sprite Decryption */

#include "emu.h"
#include "includes/raiden2.h"
#include "seibuspi.h"


/*
    The following tables have been built through analysis of the encrypted data but,
    as of now, it's unknown how they relate to the encryption tables uploaded by the
    games (which are probably modified by the Seibu COP before being used). It should be noted
    that, when combined, the total amounts of bits is lower (in raiden2 case) or equal
    (in zero team) to the number of bits of a 512*16 bits table (the same size than the
    bigger table uploaded).

    The size of the tables is different for both games, which points to the hypothesis
    that the position of them within a 512*16 table could be configured through programming.
    As illustration, these could be possible configurations:
    (r: rotate; x5: x5; x11: x11; u: unused)

    R2:
    |--|------|
    |  |  x11 |
    |  |      |
    |r |------|
    |  |  |   |
    |  |u |x5 |
    |--|--|---|

    ZT:
    |--|------|
    |r |      |
    |  |      |
    |--| x11  |
    |  |      |
    |x5|      |
    |--|------|

    This hypothesis has the strengh of explaining why x5 is indeed 5-bits wide instead of the
    much more symmetric and senseful 8-bits wide possibility (to allow configurations like
    the suggested one for zero team), but, of course, it should be taken just as what it is: a guess.
*/

static const UINT8 rotate_r2[512] = {
    0x11, 0x17, 0x0d, 0x03, 0x17, 0x1f, 0x08, 0x1a, 0x0f, 0x04, 0x1e, 0x13, 0x19, 0x0e, 0x0e, 0x05,
    0x06, 0x07, 0x08, 0x08, 0x0d, 0x18, 0x11, 0x1a, 0x0b, 0x06, 0x12, 0x0c, 0x1f, 0x0b, 0x1c, 0x19,
    0x00, 0x1b, 0x0c, 0x09, 0x1d, 0x18, 0x1a, 0x16, 0x1a, 0x08, 0x03, 0x04, 0x0f, 0x1d, 0x16, 0x07,
    0x1a, 0x12, 0x01, 0x0b, 0x00, 0x0f, 0x1e, 0x10, 0x09, 0x0f, 0x10, 0x09, 0x0a, 0x1c, 0x0d, 0x08,
    0x06, 0x1a, 0x06, 0x02, 0x11, 0x1e, 0x0c, 0x1c, 0x11, 0x0f, 0x19, 0x0a, 0x16, 0x14, 0x18, 0x11,
    0x0b, 0x0d, 0x1c, 0x1f, 0x0d, 0x1f, 0x0d, 0x19, 0x0d, 0x04, 0x19, 0x0f, 0x06, 0x13, 0x0c, 0x1b,
    0x1f, 0x12, 0x15, 0x1a, 0x04, 0x02, 0x06, 0x03, 0x0a, 0x0d, 0x12, 0x09, 0x17, 0x1d, 0x12, 0x10,
    0x05, 0x07, 0x03, 0x00, 0x14, 0x07, 0x14, 0x1a, 0x1c, 0x0a, 0x10, 0x0f, 0x0b, 0x0c, 0x08, 0x0f,
    0x07, 0x00, 0x13, 0x1c, 0x04, 0x15, 0x0e, 0x02, 0x17, 0x17, 0x00, 0x03, 0x18, 0x00, 0x02, 0x13,
    0x14, 0x0c, 0x01, 0x0a, 0x15, 0x0b, 0x0a, 0x1c, 0x1b, 0x06, 0x17, 0x1d, 0x11, 0x1f, 0x10, 0x04,
    0x1a, 0x01, 0x1b, 0x13, 0x03, 0x09, 0x09, 0x0f, 0x0d, 0x03, 0x15, 0x1c, 0x04, 0x06, 0x06, 0x0b,
    0x04, 0x0a, 0x1f, 0x16, 0x11, 0x0a, 0x05, 0x05, 0x0c, 0x1c, 0x10, 0x0c, 0x11, 0x04, 0x10, 0x1a,
    0x06, 0x10, 0x19, 0x06, 0x15, 0x0f, 0x11, 0x01, 0x10, 0x0c, 0x1d, 0x05, 0x1f, 0x05, 0x12, 0x16,
    0x02, 0x12, 0x14, 0x0d, 0x14, 0x0f, 0x04, 0x07, 0x13, 0x01, 0x11, 0x1c, 0x1c, 0x1d, 0x0e, 0x06,
    0x1d, 0x13, 0x10, 0x06, 0x0f, 0x02, 0x12, 0x10, 0x1e, 0x0c, 0x17, 0x15, 0x0b, 0x1f, 0x01, 0x19,
    0x02, 0x01, 0x07, 0x1d, 0x13, 0x19, 0x0f, 0x0f, 0x10, 0x03, 0x1e, 0x03, 0x0d, 0x0a, 0x0c, 0x0d,

    0x16, 0x1f, 0x16, 0x1a, 0x1c, 0x16, 0x01, 0x03, 0x01, 0x08, 0x14, 0x19, 0x03, 0x1e, 0x08, 0x02,
    0x02, 0x1d, 0x15, 0x00, 0x09, 0x1d, 0x03, 0x11, 0x11, 0x0b, 0x1b, 0x14, 0x01, 0x1e, 0x11, 0x12,
    0x1d, 0x06, 0x0b, 0x13, 0x1e, 0x16, 0x0d, 0x10, 0x11, 0x1f, 0x1c, 0x15, 0x0d, 0x1a, 0x13, 0x1f,
    0x0e, 0x05, 0x10, 0x06, 0x0d, 0x1c, 0x07, 0x19, 0x06, 0x1d, 0x11, 0x00, 0x1c, 0x05, 0x0b, 0x1d,
    0x1c, 0x06, 0x05, 0x1d, 0x00, 0x13, 0x00, 0x12, 0x1b, 0x17, 0x1a, 0x1b, 0x17, 0x1c, 0x16, 0x0a,
    0x11, 0x15, 0x0f, 0x0b, 0x0f, 0x07, 0x0e, 0x04, 0x13, 0x00, 0x1c, 0x05, 0x16, 0x00, 0x1a, 0x04,
    0x17, 0x04, 0x08, 0x1b, 0x05, 0x12, 0x1d, 0x0d, 0x02, 0x16, 0x12, 0x0e, 0x06, 0x08, 0x14, 0x07,
    0x0e, 0x0f, 0x15, 0x13, 0x12, 0x00, 0x1d, 0x16, 0x1b, 0x18, 0x1f, 0x05, 0x12, 0x13, 0x01, 0x0c,
    0x12, 0x04, 0x19, 0x13, 0x12, 0x15, 0x07, 0x06, 0x0a, 0x00, 0x09, 0x14, 0x1e, 0x03, 0x10, 0x1b,
    0x08, 0x1a, 0x07, 0x02, 0x1b, 0x0d, 0x18, 0x13, 0x02, 0x07, 0x1e, 0x05, 0x15, 0x02, 0x06, 0x18,
    0x12, 0x09, 0x1c, 0x07, 0x0b, 0x02, 0x03, 0x00, 0x18, 0x18, 0x03, 0x0f, 0x02, 0x0f, 0x10, 0x09,
    0x05, 0x18, 0x08, 0x1b, 0x0d, 0x10, 0x03, 0x00, 0x0c, 0x14, 0x1d, 0x08, 0x02, 0x10, 0x0b, 0x0c,
    0x00, 0x0d, 0x0d, 0x0a, 0x06, 0x1c, 0x09, 0x19, 0x1b, 0x14, 0x18, 0x0f, 0x02, 0x07, 0x05, 0x04,
    0x1c, 0x15, 0x18, 0x00, 0x0b, 0x10, 0x19, 0x1c, 0x1b, 0x08, 0x1d, 0x12, 0x17, 0x1d, 0x0c, 0x01,
    0x03, 0x0d, 0x03, 0x0d, 0x15, 0x0e, 0x16, 0x08, 0x05, 0x11, 0x1f, 0x03, 0x16, 0x03, 0x0f, 0x10,
    0x08, 0x19, 0x18, 0x15, 0x1f, 0x05, 0x00, 0x09, 0x0e, 0x05, 0x16, 0x1b, 0x01, 0x08, 0x08, 0x1f,
};

static const UINT8 x5_r2[256] = {
    0x08, 0x09, 0x1f, 0x0f, 0x09, 0x09, 0x0b, 0x1d, 0x06, 0x13, 0x02, 0x15, 0x02, 0x0c, 0x0d, 0x19,
    0x03, 0x13, 0x0c, 0x1f, 0x1a, 0x18, 0x17, 0x10, 0x0a, 0x19, 0x15, 0x04, 0x1f, 0x11, 0x1c, 0x02,
    0x0e, 0x08, 0x06, 0x0a, 0x07, 0x1c, 0x10, 0x04, 0x11, 0x0c, 0x0a, 0x19, 0x0a, 0x04, 0x17, 0x07,
    0x16, 0x1b, 0x1d, 0x15, 0x1d, 0x13, 0x0e, 0x03, 0x1a, 0x11, 0x14, 0x14, 0x03, 0x18, 0x07, 0x03,
    0x08, 0x1a, 0x02, 0x0f, 0x0b, 0x11, 0x1c, 0x05, 0x19, 0x1d, 0x05, 0x01, 0x1f, 0x1c, 0x1d, 0x07,
    0x07, 0x0c, 0x02, 0x16, 0x0e, 0x06, 0x0b, 0x07, 0x01, 0x1a, 0x09, 0x0e, 0x0e, 0x07, 0x0e, 0x15,
    0x01, 0x16, 0x13, 0x15, 0x14, 0x07, 0x0c, 0x1f, 0x1f, 0x19, 0x17, 0x12, 0x19, 0x17, 0x0a, 0x1f,
    0x0c, 0x16, 0x15, 0x1e, 0x05, 0x14, 0x05, 0x1c, 0x0b, 0x0d, 0x0c, 0x0a, 0x05, 0x09, 0x14, 0x02,
    0x10, 0x02, 0x13, 0x05, 0x12, 0x17, 0x03, 0x0b, 0x1b, 0x06, 0x15, 0x0b, 0x01, 0x0b, 0x1b, 0x09,
    0x10, 0x0a, 0x1e, 0x09, 0x08, 0x0a, 0x04, 0x13, 0x04, 0x12, 0x04, 0x0f, 0x0b, 0x0c, 0x06, 0x07,
    0x03, 0x18, 0x00, 0x1e, 0x17, 0x00, 0x16, 0x08, 0x0d, 0x1c, 0x09, 0x07, 0x17, 0x18, 0x0b, 0x0d,
    0x11, 0x0f, 0x14, 0x1e, 0x1a, 0x1b, 0x09, 0x15, 0x03, 0x07, 0x12, 0x16, 0x15, 0x11, 0x16, 0x1e,
    0x14, 0x15, 0x00, 0x05, 0x15, 0x18, 0x18, 0x12, 0x18, 0x1e, 0x06, 0x06, 0x0c, 0x1a, 0x04, 0x0b,
    0x05, 0x08, 0x04, 0x1f, 0x0c, 0x08, 0x0a, 0x1f, 0x1a, 0x16, 0x0e, 0x1e, 0x16, 0x18, 0x18, 0x05,
    0x00, 0x1a, 0x05, 0x15, 0x19, 0x10, 0x03, 0x0e, 0x10, 0x1c, 0x0a, 0x18, 0x00, 0x16, 0x0b, 0x05,
    0x05, 0x15, 0x11, 0x0a, 0x1c, 0x00, 0x1e, 0x1f, 0x17, 0x12, 0x0a, 0x1c, 0x07, 0x04, 0x1f, 0x1a,
};

static const UINT16 x11_r2[256] = {
    0x347, 0x0f2, 0x182, 0x58f, 0x1f4, 0x42c, 0x407, 0x5f0, 0x6df, 0x2db, 0x585, 0x5fe, 0x394, 0x542, 0x3e8, 0x574,
    0x4ea, 0x6d3, 0x6b7, 0x65b, 0x324, 0x143, 0x22a, 0x11d, 0x124, 0x365, 0x7ca, 0x3d6, 0x1d2, 0x7cd, 0x6b1, 0x4f1,
    0x1de, 0x674, 0x685, 0x779, 0x264, 0x6d8, 0x379, 0x7ce, 0x201, 0x73b, 0x5c9, 0x025, 0x338, 0x4b2, 0x697, 0x567,
    0x312, 0x04e, 0x78d, 0x492, 0x044, 0x203, 0x437, 0x04b, 0x729, 0x197, 0x6e2, 0x552, 0x517, 0x3c9, 0x09c, 0x3de,
    0x2f8, 0x259, 0x1f0, 0x6ce, 0x6d6, 0x55d, 0x223, 0x65e, 0x7ca, 0x330, 0x3f7, 0x348, 0x640, 0x26d, 0x340, 0x2df,
    0x752, 0x792, 0x5b0, 0x2fb, 0x398, 0x75c, 0x0a2, 0x524, 0x538, 0x74c, 0x1c5, 0x5a2, 0x522, 0x7c3, 0x6b3, 0x4f0,
    0x5ac, 0x40b, 0x3e0, 0x1c8, 0x6ff, 0x291, 0x7c4, 0x47c, 0x6d9, 0x248, 0x623, 0x78d, 0x2cd, 0x356, 0x12a, 0x0bc,
    0x582, 0x1d8, 0x1c6, 0x6eb, 0x7c2, 0x7f9, 0x650, 0x57d, 0x701, 0x7e5, 0x118, 0x1b4, 0x4ad, 0x2b8, 0x2bb, 0x765,
    0x2d9, 0x46a, 0x020, 0x2da, 0x5e4, 0x115, 0x53c, 0x2b4, 0x16d, 0x0f7, 0x633, 0x1a6, 0x0a0, 0x3e6, 0x29d, 0x77b,
    0x558, 0x185, 0x7b9, 0x0b1, 0x36e, 0x4d3, 0x7e2, 0x5f9, 0x3d2, 0x21e, 0x0e1, 0x2ac, 0x0fc, 0x0fc, 0x66d, 0x7b5,
    0x4af, 0x627, 0x0f4, 0x621, 0x58f, 0x3d7, 0x1bc, 0x10a, 0x458, 0x259, 0x451, 0x770, 0x107, 0x134, 0x162, 0x32f,
    0x5cf, 0x6c9, 0x670, 0x2d4, 0x0da, 0x739, 0x30c, 0x62f, 0x4af, 0x0e2, 0x3e3, 0x65c, 0x214, 0x066, 0x47d, 0x2f2,
    0x729, 0x566, 0x450, 0x3f2, 0x35d, 0x593, 0x593, 0x532, 0x008, 0x270, 0x479, 0x358, 0x6f3, 0x7ed, 0x240, 0x587,
    0x581, 0x00f, 0x750, 0x4d8, 0x1ab, 0x100, 0x47f, 0x34f, 0x497, 0x240, 0x769, 0x76f, 0x705, 0x375, 0x684, 0x273,
    0x01f, 0x268, 0x2cc, 0x2d7, 0x5d4, 0x284, 0x40c, 0x5e8, 0x7c1, 0x281, 0x518, 0x4b0, 0x136, 0x73b, 0x3ea, 0x023,
    0x1c1, 0x7de, 0x106, 0x275, 0x1e1, 0x503, 0x30a, 0x271, 0x4f8, 0x52b, 0x266, 0x375, 0x024, 0x399, 0x672, 0x6f8,
};

static const UINT8 rotate_zt[256] = {
    0x0a, 0x0a, 0x0b, 0x0e, 0x02, 0x12, 0x0d, 0x16, 0x01, 0x1b, 0x08, 0x15, 0x03, 0x14, 0x0f, 0x1f,
    0x10, 0x1d, 0x03, 0x0b, 0x16, 0x07, 0x10, 0x19, 0x1f, 0x1c, 0x03, 0x0b, 0x15, 0x0d, 0x00, 0x14,
    0x15, 0x0d, 0x0c, 0x1b, 0x18, 0x18, 0x0b, 0x1b, 0x16, 0x17, 0x01, 0x0e, 0x14, 0x0c, 0x1e, 0x13,
    0x13, 0x12, 0x19, 0x0b, 0x1b, 0x1a, 0x0d, 0x04, 0x00, 0x00, 0x12, 0x06, 0x0f, 0x1c, 0x19, 0x1b,
    0x01, 0x04, 0x15, 0x1c, 0x02, 0x13, 0x0f, 0x09, 0x1c, 0x05, 0x09, 0x05, 0x0d, 0x17, 0x13, 0x1f,
    0x17, 0x19, 0x0f, 0x04, 0x05, 0x03, 0x0c, 0x03, 0x13, 0x02, 0x09, 0x0d, 0x1d, 0x1a, 0x02, 0x0d,
    0x0d, 0x0f, 0x13, 0x1c, 0x05, 0x03, 0x09, 0x08, 0x03, 0x1d, 0x05, 0x1f, 0x0b, 0x15, 0x06, 0x10,
    0x0c, 0x15, 0x18, 0x0c, 0x18, 0x1f, 0x1d, 0x1d, 0x1d, 0x0e, 0x1f, 0x0c, 0x14, 0x0d, 0x0a, 0x08,
    0x0f, 0x12, 0x08, 0x06, 0x0d, 0x1d, 0x19, 0x1e, 0x1c, 0x08, 0x09, 0x1e, 0x1a, 0x14, 0x08, 0x17,
    0x11, 0x0e, 0x1a, 0x18, 0x01, 0x08, 0x05, 0x01, 0x06, 0x1e, 0x15, 0x1d, 0x0c, 0x09, 0x1b, 0x07,
    0x12, 0x00, 0x0c, 0x05, 0x03, 0x08, 0x19, 0x09, 0x09, 0x06, 0x13, 0x12, 0x0b, 0x1a, 0x09, 0x09,
    0x03, 0x09, 0x00, 0x07, 0x1c, 0x10, 0x14, 0x01, 0x17, 0x03, 0x05, 0x11, 0x1e, 0x12, 0x14, 0x19,
    0x10, 0x10, 0x08, 0x08, 0x07, 0x0c, 0x12, 0x1f, 0x02, 0x08, 0x1f, 0x14, 0x1e, 0x14, 0x02, 0x13,
    0x04, 0x10, 0x1e, 0x08, 0x0b, 0x14, 0x19, 0x1a, 0x07, 0x1c, 0x13, 0x07, 0x10, 0x14, 0x15, 0x1e,
    0x18, 0x0a, 0x0c, 0x1f, 0x0e, 0x05, 0x11, 0x08, 0x04, 0x06, 0x12, 0x0c, 0x00, 0x04, 0x0b, 0x07,
    0x14, 0x0a, 0x0c, 0x16, 0x02, 0x0a, 0x0f, 0x0d, 0x0c, 0x1d, 0x1e, 0x13, 0x0a, 0x08, 0x12, 0x0b,
};

static const UINT8 x5_zt[256] = {
    0x0d, 0x14, 0x13, 0x1b, 0x1a, 0x05, 0x1a, 0x05, 0x1b, 0x17, 0x0d, 0x0f, 0x00, 0x17, 0x09, 0x04,
    0x13, 0x0f, 0x14, 0x17, 0x14, 0x0f, 0x1b, 0x0a, 0x0a, 0x15, 0x07, 0x0b, 0x0e, 0x05, 0x18, 0x12,
    0x1c, 0x01, 0x17, 0x02, 0x04, 0x00, 0x06, 0x12, 0x00, 0x0a, 0x1c, 0x19, 0x10, 0x1b, 0x16, 0x03,
    0x1c, 0x13, 0x16, 0x13, 0x15, 0x1d, 0x04, 0x14, 0x17, 0x1d, 0x01, 0x03, 0x1f, 0x1f, 0x1a, 0x10,
    0x08, 0x0c, 0x09, 0x16, 0x04, 0x19, 0x03, 0x14, 0x0c, 0x1d, 0x09, 0x08, 0x00, 0x1c, 0x0b, 0x18,
    0x1b, 0x00, 0x16, 0x0f, 0x0b, 0x1c, 0x0d, 0x16, 0x09, 0x15, 0x0c, 0x01, 0x18, 0x04, 0x04, 0x03,
    0x11, 0x0f, 0x03, 0x17, 0x04, 0x0a, 0x05, 0x1b, 0x08, 0x07, 0x0e, 0x08, 0x06, 0x16, 0x0f, 0x0d,
    0x0a, 0x14, 0x0a, 0x11, 0x14, 0x02, 0x08, 0x1f, 0x0e, 0x13, 0x0e, 0x02, 0x08, 0x13, 0x04, 0x19,
    0x0e, 0x00, 0x01, 0x1d, 0x02, 0x19, 0x08, 0x1c, 0x13, 0x09, 0x11, 0x14, 0x1e, 0x04, 0x03, 0x18,
    0x1f, 0x05, 0x0f, 0x03, 0x09, 0x0d, 0x12, 0x0f, 0x0d, 0x0e, 0x15, 0x1a, 0x17, 0x1c, 0x1d, 0x10,
    0x15, 0x00, 0x18, 0x18, 0x00, 0x08, 0x08, 0x01, 0x0d, 0x10, 0x14, 0x12, 0x03, 0x1c, 0x05, 0x0a,
    0x0f, 0x0d, 0x15, 0x00, 0x0b, 0x1f, 0x00, 0x1d, 0x1d, 0x1a, 0x13, 0x16, 0x08, 0x1b, 0x03, 0x12,
    0x02, 0x10, 0x0e, 0x10, 0x04, 0x05, 0x09, 0x07, 0x1d, 0x04, 0x05, 0x06, 0x17, 0x05, 0x1a, 0x04,
    0x1f, 0x0e, 0x09, 0x13, 0x15, 0x12, 0x1c, 0x13, 0x03, 0x09, 0x12, 0x08, 0x19, 0x17, 0x00, 0x19,
    0x1f, 0x06, 0x1c, 0x05, 0x18, 0x0a, 0x1f, 0x1e, 0x0d, 0x05, 0x1e, 0x1a, 0x11, 0x13, 0x15, 0x0c,
    0x05, 0x16, 0x11, 0x06, 0x12, 0x0c, 0x0c, 0x0f, 0x1c, 0x18, 0x08, 0x1e, 0x1c, 0x16, 0x15, 0x03,
};

static const UINT16 x11_zt[512] = {
    0x7e9, 0x625, 0x625, 0x006, 0x006, 0x14a, 0x14a, 0x7e9, 0x657, 0x417, 0x417, 0x1a7, 0x1a7, 0x22f, 0x22f, 0x657,
    0x37e, 0x447, 0x447, 0x117, 0x117, 0x735, 0x735, 0x37e, 0x3be, 0x487, 0x487, 0x724, 0x724, 0x6e3, 0x6e3, 0x3be,
    0x646, 0x035, 0x035, 0x57e, 0x57e, 0x134, 0x134, 0x646, 0x09a, 0x4d1, 0x4d1, 0x511, 0x511, 0x5d5, 0x5d5, 0x09a,
    0x3e2, 0x5c6, 0x5c6, 0x3b6, 0x3b6, 0x354, 0x354, 0x3e2, 0x6e3, 0x7d6, 0x7d6, 0x317, 0x317, 0x1f0, 0x1f0, 0x6e3,
    0x115, 0x462, 0x462, 0x620, 0x620, 0x0ce, 0x0ce, 0x115, 0x715, 0x7ec, 0x7ec, 0x1e6, 0x1e6, 0x610, 0x610, 0x715,
    0x34a, 0x718, 0x718, 0x7ba, 0x7ba, 0x288, 0x288, 0x34a, 0x6e7, 0x193, 0x193, 0x3d2, 0x3d2, 0x78f, 0x78f, 0x6e7,
    0x57c, 0x014, 0x014, 0x6b3, 0x6b3, 0x204, 0x204, 0x57c, 0x3f4, 0x782, 0x782, 0x5b4, 0x5b4, 0x375, 0x375, 0x3f4,
    0x17d, 0x660, 0x660, 0x48d, 0x48d, 0x7ff, 0x7ff, 0x17d, 0x5b1, 0x39d, 0x39d, 0x48c, 0x48c, 0x5ab, 0x5ab, 0x5b1,
    0x3d3, 0x4f6, 0x4f6, 0x7b9, 0x7b9, 0x5df, 0x5df, 0x3d3, 0x349, 0x4c5, 0x4c5, 0x403, 0x403, 0x0ab, 0x0ab, 0x349,
    0x46b, 0x790, 0x790, 0x59f, 0x59f, 0x2b1, 0x2b1, 0x46b, 0x6c5, 0x4e7, 0x4e7, 0x023, 0x023, 0x0b0, 0x0b0, 0x6c5,
    0x2d6, 0x3e2, 0x3e2, 0x7f3, 0x7f3, 0x376, 0x376, 0x2d6, 0x2d7, 0x0bf, 0x0bf, 0x14b, 0x14b, 0x65b, 0x65b, 0x2d7,
    0x68d, 0x360, 0x360, 0x434, 0x434, 0x5fa, 0x5fa, 0x68d, 0x654, 0x386, 0x386, 0x28b, 0x28b, 0x2e1, 0x2e1, 0x654,
    0x1ee, 0x4e8, 0x4e8, 0x482, 0x482, 0x471, 0x471, 0x1ee, 0x221, 0x3de, 0x3de, 0x467, 0x467, 0x21b, 0x21b, 0x221,
    0x7a6, 0x6b6, 0x6b6, 0x157, 0x157, 0x658, 0x658, 0x7a6, 0x1e9, 0x6e3, 0x6e3, 0x340, 0x340, 0x2cd, 0x2cd, 0x1e9,
    0x045, 0x566, 0x566, 0x1f0, 0x1f0, 0x5a1, 0x5a1, 0x045, 0x559, 0x0d4, 0x0d4, 0x67c, 0x67c, 0x21a, 0x21a, 0x559,
    0x187, 0x13b, 0x13b, 0x68e, 0x68e, 0x0f6, 0x0f6, 0x187, 0x2b5, 0x4c2, 0x4c2, 0x2b6, 0x2b6, 0x6a4, 0x6a4, 0x2b5,
    0x56b, 0x1b2, 0x1b2, 0x15d, 0x15d, 0x3aa, 0x3aa, 0x56b, 0x506, 0x67a, 0x67a, 0x5db, 0x5db, 0x5ef, 0x5ef, 0x506,
    0x5ab, 0x3ed, 0x3ed, 0x2b3, 0x2b3, 0x504, 0x504, 0x5ab, 0x137, 0x761, 0x761, 0x735, 0x735, 0x6e9, 0x6e9, 0x137,
    0x35a, 0x7bb, 0x7bb, 0x190, 0x190, 0x0a8, 0x0a8, 0x35a, 0x797, 0x164, 0x164, 0x1b8, 0x1b8, 0x761, 0x761, 0x797,
    0x138, 0x076, 0x076, 0x613, 0x613, 0x055, 0x055, 0x138, 0x2e2, 0x493, 0x493, 0x0c2, 0x0c2, 0x785, 0x785, 0x2e2,
    0x60b, 0x41a, 0x41a, 0x22a, 0x22a, 0x2a1, 0x2a1, 0x60b, 0x5cb, 0x092, 0x092, 0x656, 0x656, 0x5db, 0x5db, 0x5cb,
    0x00a, 0x490, 0x490, 0x39d, 0x39d, 0x572, 0x572, 0x00a, 0x600, 0x4cd, 0x4cd, 0x33f, 0x33f, 0x0bc, 0x0bc, 0x600,
    0x047, 0x685, 0x685, 0x4b0, 0x4b0, 0x330, 0x330, 0x047, 0x7b4, 0x590, 0x590, 0x1db, 0x1db, 0x349, 0x349, 0x7b4,
    0x6ae, 0x7a3, 0x7a3, 0x6fa, 0x6fa, 0x2d4, 0x2d4, 0x6ae, 0x117, 0x2fe, 0x2fe, 0x756, 0x756, 0x627, 0x627, 0x117,
    0x337, 0x55e, 0x55e, 0x2ef, 0x2ef, 0x2a9, 0x2a9, 0x337, 0x382, 0x02a, 0x02a, 0x4af, 0x4af, 0x7fa, 0x7fa, 0x382,
    0x73f, 0x6d8, 0x6d8, 0x7d2, 0x7d2, 0x52d, 0x52d, 0x73f, 0x385, 0x303, 0x303, 0x3eb, 0x3eb, 0x689, 0x689, 0x385,
    0x6cd, 0x4c3, 0x4c3, 0x2df, 0x2df, 0x1d5, 0x1d5, 0x6cd, 0x7da, 0x44c, 0x44c, 0x764, 0x764, 0x7f6, 0x7f6, 0x7da,
    0x7c1, 0x276, 0x276, 0x27f, 0x27f, 0x1c9, 0x1c9, 0x7c1, 0x58c, 0x001, 0x001, 0x07f, 0x07f, 0x62b, 0x62b, 0x58c,
    0x374, 0x746, 0x746, 0x5b2, 0x5b2, 0x35e, 0x35e, 0x374, 0x64d, 0x1fd, 0x1fd, 0x2dc, 0x2dc, 0x550, 0x550, 0x64d,
    0x33d, 0x40c, 0x40c, 0x3a4, 0x3a4, 0x293, 0x293, 0x33d, 0x0c0, 0x462, 0x462, 0x6fb, 0x6fb, 0x4b0, 0x4b0, 0x0c0,
    0x2aa, 0x175, 0x175, 0x01e, 0x01e, 0x762, 0x762, 0x2aa, 0x216, 0x76b, 0x76b, 0x728, 0x728, 0x15f, 0x15f, 0x216,
    0x30e, 0x0d8, 0x0d8, 0x55c, 0x55c, 0x31c, 0x31c, 0x30e, 0x351, 0x3dc, 0x3dc, 0x6c1, 0x6c1, 0x651, 0x651, 0x351,
};


static UINT32 yrot(UINT32 v, int r)
{
    return (v << r) | (v >> (32-r));
}

static UINT16 gm(int i4)
{
    UINT16 x=0;

    for (int i=0; i<4; ++i)
    {
        if (BIT(i4,i))
            x ^= 0xf << (i<<2);
    }

    return x;
}

static UINT32 core_decrypt(UINT32 ciphertext, int i1, int i2, int i3, int i4,
                            const UINT8 *rotate, const UINT8 *x5, const UINT16 *x11, UINT32 preXor, UINT32 carryMask, UINT32 postXor)
{
    UINT32 v1 = BITSWAP32(yrot(ciphertext, rotate[i1]), 25,28,15,19, 6,0,3,24, 11,1,2,30, 16,7,22,17, 31,14,23,9, 27,18,4,10, 13,20,5,12, 8,29,26,21);

    UINT16 x1Low = (x5[i2]<<11) ^ x11[i3] ^ gm(i4);
    UINT32 x1 = x1Low | (BITSWAP16(x1Low, 0,8,1,9, 2,10,3,11, 4,12,5,13, 6,14,7,15)<<16);

    return partial_carry_sum32(v1, x1^preXor, carryMask) ^ postXor;
}

void raiden2_decrypt_sprites(running_machine &machine)
{
    UINT32 *data = (UINT32 *)machine.root_device().memregion("gfx3")->base();
    for(int i=0; i<0x800000/4; i++)
    {
        data[i] = core_decrypt(data[i],
            (i&0xff) ^ BIT(i,15) ^ (BIT(i,20)<<8),
            (i&0xff) ^ BIT(i,15),
            (i>>8) & 0xff,
            (i>>16) & 0xf,
            rotate_r2,
            x5_r2,
            x11_r2,
            0x60860000,
            0x176c91a8,
            0x0f488000
        );
    }
}

void zeroteam_decrypt_sprites(running_machine &machine)
{
    UINT32 *data = (UINT32 *)machine.root_device().memregion("gfx3")->base();
    for(int i=0; i<0x400000/4; i++)
    {
        data[i] = core_decrypt(data[i],
            i & 0xff,
            i & 0xff,
            (i>>7) & 0x1ff,
            (i>>16) & 0xf,
            rotate_zt,
            x5_zt,
            x11_zt,
            0xa5800000,
            0x7b67b7b9,
            0xf1412ea8
        );
    }
}
