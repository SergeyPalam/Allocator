#include "memory_pool.h"
#include <utility>

template<typename T, uint32_t RESERVE_SIZE = 0xFFFF>
struct CustomAllocator {
private:
	static MemoryPool<RESERVE_SIZE> mem_pool;
public:
	using value_type = T;	

	template<typename U>
	struct rebind {
		using other = CustomAllocator<U>;
	};

	CustomAllocator() = default;	
	~CustomAllocator() = default;

	CustomAllocator(const CustomAllocator<T, RESERVE_SIZE>&) {

	}

	template<typename U>
	CustomAllocator(const CustomAllocator<U>&) {

	}

	T *allocate(std::size_t n) {
		return (T*)(mem_pool.Alloc(n * sizeof(T)));
	}

	void deallocate(T *p, std::size_t n) {
		mem_pool.Free(p);
	}

	void construct(T *p, const T& val) {
		new ((void*)p) T(val);
	}

	template<typename U, typename ...Args>
	void construct(U *p, Args &&...args) {
		new(p) U(std::forward<Args>(args)...);
	};

	void destroy(T *p) {
		p->~T();
	}

	template<typename U>
	void destroy(U *p) {
		p->~U();
	}
};

template<typename T, uint32_t RESERVE_SIZE>
MemoryPool<RESERVE_SIZE> CustomAllocator<T, RESERVE_SIZE>:: mem_pool;