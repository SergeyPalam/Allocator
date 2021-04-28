#pragma once

#include <functional>
#include <variant>
#include <memory>
#include <cassert>

enum class CellState{NIL, DELETED};

template<class T, class Hash = std::hash<T>, class Allocator = std::allocator<T>>
class HashTable {
    using Cell = std::variant<T, CellState>;
    using CellAllocator = typename std::allocator_traits <Allocator> ::template rebind_alloc <Cell>;
    using CellAllocTrait = std::allocator_traits<CellAllocator>;

    static const int INIT_SIZE = 1;
    static const int OCCUPANCY_PERCENT = 80;

    CellAllocator alloc;
    Cell*         begin_addr;
    Cell*         end_addr;

    uint32_t  size;

    uint32_t h(uint32_t hash, uint32_t num_probe, uint32_t cap) const{
        return (hash % cap + num_probe * (hash % 2 == 0 ? hash + 1 : hash)) % cap;
    }

    void extend_capacity(uint32_t new_cap) {
        if (new_cap < Capacity())
            return;

        Cell* new_begin_addr = CellAllocTrait::allocate(alloc, new_cap);
        Cell* new_end_addr = new_begin_addr + new_cap;

        for (Cell* p_i = new_begin_addr; p_i < new_end_addr; p_i++) {
            CellAllocTrait::construct(alloc, p_i, CellState::NIL);
        }
        
        for (Cell* p_i = begin_addr, *new_p_i = new_begin_addr; p_i < end_addr; p_i++, new_p_i++) {
            if(std::holds_alternative<CellState>(*p_i))
                continue;

            for (uint32_t num_probe = 0; num_probe < new_cap; num_probe++) {
                uint32_t cur_idx = h(Hash{}(std::get<T>(*p_i)), num_probe, new_cap);
                if (std::holds_alternative<CellState>(new_begin_addr[cur_idx])) {
                    CellAllocTrait::destroy(alloc, new_begin_addr + cur_idx);
                    CellAllocTrait::construct(alloc, new_begin_addr + cur_idx, std::forward<Cell>(*p_i));
                    CellAllocTrait::destroy(alloc, p_i);
                    break;
                }
            }
        }

        CellAllocTrait::deallocate(alloc, begin_addr, end_addr - begin_addr);
        begin_addr = new_begin_addr;
        end_addr = new_end_addr;
    }

    public:

    class iterator: public std::iterator< std::bidirectional_iterator_tag, T> {
        Cell*    p_idx;
        Cell*    begin_addr;
        Cell*    end_addr;
    public:
        explicit iterator(Cell* i_idx, Cell* i_begin_addr, Cell* i_end_addr) : p_idx(i_idx),
                                                                                begin_addr(i_begin_addr),
                                                                                end_addr(i_end_addr) {}
        iterator& operator++() {
            assert(p_idx != end_addr);

            do {
                p_idx++;
            } while (std::holds_alternative<CellState>(*p_idx) && p_idx != end_addr);

            return *this;
        }

        iterator operator++(int) {
            iterator res = *this;
            ++(*this);
            return res;
        }

        iterator& operator--() {
            assert(p_idx != begin_addr);

            Cell* tmp = p_idx;

            do {
                tmp--;
            } while (std::holds_alternative<CellState>(*tmp) && tmp != begin_addr);

            if (std::holds_alternative<T>(*tmp)) {
                p_idx = tmp;
            }

            return *this;
        }

        iterator operator--(int) {
            iterator res = *this;
            --(*this);
            return res;
        }

        bool operator==(iterator other) const{
            return p_idx == other.p_idx;
        }

        bool operator!=(iterator other) const{
            return !((*this) == other);
        }

        const T& operator*() const{
            return std::get<T>(*(p_idx));
        }

        friend class HashTable<T, Hash, Allocator>;
    };

    HashTable() : size(0) {        
        begin_addr = CellAllocTrait::allocate(alloc, INIT_SIZE);
        end_addr = begin_addr + (INIT_SIZE);
        CellAllocTrait::construct(alloc, begin_addr, CellState::NIL);
    }

    ~HashTable() {
        Clear();
    }

    void Clear(){
        for(Cell* i_p = begin_addr;i_p < end_addr;i_p++){
            CellAllocTrait::destroy(alloc, i_p);
        }

        CellAllocTrait::deallocate(alloc, begin_addr, end_addr - begin_addr);
    }

    iterator begin() const{
        Cell* tmp = begin_addr;
        while (std::holds_alternative<CellState>(*tmp) && tmp != end_addr) {
            tmp++;
        }

        return iterator(tmp, begin_addr, end_addr);
    }

    iterator end() const{
        return iterator(end_addr, begin_addr, end_addr);
    }

    iterator Insert(T&& value) {
        if(Capacity() == 0){
            extend_capacity(INIT_SIZE);
        }else if ((size * 100.0) / Capacity() > OCCUPANCY_PERCENT) {
            extend_capacity(Capacity() * 2);
        }

        for (uint32_t num_probe = 0; num_probe < Capacity(); num_probe++) {
            uint32_t cur_idx = h(Hash{}(value), num_probe, Capacity());
            if (std::holds_alternative<CellState>(begin_addr[cur_idx])) {
                CellAllocTrait::destroy(alloc, begin_addr + cur_idx);
                CellAllocTrait::construct(alloc, begin_addr + cur_idx, std::forward<T>(value));

                size++;
                return iterator(begin_addr + cur_idx, begin_addr, end_addr);
            }else if(std::get<T>(begin_addr[cur_idx]) == value){
                return end();
            }
        }

        return end();
    }

    bool Erase(iterator it) {
        if (it == end())
            return false;

        CellAllocTrait::destroy(alloc, it.p_idx);
        CellAllocTrait::construct(alloc, it.p_idx, CellState::DELETED);
        size--;
        return true;
    }

    iterator Search(const T& value) const{
        for (uint32_t num_probe = 0; num_probe < Capacity(); num_probe++) {
            uint32_t cur_idx = h(Hash{}(value), num_probe, Capacity());
            if(std::holds_alternative<CellState>(begin_addr[cur_idx]))
                return end();

            if (std::get<T>(begin_addr[cur_idx]) == value) {
                return iterator(begin_addr + cur_idx, begin_addr, end_addr);
            }
        }

        return end();
    }

    uint32_t Capacity() const{
        return end_addr - begin_addr;
    }

    uint32_t Size() const{
        return size;
    }
};