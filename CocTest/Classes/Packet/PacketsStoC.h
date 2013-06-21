#ifndef _SSU_PACKETSSTOC_H_
#define _SSU_PACKETSSTOC_H_

#include "SsuObject.h"

namespace game {
namespace net {
namespace data {
namespace StoC {

class SCUserLogin_0x1: public ::ssu::Object
{
public:
  virtual ~SCUserLogin_0x1() { }

public:
  virtual unsigned char * packBuffer(unsigned char * buf);
  virtual bool unpackBuffer(const unsigned char *& buf, size_t& leftSize);
  virtual size_t size() const;


};

}
}
}
}

#endif // _SSU_PACKETSSTOC_H_
