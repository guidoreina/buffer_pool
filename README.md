# Buffer pool
Template class for allocating and managing buffers of different sizes.

To initialize a buffer pool you specify the minimum buffer size (`min_buf_size`), the maximum buffer size (`max_buf_size`) and the number of different buffer sizes in the range `min_buf_size` - `max_buf_size` (`nslots`).

To get a buffer: call the method `buffer_pool::get()` specifying the buffer size.

To return a buffer back to the pool: call the method `buffer_pool::put()` specifying the buffer to be returned.

A buffer is a template containing a header (user defined) and data (which is an array of the buffer size).

Use case: you can use the buffer pool to store IP packets. In the header you can save data like the IP addresses, ports and payload length; in the data you store the packet payload.
