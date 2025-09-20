

#include <atomic>
#include <mutex>
template<typename T>
class Singleton
{
public:
  static T* instance()
  {
    T* p = m_instance.load( std::memory_order_acquire ); // 第一次加载
    if ( p == nullptr ) {                                // 检查第一次加载的结果
      std::lock_guard<std::mutex> lock( m_mutex );
      p = m_instance.load( std::memory_order_relaxed ); // 第二次加载（在锁内）
      if ( p == nullptr ) {
        p = new T(); // NOLINT
        m_instance.store( p, std::memory_order_release );
      }
    }
    return p; // 返回第一次或第二次加载的结果，而不是重新加载
  }

  Singleton() = delete;
  ~Singleton() = delete;
  Singleton( const Singleton& ) = delete;
  Singleton& operator=( const Singleton& ) = delete;

private:
  static std::atomic<T*> m_instance;
  static std::mutex m_mutex;
};

template<typename T>
class SingletonStatic
{
public:
  SingletonStatic() = delete;
  ~SingletonStatic() = delete;
  SingletonStatic( const SingletonStatic& ) = delete;
  SingletonStatic& operator=( const SingletonStatic& ) = delete;
  static T* instance()
  {
    static T instance; // 局部静态变量，C++11及以上标准保证线程安全
    return &instance;
  }
};