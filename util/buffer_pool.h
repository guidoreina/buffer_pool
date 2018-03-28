#ifndef UTIL_BUFFER_POOL_H
#define UTIL_BUFFER_POOL_H

#include <stdlib.h>
#include <limits.h>
#include <new>
#include "util/buffer.h"

namespace util {
  template<typename Buffer>
  class buffer_pool {
    public:
      typedef Buffer buffer_type;
      typedef typename buffer_type::size_type size_type;

      // Constructor.
      buffer_pool(size_t max_memory = ULONG_MAX);

      // Destructor.
      ~buffer_pool();

      // Initialize.
      bool init(size_type min_buf_size,
                size_type max_buf_size,
                size_type nslots);

      // Clear buffer pool.
      void clear();

      // Get buffer.
      buffer_type* get(size_type size);

      // Put buffer.
      void put(buffer_type* buf);

      // Get how much memory has been allocated.
      size_t allocated() const;

    private:
      static const size_t number_preallocations = 1024;
      static const size_type max_slots = 256;

      size_type _M_min_buf_size;
      size_type _M_max_buf_size;
      size_type _M_nslots;

      size_type _M_step;

      size_type _M_slot_sizes[max_slots];

      buffer_type* _M_bufs[max_slots];

      size_t _M_max_memory;
      size_t _M_allocated;

      // Allocate.
      bool allocate(size_type idx);

      // Get index.
      size_type index(size_type size) const;
  };

  template<typename Buffer>
  inline buffer_pool<Buffer>::buffer_pool(size_t max_memory)
    : _M_nslots(0),
      _M_max_memory(max_memory),
      _M_allocated(0)
  {
    for (size_type i = 0; i < max_slots; i++) {
      _M_bufs[i] = nullptr;
    }
  }

  template<typename Buffer>
  inline buffer_pool<Buffer>::~buffer_pool()
  {
    clear();
  }

  template<typename Buffer>
  bool buffer_pool<Buffer>::init(size_type min_buf_size,
                                 size_type max_buf_size,
                                 size_type nslots)
  {
    if ((min_buf_size <= max_buf_size) &&
        (nslots > 0) &&
        (nslots <= max_slots)) {
      _M_min_buf_size = min_buf_size;
      _M_max_buf_size = max_buf_size;

      size_type nsizes = max_buf_size - min_buf_size + 1;

      _M_nslots = (nsizes > nslots) ? nslots : nsizes;

      _M_step = nsizes / _M_nslots;

      if ((nsizes % _M_nslots) != 0) {
        _M_step++;
      }

      for (size_type i = 0; i < _M_nslots; i++) {
        _M_slot_sizes[i] = (i * _M_step) + _M_min_buf_size + (_M_step - 1);
      }

      return true;
    }

    return false;
  }

  template<typename Buffer>
  void buffer_pool<Buffer>::clear()
  {
    for (size_type i = 0; i < _M_nslots; i++) {
      buffer_type* buf = _M_bufs[i];

      while (buf) {
        buffer_type* next = buf->_M_next;

        free(buf);
        buf = next;
      }

      _M_bufs[i] = nullptr;
    }
  }

  template<typename Buffer>
  typename
  buffer_pool<Buffer>::buffer_type* buffer_pool<Buffer>::get(size_type size)
  {
    // If the buffer size is valid...
    if ((size >= _M_min_buf_size) && (size <= _M_max_buf_size)) {
      // Get index.
      size_type idx = index(size);

      if ((_M_bufs[idx]) || (allocate(idx))) {
        buffer_type* buf = _M_bufs[idx];

        _M_bufs[idx] = buf->_M_next;

        // Invoke constructor.
        return new (buf) buffer_type();
      }
    }

    return nullptr;
  }

  template<typename Buffer>
  inline void buffer_pool<Buffer>::put(buffer_type* buf)
  {
    // Get index.
    size_type idx = index(buf->size());

    // Invoke destructor.
    buf->buffer_type::~buffer_type();

    buf->_M_next = _M_bufs[idx];

    _M_bufs[idx] = buf;
  }

  template<typename Buffer>
  inline size_t buffer_pool<Buffer>::allocated() const
  {
    return _M_allocated;
  }

  template<typename Buffer>
  bool buffer_pool<Buffer>::allocate(size_type idx)
  {
    size_type datasize = _M_slot_sizes[idx];

    size_t bufsize = sizeof(buffer_type) + datasize - 1;

    // Round up to multiple of word size.
    bufsize = (bufsize + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

    for (size_t i = 0;
         (i < number_preallocations) && (_M_allocated < _M_max_memory);
         i++, _M_allocated += bufsize) {
      buffer_type* buf;
      if ((buf = static_cast<buffer_type*>(malloc(bufsize))) != nullptr) {
        buf->_M_size = datasize;

        buf->_M_next = _M_bufs[idx];
        _M_bufs[idx] = buf;
      } else {
        break;
      }
    }

    return (_M_bufs[idx] != nullptr);
  }

  template<typename Buffer>
  inline typename buffer_pool<Buffer>::size_type
  buffer_pool<Buffer>::index(size_type size) const
  {
    return (size - _M_min_buf_size) / _M_step;
  }
}

#endif // UTIL_BUFFER_POOL_H
