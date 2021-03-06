#include "yocto/gfx/color/ramp/cold_to_hot.hpp"
namespace yocto { namespace gfx{

static const RGBA cold_to_hot_codes[256]={
RGBA(0x00,0x00,0xff),RGBA(0x00,0x03,0xff),RGBA(0x00,0x07,0xff),RGBA(0x00,0x0b,0xff),RGBA(0x00,0x0f,0xff),RGBA(0x00,0x13,0xff),RGBA(0x00,0x17,0xff),RGBA(0x00,0x1b,0xff),
RGBA(0x00,0x1f,0xff),RGBA(0x00,0x23,0xff),RGBA(0x00,0x27,0xff),RGBA(0x00,0x2b,0xff),RGBA(0x00,0x2f,0xff),RGBA(0x00,0x33,0xff),RGBA(0x00,0x37,0xff),RGBA(0x00,0x3b,0xff),
RGBA(0x00,0x3f,0xff),RGBA(0x00,0x43,0xff),RGBA(0x00,0x47,0xff),RGBA(0x00,0x4b,0xff),RGBA(0x00,0x4f,0xff),RGBA(0x00,0x53,0xff),RGBA(0x00,0x57,0xff),RGBA(0x00,0x5b,0xff),
RGBA(0x00,0x5f,0xff),RGBA(0x00,0x63,0xff),RGBA(0x00,0x67,0xff),RGBA(0x00,0x6b,0xff),RGBA(0x00,0x6f,0xff),RGBA(0x00,0x73,0xff),RGBA(0x00,0x77,0xff),RGBA(0x00,0x7b,0xff),
RGBA(0x00,0x7f,0xff),RGBA(0x00,0x84,0xff),RGBA(0x00,0x87,0xff),RGBA(0x00,0x8c,0xff),RGBA(0x00,0x8f,0xff),RGBA(0x00,0x94,0xff),RGBA(0x00,0x97,0xff),RGBA(0x00,0x9c,0xff),
RGBA(0x00,0x9f,0xff),RGBA(0x00,0xa4,0xff),RGBA(0x00,0xa7,0xff),RGBA(0x00,0xac,0xff),RGBA(0x00,0xaf,0xff),RGBA(0x00,0xb4,0xff),RGBA(0x00,0xb7,0xff),RGBA(0x00,0xbc,0xff),
RGBA(0x00,0xbf,0xff),RGBA(0x00,0xc4,0xff),RGBA(0x00,0xc7,0xff),RGBA(0x00,0xcc,0xff),RGBA(0x00,0xcf,0xff),RGBA(0x00,0xd4,0xff),RGBA(0x00,0xd7,0xff),RGBA(0x00,0xdc,0xff),
RGBA(0x00,0xdf,0xff),RGBA(0x00,0xe4,0xff),RGBA(0x00,0xe7,0xff),RGBA(0x00,0xec,0xff),RGBA(0x00,0xef,0xff),RGBA(0x00,0xf4,0xff),RGBA(0x00,0xf7,0xff),RGBA(0x00,0xfc,0xff),
RGBA(0x00,0xff,0xfe),RGBA(0x00,0xff,0xf9),RGBA(0x00,0xff,0xf6),RGBA(0x00,0xff,0xf1),RGBA(0x00,0xff,0xee),RGBA(0x00,0xff,0xe9),RGBA(0x00,0xff,0xe6),RGBA(0x00,0xff,0xe1),
RGBA(0x00,0xff,0xde),RGBA(0x00,0xff,0xd9),RGBA(0x00,0xff,0xd6),RGBA(0x00,0xff,0xd1),RGBA(0x00,0xff,0xce),RGBA(0x00,0xff,0xc9),RGBA(0x00,0xff,0xc6),RGBA(0x00,0xff,0xc1),
RGBA(0x00,0xff,0xbe),RGBA(0x00,0xff,0xb9),RGBA(0x00,0xff,0xb6),RGBA(0x00,0xff,0xb1),RGBA(0x00,0xff,0xae),RGBA(0x00,0xff,0xa9),RGBA(0x00,0xff,0xa6),RGBA(0x00,0xff,0xa1),
RGBA(0x00,0xff,0x9e),RGBA(0x00,0xff,0x99),RGBA(0x00,0xff,0x96),RGBA(0x00,0xff,0x91),RGBA(0x00,0xff,0x8e),RGBA(0x00,0xff,0x89),RGBA(0x00,0xff,0x86),RGBA(0x00,0xff,0x81),
RGBA(0x00,0xff,0x7e),RGBA(0x00,0xff,0x7a),RGBA(0x00,0xff,0x76),RGBA(0x00,0xff,0x72),RGBA(0x00,0xff,0x6e),RGBA(0x00,0xff,0x6a),RGBA(0x00,0xff,0x66),RGBA(0x00,0xff,0x62),
RGBA(0x00,0xff,0x5e),RGBA(0x00,0xff,0x5a),RGBA(0x00,0xff,0x56),RGBA(0x00,0xff,0x52),RGBA(0x00,0xff,0x4e),RGBA(0x00,0xff,0x4a),RGBA(0x00,0xff,0x46),RGBA(0x00,0xff,0x42),
RGBA(0x00,0xff,0x3d),RGBA(0x00,0xff,0x39),RGBA(0x00,0xff,0x35),RGBA(0x00,0xff,0x31),RGBA(0x00,0xff,0x2d),RGBA(0x00,0xff,0x29),RGBA(0x00,0xff,0x25),RGBA(0x00,0xff,0x21),
RGBA(0x00,0xff,0x1d),RGBA(0x00,0xff,0x19),RGBA(0x00,0xff,0x15),RGBA(0x00,0xff,0x11),RGBA(0x00,0xff,0x0d),RGBA(0x00,0xff,0x09),RGBA(0x00,0xff,0x05),RGBA(0x00,0xff,0x01),
RGBA(0x01,0xff,0x00),RGBA(0x05,0xff,0x00),RGBA(0x09,0xff,0x00),RGBA(0x0d,0xff,0x00),RGBA(0x11,0xff,0x00),RGBA(0x15,0xff,0x00),RGBA(0x19,0xff,0x00),RGBA(0x1d,0xff,0x00),
RGBA(0x21,0xff,0x00),RGBA(0x25,0xff,0x00),RGBA(0x29,0xff,0x00),RGBA(0x2d,0xff,0x00),RGBA(0x31,0xff,0x00),RGBA(0x35,0xff,0x00),RGBA(0x39,0xff,0x00),RGBA(0x3d,0xff,0x00),
RGBA(0x42,0xff,0x00),RGBA(0x46,0xff,0x00),RGBA(0x4a,0xff,0x00),RGBA(0x4e,0xff,0x00),RGBA(0x52,0xff,0x00),RGBA(0x56,0xff,0x00),RGBA(0x5a,0xff,0x00),RGBA(0x5e,0xff,0x00),
RGBA(0x62,0xff,0x00),RGBA(0x66,0xff,0x00),RGBA(0x6a,0xff,0x00),RGBA(0x6e,0xff,0x00),RGBA(0x72,0xff,0x00),RGBA(0x76,0xff,0x00),RGBA(0x7a,0xff,0x00),RGBA(0x7e,0xff,0x00),
RGBA(0x81,0xff,0x00),RGBA(0x86,0xff,0x00),RGBA(0x89,0xff,0x00),RGBA(0x8e,0xff,0x00),RGBA(0x91,0xff,0x00),RGBA(0x96,0xff,0x00),RGBA(0x99,0xff,0x00),RGBA(0x9e,0xff,0x00),
RGBA(0xa1,0xff,0x00),RGBA(0xa6,0xff,0x00),RGBA(0xa9,0xff,0x00),RGBA(0xae,0xff,0x00),RGBA(0xb1,0xff,0x00),RGBA(0xb6,0xff,0x00),RGBA(0xb9,0xff,0x00),RGBA(0xbe,0xff,0x00),
RGBA(0xc1,0xff,0x00),RGBA(0xc6,0xff,0x00),RGBA(0xc9,0xff,0x00),RGBA(0xce,0xff,0x00),RGBA(0xd1,0xff,0x00),RGBA(0xd6,0xff,0x00),RGBA(0xd9,0xff,0x00),RGBA(0xde,0xff,0x00),
RGBA(0xe1,0xff,0x00),RGBA(0xe6,0xff,0x00),RGBA(0xe9,0xff,0x00),RGBA(0xee,0xff,0x00),RGBA(0xf1,0xff,0x00),RGBA(0xf6,0xff,0x00),RGBA(0xf9,0xff,0x00),RGBA(0xfe,0xff,0x00),
RGBA(0xff,0xfc,0x00),RGBA(0xff,0xf7,0x00),RGBA(0xff,0xf4,0x00),RGBA(0xff,0xef,0x00),RGBA(0xff,0xec,0x00),RGBA(0xff,0xe7,0x00),RGBA(0xff,0xe4,0x00),RGBA(0xff,0xdf,0x00),
RGBA(0xff,0xdc,0x00),RGBA(0xff,0xd7,0x00),RGBA(0xff,0xd4,0x00),RGBA(0xff,0xcf,0x00),RGBA(0xff,0xcc,0x00),RGBA(0xff,0xc7,0x00),RGBA(0xff,0xc4,0x00),RGBA(0xff,0xbf,0x00),
RGBA(0xff,0xbc,0x00),RGBA(0xff,0xb7,0x00),RGBA(0xff,0xb4,0x00),RGBA(0xff,0xaf,0x00),RGBA(0xff,0xac,0x00),RGBA(0xff,0xa7,0x00),RGBA(0xff,0xa4,0x00),RGBA(0xff,0x9f,0x00),
RGBA(0xff,0x9c,0x00),RGBA(0xff,0x97,0x00),RGBA(0xff,0x94,0x00),RGBA(0xff,0x8f,0x00),RGBA(0xff,0x8c,0x00),RGBA(0xff,0x87,0x00),RGBA(0xff,0x84,0x00),RGBA(0xff,0x7f,0x00),
RGBA(0xff,0x7b,0x00),RGBA(0xff,0x77,0x00),RGBA(0xff,0x73,0x00),RGBA(0xff,0x6f,0x00),RGBA(0xff,0x6b,0x00),RGBA(0xff,0x67,0x00),RGBA(0xff,0x63,0x00),RGBA(0xff,0x5f,0x00),
RGBA(0xff,0x5b,0x00),RGBA(0xff,0x57,0x00),RGBA(0xff,0x53,0x00),RGBA(0xff,0x4f,0x00),RGBA(0xff,0x4b,0x00),RGBA(0xff,0x47,0x00),RGBA(0xff,0x43,0x00),RGBA(0xff,0x3f,0x00),
RGBA(0xff,0x3b,0x00),RGBA(0xff,0x37,0x00),RGBA(0xff,0x33,0x00),RGBA(0xff,0x2f,0x00),RGBA(0xff,0x2b,0x00),RGBA(0xff,0x27,0x00),RGBA(0xff,0x23,0x00),RGBA(0xff,0x1f,0x00),
RGBA(0xff,0x1b,0x00),RGBA(0xff,0x17,0x00),RGBA(0xff,0x13,0x00),RGBA(0xff,0x0f,0x00),RGBA(0xff,0x0b,0x00),RGBA(0xff,0x07,0x00),RGBA(0xff,0x03,0x00),RGBA(0xff,0x00,0x00)
};

cold_to_hot:: cold_to_hot() throw() : ramp(cold_to_hot_codes) {}

cold_to_hot::~cold_to_hot() throw() {}


} }
