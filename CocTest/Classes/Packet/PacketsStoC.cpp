#include "PacketsStoC.h"

namespace game {
namespace net {
namespace data {
namespace StoC {

unsigned char * SCUserLogin_0x1::packBuffer(unsigned char * buf)
{
  return buf;
}

bool SCUserLogin_0x1::unpackBuffer(const unsigned char *& buf, size_t& leftSize)
{
  return true;
}

size_t SCUserLogin_0x1::size() const
{
  return 0;
}

}
}
}
}
