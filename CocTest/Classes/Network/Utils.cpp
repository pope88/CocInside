#include "Config.h"
#include "Utils.h"

#if defined(SYSTEM_SYSTEM_WINDOWS)

#include "Win32/UtilsImpl.cpp"

#else

#include "Else/UtilsImpl.cpp"

#endif

namespace Network
{

	Utils::Utils()
	{
		utilsImpl = new priv::UtilsImpl;
	}

	Utils::~Utils()
	{
		delete utilsImpl;
	}

}
