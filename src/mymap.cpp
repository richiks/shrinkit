#include "mymap.h"
#include "vector.h"
#include <algorithm>
using namespace std;

// constructor for MyMap

MyMap::MyMap() {
    nBuckets = 10;
    buckets = createBucketArray(nBuckets);
}

// destructor for MyMap

MyMap::~MyMap() {
    for(int i = 0; i < nBuckets; ++i){
            key_val_pair* tempNode = buckets[i];
            while(tempNode != nullptr){
                tempNode = tempNode->next;
                delete tempNode;
            }
        }
        delete[] buckets;
}

/* This function adds a node with a key and a value to the map.
 * Parameters are the key and the value.
 */

void MyMap::put(int key, int value) {
    key_val_pair *current = buckets[hashFunction(key) % nBuckets];

    if(!containsKey(key)) {

        key_val_pair* node = new key_val_pair;
        node->key = key;
        node->value = value;
        node->next = current;
        buckets[hashFunction(key) % nBuckets] = node;
    }
    else {
        while(current != nullptr) {
               if(current->key == key){
                   current->value = (current -> value)+1;
                   return;
               }
               current = current->next;
        }
    }
    nElems++;
}

/* This function gets the value associated with a certain key in the map. Throws a string exception
 * if the key doesn't exist in the map.
 * Parameter is the key.
 * Returns the value associated with the key.
 */

int MyMap::get(int key) const {

  key_val_pair *current = buckets[hashFunction(key) % nBuckets];

  while(current != nullptr){
      if(current->key == key){
          return current->value;
      }
      current = current->next;
  }
  throw ("The key is not in the map...");
  return 1;
}

/* This function checks if the map contains the key.
 * Parameter is the key.
 * Returns true if the map contains the key, otherwise false.
 */

bool MyMap::containsKey(int key) {

        key_val_pair *current = buckets[hashFunction(key) % nBuckets];

        while(current != nullptr){
            if(current->key == key){
                return true;
            }
            current = current->next;
        }
    return false;
}

/* This function returns a vector of keys in the map.
 * No parameters.
 * Returns a vector of all the keys in the map.
 */

Vector<int> MyMap::keys() const {
     Vector<int> keys;
     for(int i = 0; i < nBuckets; i++){
         key_val_pair *current = buckets[i];

         while(current != nullptr){
             keys.add(current->key);
             current = current->next;
         }
     }
     sort(keys.begin(),keys.end());
     return keys;
}

/* This function returns the number of entries in the map.
 * No parameters.
 */

int MyMap::size() {
    return nElems;
}

/**
 * STARTER CODE, DO NOT MODIFY
 */

// copy constructor
MyMap::MyMap(const MyMap &myMap) {
    // make a deep copy of the map
    nBuckets = myMap.nBuckets;

    buckets = createBucketArray(nBuckets);

    // walk through the old array and add all elements to this one
    Vector<int> keys = myMap.keys();
    for (int i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

}

// assignment overload
MyMap& MyMap::operator= (const MyMap &myMap) {
    // make a deep copy of the map

    // watch for self-assignment
    if (this == &myMap) {
        return *this;
    }

    // if data exists in the map, delete it
    for (int i=0; i < nBuckets; i++) {
        MyMap::key_val_pair* bucket = buckets[i];
        while (bucket != nullptr) {
            // walk the linked list and delete each node
            MyMap::key_val_pair* temp = bucket;
            bucket = bucket->next;
            delete temp;
        }
        buckets[i] = nullptr;
    }
    nElems = 0;
    // walk through the old array and add all elements to this one
    Vector<int> keys = myMap.keys();
    for (int i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

    // return the existing object so we can chain this operator
    return *this;
}

ostream &operator<<(ostream &out, MyMap &myMap) {
    out << "{";
    Vector<int> keys = myMap.keys();
    for (int i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        out << key << ":" << value;
        if (i < keys.size() - 1) { // print the comma after each pair except the last
            out << ", ";
        }
    }
    out << "}";
    return out;
}

istream &operator>>(istream &in, MyMap &myMap) {
    // assume the format {1:2, 3:4}
    bool done = false;
    in.get(); // get the first char, {
    int nextChar = in.get(); // get the first real character
    while (!done) {
        string nextInput;
        while (nextChar != ',' and nextChar != '}') {
                nextInput += nextChar;
                nextChar = in.get();
        }
        if (nextChar == ',') {
            // read the space as well
            in.get(); // should be a space
            nextChar = in.get(); // get the next character
        } else {
            done = true; // we have reached }
        }
        // at this point, nextInput should be in the form 1:2
        // (we should have two integers separated by a colon)
        // BUT, we might have an empty map (special case)
        if (nextInput != "") {
            vector<string> kvp = stringSplit(nextInput,":");
            myMap.put(stringToInteger(kvp[0]),stringToInteger(kvp[1]));
        }
    }
    return in;
}
/**
 * STARTER CODE, DO NOT MODIFY
 *
 * Given a number of buckets, creates a hashtable (array of linked list heads).
 *
 * @param nBuckets the number of buckets you want in the hashtable.
 * return an array of heads of linked lists of key_val_pairs
 */
MyMap::bucketArray MyMap::createBucketArray(int nBuckets) {
    bucketArray newBuckets = new key_val_pair*[nBuckets];
    for (int i = 0; i < nBuckets; i++) {
        newBuckets[i] = nullptr;
    }
    return newBuckets;
}

/**
 * STARTER CODE
 * The hash function for your HashMap implementation.
 * For an extension, you might want to improve this function.
 *
 * @param input - an integer to be hashed
 * return the hashed integer
 */
int MyMap::hashFunction(int input) const {
    // use unsigned integers for calculation
    // we are also using so-called "magic numbers"
    // see https://stackoverflow.com/a/12996028/561677 for details
    unsigned int temp = ((input >> 16) ^ input) * 0x45d9f3b;
    temp = ((temp >> 16) ^ temp) * 0x45d9f3b;
    temp = (temp >> 16) ^ temp;

    // convert back to positive signed int
    // (note: this ignores half the possible hashes!)
    int hash = (int) temp;
    if (hash < 0) {
        hash *= -1;
    }

    return hash;
}

/**
* @brief MyMap::sanityCheck
*
* Checks a map for internal consistency
*/
void MyMap::sanityCheck(){
  for(int i = 0; i < 1000; i++) {
      put(i,i);
  }
  for(int i = 0; i < 1000; i++) {

      if(!containsKey(i)) {
          string err = integerToString(i) + " should be a key in the map but cannot be found";
      }
      int val;

      try {
          val = get(i);
      } catch(string exception) {
          string err = "Unable to get value for " + integerToString(i);
          throw(err);
      }

      if (i != val) {
          string err = integerToString(i) + " should be mapped to " + integerToString(i) + " but is mapped to " + integerToString(val) + "instead";
          throw(err);
      }
  }
  cout << "Map seems ok" << endl;
}
