#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util/buffer_pool.h"

class header {
  public:
    // Constructor.
    header() = default;

    // Destructor.
    ~header() = default;

    uint8_t buf[16];
    size_t count;
};

int main()
{
  typedef uint16_t size_type;

  static const size_type min_buf_size = 100;
  static const size_type max_buf_size = 1500;
  static const size_type nslots = 10;

  static const size_t niterations = 1024;
  static const size_t nbuffers = 1024;

  typedef util::buffer<header, size_type> buffer;

  util::buffer_pool<buffer> buffer_pool;

  // Initialize buffer pool.
  if (buffer_pool.init(min_buf_size, max_buf_size, nslots)) {
    for (size_t i = 0; i < niterations; i++) {
      buffer* bufs[nbuffers];

      for (size_t j = 0; j < nbuffers; j++) {
        size_type size = min_buf_size +
                         (random() % (max_buf_size - min_buf_size + 1));

        // Get buffer.
        buffer* buf;
        if ((buf = buffer_pool.get(size)) != nullptr) {
          memset(buf->data, '0', size);

          bufs[j] = buf;
        } else {
          fprintf(stderr, "Error getting buffer of size %u.\n", size);

          // Return buffers.
          for (size_t k = 0; k < j; k++) {
            buffer_pool.put(bufs[k]);
          }

          return -1;
        }
      }

      // Return buffers.
      for (size_t j = 0; j < nbuffers; j++) {
        buffer_pool.put(bufs[j]);
      }
    }

    return 0;
  } else {
    fprintf(stderr, "Error initializing buffer pool.\n");
    return -1;
  }
}
