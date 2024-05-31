# spObjectStore Library

## Intro

A container object to store, retrieve or interate through objects based on an identifier, whereby  
- these other objects are of one specific class
- and such class being defined when creating the storage container object

As the storage container is designed as a templated class, meaning any class type of objects can be stored, all you need to condider is
- the design and functions of the class to be stored
- the identifier strings used to id your objects

The storage container can be created to store objects unsorted, sorted in A to Z order or sorted by a custom callback function, with which you can sort in any way you want.

Enjoy

krokoreit <img>


## Why is it useful?

change 3 and more 


## More details

### 
Use with any class type like

```cpp
    spObjectStore<myObject> myObjectStore; // stores objects of myObject class
```

and

```cpp
    class myObject {
      String _text = "";
      uint32_t _number = 0;
        ...
      myObject();
      myObject(String text);
      myObject(String text, uint32_t number);
        ...
    };
```

It is important to have at least one class constructor without parameters, i.e. myObject(){..}, as 
this is needed when the object is created with 

```cpp
    myObjectobj = myObjectStore.addObj("newID");
```

or

```cpp
    myObjectobj = myObjectStore.getObj("newID", true);
```


In case there are alternative object constructors with parameters, then the objects can be created also by providing the arguments with

```cpp
    myObjectobj = myObjectStore.addObj("newID", "my text", 1234);
```

or

```cpp
    myObjectobj = myObjectStore.getObj("newID", true, "my text", 1234);
```

An alternative method to create or modify an new entry in the store is with

```cpp
    myObjectStore.setObj("new_or_old_ID", obj);
```

This will replace the object of an existing id or otherwise create a new entry


For all three methods (addObj, getObj and setObj), the status returned with

```cpp
    bool x = myObjectStore.getAdded();
```

represents wether a new entry was added in the last function call.



Delete an object from the store with

```cpp
    myObjectStore.deleteObj("newID");
```

Delete all objects with

```cpp
    myObjectStore.reset();
```

To loop through the stored objects, use the forEach method(s) with

```cpp
    myObjectStore.forEach(callback_function);
```

whereby callback_function is either
  a)  bool callback_function(myObjecto) { .. }              // = for (value in store)
  b)  bool callback_function(String id, myObjecto) { .. }   // = for (key, value in store)
Note that the callback_function must return a boolean, i.e. true to continue or false to stop looping

The spObjectStore class is optimized to expand it's capacity in increments (default is 10), when 
an entry is added and the current capacity is reached. This avoids the reallocation of memory and 
copy / move operations of the stored objects each time a new object is added. However, it reserves 
more memory space than actually occupied and could cause issues with large objects.
The optimization can be adjusted in one or the other direction with
```cpp
    myObjectStore.setCapaInc(num);
```

I.e. a higher number in order to optimized speed over memory (many or frequent object additions) or
a lower number to save on memory.
The current increment value can be retrieved with
```cpp
    int32_t num = myObjectStore.getCapaInc();
```

Sorting objects by their id:
  There are three options on controlling the objects being sorted by their id (or not). This depends
  on the constructor used, i.e. either
    spObjectStore();  (unsorted)
    spObjectStore(bool sorted);  (if true, sorted by strcmp = A to Z)
    spObjectStore(sposCompareCB callback);  (sorted by whatever you do with key / id values comparison)

  Note that in case that spObjectStore is a member of another class, then it is initialized but cannot be
  declared in that class definition, e.g.
    class myClass{
      private:
        spObjectStore<myObject> myObjectStore;
        ...
      public:
        myClass()
        ...
  In order to activate the sorting, the 2nd and 3rd contructor of spObjectStore should be directly linked
  in the class constructor, i.e.
    myClass::myClass() : myObjectStore(true) { .. } or
    myClass::myClass() : myObjectStore(std::bind(&myClass::compareIdsCB, this, std::placeholders::_1, std::placeholders::_2)) { .. }
  with the myClass::compareIdsCB function being of typedef sposCompareCB (= std::function<int(String, String)>)
