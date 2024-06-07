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


// the store
spObjectStore<myObject> myObjectStore;


/**
 * @brief callback function to print a stored object with key and object members
 * 
 * @param key 
 * @param pObj pointer to myObject object 
 * @return true (as we do not want to stop iterarion)
 */
bool printMyObject(std::string key, myObject* pObj)
{
  printf("Key: %s, text: %s, number: %i\n", key.c_str(), pObj->_text.c_str(), pObj->_number);
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
  myObjectStore.forEach(&printMyObject);
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
    myObjectStore.addObj("one", "one", 1);
    myObjectStore.addObj("two", "two", 2);
    myObjectStore.addObj("no number", "no numer given");

    // getObj() with implicit object creation
    myObject *pObj3 = myObjectStore.getObj("three", true, "three", 3);
    myObject *pObj4 = myObjectStore.getObj("four", true);
    pObj4->_text = "text assigned later";

    // setObj() with existing object
    myObject obj5("five", 5);
    myObjectStore.setObj("five", obj5);

    // print current state
    printObjectStore();

    myObjectStore.deleteObj("one");
    myObjectStore.deleteObj("two");
    myObjectStore.deleteObj("four");

    // print current state 2
    printf("after deleting objects:\n");
    printObjectStore();

    myObject *pObj6 = myObjectStore.getObj("five");

    myObject *pObj7 = myObjectStore.getObj("not_there");

    printf("done\n");
}
