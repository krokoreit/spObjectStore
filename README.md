# spObjectStore Library

This library provides a container object to store, retrieve or interate through objects based on an identifier, whereby  
\- these objects being stored are of one specific class  
\- and such class being defined when creating the storage container object

As the storage container is designed as a templated class, meaning any class type of objects can be stored, all you need to consider is  
\- the design and functions of the class to be stored  
\- the identifier strings used to id your objects

The storage container can be created to store objects without sorting, sorting them in A to Z order or sorting them by a custom callback function, which allows you to sort items in any way you want.

The .cpp files in the /examples folder demonstrate the various options to use the functions.


Enjoy

&emsp;krokoreit  
&emsp;&emsp;&emsp;<img src="assets/krokoreit-01.svg" width="140"/>


## Usage & API

### Content
* [Storage Container & Class of Objects to store](#storage-container--class-of-objects-to-store)
* [Adding Objects](#adding-objects)
* [Retrieving Objects](#retrieving-objects)
* [Deleting Objects](#deleting-objects)
* [Iterate](#iterate)
* [Storage Capacity](#storage-capacity)
* [Sorting](#sorting)

### Storage Container & Class of Objects to store
Use with any class type like

```cpp
// stores objects of 'myObject' class
spObjectStore<myObject> myObjectStore;
```

and

```cpp
class myObject 
{
  public:
    std::string _text = "";
    uint32_t _number = 0;
      . . .
    myObject();
    myObject(std::string text);
    myObject(std::string text, uint32_t number);
      . . .
};
```

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Adding Objects

There are three ways to add objects

1) create and add the object in one shot with an id only (and - if required - set other object properties later)  
In this case it is important that the class of objects to store has one constructor without parameters, i.e. myObject(){..}, as this constructor is called indirectly by calling the container's functions
    ```cpp
    myObject* pObj = myObjectStore.addObj("newID");
    ```
    or 
    ```cpp
    myObject* pObj = myObjectStore.getObj("newID", true);
    ```
    Note that ```getObj()``` function is used with the second parameter 'addIfNeeded' set to true, which will create and add the object before returning it.

2) create and add the object in one shot with an id and the object's properties  
In this case and such alternative object constructors with parameters defined, the objects can be created also by providing the arguments when calling the container's functions like
    ```cpp
    myObject* pObj = myObjectStore.addObj("newID", "my text", 1234);
    ```
    or
    ```cpp
    myObject* pObj = myObjectStore.getObj("newID", true, "my text", 1234);
    ```

3) create the object and then place it into the container
    ```cpp
    myObject obj("my text", 1234);
    myObjectStore.setObj("new_or_old_ID", obj);
    ```
    The container's ```setObj()``` function will replace the object of an existing id or otherwise create a new entry.


</br>For all three methods (```addObj()```, ```getObj()``` and ```setObj()```), the status returned with
```cpp
bool added = myObjectStore.getAdded();
```
represents wether a new entry was added in the last function call.

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Retrieving Objects 

Objects will be retrieved from the store with
```cpp
myObject* pObj = myObjectStore.getObj("findID");
```
which will return a pointer to the found object or a nullptr if the object with the given key does not exist. 

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Deleting Objects 

Objects will be deleted from the store with
```cpp
myObjectStore.deleteObj("deleteID");
```

To delete all objects, use

```cpp
myObjectStore.reset();
```

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Iterate 

To loop through the stored objects, use the container's ```forEach()``` method(s) with

```cpp
myObjectStore.forEach(iterate_CB);
```

whereby the callback function is either  
```cpp
// calling with each object in the container
bool iterate_CB(myObject obj) { .. }  
```
or

```cpp
// calling with each identifier & object pair in the container
bool iterate_CB(String id, myObject* pObj) { .. }  
```
Note that the callback function must return a boolean, i.e. true to continue or false to stop looping.

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Storage Capacity
The spObjectStore class is optimized to expand it's capacity in increments (default is 10) whenever an entry is added and the current capacity is used up. This avoids the reallocation of memory and copy / move operations of the stored objects each time a new object is added, i.e. it delays this costly process to every 10th addition of an object. However, using an increment > 1 reserves more memory space than actually needed at that point in time and could cause issues with values.</br></br>The optimization can be adjusted in one or the other direction with 
```cpp
myObjectStore.setCapaInc(num);
```
Use a higher number in order to optimize speed over memory (i.e. when having many or frequent object additions) or a lower number to save on memory.</br></br>The current increment value can be retrieved with
```cpp
int32_t num = myObjectStore.getCapaInc();
```

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Sorting

There are three options on controlling the objects being sorted by their id (or not). This depends on the constructor used, i.e. either  
```spObjectStore();```  (unsorted)  
```spObjectStore(bool sorted);```  (if true, sorted by strcmp = A to Z)  
```spObjectStore(sposCompareCB callback);```  (sorted by whatever you do with key / id values comparison)  

Note that in the case of spObjectStore being a member of another class, it is initialized in but cannot be declared in that class definition, e.g.
```cpp
class myClass{
  private:
    spObjectStore<myObject> myObjectStore;
    ...
  public:
    myClass()
    ...
}
```

In order to activate the sorting, the 2nd and 3rd contructor option of spObjectStore should be directly linked in the owner's class constructor, i.e.
```cpp
myClass::myClass() : myObjectStore(true) {
  .. 
}
``` 
or
```cpp
myClass::myClass() : myObjectStore(std::bind(&myClass::compareIdsCB, this, std::placeholders::_1, std::placeholders::_2)) { 
  ..
}
```
with - in the second example - the ```myClass::compareIdsCB()``` function bound with ```std::bind()``` and two placeholders.


Any callback function used has to match ```typedef sposCompareCB``` (which is std::function<int(String, String)>) and has to return as a result of comparing the strings A and B:
| return value | based on comparison result |
|:--------------:|:--------------------:|
| &lt;0 | A has lower value than B |
| 0 | A equal B |
| &gt;0 | A has higher value than B |

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

## License
MIT license  
Copyright &copy; 2024 by krokoreit
