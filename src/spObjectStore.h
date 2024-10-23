/**
 * @file spObjectStore.h
 * @author krokoreit (krokoreit@gmail.com)
 * @brief a templated container class to add, retrieve, delete and iterate through objects
 * @version 2.1.1
 * @date 2024-10-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/**
 * Version history:
 * v1       original develoment for use in platformio / arduino on an ESP32 project
 * v2.0.0   changed to standard C++ 
 * v2.1.0   various additions and changes
 *          - added id creation functionality
 *          - added comparison callback
 *          - changed sorting to enum and amended constructor
 *          - remodelled callback typedefs
 * v2.1.1   eliminated printf() used
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
 *  - for a templated vector as used below, we cannot use std::vector<T>::iterator to loop, thus
 *    using a regular for loop and obj pointer instead
 * 
*/



/**
 * @brief enum for type of sorting to be used
 */
enum sposSort
{
  None,
  ASC,
  DESC
};


/**
 * @brief the object storage class
 * @tparam T  class typename of objects to store
 */
template <class T>
class spObjectStore
{
   public:
    /*  typedef for comparison function
        int myCmpFunc(const T &obj_A, const T &obj_B);
        return value: <0 = A has lower value, 0 = same, >0 = A has higher value   */
    typedef std::function<int(const T&, const T&)> spos_compare_callback;
    /*  typedef for id creating function
        std::string myCreateIdFunc(const T &obj);  */
    typedef std::function<std::string(const T&)> spos_create_id_callback;
    /*  typedef for interation function, object only
        std::string myIterateFunc(const T &obj);  */
    typedef std::function<bool(const T&)> spos_forEach_O_callback;
    /*  typedef for interation function, id and object
        std::string myIterateFunc(const std::string &id, const T &obj);  */
    typedef std::function<bool(const std::string&, const T&)> spos_forEach_IO_callback;

   private:
    std::vector<std::string> _ids;
    std::vector<T> _objects;
    int32_t _index = -1;
    size_t _capaInc = 10;
    spos_compare_callback _compareCB;
    bool _added = false;
    sposSort _sorting = None;
    std::string _idSep = "#/#";
    uint32_t _autoId = 10000;
    uint8_t _idNumDigits = 8;
    uint8_t _idNumDecimals = 6;
    uint8_t _idNumSize = 16;
    spos_create_id_callback _createIdCB;

    int32_t compareIds(const std::string &id1, const std::string &id2);
    int32_t indexOf(const std::string &id, T *obj);
    void setCapacity(size_t capacity);
    void setAdded(bool added);
    std::string stringify(const uint64_t &value);
    std::string stringify(const uint32_t &value);
    std::string stringify(const uint16_t &value);
    std::string stringify(const uint8_t &value);
    std::string stringify(const int64_t &value);
    std::string stringify(const int32_t &value);
    std::string stringify(const int16_t &value);
    std::string stringify(const int8_t &value);
    std::string stringify(const long double &value);
    std::string stringify(const double &value);
    std::string stringify(const float &value);
    std::string stringify(const std::string &value);
    std::string stringify(const char &value);
    std::string makeIdFrom();
    template<class U>
    std::string makeIdFrom(U arg);
    template<class U, class... Vs>
    std::string makeIdFrom(U arg, Vs... args);
    std::string createId(const T &obj);
    void recreate(bool preserveIds);

