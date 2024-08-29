# spObjectStore Library

The spObjectStore class is a storage container to easily add, retrieve, delete or iterate through objects based on an identifier or values of these objects, whereby  
\- these objects being stored are of one specific class  
\- and such class being defined when creating the storage container object.

The storage container itself is designed as a templated class, meaning any class type of objects can be stored. Furthermore, the use of templated functions with variadic parameters provides complete freedom on the number and type of values used for the object classes to be stored. All you need to consider is  
\- the design and the functions of the class to be stored  
\- the identifier strings or the object values to be used to id and sort your objects.

Although the storage container can be created to store objects without sorting, it will be faster to retrieve objects when they are sorted. Sorting can be done by their id in ascending or descending order or with a custom callback function, which allows you to sort objects based on their member values in any way you want.

The following are explanations about all public functions and their usage. Additionally the library source code has doc strings explaining the functions' purpose, parameters and return value types.

One general point to remember is that the objects to store are in fact created and managed by the container class. Several functions will therefore return a pointer to these objects. Note that these pointers are valid at this point in time and may become invalid through further operation. Specifically after recreating the content with a new sorting method, any previously obtained pointers are no longer valid.

The library also contains several documented code examples, serving as a basis to learn the various functions. See the .cpp files in the /examples folder for demonstration.


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
* [Storage Size Used](#storage-size-used)
* [Sorting](#sorting)
* [Make Ids From Arguments](#make-ids-from-arguments)

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

1) create and add the object in one shot with an id only (and - if required - set other object properties later).  
In this case it is important that the class of objects to store has one constructor without parameters, i.e. myObject(){..}, as this constructor is called indirectly by calling the container's functions
    ```cpp
    myObject* pObj = myObjectStore.addObjWithId("newID");
    ```
    or 
    ```cpp
    myObject* pObj = myObjectStore.addObjFromArgs();
    ```
    Note that ```addObjFromArgs()``` function will automatically assign an id, either from an incremented number or from a 'create_id' callback function set via setCreateIdCallback().

2) create and add the object in one shot with an id and the object's properties  
In this case and when such alternative object constructors with parameters are defined, the objects can be created also by providing the arguments when calling the container's functions like
    ```cpp
    myObject* pObj = myObjectStore.addObjWithId("newID", "my text", 1234);
    ```
    or
    ```cpp
    myObject* pObj = myObjectStore.addObjFromArgs("my text", 1234);
    ```

3) create the object separately and then place it into the container
    ```cpp
    myObject obj("my text", 1234);
    myObject* pObj = myObjectStore.setObjWithId("new_or_old_ID", obj);
    ```
    The container's ```setObjWithId()``` function will replace the object of an existing id or otherwise create a new entry.  
    Note that in this case it is not 'obj', but a copy of 'obj', which will be added to the store and for which a pointer will be returned.


</br>For all three methods - ```addObjWithId()```, ```addObjFromArgs()``` and ```setObjWithId()``` - the status returned with
```cpp
bool added = myObjectStore.isAdded();
```
represents whether or not a new entry was added in the last function call.

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Retrieving Objects 

Objects will be retrieved from the store with
```cpp
myObject* pObj = myObjectStore.getObjById("findID");
```
or
```cpp
myObject* pObj = myObjectStore.getObjFromArgs("my text", 1234);
```
which will return a pointer to the found object or a nullptr if the object with the given id or values does not exist. 

Similarly, the id of an object can be obtained with
```cpp
std::string id = myObjectStore.getIdFromArgs("my text", 1234);
```
This will allow getting the id for an object after ids were recreated, i.e. by changing the 'create_id' callback function.


<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Deleting Objects 

