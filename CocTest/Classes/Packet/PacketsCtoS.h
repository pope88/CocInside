#ifndef _SSU_PACKETSCTOS_H_
#define _SSU_PACKETSCTOS_H_

#include "SsuObject.h"

namespace game {
namespace net {
namespace data {
namespace CtoS {

class CSUserLogin_0x1: public ::ssu::Object
{
public:
  virtual ~CSUserLogin_0x1() { }
public:
	inline unsigned int result() const { return _result; }
	inline void setResult(unsigned int val__) { _result = val__; }
public:
  virtual unsigned char * packBuffer(unsigned char * buf);
  virtual bool unpackBuffer(const unsigned char *& buf, size_t& leftSize);
  virtual size_t size() const;
protected:
	unsigned int _result;
};

}
}
}
}

#endif // _SSU_PACKETSCTOS_H_
