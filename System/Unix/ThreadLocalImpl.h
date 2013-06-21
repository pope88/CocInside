#ifndef SYSTEM_THREADLOCALIMPL_H
#define SYSTEM_THREADLOCALIMPL_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/NonCopyable.h>
#include <pthread.h>


namespace System
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Unix implementation of thread-local storage
////////////////////////////////////////////////////////////
class ThreadLocalImpl : NonCopyable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor -- allocate the storage
    ///
    ////////////////////////////////////////////////////////////
    ThreadLocalImpl();

    ////////////////////////////////////////////////////////////
    /// \brief Destructor -- free the storage
    ///
    ////////////////////////////////////////////////////////////
    ~ThreadLocalImpl();

    ////////////////////////////////////////////////////////////
    /// \brief Set the thread-specific value of the variable
    ///
    /// \param value Value of the variable for this thread
    ///
    ////////////////////////////////////////////////////////////
    void SetValue(void* value);

    ////////////////////////////////////////////////////////////
    /// \brief Retrieve the thread-specific value of the variable
    ///
    /// \return Value of the variable for this thread
    ///
    ////////////////////////////////////////////////////////////
    void* GetValue() const;

private :

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    pthread_key_t myKey; ///< Index of our thread-local storage slot
};

} // namespace priv

} // namespace System


#endif // SYSTEM_THREADLOCALIMPL_H
