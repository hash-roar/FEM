#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace t = std::chrono;

template<typename T>
class MyVector
{
public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;
  using iterator = T*;
  using const_iterator = const T*;

  MyVector() noexcept : _start( nullptr ), _finish( nullptr ), _end_of_storage( nullptr ) {}

  MyVector( const MyVector& other )
  {
    size_type n = other.size();

    _start = allocate( n );

    _finish = std::uninitialized_copy( other.begin(), other.end(), _start );
    _end_of_storage = _start + n;
  }

  MyVector( MyVector&& other ) noexcept
    : _start( other._start ), _finish( other._finish ), _end_of_storage( other._end_of_storage )
  {

    other._start = other._finish = other._end_of_storage = nullptr;
  }

  ~MyVector() { destroy_and_deallocate(); }

  MyVector& operator=( const MyVector& other )
  {
    if ( this != &other ) {
      MyVector temp( other );
      swap( temp );
    }
    return *this;
  }

  MyVector& operator=( MyVector&& other ) noexcept
  {
    if ( this != &other ) {
      destroy_and_deallocate();

      _start = other._start;
      _finish = other._finish;
      _end_of_storage = other._end_of_storage;

      other._start = other._finish = other._end_of_storage = nullptr;
    }
    return *this;
  }

  iterator begin() { return _start; }
  const_iterator begin() const { return _start; }
  const_iterator cbegin() const { return _start; }

  iterator end() { return _finish; }
  const_iterator end() const { return _finish; }
  const_iterator cend() const { return _finish; }

  size_type size() const { return _finish - _start; }
  size_type capacity() const { return _end_of_storage - _start; }
  bool empty() const { return _start == _finish; }

  void reserve( size_type new_cap )
  {
    if ( new_cap > capacity() ) {
      reallocate( new_cap );
    }
  }

  reference operator[]( size_type n ) { return _start[n]; }
  const_reference operator[]( size_type n ) const { return _start[n]; }

  reference at( size_type n )
  {
    if ( n >= size() ) {
      throw std::out_of_range( "MyVector::at" );
    }
    return _start[n];
  }
  const_reference at( size_type n ) const
  {
    if ( n >= size() ) {
      throw std::out_of_range( "MyVector::at" );
    }
    return _start[n];
  }

  reference front() { return *_start; }
  const_reference front() const { return *_start; }

  reference back() { return *( _finish - 1 ); }
  const_reference back() const { return *( _finish - 1 ); }

  void push_back( const_reference value )
  {
    if ( _finish == _end_of_storage ) {

      size_type new_cap = capacity() == 0 ? 1 : 2 * capacity();
      reallocate( new_cap );
    }

    new ( _finish ) T( value );
    _finish++;
  }

  void push_back( T&& value )
  {
    if ( _finish == _end_of_storage ) {
      size_type new_cap = capacity() == 0 ? 1 : 2 * capacity();
      reallocate( new_cap );
    }

    new ( _finish ) T( std::move( value ) );
    _finish++;
  }

  void pop_back()
  {
    if ( !empty() ) {
      _finish--;
      _finish->~T();
    }
  }

  void clear()
  {
    destroy_elements();
    _finish = _start;
  }

  void swap( MyVector& other ) noexcept
  {
    std::swap( _start, other._start );
    std::swap( _finish, other._finish );
    std::swap( _end_of_storage, other._end_of_storage );
  }

private:
  pointer _start;
  pointer _finish;
  pointer _end_of_storage;

  pointer allocate( size_type n ) { return static_cast<pointer>( ::operator new( n * sizeof( T ) ) ); }

  void deallocate()
  {
    if ( _start ) {
      ::operator delete( _start );
    }
  }

  void destroy_elements()
  {
    for ( pointer p = _start; p != _finish; ++p ) {
      p->~T();
    }
  }

  void destroy_and_deallocate()
  {
    destroy_elements();
    deallocate();
  }

