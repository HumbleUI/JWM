#pragma once

#include <wayland-client.h>
#include <utility>

namespace jwm {
    class ShmPool {
    public:
        ShmPool(wl_shm* shm, size_t size);
        ~ShmPool();

        size_t _size;
        int _fd;
        wl_shm_pool* _pool;
        uint8_t* _rawData;

        // grows current file to at least this size
        void grow(size_t size);

        std::pair<wl_buffer*, uint8_t*> createBuffer(int offset, int width, int height, int stride, uint32_t format);

        int _createShmFile();
        int _allocateShmFile(size_t size);
        void close();
    };
}
