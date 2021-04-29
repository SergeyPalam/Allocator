#pragma once
#include <cstdint>
#include <cstdlib>

template<uint32_t SIZE_POOL>
class MemoryPool {
	struct BlockLink_t {
		BlockLink_t* p_next_free_block;
		uint32_t     size_block;
	};

public:
	static const uint32_t SIZE_BLOCK_INFO = sizeof(BlockLink_t);
	static_assert(SIZE_POOL > SIZE_BLOCK_INFO * 2, "Size_pool is too small");
	static_assert(SIZE_POOL < (1 << 31), "Size pool is too large");

	static const uint32_t ALLOC_FLAG = (1 << 31);

	MemoryPool(const MemoryPool&) = delete;
	MemoryPool(MemoryPool&&) = delete;
	MemoryPool& operator = (const MemoryPool&) = delete;
	MemoryPool& operator = (MemoryPool&&) = delete;



private:
	uint32_t free_bytes;

	BlockLink_t  free_mem_start;
	BlockLink_t* p_free_mem_end;
	uint8_t* mem_pool;

	void insert_free_block(BlockLink_t* p_free_block_to_insert) {
		BlockLink_t* p_iterator;
		uint8_t *p_current_addr;

		/* Iterate through the list until a block is found that has a higher address
		than the block being inserted. */
		for (p_iterator = &free_mem_start; p_iterator->p_next_free_block < p_free_block_to_insert;
			p_iterator = p_iterator->p_next_free_block);


		/* Do the block being inserted, and the block it is being inserted after
		make a contiguous block of memory? */
		p_current_addr = (uint8_t *)p_iterator;
		if ((p_current_addr + p_iterator->size_block) == (uint8_t *)p_free_block_to_insert) {
			p_iterator->size_block += p_free_block_to_insert->size_block;
			p_free_block_to_insert = p_iterator;
			free_bytes += SIZE_BLOCK_INFO;
		}


		/* Do the block being inserted, and the block it is being inserted before
		make a contiguous block of memory? */
		p_current_addr = (uint8_t *)p_free_block_to_insert;
		if ((p_current_addr + p_free_block_to_insert->size_block) == (uint8_t *)p_iterator->p_next_free_block) {
			if (p_iterator->p_next_free_block != p_free_mem_end) {
				/* Form one big block from the two blocks. */
				p_free_block_to_insert->size_block += p_iterator->p_next_free_block->size_block;
				p_free_block_to_insert->p_next_free_block = p_iterator->p_next_free_block->p_next_free_block;
				free_bytes += SIZE_BLOCK_INFO;
			}
			else
			{
				p_free_block_to_insert->p_next_free_block = p_free_mem_end;
			}
		}
		else
		{
			p_free_block_to_insert->p_next_free_block = p_iterator->p_next_free_block;
		}


		if (p_iterator != p_free_block_to_insert)
		{
			p_iterator->p_next_free_block = p_free_block_to_insert;
		}
	}

public:
	MemoryPool() {
		BlockLink_t *p_first_free_block;
		uint32_t total_heap_size = SIZE_POOL;

		mem_pool = (uint8_t*)malloc(SIZE_POOL);

		free_mem_start.p_next_free_block = (BlockLink_t*)mem_pool;
		free_mem_start.size_block = 0;

		p_free_mem_end = (BlockLink_t*)(mem_pool + SIZE_POOL - SIZE_BLOCK_INFO);
		p_free_mem_end->size_block = 0;
		p_free_mem_end->p_next_free_block = nullptr;

		p_first_free_block = (BlockLink_t*)(mem_pool);
		p_first_free_block->size_block = SIZE_POOL - SIZE_BLOCK_INFO;

		p_first_free_block->p_next_free_block = p_free_mem_end;

		free_bytes = p_first_free_block->size_block - SIZE_BLOCK_INFO;
	}

	~MemoryPool() {
		free(mem_pool);
	}

	void* Alloc(uint32_t wanted_size) {
		BlockLink_t* p_current_block;
		BlockLink_t* p_prev_block;
		BlockLink_t* p_new_block;

		void *p_return = nullptr;

		wanted_size += SIZE_BLOCK_INFO;

		if (wanted_size <= free_bytes + SIZE_BLOCK_INFO) {
			p_prev_block = &free_mem_start;
			p_current_block = free_mem_start.p_next_free_block;
			while ((p_current_block->size_block < wanted_size) && (p_current_block->p_next_free_block != nullptr)) {
				p_prev_block = p_current_block;
				p_current_block = p_current_block->p_next_free_block;
			}

			if (p_current_block != p_free_mem_end) {
				/* Return the memory space pointed to - jumping over the
				BlockLink_t structure at its start. */
				p_return = (void *)(((uint8_t *)p_prev_block->p_next_free_block) + SIZE_BLOCK_INFO);

				/* This block is being returned for use so must be taken out
				of the list of free blocks. */
				p_prev_block->p_next_free_block = p_current_block->p_next_free_block;

				/* If the block is larger than required it can be split into
				two. */
				if ((p_current_block->size_block - wanted_size) > SIZE_BLOCK_INFO)
				{
					p_new_block = (BlockLink_t *)(((uint8_t *)p_current_block) + wanted_size);

					/* Calculate the sizes of two blocks split from the
					single block. */
					p_new_block->size_block = p_current_block->size_block - wanted_size;
					p_current_block->size_block = wanted_size;
					free_bytes -= SIZE_BLOCK_INFO;

					/* Insert the new block into the list of free blocks. */
					insert_free_block(p_new_block);
				}

				free_bytes -= p_current_block->size_block - SIZE_BLOCK_INFO;

				/* The block is being returned - it is allocated and owned
				by the application and has no "next" block. */

				p_current_block->size_block |= ALLOC_FLAG;
				p_current_block->p_next_free_block = nullptr;
			}

		}

		return p_return;
	}

	void Free(void* p) {
		uint8_t *p_free_addr = (uint8_t *)p;
		BlockLink_t* p_free_block;

		if (p_free_addr != nullptr) {
			p_free_addr -= SIZE_BLOCK_INFO;

			p_free_block = (BlockLink_t*)p_free_addr;

			if ((p_free_block->size_block & ALLOC_FLAG) != 0) {
				if (p_free_block->p_next_free_block == nullptr)
				{
					/* The block is being returned to the heap - it is no longer
					allocated. */
					p_free_block->size_block &= ~ALLOC_FLAG;


					/* Add this block to the list of free blocks. */
					free_bytes += p_free_block->size_block - SIZE_BLOCK_INFO;
					insert_free_block(p_free_block);
				}
			}
		}
	}

	uint32_t GetFreeBytes() const {
		return free_bytes;
	}
};