  void reallocate( size_type new_cap )
  {
    pointer new_start = allocate( new_cap );

    pointer new_finish = std::uninitialized_move( begin(), end(), new_start );

    destroy_and_deallocate();

    _start = new_start;
    _finish = new_finish;
    _end_of_storage = _start + new_cap;
  }
};
struct Widget
{
  int id;
  Widget( int i = 0 ) : id( i ) { std::cout << "  Widget(" << id << ") constructed.\n"; }
  ~Widget() { std::cout << "  Widget(" << id << ") destructed.\n"; }
  Widget( const Widget& other ) : id( other.id ) { std::cout << "  Widget(" << id << ") copy-constructed.\n"; }
  Widget( Widget&& other ) noexcept : id( other.id )
  {
    other.id = -1;
    std::cout << "  Widget(" << id << ") move-constructed.\n";
  }
  Widget& operator=( const Widget& other )
  {
    id = other.id;
    std::cout << "  Widget(" << id << ") copy-assigned.\n";
    return *this;
  }
  Widget& operator=( Widget&& other ) noexcept
  {
    id = other.id;
    other.id = -1;
    std::cout << "  Widget(" << id << ") move-assigned.\n";
    return *this;
  }
};

template<typename K, typename V>
class LRU
{
public:
  using ListPair = std::pair<K, V>;
  using ListIterator = typename std::list<ListPair>::iterator;
  explicit LRU( size_t capacity ) : _capacity( capacity ) {}

  std::optional<V> get( const K& key )
  {
    auto it = _map.find( key );
    if ( it == _map.end() ) {
      return std::nullopt;
    }
    _list.splice( _list.begin(), _list, it->second );
    return it->second->second;
  }

  void put( const K& key, const V& value )
  {
    auto it = _map.find( key );
    if ( it != _map.end() ) {
      it->second->second = value;
      _list.splice( _list.begin(), _list, it->second );
      return;
    }

    if ( _list.size() == _capacity ) {
      auto last = _list.back();
      _map.erase( last.first );
      _list.pop_back();
    }

    _list.emplace_front( key, value );
    _map[key] = _list.begin();
  }
  ~LRU();

  size_t size() const { return _map.size(); }
  bool empty() const { return _map.empty(); }
  size_t capacity() const { return _capacity; }

private:
  std::list<ListPair> _list;
  std::unordered_map<K, ListIterator> _map;
  size_t _capacity;
};

template<typename K, typename V>
class LruTTL
{
public:
  struct Item
  {
    K key;
    V value;
    t::steady_clock::time_point timestamp;
    Item( const K& k, const V& v ) : key( k ), value( v ), timestamp( t::steady_clock::now() ) {}
  };
  using ListIterator = typename std::list<Item>::iterator;
  LruTTL( int capacity, t::seconds ttl ) : _capacity( capacity ), _ttl( ttl ) {};

  std::optional<V> get( const K& key )
  {
    auto it = _map.find( key );
    if ( it == _map.end() ) {
      return std::nullopt;
    }
    auto now = t::steady_clock::now();
    if ( now - it->second->timestamp > _ttl ) {
      _list.erase( it->second );
      _map.erase( it );
      return std::nullopt;
    }
    it->second->timestamp = now;
    _list.splice( _list.begin(), _list, it->second );
    return it->second->value;
  }
  void put( const K& key, const V& value )
  {
    auto it = _map.find( key );
    if ( it != _map.end() ) {
      it->second->value = value;
      it->second->timestamp = t::steady_clock::now();
      _list.splice( _list.begin(), _list, it->second );
      return;
    }

    if ( _list.size() == _capacity ) {
      auto last = _list.back();
      _map.erase( last.key );
      _list.pop_back();
    }

    _list.emplace_front( key, value );
    _map[key] = _list.begin();
  }
  ~LruTTL();

private:
  std::list<Item> _list;
  std::unordered_map<K, ListIterator> _map;
  size_t _capacity;
  t::seconds _ttl;
};

template<typename T1, typename T2, bool T1Empty = std::is_empty_v<T1>, bool T2Empty = std::is_empty_v<T2>>
class CompressPair
{
public:
  CompressPair();
  ~CompressPair();

private:
};

template<typename T1, typename T2>
class CompressPair<T1, T2, true, true>
  : private T1
  , private T2
{
public:
  CompressPair() = default;
  CompressPair( const T1& t1, const T2& t2 ) : T1( t1 ), T2( t2 ) {}
  ~CompressPair() = default;
  T1& first() { return *this; }
  const T1& first() const { return *this; }
  T2& second() { return *this; }
  const T2& second() const { return *this; }
};

