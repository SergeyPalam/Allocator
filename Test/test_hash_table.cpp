#include "hash_table.h"
#include <string>
#include <gtest/gtest.h>
#include <set>

using namespace std;



TEST(HashTable, TestPrefixInc){
    HashTable<string> h_strings;
    ASSERT_EQ(h_strings.Size(), 0);
    h_strings.Insert("A");
    h_strings.Insert("C");
    h_strings.Insert("B");
    ASSERT_EQ(h_strings.Size(), 3);

    set<string> sorted_strings;
    auto it = h_strings.begin();
    sorted_strings.insert(*it);
    sorted_strings.insert(*(++it));
    sorted_strings.insert(*(++it));
    ++it;


    ASSERT_EQ(it, h_strings.end());

    ASSERT_EQ(sorted_strings.size(), 3);

    auto set_it = sorted_strings.begin();
    ASSERT_EQ(*(set_it++), "A"); 
    ASSERT_EQ(*(set_it++), "B");
    ASSERT_EQ(*(set_it++), "C");   
}

TEST(HashTable, TestPostfixInc){
    HashTable<string> h_strings;
    h_strings.Insert("B");
    h_strings.Insert("C");
    h_strings.Insert("A");

    set<string> sorted_strings;
    auto it = h_strings.begin();
    
    for(int i = 0;i < h_strings.Size();i++){
        sorted_strings.insert(*it);
        auto tmp1 = it;
        auto tmp2 = it++;    
        ASSERT_EQ(tmp1, tmp2);
        tmp2 = it;
        ASSERT_NE(tmp1, tmp2);        
    }

    ASSERT_EQ(it, h_strings.end());

    auto set_it = sorted_strings.begin();
    ASSERT_EQ(*(set_it++), "A"); 
    ASSERT_EQ(*(set_it++), "B");
    ASSERT_EQ(*(set_it++), "C");   
}

TEST(HashTable, TestPrefixDec){
    HashTable<string> h_strings;
    h_strings.Insert("C");
    h_strings.Insert("B");
    h_strings.Insert("A");

    set<string> sorted_strings;
    auto it = h_strings.end();
    sorted_strings.insert(*(--it));
    sorted_strings.insert(*(--it));
    sorted_strings.insert(*(--it));

    ASSERT_EQ(it, h_strings.begin());
    ASSERT_EQ(sorted_strings.size(), 3);

    auto set_it = sorted_strings.begin();
    ASSERT_EQ(*(set_it++), "A"); 
    ASSERT_EQ(*(set_it++), "B");
    ASSERT_EQ(*(set_it++), "C");   
}

TEST(HashTable, TestPostfixDec){
    HashTable<string> h_strings;
    h_strings.Insert("B");
    h_strings.Insert("C");
    h_strings.Insert("A");

    set<string> sorted_strings;
    auto it = h_strings.end();

    for(int i = 0;i < h_strings.Size();i++){
        auto tmp1 = it;
        auto tmp2 = it--;    
        ASSERT_EQ(tmp1, tmp2);
        tmp2 = it;
        ASSERT_NE(tmp1, tmp2);
        sorted_strings.insert(*it);
    }

    ASSERT_EQ(it, h_strings.begin());

    auto set_it = sorted_strings.begin();
    ASSERT_EQ(*(set_it++), "A"); 
    ASSERT_EQ(*(set_it++), "B");
    ASSERT_EQ(*(set_it++), "C");   
}

TEST(HashTable, TestSearch){
    HashTable<string> h_strings;
    
    for(int i = 0;i < 20000;i++){
        h_strings.Insert("Six");
    }

    h_strings.Insert("One");
    h_strings.Insert("Two");
    h_strings.Insert("Three");
    h_strings.Insert("Four");
    h_strings.Insert("Five");

    ASSERT_EQ(h_strings.Size(), 6);
    auto it = h_strings.Search("Five");
    ASSERT_EQ(*it, "Five");

    it = h_strings.Search("Seven");
    ASSERT_EQ(it, h_strings.end());      
}

TEST(HashTable, TestErase){
    HashTable<string> h_strings;
    
    h_strings.Insert("One");
    h_strings.Insert("Two");
    h_strings.Insert("Three");

    auto it = h_strings.Search("Two");
    ASSERT_TRUE(h_strings.Erase(it));    

    set<string> sorted_strings;
    for(const auto& str : h_strings){
        sorted_strings.insert(str);
    }

    ASSERT_EQ(sorted_strings.size(), 2);

    auto sorted_it = sorted_strings.begin();
    ASSERT_EQ(*sorted_it, string("One"));
    sorted_it++;
    ASSERT_EQ(*sorted_it, string("Three"));

    sorted_strings.clear();
    it = h_strings.Search("Two");
    ASSERT_FALSE(h_strings.Erase(it));   

    for(const auto& str : h_strings){
        sorted_strings.insert(str);
    }

    ASSERT_EQ(sorted_strings.size(), 2);

    sorted_it = sorted_strings.begin();
    ASSERT_EQ(*sorted_it, string("One"));
    sorted_it++;
    ASSERT_EQ(*sorted_it, string("Three"));

    sorted_strings.clear();
    it = h_strings.Search("Three");
    ASSERT_TRUE(h_strings.Erase(it));   

    for(const auto& str : h_strings){
        sorted_strings.insert(str);
    }

    ASSERT_EQ(sorted_strings.size(), 1);
    sorted_it = sorted_strings.begin();
    ASSERT_EQ(*sorted_it, string("One"));

    ASSERT_TRUE(h_strings.Erase(h_strings.begin()));
    ASSERT_EQ(h_strings.Size(), 0);
}