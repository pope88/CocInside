#include "PacketsCtoS.h"

namespace game {
namespace net {
namespace data {
namespace CtoS {

unsigned char * CSUserLogin_0x1::packBuffer(unsigned char * buf)
{
	buf = ::ssu::Utils::packUInt32Tag(buf, 1, _result);
  return buf;
}

bool CSUserLogin_0x1::unpackBuffer(const unsigned char *& buf, size_t& leftSize)
{
	unsigned int tag_; unsigned char type_;
	while(leftSize > 0)
	{
		if(!::ssu::Utils::unpackTag(buf, leftSize, tag_, type_)) return false;
		switch(tag_)
		{
		case 1:
			if(type_ != 0 || !::ssu::Utils::unpackUInt32(buf, leftSize, _result)) return false;
			break;
		}
	}
  return true;
}

size_t CSUserLogin_0x1::size() const
{
   return 1 + ::ssu::Utils::sizeUInt32(_result);
}

}
}
}
}