   public:
    spObjectStore();
    spObjectStore(sposSort sorting);
    spObjectStore(spos_compare_callback callback);
    template <class... Vs>
    T* addObjWithId(const std::string &id, Vs... args);
    template <class... Vs>
    T* addObjFromArgs(Vs... args);
    T* setObjWithId(const std::string &id, T &newObj);
    T* getObjById(const std::string &id);
    template <class... Vs>
    T* getObjFromArgs(Vs... args);
    template <class... Vs>
    std::string getIdFromArgs(Vs... args);
    bool deleteObjById(const std::string &id);
    template <class... Vs>
    bool deleteObjFromArgs(Vs... args);
    void reset();
    void forEach(spos_forEach_O_callback callback);
    void forEach(spos_forEach_IO_callback callback);
    size_t getCapacityInc();
    void setCapacityInc(size_t newInc);
    size_t getSize();
    bool isAdded();
    bool isSorted();
    sposSort getSorting();
    void setSorting(sposSort sorting);
    std::string getIdSeparator();
    void setIdSeparator(std::string separator);
    uint8_t getIdNumDecimals();
    void setIdNumDecimals(uint8_t decimals);
    uint8_t getIdNumDigits();
    void setIdNumDigits(uint8_t digits);
    template <class... Vs>
    std::string makeIdFromArgs(Vs... args);
    void setCreateIdCallback(spos_create_id_callback callback);
    void setCompareCallback(spos_compare_callback callback);
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
 * constructor - plain vanilla = no sorting
 */
template <class T>
spObjectStore<T>::spObjectStore()
{
  _sorting = None;
}

/**
 * constructor - sorting as None, ASC or DESC
 */
template <class T>
spObjectStore<T>::spObjectStore(sposSort sorting)
{
  _sorting = sorting;
}

/**
 * constructor - sorting by comparison callback
 */
template <class T>
spObjectStore<T>::spObjectStore(spos_compare_callback callback)
{
  _sorting = None;
  _compareCB = callback;
}

/**
 * @brief Create an object, add it with the given id and return a pointer to it.
 *        If an object with this id already exists, then a new object is 
 *        stored under this id.
 * 
 * @param id  id of the object to store
 * @param args optional arguments to construct T
 * @return T* pointer to object stored
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::addObjWithId(const std::string &id, Vs... args)
{
  if (indexOf(id, nullptr) == -1){
    setAdded(true);
    _ids.insert(_ids.begin() + _index, id);
    _objects.emplace(_objects.begin() + _index, args...);
  } else {
    setAdded(false);
    _objects[_index] = T(args...);
  }
  return &_objects[_index];
}

/**
 * @brief Create an object, add it with an autocreated id and return a pointer to it.
 * 
 * @param args optional arguments to construct T
 * @return T* pointer to object stored
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::addObjFromArgs(Vs ...args)
{
  T newObj = T(args...);
  std::string id = createId(newObj);
  // must be index of -1
  if (indexOf(id, &newObj) == -1){
    setAdded(true);
    _ids.insert(_ids.begin() + _index, id);
    _objects.insert(_objects.begin() + _index, newObj);
    return &_objects[_index];
  }
  return nullptr;
}

/**
 * @brief Set a copy(!) of an object with the given id, which is either replacing 
 *        an existing one or adding a new id - object pair. This is similar to 
 *        addObjWithId() but using an object instead of args to create the stored object
 * 
 * @param id  id of the object to store
 * @param newObj an object of class T, based on which a copy is created and stored
 */
template <class T>
T* spObjectStore<T>::setObjWithId(const std::string &id, T &newObj)
{
  if (indexOf(id, &newObj) == -1){
    setAdded(true);
    _ids.insert(_ids.begin() + _index, id);
    _objects.insert(_objects.begin() + _index, newObj);

  } else {
    setAdded(false);
    _objects[_index] = newObj;
  }
  return &_objects[_index];
}

/**
 * @brief Get an object with the given id and return a pointer to it.
 *        If no object with this id exists, a nullptr is returned
 * 
 * @param id  id of the object to find
 * @return T* pointer to object stored 
 */
template<class T>
T* spObjectStore<T>::getObjById(const std::string &id)
{
  if (indexOf(id, nullptr) > -1){
    return &_objects[_index];
  }
  return nullptr;
}

/**
 * @brief Get an object with the given object arguments and return a pointer to it.
 *        If no object with these arguments exists, a nullptr is returned
 * 
 * @param args arguments used when object was constructed 
 * @return T* pointer to object stored 
 */
template<class T> template<class... Vs>
T* spObjectStore<T>::getObjFromArgs(Vs... args)
{
  T obj = T(args...);
  if (indexOf("", &obj) > -1){
    return &_objects[_index];
  }
  return nullptr;
}

/**
 * @brief Get the id of the object with the given arguments.
 *        If no object with these arguments exists, an empty string is returned
 * 
 * @param args arguments used when object was constructed 
 * @return std::string  the id of the object stored
 */
template<class T> template<class... Vs>
std::string spObjectStore<T>::getIdFromArgs(Vs... args)
{
  T obj = T(args...);
  if (indexOf("", &obj) > -1){
    return _ids[_index];
  }
  return "";
}

/**
 * @brief Delete the object with the given id and return success
 * 
 * @param id  id of the object to delete
 * @return true / false 
 */
template<class T>
bool spObjectStore<T>::deleteObjById(const std::string &id)
{
  if (indexOf(id, nullptr) == -1){
    return false;
  }
  _ids.erase(_ids.begin() + _index);
  _objects.erase(_objects.begin() + _index);
  return true;
}

/**
 * @brief Delete the object with the given object arguments and return success
 * 
 * @param args arguments used when object was constructed 
 * @return true / false 
 */
template<class T> template<class... Vs>
bool spObjectStore<T>::deleteObjFromArgs(Vs ...args)
{
  T obj = T(args...);
  if (indexOf("", &obj) == -1){
    return false;
  }
  _ids.erase(_ids.begin() + _index);
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
  _ids.clear();
  _objects.clear();
}

/**
 * @brief Loop through all entries and call function callback(obj)
 * 
 * @param callback  function of type func(const class &obj)
 */
template <class T>
void spObjectStore<T>::forEach(spos_forEach_O_callback callback)
{
  size_t count = _objects.size();
  for (size_t i = 0; i < count; i++) {
    if (callback(_objects.at(i)) == false){
      break;
    }
  }
}

/**
 * @brief Loop through all entries and call function callback(id, obj)
 * 
 * @param callback  function of type func(const std::string &id, const class &obj)
 */
template <class T>
void spObjectStore<T>::forEach(spos_forEach_IO_callback callback)
{
  size_t count = _objects.size();
  for (size_t i = 0; i < count; i++) {
    if (callback(_ids[i] , _objects.at(i)) == false){
      break;
    }
  }
}

/**
 * @brief Returns the value by which the capacity is incremented when needed
 * 
 * @return int32_t value of increment
 */
template <class T>
size_t spObjectStore<T>::getCapacityInc()
{
  return _capaInc;
}

/**
 * @brief Set the value used to increment the capacity when needed
 * 
 * @param newInc value of new increment
 */
template <class T>
void spObjectStore<T>::setCapacityInc(size_t newInc)
{
  if (newInc > 1){
    _capaInc = newInc;
  }
}

/**
 * @brief Returns the number of objects in the store
 * 
 * @return size_t number
 */
template <class T>
size_t spObjectStore<T>::getSize()
{
  return _ids.size();
}

/**
 * @brief Returns the status of last call to addObjWithId(), addObjFromArgs() and 
 *        setObjWithId() with regard to a new entry having been added 
 * 
 * @return true / false 
 */
template <class T>
bool spObjectStore<T>::isAdded()
{
  return _added;
}

/**
 * @brief Returns whether or not the store is sorted 
 * 
 * @return true / false 
 */
template <class T>
bool spObjectStore<T>::isSorted()
{
  return ((_compareCB != nullptr) || (_sorting != None));
}

/**
 * @brief Returns the current sorting method
 * 
 * @tparam T 
 * @return sposSort 
 */
template <class T>
sposSort spObjectStore<T>::getSorting()
{
  return _sorting;
}

/**
 * @brief Set the sorting
 * 
 * @param sorting  either None, ASC or DESC
 */
template <class T>
void spObjectStore<T>::setSorting(sposSort sorting)
{
  if (sorting == _sorting)
  {
    return;
  }
  _sorting = sorting;

  if (sorting != None)
  {
    // recreate with preserved ids
    recreate(true);
  }
}

/**
 * @brief Returns the string used as separator when creating ids from arguments. 
 *        The default is '#/#', which is producing ids of key1#/#key2.. pattern
 * 
 * @return std::string  the separator
 */
template <class T>
std::string spObjectStore<T>::getIdSeparator()
{
  return _idSep;
}

/**
 * @brief Set the string to be used as separator when creating ids from arguments. 
 *        The default is '#/#', which is producing ids of key1#/#key2.. pattern
 * 
 * @param separator 
 */
template <class T>
void spObjectStore<T>::setIdSeparator(std::string separator)
{
  if (separator.length() > 0)
  {
    _idSep = separator;
  }
}

/**
 * @brief Returns the number of decimals used to format an id from a floating point value.
 *        
 * @return uint8_t  the length of string for decimals portion of value
 */
template <class T>
uint8_t spObjectStore<T>::getIdNumDecimals()
{
  return _idNumDecimals;
}

/**
 * @brief Set the number of decimals used to format an id from a floating point value.
 * 
 * @param digits  the length of the string for the decimals portion of a value
 */
template <class T>
void spObjectStore<T>::setIdNumDecimals(uint8_t decimals)
{
  if (decimals > 0)
  {
    _idNumDecimals = decimals;
  }
}

/**
 * @brief Returns the number of digits used to format an id from a numeric value.
 *        
 * @return uint8_t  the length of the string (integer portion for floating point value)
 */
template <class T>
uint8_t spObjectStore<T>::getIdNumDigits()
{
  return _idNumDigits;
}

/**
 * @brief Set the number of digits used to format an id from a numeric value.
 * 
 * @param digits  the length of the string (integer portion for floating point value)
 */
template <class T>
void spObjectStore<T>::setIdNumDigits(uint8_t digits)
{
  if (digits > 0)
  {
    _idNumDigits = digits;
  }
}

/**
 * @brief Returns an id created from arguments. In case no argument is given,
 *        an auto generated one will be returned
 * 
 * @param args  any arguments privide will be concatenated into an id
 * @return std::string  the id created
 */
template <class T> template <class... Vs>
std::string spObjectStore<T>::makeIdFromArgs(Vs... args)
{
  int8_t numArgs = sizeof...(args);
  if (numArgs > 0)
  {
    return makeIdFrom(args...);
  }
  else
  {
    return makeIdFrom(_autoId++);
  }
}

/**
 * @brief Set the callback to be invoked for creating ids and then rebuild 
 *        all stored ids. If needed, sort all existing entries
 * 
 * @param callback  function of type func(const class &obj)
 */
template <class T>
void spObjectStore<T>::setCreateIdCallback(spos_create_id_callback callback)
{
  if (&_createIdCB == &callback)
  {
    return;
  }
  _createIdCB = callback;

  // recreate with new ids
  recreate(false);
}

/**
 * @brief Set the callback to be invoked when sorting objects (i.e. comparing two objects)
 *        and sort all existing entries
 * 
 * @param callback  function of type func(const class &obj1, const class &obj2)
 */
template <class T>
void spObjectStore<T>::setCompareCallback(spos_compare_callback callback)
{
  if (&_compareCB == &callback)
  {
    return;
  }
  _compareCB = callback;

  // recreate with preserved ids
  recreate(true);
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
 * @brief Return the result of comparing of two ids in dependence of None, ASC or DESC 
 * 
 * @param id1 
 * @param id2 
 * @return int32_t 
 */
template <class T>
int32_t spObjectStore<T>::compareIds(const std::string &id1, const std::string &id2)
{
  int32_t cmpRes = strcmp(id1.c_str(), id2.c_str());
  if (_sorting == DESC)
  {
    cmpRes *= -1;
  }
  return cmpRes;
}

/**
 * @brief Get the index (=_index) for an object's id and/or object.
 *        Returns -1 if it does not exist (_index is then the position to insert new entry)
 * 
 * @param id 
 * @return int32_t index 
 */
template <class T>
int32_t spObjectStore<T>::indexOf(const std::string &id, T *obj)
{
  size_t count = _ids.size();
  // we already worked on it?
  if ((_index > -1) && (_index < count) && (_ids[_index] == id)){
    return _index;
  }
  if (isSorted()){
    // let's find it with lower bound implementation
    uint32_t step;
    uint32_t first = 0;
    uint32_t sIdx;
    int32_t cmpRes;
    
    while (count > 0){
      sIdx = first;
      step = count / 2;
      sIdx += step;
      //cmpRes of <0 = A has lower value, 0 = same, >0 = A has higher value
      if ((_compareCB == nullptr) || (obj == nullptr)){
        cmpRes = compareIds(_ids[sIdx].c_str(), id.c_str());
      } else {
        cmpRes = _compareCB(_objects[sIdx], *obj);
        // when looking at same obj values, i.e. xmpRes = 0, we need to check ids, except id == ""
        if ((cmpRes == 0) && (id.length() > 0))
        {
          cmpRes = compareIds(_ids[sIdx].c_str(), id.c_str());
        }
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
    // not sorted, let's find it by looping from 0 to n
    for (_index = 0; _index < count; _index++) {
      if (_ids[_index] == id) {
        return _index;
      }
    }
  }
  return -1;
}

/**
 * @brief Set capacity to new increased value
 * 
 * @param capacity  new size
 */
template <class T>
void spObjectStore<T>::setCapacity(size_t capacity)
{
  if (capacity > _ids.capacity())
  {
    _ids.reserve(capacity);
    _objects.reserve(capacity);
  }  
}


/**
 * @brief Set _added status and, if needed, expand the vectors' capacity
 * 
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
  size_t count = _ids.size();
  if (added && (count + 2  > _ids.capacity())){
    setCapacity(count + _capaInc);
  }
}

/**
 * @brief Returns the formatted string representation of value.
 *        Overloaded function for all types and formatted with default sizes
 * 
 * @param value 
 * @return std::string 
 */
template <class T>
std::string spObjectStore<T>::stringify(const uint64_t &value)
{
  char buf[100];
  snprintf(buf, 100, "%0*llu", 8, value);
  return std::string(buf);
}
template <class T>
std::string spObjectStore<T>::stringify(const uint32_t &value)
{
  return stringify(static_cast<uint64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const uint16_t &value)
{
  return stringify(static_cast<uint64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const uint8_t &value)
{
  return stringify(static_cast<uint64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const int64_t &value)
{
  char buf[100];
  snprintf(buf, 100, "%0*lld", _idNumDigits, value);
  return std::string(buf);
}
template <class T>
std::string spObjectStore<T>::stringify(const int32_t &value)
{
  return stringify(static_cast<int64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const int16_t &value)
{
  return stringify(static_cast<int64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const int8_t &value)
{
  return stringify(static_cast<int64_t>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const long double &value)
{
  char buf[100];
  snprintf(buf, 100, "%+0*.*LF", _idNumSize, _idNumDecimals, value);
  return std::string(buf);
}
template <class T>
std::string spObjectStore<T>::stringify(const double &value)
{
  return stringify(static_cast<long double>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const float &value)
{
  return stringify(static_cast<long double>(value));
}
template <class T>
std::string spObjectStore<T>::stringify(const std::string &value)
{
  return std::string(value);
}
template <class T>
std::string spObjectStore<T>::stringify(const char &value)
{
  return std::string(1, value);
}

/**
 * @brief Make an id from variadic arguments, joined together with separator.
 *        Overloaded function for no, one and many arguments
 * 
 * @return std::string 
 */
// do nothing func
template <class T>
std::string spObjectStore<T>::makeIdFrom()
{
  return "";
}
// one arg
template <class T> template<class U>
std::string spObjectStore<T>::makeIdFrom(U arg)
{
  //return spos_stringify(arg);
  return stringify(arg);
}
// many args
template<class T> template<class U, class... Vs>
std::string spObjectStore<T>::makeIdFrom(U arg, Vs... args)
{
  //std::string res = spos_stringify(arg);
  std::string res = stringify(arg);
  res.append(_idSep);
  return res.append(makeIdFrom(args...));
}

/**
 * @brief  Returns an id created either by _createIdCB() or _autoId 
 * 
 * @param obj 
 * @return std::string 
 */
template <class T>
std::string spObjectStore<T>::createId(const T &obj)
{
  if (_createIdCB != nullptr)
  {
    return _createIdCB(obj);
  }
  else
  {
    return makeIdFrom(_autoId++);
  }
}


/**
 * @brief Recreate the vectors (if there are entries) based on sorting choosen,
 *        either with preserved ids or newly created ones 
 * 
 * @param preserveIds  true to keep the ids
 */
template <class T>
void spObjectStore<T>::recreate(bool preserveIds)
{
  size_t count = _ids.size();
  if (count > 0)
  {
    std::vector<std::string> old_ids(_ids);
    if (preserveIds)
    {
      for (size_t i = 0; i < count; i++)
      {
        old_ids.push_back(_ids[i]);
      }
    }
    std::vector<T> old_objects(_objects);
    reset();
    setCapacity(count + _capaInc);
    for (size_t i = 0; i < count; i++)
    {
      if (preserveIds)
      {
        setObjWithId(old_ids[i], old_objects[i]);
      }
      else
      {
        setObjWithId(createId(old_objects[i]), old_objects[i]); 
      }
    }
  }
}

#endif // SPOBJECTSTORE_H_