#include <gtest/gtest.h>
#include "custom_allocator.h"
#include <vector>

using namespace std;

class Counter{
    static int cntr;
public:
    Counter(){
        cntr++;
    }

    Counter(const Counter&){
        cntr++;
    }

    ~Counter(){
        cntr--;
    }

    static int GetCounter(){
        return cntr;
    }
};

int Counter::cntr = 0;

TEST(custom_allocator, test_count_constructions){
    vector<Counter, CustomAllocator<Counter>> test_vec;
    for(int i = 0;i < 10;i++){
        test_vec.push_back(Counter());
    }

    ASSERT_NE(Counter::GetCounter(), 0);
    test_vec.clear();
    ASSERT_EQ(Counter::GetCounter(), 0);   

}