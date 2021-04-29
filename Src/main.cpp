#include "custom_allocator.h"
#include "hash_table.h"
#include "memory_pool.h"
#include <map>
#include <iostream>
#include <vector>

using namespace std;

uint32_t Factorial(uint32_t val){
    uint32_t prod = 1;
    for(int i = 1;i <= val;i++){
        prod *= i;
    }
    return prod;
}

int main(){
    map<int, int> standart_dict;
    map<int, int, less<int>, CustomAllocator<pair<int, int>, 10 * sizeof(pair<int, int>)>> custom_dict;
    for(int i = 0;i < 10;i++){
        standart_dict[i] = Factorial(i);
        custom_dict[i] = Factorial(i);
    }

    for(int i = 0;i < 10;i++){
        cout << "Standart dict " << i << " " << standart_dict[i] << endl;
        cout << "Custom dict " << i << " " << custom_dict[i] << endl; 
    }
 
    HashTable<int> standart_hash;
    HashTable<int, std::hash<int>, CustomAllocator<int, sizeof(int) * 10 * 2>> custom_hash;
    for(int i = 0;i < 10;i++){
        standart_hash.Insert(Factorial(i));
        custom_hash.Insert(Factorial(i));           
    }

    for(int val : standart_hash){
        cout << "Standart hash " << val << endl;
    }

    for(int val : custom_hash){
        cout << "Custom hash  " << val << endl;
    }

}