template<typename T1, typename T2>
class CompressPair<T1, T2, true, false> : private T1
{
public:
  CompressPair() = default;
  CompressPair( const T1& t1, const T2& t2 ) : T1( t1 ), _second( t2 ) {}
  ~CompressPair() = default;
  T1& first() { return *this; }
  const T1& first() const { return *this; }
  T2& second() { return _second; }
  const T2& second() const { return _second; }

private:
  T2 _second;
};
template<typename T1, typename T2>
class CompressPair<T1, T2, false, true> : private T2
{
public:
  CompressPair() = default;
  CompressPair( const T1& t1, const T2& t2 ) : T2( t2 ), _first( t1 ) {}
  ~CompressPair() = default;
  T1& first() { return _first; }
  const T1& first() const { return _first; }
  T2& second() { return *this; }
  const T2& second() const { return *this; }

private:
  T1 _first;
};

template<typename T1, typename T2>
class CompressPair<T1, T2, false, false>
{
public:
  CompressPair() = default;
  CompressPair( const T1& t1, const T2& t2 ) : _first( t1 ), _second( t2 ) {}
  ~CompressPair() = default;
  T1& first() { return _first; }
  const T1& first() const { return _first; }
  T2& second() { return _second; }

  const T2& second() const { return _second; }

private:
  T1 _first;
  T2 _second;
};

template<typename T>
struct remove_reference
{
  using type = T;
};

template<typename T>
struct remove_reference<T&>
{
  using type = T;
};

template<typename T>
struct remove_reference<T&&>
{
  using type = T;
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
remove_reference_t<T>&& my_move( T&& arg ) noexcept
{
  return static_cast<remove_reference_t<T>&&>( arg );
}

template<typename T>
T&& my_forward( remove_reference_t<T>& arg ) noexcept
{
  return static_cast<T&&>( arg );
}

template<typename Head, typename... Tail>
struct Tuple : Tuple<Tail...>
{
  using Base = Tuple<Tail...>;
  explicit Tuple( Head&& h, Tail&&... t ) : Base( std::forward<Tail>( t )... ), head( std::forward<Head>( h ) ) {}
  Head head;

  template<size_t index>
  decltype( auto ) get()
  {
    if constexpr ( index == 0 ) {
      return head;
    } else {
      return Base::template get<index - 1>();
    }
  }
};

template<typename T>
auto advance( T& value, int n ) -> void
  requires( std::is_integral_v<T> )
{
  value += n;
}

template<typename T>
class RingBuffer
{
public:
  explicit RingBuffer( int capacity ) : buffer_( capacity ), head_( 0 ), tail_( 0 ), size_( 0 )
  {
    if ( capacity <= 0 ) {
      throw std::invalid_argument( "Capacity must be greater than zero." );
    }
  }
  ~RingBuffer();

  void push( const T& item ) { push_impl( item ); }
  void push( T&& item ) { push_impl( std::move( item ) ); }

  T pop()
  {
    std::unique_lock<std::mutex> lock( mutex_ );
    not_empty_.wait( lock, [this] { return size_ > 0; } );
    T item = std::move( buffer_[tail_] );
    tail_ = ( tail_ + 1 ) % buffer_.size();
    --size_;
    lock.unlock();
    not_full_.notify_one();
    return item;
  }

  std::optional<T> try_pop()
  {
    std::lock_guard<std::mutex> lock( mutex_ );
    if ( size_ == 0 ) {
      return std::nullopt;
    }
    T item = std::move( buffer_[tail_] );
    tail_ = ( tail_ + 1 ) % buffer_.size();
    --size_;
    not_full_.notify_one();
    return item;
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock( mutex_ );
    return size_;
  }

private:
  void push_impl( T&& item )
  {
    std::unique_lock<std::mutex> lock( mutex_ );
    not_full_.wait_until( lock, [this] { return size_ < buffer_.size(); } );
    buffer_[head_] = std::forward<T>( item );
    head_ = ( head_ + 1 ) % buffer_.size();
    ++size_;
    lock.unlock();
    not_empty_.notify_one();
  }

  std::vector<T> buffer_;
  size_t head_;
  size_t tail_;
  size_t size_;

  mutable std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
};

class ThreadPool
{
public:
  explicit ThreadPool( int num_threads ) :
  {
    for ( int i = 0; i < num_threads; ++i ) {
      workers_.emplace_back( [this] { this->worker_loop(); } );
    }
  }
  ~ThreadPool()
  {
    {
      std::unique_lock<std::mutex> lock( queue_mutex_ );
      stop_ = true;
    }
    condition_.notify_all();
    for ( std::thread& worker : workers_ ) {
      if ( worker.joinable() ) {
        worker.join();
      }
    }
  }

