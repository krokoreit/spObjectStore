/**
 * example code for spObjectStore library
 * 
 */

// for Windows console codepage issues, comment out if not needed
#include <Windows.h>

#include <spObjectStore.h>

class city
{
  public:
    std::string _key = "";
    std::string _name = "";
    std::string _region = "";
    std::string _location = "";
    uint32_t _inhabitants = 0;
    city(std::string key, std::string name, std::string region,
         std::string location, uint32_t inhabitants);
};

city::city(std::string key, std::string name, std::string region,
           std::string location, uint32_t inhabitants)
{
  _key = key;
  _name = name;
  _region = region;
  _location = location;
  _inhabitants = inhabitants;
}

// the store
spObjectStore<city> cities;

/**
 * @brief forEach callback function to print a stored object with id and object members
 * 
 * @param id 
 * @param obj a city object 
 * @return true (as we do not want to stop iterarion)
 */
bool printCity(const std::string &id, const city &obj)
{
  printf("id: %s, key: %s, name: %s, region: %s, location: %s, inhabitants: %i\n", id.c_str(),
         obj._key.c_str(), obj._name.c_str(), obj._region.c_str(), obj._location.c_str(), obj._inhabitants);
  return true;
}


/**
 * create id callback
 */
std::string createId_1(const city &obj)
{
  return cities.makeIdFromArgs(obj._name);
}
std::string createId_2(const city &obj)
{
  return cities.makeIdFromArgs(obj._key);
}
std::string createId_3(const city &obj)
{
  return cities.makeIdFromArgs(obj._region, obj._name);
}
std::string createId_4(const city &obj)
{
  return cities.makeIdFromArgs(obj._inhabitants, obj._key);
}

/**
 * compare callback
 */
int32_t compareObj_1(const city &obj1, const city &obj2)
{
  int32_t cmpRes = strcmp(obj1._region.c_str(), obj2._region.c_str());
  if (cmpRes == 0)
  {
    cmpRes = obj2._inhabitants - obj1._inhabitants;
  }
  return cmpRes;
}

/**
 * @brief our main function
 * 
 */
int main(int argc, char *argv[])
{
  int c;
  std::string outText;
  std::string prompt("Press option '1' to '5' or 'q' to quit");

  // for Windows console codepage issues, comment out if not needed
  SetConsoleOutputCP(CP_UTF8);

  // set sorting
  cities.setSorting(ASC);

  // add cities without id
  cities.addObjFromArgs("IEV", "Kyiv", "Europe", "50°27'00\"N 30 31'24\"E", 2952301); 
  cities.addObjFromArgs("PAR", "Paris", "Europe", "48°51'24\"N 2°21'8\"E", 2102650);
  cities.addObjFromArgs("TYO", "Tokyo", "Asia", "35°41'23\"N 139°41'32\"E", 14094034);
  cities.addObjFromArgs("LAX", "Los Angeles", "America", "34°03'N 118°15'W", 3898747);
  cities.addObjFromArgs("LON", "London", "Europe", "51°30'26\"N 0°7'39\"W", 8799800);
  cities.addObjFromArgs("BKK", "Bangkok", "Asia", "13°45'09\"N 100°29'39\"E", 8305218);
  cities.addObjFromArgs("MUC", "München", "Europe", "48°08'15\"N 11°34'30\"E", 1512491);

  // print all cities
  printf("cities known: %d\n", cities.getSize());
  cities.forEach(&printCity);
  printf("\n%s\n", prompt.c_str());

  // get input 1-5 or q
  while ((c = fgetc(stdin)) != 'q')
  {
    if (c == '1' || c == '2' || c == '3' || c == '4')
    {
      cities.setCompareCallback(nullptr);

      if (c == '1')
      {
        cities.setCreateIdCallback(createId_1);
        outText = "createId_1, id = name";
      }
      else if (c == '2')
      {
        cities.setCreateIdCallback(createId_2);
        outText = "createId_2, id = key";
      }
      else if (c == '3')
      {
        cities.setCreateIdCallback(createId_3);
        outText = "createId_3, id = region && name";
      }
      else if (c == '4')
      {
        cities.setCreateIdCallback(createId_4);
        outText = "createId_3, id = inhabitants && key";
      }
    }
    else if (c == '5')
    {
      cities.setCreateIdCallback(nullptr);
      cities.setCompareCallback(compareObj_1);
      outText = "compareObj_1, sort by region ASC && inhabitants DESC";
    }
    else
    {
      // empty text
      outText = "";
    }

    if (outText.length() > 0)
    {
      // print list of cities
      printf("after: %s\n", outText.c_str());
      cities.forEach(&printCity);
      printf("\n%s\n", prompt.c_str());
    }
  }

  printf("done\n");
}
