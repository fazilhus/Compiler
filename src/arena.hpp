//
// Created by khusn on 9/8/2023.
//

#pragma once

class ArenaAllocator {
public:
    inline ArenaAllocator(std::size_t bytes) : m_size(bytes) {
        m_buffer = static_cast<std::byte*>(malloc(m_size));
        m_offset = m_buffer;
    }

    inline ArenaAllocator(const ArenaAllocator&) = delete;
    inline ~ArenaAllocator() {
        free(m_buffer);
    }

    inline ArenaAllocator operator=(const ArenaAllocator&) = delete;

    template <typename T>
    inline T* alloc() {
        void* offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T*>(offset);
    }

private:
    std::size_t m_size;
    std::byte* m_buffer;
    std::byte* m_offset;
};
