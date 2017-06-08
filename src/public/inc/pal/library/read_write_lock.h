/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:read_write_lock.h
****/
#pragma once

#include <cassert>
#include <cstring>
#include <mutex>
#include <type_traits>
#include <pal/pal_types.h>
#include <pal/pal_atomic.h>
#include <pal/pal_critical_section.h>
#include <pal/pal_semaphore.h>

namespace pal {
    namespace library {

        using pal_atomic_signed_integer_t = std::make_signed<_pal_AtomicIntegerType>::type;

        class ReadWriteLock
        {
        private:
            // Allow 2^30 shared owners.
            const static pal_atomic_signed_integer_t MAX_SHARED_OWNERS = (1 << 30);

            //
            // Mutex for synchronization on exclusive access.
            //
            _PAL_CRITICAL_SECTION _cs;

            //
            // Semaphore for exclusive waiter that waits for shared owners to
            // release the resource.
            //
            _PAL_SEMAPHORE _writer_semaphore;

            //
            // Semaphore for shared waiters that Wait for the exclusive owner to release
            // the resource.
            //
            _PAL_SEMAPHORE _readers_semaphore;

            pal_atomic_signed_integer_t _shared_owners;
            volatile pal_atomic_signed_integer_t _lock;

        public:
            ReadWriteLock(bool autoInit = true) :
                _shared_owners(0),
                _lock(0)
            {
                if (autoInit)
                {
                    Init();
                    assert(IsValid());
                }
            }

            ~ReadWriteLock()
            {
                Cleanup();
                assert(!IsValid());
            }

            bool IsValid() const
            {
                return (_lock != 0);
            }

            bool Init()
            {
                if (IsValid())
                {
                    assert(!IsValid());
                    return false;
                }

                //
                // Mutex for synchronization on exclusive access.
                //
                _pal_InitializeCriticalSection(&_cs, /* recursive = */ true);

                //
                // Semaphore for exclusive waiter that waits for shared owners to
                // release the resource.
                //
                _pal_CreateSemaphore(&_writer_semaphore, 0, MAX_SHARED_OWNERS);

                //
                // Semaphore for shared waiters that Wait for the exclusive owner to release
                // the resource.
                //
                _pal_CreateSemaphore(&_readers_semaphore, 0, MAX_SHARED_OWNERS);

                _lock = -MAX_SHARED_OWNERS;

                return true;
            }

            void Cleanup()
            {
                if (!IsValid())
                {
                    assert(IsValid());
                    return;
                }

                _pal_DeleteSemaphore(&_readers_semaphore);
                memset(&_readers_semaphore, 0, sizeof(_PAL_SEMAPHORE));

                _pal_DeleteSemaphore(&_writer_semaphore);
                memset(&_writer_semaphore, 0, sizeof(_PAL_SEMAPHORE));

                _pal_DeleteCriticalSection(&_cs);

                _lock = 0;
            }

            void AcquireExclusive()
            {
                //
                // Exclusive access calls are synchronized via mutex.
                //
                _pal_EnterCriticalSection(&_cs);

                //
                // Only one call to exclusive access will obtain the mutex. All other will
                // Wait on the mutex till exclusive release of this thread.
                //
                _shared_owners = _pal_atomic_exchange(&_lock, MAX_SHARED_OWNERS) + MAX_SHARED_OWNERS;

                //
                // After interlocked exchange we have the number of shared owners of the resource.
                // The number of shared owners that was observed upon entry to the routine is
                // stored in the global variable that is protected by the mutex.
                //
                assert(_shared_owners >= 0);

                //
                // Wait for all shared owners to release the semaphore.
                //
                for (pal_atomic_signed_integer_t i = 0; i < _shared_owners; ++i)
                {
                    //
                    // Exclusive waiter waits on the semaphore that is released by the shared owners.
                    //
                    _pal_WaitSemaphore(&_writer_semaphore);
                }
            }

            void ReleaseExclusive()
            {
                //
                // Exchange will mark resource available for shared access.
                //
                pal_atomic_signed_integer_t shared_waiters =
                    _pal_atomic_exchange(&_lock, -MAX_SHARED_OWNERS) - MAX_SHARED_OWNERS + _shared_owners;

                //
                // The number of shared waiters is obtained. It is a difference between lock
                // values that was caused by increment that was executed in AcquireShared().
                //
                assert(shared_waiters >= 0);

                //
                // Allow all shared waiters to continue.
                //
                if (shared_waiters > 0)
                {
                    _pal_ReleaseSemaphore(&_readers_semaphore, shared_waiters, nullptr);
                }

                _pal_LeaveCriticalSection(&_cs);
            }

            void AcquireShared()
            {
                while (_pal_InterlockedIncrement(&_lock) > 0)
                {
                    //
                    // If lock is positive then there is an exclusive owner or exclusive
                    // waiter. In both cases Wait till exclusive owner will release the
                    // resource.
                    //
                    _pal_WaitSemaphore(&_readers_semaphore);
                }
            }

            void ReleaseShared()
            {
                if (_pal_InterlockedDecrement(&_lock) > 0)
                {
                    //
                    // If the lock is positive, it means that there is an exclusive waiter.
                    // Release the semaphore to allow exclusive waiter to obtain the resource
                    // when all other shared owners have released it.
                    //
                    _pal_ReleaseSemaphore(&_writer_semaphore, 1, nullptr);
                }
            }

        private:
            // Disallow copy.
            ReadWriteLock(const ReadWriteLock&) = delete;
            ReadWriteLock& operator=(const ReadWriteLock&) = delete;

            // Disallow move.
            ReadWriteLock(ReadWriteLock&&) = delete;
            ReadWriteLock& operator=(ReadWriteLock&&) = delete;
        }; // class ReadWriteLock

        class AutoReadLock
        {
            ReadWriteLock& _lock;

        public:
            explicit AutoReadLock(ReadWriteLock& lock) :
                _lock(lock)
            {
                _lock.AcquireShared();
            }

            AutoReadLock(ReadWriteLock& lock, std::adopt_lock_t) :
                _lock(lock)
            {
            }

            ~AutoReadLock()
            {
                _lock.ReleaseShared();
            }

        private:
            AutoReadLock(const AutoReadLock&) = delete;
            AutoReadLock& operator=(const AutoReadLock&) = delete;
            AutoReadLock(AutoReadLock&&) = delete;
            AutoReadLock& operator=(AutoReadLock&&) = delete;
        };

        class AutoWriteLock
        {
            ReadWriteLock& _lock;

        public:
            explicit AutoWriteLock(ReadWriteLock& lock) :
                _lock(lock)
            {
                _lock.AcquireExclusive();
            }

            AutoWriteLock(ReadWriteLock& lock, std::adopt_lock_t) :
                _lock(lock)
            {
            }

            ~AutoWriteLock()
            {
                _lock.ReleaseExclusive();
            }

        private:
            AutoWriteLock(const AutoWriteLock&) = delete;
            AutoWriteLock& operator=(const AutoWriteLock&) = delete;
            AutoWriteLock(AutoWriteLock&&) = delete;
            AutoWriteLock& operator=(AutoWriteLock&&) = delete;
        };

    } // namespace library
} // namespace pal
