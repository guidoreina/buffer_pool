#ifndef UTIL_BUFFER_H
#define UTIL_BUFFER_H

#include <stdint.h>

namespace util {
  template<typename Header, typename SizeType = uint16_t>
  class buffer {
    template<typename Buffer>
    friend class buffer_pool;

    public:
      typedef Header header_type;
      typedef SizeType size_type;

      // Constructor.
      buffer();

      // Destructor.
      ~buffer() = default;

      // Get size.
      size_type size() const;

    private:
      buffer* _M_next;

      size_type _M_size;

    public:
      header_type header;

      uint8_t data[1];
  };

  template<typename Header, typename SizeType>
  inline buffer<Header, SizeType>::buffer()
  {
  }

  template<typename Header, typename SizeType>
  inline typename
  buffer<Header, SizeType>::size_type buffer<Header, SizeType>::size() const
  {
    return _M_size;
  }
}

#endif // UTIL_BUFFER_H
