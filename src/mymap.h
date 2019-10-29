#pragma once

#include "vector.h"
#include <ostream>
#include <istream>
using namespace std;


class MyMap
{
public:
    MyMap();
    ~MyMap();

    int get(int key) const;
    void put(int key, int value);
    bool containsKey(int key);
    Vector<int> keys() const;
    int size();

    void sanityCheck();
    MyMap(const MyMap &myMap); // copy constructor
    MyMap& operator= (const MyMap &myMap); // assignment overload
    friend ostream &operator<<(ostream &out, MyMap &myMap);
    friend istream &operator>>(istream &in, MyMap &myMap);
private:

    struct key_val_pair {
        int key;
        int value;
        key_val_pair* next;
    };

    typedef key_val_pair** bucketArray;

    bucketArray createBucketArray(int nBuckets);
    int hashFunction(int input) const;

    bucketArray buckets;

    int nBuckets;
    int nElems;
};
