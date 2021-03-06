////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/ThreadLocal.h>


#if defined(SYSTEM_SYSTEM_WINDOWS)

	#include "Win32/ThreadLocalImpl.cpp"

#else

	#include "Unix/ThreadLocalImpl.cpp"

#endif


namespace System
{
////////////////////////////////////////////////////////////
ThreadLocal::ThreadLocal(void* value)
{
    myImpl = new priv::ThreadLocalImpl;
    SetValue(value);
}


////////////////////////////////////////////////////////////
ThreadLocal::~ThreadLocal()
{
    delete myImpl;
}


////////////////////////////////////////////////////////////
void ThreadLocal::SetValue(void* value)
{
    myImpl->SetValue(value);
}


////////////////////////////////////////////////////////////
void* ThreadLocal::GetValue() const
{
    return myImpl->GetValue();
}

} // namespace System