Objects will be deleted from the store with
```cpp
bool success = myObjectStore.deleteObjById("deleteID");
```
or
```cpp
bool success = myObjectStore.deleteObjFromArgs("my text", 1234);
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
bool iterate_CB(const myObject &obj) { .. }  
```
or
```cpp
// calling with each identifier & object pair in the container
bool iterate_CB(const std::string &id, const myObject &obj) { .. }  
```
Note that the callback function must return a boolean, i.e. true to continue or false to stop looping.

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Storage Capacity
The spObjectStore class is optimized to expand its capacity in increments (default is 10) whenever an entry is added and the current capacity is used up. This avoids the reallocation of memory and copy / move operations of the stored objects each time a new object is added, i.e. it delays this costly process to every 10th addition of an object. However, using an increment > 1 reserves more memory space than actually needed at that point in time and could cause issues when setting high values.</br></br>The optimization can be adjusted in one or the other direction with 
```cpp
myObjectStore.setCapacityInc(num);
```
Use a higher number in order to optimize speed over memory (i.e. when having many or frequent object additions) or a lower number to save on memory.</br></br>The current increment value can be retrieved with
```cpp
size_t num = myObjectStore.getCapacityInc();
```

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Storage Size Used
The number of currently stored items can be obtained with 
```cpp
size_t num = myObjectStore.getSize();
```

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Sorting

There are three options for controlling the objects being sorted by their id or object values. This depends on the constructor used, i.e. either  
```spObjectStore<myObject> myObjectStore;```  (unsorted)  
```spObjectStore<myObject> myObjectStore(sorting);```  (sort by id with sorting being of enum sposSort: None, ASC or DESC)  
```spObjectStore<myObject> myObjectStore(callback);```  (with a 'compare_obj' callback to sort by whatever you want to do with object values comparison)  

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

In order to activate the sorting, the 2nd and 3rd constructor option of spObjectStore should be directly linked in the owner's class constructor, i.e.
```cpp
myClass::myClass() : myObjectStore(ASC) {
  .. 
}
``` 
or
```cpp
myClass::myClass() : myObjectStore(std::bind(&myClass::compareObjsCB, this, std::placeholders::_1, std::placeholders::_2)) { 
  ..
}
```
whereby - in the second example - the ```myClass::compareObjsCB()``` function is bound with ```std::bind()``` and two placeholders.  
</br>

The sorting by ids (i.e. sorting being None, ASC, DESC) can also be set at any time with
```cpp
myObjectStore.setSorting(sorting);
```
To check the current sorting use
```cpp
sposSort sorting = myObjectStore.getSorting();
```
and
```cpp
bool sorted = myObjectStore.isSorted();
```
</br>

In order to manage the sorting set either the 'create_id' callback function with
```cpp
myObjectStore.setCreateIdCallback(callback);
```
or the 'compare_obj' callback function with
```cpp
myObjectStore.setCompareCallback(callback);
```
Both cases will - when set with existing entries - recreate the stored content with new entries sorted by either newly created ids (ASC or DESC) or the new comparison function applied, depending on which callback function is set.  

Any 'create_id' callback function (typedef spos_create_id_callback, which is std::function<std::string(const myObject &obj)>) has to return a unique string used to identify and sort objects. For an easy way to create these ids, see below [Make Ids From Arguments](#make-ids-from-arguments).


Any 'compare_obj' callback function (typedef spos_compare_callback, which is std::function<int(const myObject &obj1, const myObject &obj2)>) has to return the result of comparing obj1 with obj2 as follows:
| return value | based on comparison result |
|:--------------:|:--------------------:|
| &lt;0 | obj1 has lower value than obj2 |
| 0 | obj1 equal obj2 |
| &gt;0 | obj1 has higher value than obj2 |

<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

### Make Ids From Arguments

To simplify the creation of ids, the makeIdFromArgs() function can be used like
```cpp
std::string id = myObjectStore.makeIdFromArgs(arg, ...);
```
whereby args can be any number or types of arguments. All arguments will be chained together by a separator ('#/#' by default) with numbers formatted with a specific number of digits (8 by default) and decimals (6 by default in case of floating point values). 

As an example, makeIdFromArgs("text", 1234); will return 'text#/#00001234'. 

To finetune the outcome and verify or change the settings, the following functions are useful:
```cpp
std::string separator = myObjectStore.getIdSeparator();
```
```cpp
myObjectStore.setIdSeparator(separator);
```
```cpp
uint8_t decimals = myObjectStore.getIdNumDecimals();
```
```cpp
myObjectStore.setIdNumDecimals(decimals);
```
```cpp
uint8_t digits = myObjectStore.getIdNumDigits();
```
```cpp
myObjectStore.setIdNumDigits(digits);
```


<div style="text-align: right"><a href="#content">&#8679; back up to content list</a></div>

</br>

## License
MIT license  
Copyright &copy; 2024 by krokoreit