/**
 * @file spObjectStore.h
 * @author krokoreit (krokoreit@gmail.com)
 * @brief a templated class to store, retrieve, delete and iterate through objects based on an id
 * @version 2.0.0
 * @date 2024-06-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/**
 * Version history:
 * v1 original develoment for use in platformio / arduino on an ESP32 project
 * v2 changed to standard C++ 
 *  
 */


#ifndef SPOBJECTSTORE_H_
#define SPOBJECTSTORE_H_


#include <stdint.h>
#include <string>
#include <string.h>
#include <functional>
#include <vector>



/**
 *  Notes:
 *  - we cannot have the class and function definitions in a separate .cpp file
 *    because this will cause “undefined reference to” these templated class function
 *    see https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function 
 *    for further explanations 
 *    classes and their member functions can be separated into declaration and definition as per below 
 *  - templated vector as used below, cannot use std::vector<T>::iterator to loop, thus
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
 *        int myCmpFunc(std::string A, std::string B);
 *        return value: <0 = A has lower value, 0 = same, >0 = A has higher value 
 */
typedef std::function<int(std::string, std::string)> sposCompareCB;



/**
 * @brief the object storage class
 * @tparam T  class typename of objects to store
 */
template <class T>
class spObjectStore
{
   private:
    std::vector<std::string> _keys;
    std::vector<T> _objects;
    int32_t _index = -1;
    int32_t _capaInc = 10;
    sposCompareCB _compareCB;
    bool _sorted;
    bool _added = false;
    int32_t indexOfId(std::string &id);
    void setAdded(bool added);

   public:
    typedef std::function<bool(T*)> forEach_V_callback;
    typedef std::function<bool(std::string, T*)> forEach_KV_callback;

    spObjectStore();  // unsorted
    spObjectStore(bool sorted);  // if true, sorted by strcmp = A to Z
    spObjectStore(sposCompareCB callback);  // sorted by whatever you do with key / id values comparison
    template <class... Vs>
    T* addObj(std::string id, Vs... args); // variadic as optional args to call constructor of T
    template <class... Vs>
    T* getObj(std::string id, bool addIfNeeded = false, Vs... args); // variadic as optional args to call constructor of T
    void setObj(std::string id, T newObj);
    bool deleteObj(std::string id);
    void reset();
    void forEach(forEach_V_callback callback);
    void forEach(forEach_KV_callback callback);
    int32_t getCapaInc();
    void setCapaInc(int32_t newInc);
    size_t getSize();
    bool isAdded();
    bool isSorted();
};



/*    PUBLIC    PUBLIC    PUBLIC    PUBLIC    

      xxxxxxx   xx    xx  xxxxxxx   xx           xx      xxxxxx 
      xx    xx  xx    xx  xx    xx  xx           xx     xx    xx
      xx    xx  xx    xx  xx    xx  xx           xx     xx      
      xxxxxxx   xx    xx  xxxxxxx   xx           xx     xx      
      xx        xx    xx  xx    xx  xx           xx     xx      
      xx        xx    xx  xx    xx  xx           xx     xx    xx
      xx         xxxxxx   xxxxxxx   xxxxxxxx     xx      xxxxxx 
     

      PUBLIC    PUBLIC    PUBLIC    PUBLIC    */


/**
 * constructors
 */
template <class T>
spObjectStore<T>::spObjectStore()
{
  _sorted = false;
}

template <class T>
spObjectStore<T>::spObjectStore(bool sorted)
{
  _sorted = true;
}

template <class T>
spObjectStore<T>::spObjectStore(sposCompareCB callback)
{
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
 * @return T* pointer to object stored
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::addObj(std::string id, Vs ...args)
{
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
 * @return T* pointer to object stored 
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::getObj(std::string id, bool addIfNeeded, Vs... args)
{
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
void spObjectStore<T>::setObj(std::string id, T newObj)
{
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
 * @return true / false 
 */
template <class T>
bool spObjectStore<T>::deleteObj(std::string id)
{
  if (indexOfId(id) == -1){
    return false;
  }
  _keys.erase(_keys.begin() + _index);
  _objects.erase(_objects.begin() + _index);
  return true;
}

/**
 * @brief Delete all objects
 * 
 */
template <class T>
void spObjectStore<T>::reset()
{
  _keys.clear();
  _objects.clear();
}

/**
 * @brief loop through all entries and call function callback(*obj) = with value
 * 
 * @param callback 
 */
template <class T>
void spObjectStore<T>::forEach(forEach_V_callback callback)
{
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
void spObjectStore<T>::forEach(forEach_KV_callback callback)
{
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
int32_t spObjectStore<T>::getCapaInc()
{
  return _capaInc;
}

/**
 * @brief set the value to increment the capacity (if needed)
 * 
 * @param newInc value of new increment
 */
template <class T>
void spObjectStore<T>::setCapaInc(int32_t newInc)
{
  if (newInc > 1){
    _capaInc = newInc;
  }
}

/**
 * @brief returns the number of objects in the store
 * 
 * @return size_t number
 */
template <class T>
size_t spObjectStore<T>::getSize()
{
  return _keys.size();
}

/**
 * @brief returns the status of last call to addObj, getObj and setObj with regard to a new 
 *        entry having been added 
 * 
 * @tparam T 
 * @return true / false 
 */
template <class T>
bool spObjectStore<T>::isAdded()
{
  return _added;
}

/**
 * @brief returns whether or not the store is sorted 
 * 
 * @tparam T 
 * @return true / false 
 */
template <class T>
bool spObjectStore<T>::isSorted()
{
  return _sorted;
}




/*    PRIVATE    PRIVATE    PRIVATE    PRIVATE

      xxxxxxx   xxxxxxx      xx     xx    xx     xx     xxxxxxxx  xxxxxxxx
      xx    xx  xx    xx     xx     xx    xx    xxxx       xx     xx      
      xx    xx  xx    xx     xx     xx    xx   xx  xx      xx     xx      
      xxxxxxx   xxxxxxx      xx      xx  xx   xx    xx     xx     xxxxxxx    
      xx        xx    xx     xx      xx  xx   xxxxxxxx     xx     xx    
      xx        xx    xx     xx       xxxx    xx    xx     xx     xx      
      xx        xx    xx     xx        xx     xx    xx     xx     xxxxxxxx
     

      PRIVATE    PRIVATE    PRIVATE    PRIVATE    */




/**
 * @brief get the index (=_index) for an object's id
 *        return -1 if it does not exist (_index is then the position to insert new entry)
 * 
 * @param id 
 * @return int32_t index 
 */
template <class T>
int32_t spObjectStore<T>::indexOfId(std::string &id)
{
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
        //cmpRes of <0 = A has lower value, 0 = same, >0 = A has higher value
        cmpRes = strcmp(_keys[sIdx].c_str(), id.c_str());
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

/**
 * @brief set _added status and if - if needed - expand vectors' capacity
 * 
 * @tparam T 
 * @param added 
 */
template <class T>
void spObjectStore<T>::setAdded(bool added)
{
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