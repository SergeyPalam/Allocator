#include "memory_pool.h"
#include <gtest/gtest.h>

TEST(MemoryPool, CheckSizePool){
    {
        const uint32_t SIZE_B_INF = MemoryPool<47>::SIZE_BLOCK_INFO;
        const uint32_t SIZE_POOL = SIZE_B_INF * 2 + 1;
        MemoryPool<SIZE_POOL> mem_pool;
        ASSERT_EQ(mem_pool.GetFreeBytes(), 1);
    }

    {
        const uint32_t SIZE_B_INF = MemoryPool<47>::SIZE_BLOCK_INFO;
        const uint32_t SIZE_POOL = 1024;
        MemoryPool<SIZE_POOL> mem_pool;
        ASSERT_EQ(mem_pool.GetFreeBytes(), SIZE_POOL - SIZE_B_INF * 2);
        
        void* p = mem_pool.Alloc(mem_pool.GetFreeBytes());
        ASSERT_NE(p, nullptr);
        ASSERT_EQ(mem_pool.GetFreeBytes(), 0);
    }

    {
        const uint32_t SIZE_B_INF = MemoryPool<47>::SIZE_BLOCK_INFO;
        const uint32_t SIZE_POOL = 512;
        MemoryPool<SIZE_POOL> mem_pool;        
        
        void* p = mem_pool.Alloc(mem_pool.GetFreeBytes() + 1);        
        ASSERT_EQ(p, nullptr);
    }

    {
        const uint32_t SIZE_B_INF = MemoryPool<47>::SIZE_BLOCK_INFO;
        const int ALLOC_SIZE = 8;

        const uint32_t SIZE_POOL =  4 * SIZE_B_INF + 3 * ALLOC_SIZE;
        MemoryPool<SIZE_POOL> mem_pool;        
        
        ASSERT_EQ(mem_pool.GetFreeBytes(), 2 * SIZE_B_INF + 3 * ALLOC_SIZE);
        
        void* p_lhs = mem_pool.Alloc(ALLOC_SIZE);
        ASSERT_NE(p_lhs, nullptr);

        *((uint16_t*) p_lhs) = 0xFFFF;
        ASSERT_EQ(mem_pool.GetFreeBytes(), SIZE_B_INF + 2 * ALLOC_SIZE);

        void* p_mid = mem_pool.Alloc(ALLOC_SIZE);                
        ASSERT_NE(p_mid, nullptr);
        *((uint16_t*) p_mid) = 0xFFFF;

        ASSERT_EQ(mem_pool.GetFreeBytes(), ALLOC_SIZE);

        void* p_rhs = mem_pool.Alloc(ALLOC_SIZE);                
        ASSERT_NE(p_rhs, nullptr);
        *((uint16_t*) p_rhs) = 0xFFFF;

        ASSERT_EQ(mem_pool.GetFreeBytes(), 0);

        mem_pool.Free(p_mid);
        ASSERT_EQ(mem_pool.GetFreeBytes(), ALLOC_SIZE);
        mem_pool.Free(p_lhs);
        ASSERT_EQ(mem_pool.GetFreeBytes(), SIZE_B_INF + 2 * ALLOC_SIZE);
        mem_pool.Free(p_rhs);
        ASSERT_EQ(mem_pool.GetFreeBytes(), 2 * SIZE_B_INF + 3 * ALLOC_SIZE);
    }    
}