  // delete copy and move constructors and assignment operators
  ThreadPool( const ThreadPool& ) = delete;
  ThreadPool( ThreadPool&& ) = delete;
  ThreadPool& operator=( const ThreadPool& ) = delete;
  ThreadPool& operator=( ThreadPool&& ) = delete;

  template<typename F, typename... Args>
  auto submit( F&& f, Args&&... args )
  {
    using return_type = std::invoke_result_t<F, Args...>;
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
      std::bind<F, Args...>( std::forward( f ), std::forward( args )... ) );
    std::future<return_type> future = task_ptr.get_future();
    {
      std::lock_guard<std::mutex> lock( queue_mutex_ );
      tasks_.emplace( [task_ptr] { ( *task_ptr )(); } );
    }
    return future;
  }

private:
  void worker_loop()
  {
    while ( true ) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock( queue_mutex_ );
        condition_.wait( lock, [this] { return stop_ || !tasks_.empty(); } );
        if ( stop_ && tasks_.empty() ) {
          return;
        }
        task = std::move( tasks_.front() );
        tasks_.pop();
      }
      task();

      auto a = [] { return 1; };
      a();

      struct A
      {
        virtual void a();
      };
      auto* b = new A();
      auto p = typeid( b );
    }
  }

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queue_mutex_;
  std::condition_variable condition_;
  bool stop_ {};
};

template<typename R, typename... Args>
class Function;

template<typename R, typename... Args>
class Function<R( Args... )>
{
public:
  Function();
  ~Function();

  template<typename F>
  explicit Function( F f ) : call_ptr_( std::make_unique<Model<F>>( std::move( f ) ) )
  {}

  struct CallableConcept
  {
    virtual R invoke( Args... args );
  };

  template<typename F>
  struct Model : CallableConcept
  {
    F f;

    explicit Model( F f_ ) : f( std::move( f_ ) ) {}

    R invoke( Args... args ) override { return std::invoke( f, std::forward<Args>( args )... ); }
  };

  R operator()( Args... args ) { return call_ptr_->invoke( std::forward<Args>( args )... ); }

private:
  std::unique_ptr<CallableConcept> call_ptr_;
};

template<bool B, typename T>
struct enable_if
{};

template<typename T>
struct enable_if<false, T>
{
  using type = T;
};
;

template<bool B, typename T>
using enable_if_t = enable_if<B, T>::type;

template<typename T>
enable_if_t<std::is_array_v<T>, void> process( T& v )
{
  for ( auto a : v ) {}
}

template<typename F, typename... Args>
inline std::future<std::result_of_t<F( Args... )>> launch_async( F&& f, Args&&... args )
{
  return std::async( std::launch::async, std::forward<F>( f ), std::forward<Args>( args )... );
}
template<typename F, typename... Ts>
inline std::future<std::result_of_t<F( Ts... )>> reallyAsync( F&& f, Ts&&... params )
{
  return std::async( std::launch::async, std::forward<F>( f ), std::forward<Ts>( params )... );
}

struct AlignedInt
{
  alignas( std::hardware_destructive_interference_size ) int i;
};

class V1
{
public:
  V1();

  virtual void a();

  virtual ~V1();

private:
  int b;
};

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

template<typename T>
class McMpRingBuffer
{
public:
  struct Slots
  {
    alignas( T ) std::byte data[sizeof( T )] {}; // NOLINT
    alignas( hardware_destructive_interference_size ) std::atomic<size_t> sequence;
  };
  // cap should be 2^x
  explicit McMpRingBuffer( size_t cap ) : cap_( cap ), head_( 0 ), head_cache_( 0 ), tail_( 0 ), tail_cache_( 0 )
  {
    if ( cap_ <= 0 ) {
      throw std::runtime_error( "error cap" );
    }

    slot_ = ::operator new( cap_ * sizeof( Slots ) );
    // handle sequence init
    for ( int i = 0; i < cap_; i++ ) {
      slot_[i].sequence.store( i, std::memory_order_relaxed );
    }
  }
  ~McMpRingBuffer()
  {
    if ( !slot_ ) {
      return;
    }

    if constexpr ( !std::is_trivially_destructible_v<T> ) {
      auto head = head_.load( std::memory_order_relaxed );
      auto tail = tail_.load( std::memory_order_relaxed );
      for ( size_t i = head; i < tail; i++ ) {
        reinterpret_cast<T*>( &slot_[i % cap_].data )->~T(); // NOLINT
      }
    }

    ::operator delete( slot_ );
  }

