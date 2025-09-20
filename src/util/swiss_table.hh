#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
class SwissTable
{

public:
  enum SlotStatus : int8_t
  {
    KEmpty = -128

  };

  struct Slot
  {
    union {
      std::pair<K, V> values;
      char dummy {};
    };
  };
  SwissTable();
  ~SwissTable();

  Hash hasher_ {};
  KeyEqual key_equal_ {};

private:
  uint16_t match_group( size_t group_start_index, int8_t h2 ) const;

  std::unique_ptr<int8_t[]> control_bytes_ { nullptr }; // NOLINT
  std::unique_ptr<Slot[]> slots_ { nullptr };           // NOLINT
};