
#include "common.hh"
#include <atomic>
#include <mutex>
#include <pthread.h>
class SpinLock
{
public:
  SpinLock() = default;
  ~SpinLock() = default;
  void lock()
  {
    while ( flag.test_and_set( std::memory_order_acquire ) ) {
      // Busy-wait
    }
  }
  void unlock() { flag.clear( std::memory_order_release ); }

private:
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

class SpinLockPthread
{
public:
  using scoped_lock = std::lock_guard<SpinLockPthread>;

  SpinLockPthread() { pthread_spin_init( &spinlock_, PTHREAD_PROCESS_PRIVATE ); }
  ~SpinLockPthread() { pthread_spin_destroy( &spinlock_ ); }
  void lock() { pthread_spin_lock( &spinlock_ ); }
  void unlock() { pthread_spin_unlock( &spinlock_ ); }

private:
  pthread_spinlock_t spinlock_ {};
};

class TicketSpinLock
{
public:
  TicketSpinLock();
  ~TicketSpinLock();
  void lock()
  {
    int my_ticket = ticket_.fetch_add( 1, std::memory_order_relaxed );
    for ( ;; ) {
      int pos = my_ticket - serving_.load( std::memory_order_acquire );
      if ( pos == 0 ) {
        return;
      }
      // Optionally, use a pause instruction to reduce contention
      do { // NOLINT
        spin_loop_pause();
      } while ( --pos );
    }
  }

  void unlock()
  {
    int next_serving = serving_.load( std::memory_order_relaxed ) + 1;
    serving_.store( next_serving, std::memory_order_release );
  }

private:
  alignas( CACHE_LINE_SIZE ) std::atomic<int> ticket_ { 0 };
  alignas( CACHE_LINE_SIZE ) std::atomic<int> serving_ { 0 };
};
