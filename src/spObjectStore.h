/**
 * @file spObjectStore.h
 * @author krokoreit (krokoreit@gmail.com)
 * @brief a templated class to store, retrieve, delete and iterate through objects based on an id
 * @version 2.0.0
 * @date 2024-05-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SPOBJECTSTORE_H_
#define SPOBJECTSTORE_H_


#include <Arduino.h>
#include <vector>

/**
 *  Notes:
 *  - we cannot have the class and function definitions in a separate .cpp file
 *    because this will cause “undefined reference to” these templated class function
 *    see https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function for further explanations 
 *  - classes and their member functions can be separated into declaration and definition as per below 
 *  - templated vector as used below, cannot use std::vector<T>::iterator to loop
 *    using regular for loop and obj pointer instead
 * 
*/


/**
 * ToDo:
 * 
 *  - addObj with 1st variadict being an object of class T to act like setObj
 * 
 */



/**
 * @brief typedef for comparison function
 *        int myCmpFunc(String A, String B);
 *        return value: <0 = A has lower value, 0 = same, >0 = A has higher value 
 */
typedef std::function<int(String, String)> sposCompareCB;



template <class T>
class spObjectStore {
   private:
    std::vector<String> _keys;
    std::vector<T> _objects;
    int32_t _index = -1;
    int32_t _capaInc = 10;
    sposCompareCB _compareCB;
    bool _sorted;
    bool _added = false;
    int32_t indexOfId(String &id);
    void setAdded(bool added);

   public:
    typedef std::function<bool(T*)> forEach_V_callback;
    typedef std::function<bool(String, T*)> forEach_KV_callback;

    spObjectStore();  // unsorted
    spObjectStore(bool sorted);  // if true, sorted by strcmp = A to Z
    spObjectStore(sposCompareCB callback);  // sorted by whatever you do with key / id values comparison
    template <class... Vs>
    T* addObj(String id, Vs... args); // variadic as optional args to call constructor of T
    template <class... Vs>
    T* getObj(String id, bool addIfNeeded = false, Vs... args); // variadic as optional args to call constructor of T
    void setObj(String id, T newObj);
    bool deleteObj(String id);
    void reset();
    void forEach(forEach_V_callback callback);
    void forEach(forEach_KV_callback callback);
    int32_t getCapaInc();
    void setCapaInc(int32_t newInc);
    bool getAdded();

};



// Function definitions must always be with leading "template <class T>"



/*    PUBLIC    PUBLIC    PUBLIC    PUBLIC    */

template <class T>
spObjectStore<T>::spObjectStore(){
  _sorted = false;
}

template <class T>
spObjectStore<T>::spObjectStore(bool sorted){
  _sorted = true;
}

template <class T>
spObjectStore<T>::spObjectStore(sposCompareCB callback){
  _sorted = true;
  _compareCB = callback;
}


/**
 * @brief Add an object with the given id and return a pointer to it.
 *        If an object with this id already exists, then this old one is returned
 *        or - in case args provided, a new object is stored under this id and returned
 * 
 * @param id  id of the object to 
 * @param args optional arguments to construct T
 * @return T*
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::addObj(String id, Vs ...args) {
  setAdded(indexOfId(id) == -1);
  if (_added){        
    _keys.insert(_keys.begin() + _index, id);
    _objects.emplace(_objects.begin() + _index, args...);
    return &_objects[_index];

  } else {
      if (sizeof...(Vs) > 0){
        _objects[_index] = T(args...);
      }
      return &_objects[_index];
  }
}


/**
 * @brief Get an object with the given id and return a pointer to it.
 *        If no object with this id exists, a nullptr is returned unless 
 *        addIfNeeded is set to true (pointer to a newly created object returned)
 * 
 * @param id 
 * @param addIfNeeded optional to create object in case it does not exist yet
 * @param args optional arguments to construct T
 * @return T* 
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::getObj(String id, bool addIfNeeded, Vs... args) {
  if (indexOfId(id) > -1){
    setAdded(false);
    return &_objects[_index];
  }
  if (addIfNeeded) {
    setAdded(true);
    _keys.insert(_keys.begin() + _index, id);
    _objects.emplace(_objects.begin() + _index, args...);
    return &_objects[_index];

  }
  setAdded(false);
  return nullptr;
}

/**
 * @brief Set an object with the given id, which is either replacing the old one
 *        or adding a new one
 *        This is similar to addObj() but using an object instead of args
 * 
 * @param id 
 * @param newObj the object of class T to store
 */