  std::optional<T> try_pop()
  {
    auto head = head_.load( std::memory_order::relaxed );
    if ( tail_cache_.load( std::memory_order::relaxed ) - head <= 0 ) {
      tail_cache_.store( tail_.load( std::memory_order::acquire ) );
      if ( tail_cache_.load( std::memory_order::relaxed ) - head <= 0 ) {
        return std::nullopt_t;
      }
    }

    if ( !head_.compare_exchange_strong( head, head + 1, std::memory_order::relaxed ) ) {
      return std::nullopt_t;
    }

    auto& slot = slot_[head % cap_];

    while ( slot_[head % cap_].sequence.load( std::memory_order::acquire ) != head + 1 ) {}

    T tmp = std::move( *reinterpret_cast<T*>( &slot.data ) );

    if constexpr ( !std::is_trivially_destructible_v<T> ) {
      reinterpret_cast<T*>( &slot.data )->~T();
    }

    slot.sequence.store( head + cap_, std::memory_order::release );
    return tmp;
  }

  bool try_push( T&& item )
  {
    auto tail = tail_.load( std::memory_order::relaxed );
    if ( tail - head_cache_.load( std::memory_order::relaxed ) >= cap_ ) {
      head_cache_.store( head_.load( std::memory_order::acquire ) );
      if ( tail - head_cache_.load( std::memory_order::relaxed ) >= cap_ ) {
        return false;
      }
    }

    if ( !tail_.compare_exchange_strong( tail, tail + 1, std::memory_order::relaxed ) ) {
      return false;
    }

    auto& slot = slot_[tail % cap_];

    while ( slot.sequence.load( std::memory_order::acquire ) != tail ) {}

    new ( &slot.data ) T( std::move( item ) ); // NOLINT

    slot.sequence.store( tail + 1, std::memory_order::release );
    return true;
  }

private:
  size_t cap_;
  Slots* slot_;
  alignas( hardware_destructive_interference_size ) std::atomic<size_t> head_;
  alignas( hardware_destructive_interference_size ) std::atomic<size_t> head_cache_;
  alignas( hardware_destructive_interference_size ) std::atomic<size_t> tail_;
  alignas( hardware_destructive_interference_size ) std::atomic<size_t> tail_cache_;
};

int main()
{
  std::cout << "--- Test 1: push_back and reallocation ---\n";
  {
    MyVector<Widget> vec;
    std::cout << "Initial size: " << vec.size() << ", capacity: " << vec.capacity() << std::endl;

    std::cout << "\nPushing 1:\n";
    vec.push_back( Widget( 1 ) );
    std::cout << "Size: " << vec.size() << ", capacity: " << vec.capacity() << std::endl;

    std::cout << "\nPushing 2 (triggers reallocation):\n";
    vec.push_back( Widget( 2 ) );
    std::cout << "Size: " << vec.size() << ", capacity: " << vec.capacity() << std::endl;

    std::cout << "\nPushing 3 (triggers reallocation):\n";
    vec.push_back( Widget( 3 ) );
    std::cout << "Size: " << vec.size() << ", capacity: " << vec.capacity() << std::endl;

    std::cout << "\nVector goes out of scope:\n";
  }

  std::cout << "\n--- Test 2: Copy and Move ---\n";
  MyVector<Widget> original;
  original.push_back( 10 );
  original.push_back( 20 );

  std::cout << "\nCreating copy:\n";
  MyVector<Widget> copy = original;

  std::cout << "\nCreating moved:\n";
  MyVector<Widget> moved = std::move( original );

  std::cout << "\nOriginal after move (should be empty):\n";
  std::cout << "Original size: " << original.size() << ", capacity: " << original.capacity() << std::endl;

  std::cout << "\nClearing 'moved' vector:\n";
  moved.clear();
  std::cout << "Moved size after clear: " << moved.size() << ", capacity: " << moved.capacity() << std::endl;

  std::cout << "\nTest scope ends:\n";
  return 0;
}
