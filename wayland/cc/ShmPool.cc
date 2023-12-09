#include "ShmPool.hh"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <system_error>

using namespace jwm;

static void randname(char *buf)
{
    	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	long r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = 'A'+(r&15)+(r&16)*2;
		r >>= 5;
	}
}
ShmPool::ShmPool(wl_shm* shm, size_t size):
    _size(size) {
        _fd = _allocateShmFile(size);
        if (_fd < 0) {
            // why : (
            throw std::system_error(EIO, std::generic_category(), "Couldn't allocate buffer");
        }
        _rawData = (uint8_t*)mmap(nullptr, size,
                PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
        _pool = wl_shm_create_pool(shm, _fd, size);

    }
ShmPool::~ShmPool() {
    close();
}
void ShmPool::close() {
    wl_shm_pool_destroy(_pool);
    ::close(_fd);
    munmap(_rawData, _size);
}

void ShmPool::grow(size_t size) {
    if (size <= _size)
        return;
    int ret;
    do {
        ret = ftruncate(_fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        // AAHHHHH!
        throw std::system_error(EIO, std::generic_category(), "Couldn't grow buffer");
    }
    uint8_t* newData = (uint8_t*)mmap(nullptr, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    // do I need to memcpy??? lets say no :troll:
    // TODO: error checking :troll:
    munmap(_rawData, _size);
    _rawData = newData;
    _size = size;
    wl_shm_pool_resize(_pool, size);
}

int ShmPool::_createShmFile() {
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

int ShmPool::_allocateShmFile(size_t size) {
    int fd = _createShmFile();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        ::close(fd);
        return -1;
    }
    return fd;
}

std::pair<wl_buffer*, uint8_t*> ShmPool::createBuffer(int offset, int width, int height, int stride, uint32_t format) {
   wl_buffer* buffer = wl_shm_pool_create_buffer(_pool, offset, width, height, stride, format);
   uint8_t* data = &_rawData[offset];
   return std::pair<wl_buffer*, uint8_t*>(buffer, data);
}
