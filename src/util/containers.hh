

#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
class FemStr
{
public:
  static constexpr int SSO_CAP = 15;
  FemStr() : size_( 0 ) { sso_[0] = '\0'; }; // NOLINT
  explicit FemStr( const char* str )
  {
    size_t str_len = std::strlen( str );
    size_ = str_len;
    if ( is_sso() ) {
      std::strcpy( sso_, str ); // NOLINT
    } else {
      heap_.cap = str_len;
      heap_.ptr = new char[heap_.cap + 1];
      std::strcpy( heap_.ptr, str );
    }
  }

  FemStr( const FemStr& other )
  {
    if ( !is_sso() ) {
      delete[] heap_.ptr;
    }
    if ( other.is_sso() ) {
      strcpy( sso_, other.sso_ );
      size_ = other.size_;
    } else {
      heap_.ptr = new char[other.heap_.cap + 1];
      heap_.cap = other.heap_.cap;
      strcpy( heap_.ptr, other.heap_.ptr );
      size_ = other.size_;
    }
  }

  FemStr( FemStr&& other ) noexcept
  {
    if ( !is_sso() ) {
      delete[] heap_.ptr;
    }
    heap_ = other.heap_;
    size_ = other.size_;
    other.size_ = 0;
    other.sso_[0] = '\0';
  }

  size_t length() const { return size_; }
  size_t size() const { return size_; }
  size_t cap() const { return is_sso() ? SSO_CAP : heap_.cap; }
  const char* c_str() const { return is_sso() ? sso_ : heap_.ptr; }

  char* data() { return is_sso() ? sso_ : heap_.ptr; }

  ~FemStr();

private:
  bool is_sso() const { return size_ <= SSO_CAP; }
  std::size_t size_;
  union {
    struct
    {
      char* ptr;
      size_t cap;
    } heap_;
    char sso_[SSO_CAP + 1]; // NOLINT
  };
};

template<typename T>
class FemVector
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
  FemVector() : start_( nullptr ), end_( nullptr ), cap_( nullptr ) {};
  explicit FemVector( int n ) { allocate( n ); }

  explicit FemVector( int n, const T& value )
  {
    allocate( n );
    std::uninitialized_fill_n( start_, n, value );
  }

  ~FemVector() { destroy_and_deallocate(); }

private:
  void allocate( int n )
  {
    start_ = ::operator new( n * sizeof( T ) );
    end_ = start_;
    cap_ = start_ + n;
  }

  void destroy_and_deallocate()
  {
    for ( pointer p = start_; p < end_; p++ ) {
      p->~T();
    }
    ::operator delete( start_ );
  }

  pointer start_;
  pointer end_;
  pointer cap_;
};