#pragma once

#include <cassert>
#include <stdexcept>
#include <string>

namespace engine::memory {

    template<typename T>
    class Pool {

        // We use the space in the pool
        // to store both the elements of type T
        // and a linked-list of free slots
        // and achieve a O(1) insert/delete complexity
        union PoolElement {
            T datum;
            PoolElement *next;
        };

    public:
        Pool() = delete;

        Pool(Pool &&) = delete;

        Pool(const Pool &) = delete;

        explicit Pool(size_t size) : size(size) {
            // Reserve memory 
            data = (PoolElement *) calloc(size, sizeof(PoolElement));
            if (data == nullptr)
                throw std::runtime_error("Out of memory");

            // Make sure the OS gives us the actual memory right now
            memset((void *) data, 0, size * sizeof(PoolElement));

            // Set head pointer 
            firstFreeBlock = data;

            // Initialize free-list
            for (int i = 0; i < size - 1; i++) {
                data[i].next = &data[i + 1];
            }
        }

        ~Pool() {
            // if there is (c)alloc, there must be free
            free(data);

            // always null ptrs
            data = nullptr;
        }

        // Reserve the size for one T        
        // Note: does not constructor
        T *alloc(size_t count) {
            // allocate a single block
            assert(count == 1);

            // allow just a single element 
            assert(numAllocated < size);

            T *ptr = (T *) firstFreeBlock;
            firstFreeBlock = firstFreeBlock->next;
            numAllocated++;
            return ptr;
        }

        // Free the size for one T        
        // Note: does not destructor
        void free(T *ptr) {
            PoolElement *head = (PoolElement *) ptr;
            head->next = firstFreeBlock;
            firstFreeBlock = head;
            numAllocated--;
        }

    public:
        const size_t size;

    private:
        size_t numAllocated = 0;
        PoolElement *firstFreeBlock = nullptr;
        PoolElement *data = nullptr;
    };

}