template <class T>
void spObjectStore<T>::setObj(String id, T newObj){
  setAdded(indexOfId(id) == -1);
  if (_added){
    _keys.insert(_keys.begin() + _index, id);
    _objects.emplace(_objects.begin() + _index, newObj);

  } else {
    _objects[_index] = newObj;
  }
}


/**
 * @brief Delete object with given id and returns success
 * 
 * @param id 
 * @return true 
 * @return false 
 */
template <class T>
bool spObjectStore<T>::deleteObj(String id) {
  if (indexOfId(id) == -1){
    return false;
  }
  _keys.erase(_keys.begin() + _index);
  _objects.erase(_objects.begin() + _index);
  return true;
}


/**
 * @brief Delete all elements
 * 
 */
template <class T>
void spObjectStore<T>::reset() {
  _keys.clear();
  _objects.clear();
}


/**
 * @brief loop through all entries and call function callback(*obj) = with value
 * 
 * @param callback 
 */
template <class T>
void spObjectStore<T>::forEach(forEach_V_callback callback) {
  size_t count = _objects.size();
  for (size_t i = 0; i < count; i++) {
    if (callback(&_objects.at(i)) == false){
      break;
    }
  }
}

/**
 * @brief loop through all entries and call function callback(id, *obj) = with key, value
 * 
 * @param callback 
 */
template <class T>
void spObjectStore<T>::forEach(forEach_KV_callback callback) {
  size_t count = _objects.size();
  for (size_t i = 0; i < count; i++) {
    if (callback(_keys[i] , &_objects.at(i)) == false){
      break;
    }
  }
}

/**
 * @brief returns the value set to increment the capacity (if needed)
 * 
 * @return int32_t value of increment
 */
template <class T>
int32_t spObjectStore<T>::getCapaInc(){
  return _capaInc;
}

/**
 * @brief set the value to increment the capacity (if needed)
 * 
 * @param newInc value of new increment
 */
template <class T>
void spObjectStore<T>::setCapaInc(int32_t newInc){
  if (newInc > 1){
    _capaInc = newInc;
  }
}



/**
 * @brief returns the status of last call to addObj, getObj and setObj with regard to a new 
 *        entry having been added 
 * 
 * @tparam T 
 * @return true 
 * @return false 
 */
template <class T>
bool spObjectStore<T>::getAdded(){
  return _added;
}



/*    PRIVATE    PRIVATE    PRIVATE    PRIVATE     */


// lower bound implementation, see https://en.cppreference.com/w/cpp/algorithm/lower_bound 


/**
 * @brief get the index (=_index) for an object's id
 *        return -1 if it does not exist (_index is then the position to insert new entry)
 * 
 * @param id 
 * @return int32_t index 
 */
template <class T>
int32_t spObjectStore<T>::indexOfId(String &id){
  size_t count = _keys.size();
  // we already worked on it?
  if ((_index > -1) && (_index < count) && (_keys[_index] == id)){
    return _index;
  }
  if (_sorted){
    // let's find it with lower bound implementation
    uint32_t step;
    uint32_t first = 0;
    uint32_t sIdx;
    int32_t cmpRes;

    
    while (count > 0){
      sIdx = first;
      step = count / 2;
      sIdx += step;
      if (_compareCB == nullptr){
        //A.compareTo(B); // <0 = A has lower value, 0 = same, >0 = A has higher value
        cmpRes = _keys[sIdx].compareTo(id);
        //cmpRes = strcmp(_keys[sIdx].c_str(), id.c_str());
      } else {
        cmpRes = _compareCB(_keys[sIdx], id);
      }
      
      if (cmpRes < 0){
          first = ++sIdx;
          count -= step + 1;
      } else if(cmpRes == 0) {
        _index = sIdx;
        return _index;
      } else {
        count = step;
      }
    }
    _index = first;
    return -1;

  } else {
    // let's find it (loop 0 to n)
    for (_index = 0; _index < count; _index++) {
      if (_keys[_index] == id) {
        return _index;
      }
    }
  }
  return -1;
}


template <class T>
void spObjectStore<T>::setAdded(bool added){
  // if already false, return
  if (!added && !_added){
    return;
  }
  // otherwise set and check capa
  _added = added;
  size_t count = _keys.size();
  if (added && (count + 2  > _keys.capacity())){
    _keys.reserve(count + 10);
    _objects.reserve(count + 10);
  }
}


#endif // SPOBJECTSTORE_H_