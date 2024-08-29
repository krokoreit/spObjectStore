/**
 * example code for spObjectStore library
 * 
 * 
 */
#include <stdio.h>
#include <string>
#include <filesystem>

#include <spObjectStore.h>


/**
 * @brief class of objects we want to store
 * 
 */
class myObject 
{
  public:
    std::string _text = "";
    uint32_t _number = 0;
    myObject();
    myObject(std::string text);
    myObject(std::string text, uint32_t number);
};

/**
 * constructors
 */
myObject::myObject()
{
}

myObject::myObject(std::string text)
{
  _text = text;
}

myObject::myObject(std::string text, uint32_t number)
{
  _text = text;
  _number = number;
}


// the store - no sorting
spObjectStore<myObject> myObjectStore;


/**
 * @brief callback function to print a stored object with id and object members
 * 
 * @param id 
 * @param obj a myObject object 
 * @return true (as we do not want to stop iterarion)
 */
bool printStoredObject(const std::string &id, const myObject &obj)
{
  printf("id: %s, text: %s, number: %i\n", id.c_str(), obj._text.c_str(), obj._number);
  return true;
}

/**
 * @brief print content of store
 * 
 */
void printObjectStore()
{
  std::string sortedTxt = "";
  if (!myObjectStore.isSorted())
  {
    sortedTxt = "not ";
  }
  printf("object store content: %d objects, %ssorted:\n", myObjectStore.getSize(), sortedTxt.c_str());
  // loop output
  myObjectStore.forEach(&printStoredObject);
}

/**
 * @brief print object at pointer supplied
 * 
 * @param pObj  pointer to myObject
 */
void printObjAtPointer(const std::string &id, myObject *pObj)
{
  if (pObj == nullptr)
  {
    printf("printObjAtPointer: object with id %s does not exist\n", id.c_str());
  }
  else
  {
    printf("printObjAtPointer: object with id: %s, text: %s, number: %i\n", id.c_str(), pObj->_text.c_str(), pObj->_number);
  }
}

/**
 * @brief our main function
 * 
 */
int main(int argc, char *argv[])
{
    std::string a = argv[0];
    printf("running %s\n", a.substr(a.rfind(std::filesystem::path::preferred_separator) + 1).c_str());

    // addObj() with implicit object creation
    myObjectStore.addObjWithId("one", "one", 1);
    myObjectStore.addObjWithId("two", "two", 2);
    myObjectStore.addObjWithId("no number", "no numer given");

    // setObj() after separate object creation
    myObject obj3("three", 3);
    myObjectStore.setObjWithId("three", obj3);
    myObject obj4("four");
    myObject *pObj4 = myObjectStore.setObjWithId("four", obj4);

    // note that obj4 is copied, i.e. not the object in store!
    obj4._text = "text assigned later, but not on object in store";
    // changing pObj4 is possible until recreation of store, i.e. changed sorting
    pObj4->_text = "text assigned later on object in store";

    // print current state
    printObjectStore();

    myObjectStore.deleteObjById("one");
    myObjectStore.deleteObjById("two");

    // print current state 2
    printf("after deleting objects:\n");
    printObjectStore();

    std::string id = "four";
    myObject *pObj6 = myObjectStore.getObjById(id);
    printObjAtPointer(id, pObj6);
    id = "not_there";
    myObject *pObj7 = myObjectStore.getObjById(id);
    printObjAtPointer(id, pObj7);

    printf("done\n");
}
