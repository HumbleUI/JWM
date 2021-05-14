#pragma once
#include <atomic>
#include <cassert>
#include <iostream>

namespace jwm {

// Adopted from https://github.com/google/skia/blob/master/include/core/SkRefCnt.h
class RefCounted {
public:
    RefCounted(): fRefCount(1) {
    }

    virtual ~RefCounted() {
        // std::cout << "~RefCounted " << getRefCount() << std::endl;
        assert(getRefCount() == 0);
        fRefCount.store(0, std::memory_order_relaxed);
    }

    /**
     * Increment the reference count. Must be balanced by a call to unref().
     */
    void ref() const {
        assert(getRefCount() > 0);
        (void) fRefCount.fetch_add(+1, std::memory_order_relaxed);
    }

    /** 
     * Decrement the reference count. If the reference count is 1 before the
     * decrement, then delete the object. Note that if this is the case, then
     * the object needs to have been allocated via new, and not on the stack.
     */
    void unref() const {
        assert(getRefCount() > 0);
        // A release here acts in place of all releases we "should" have been doing in ref().
        if (1 == fRefCount.fetch_add(-1, std::memory_order_acq_rel)) {
            // Like unique(), the acquire is only needed on success, to make sure
            // code in internal_dispose() doesn't happen before the decrement.
            // std::cout << "unref " << getRefCount() << std::endl;
            delete this;
        }
    }

    int32_t getRefCount() const {
        return fRefCount.load(std::memory_order_relaxed);
    }

    /**
     * May return true if the caller is the only owner.
     * Ensures that all previous owner's actions are complete.
     */
    bool unique() const {
        if (1 == fRefCount.load(std::memory_order_acquire)) {
            // The acquire barrier is only really needed if we return true.  It
            // prevents code conditioned on the result of unique() from running
            // until previous owners are all totally done calling unref().
            return true;
        }
        return false;
    }

private:    
    mutable std::atomic<int32_t> fRefCount;

    RefCounted(RefCounted&&) = delete;
    RefCounted(const RefCounted&) = delete;
    RefCounted& operator=(RefCounted&&) = delete;
    RefCounted& operator=(const RefCounted&) = delete;
};

void unrefRefCounted(RefCounted* instance);

template <typename T> static inline T* ref(T* obj) {
    obj->ref();
    return obj;
}

template <typename T> static inline void unref(T** obj) {
    (*obj)->unref();
    *obj = nullptr;
}

} // namespace jwm
