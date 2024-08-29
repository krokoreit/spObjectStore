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
    city(std::string key, std::string name, std::string region, std::string location, uint32_t inhabitants);
};

city::city(std::string key, std::string name, std::string region, std::string location, uint32_t inhabitants)
{
  _key = key;
  _name = name;
  _region = region;
  _location = location;
  _inhabitants = inhabitants;
}

// the store - sort by id DESC
spObjectStore<city> cities(DESC);

// for print
std::string addedTxt = "";


/**
 * @brief add object to store and use key as id
 * 
 * @param key 
 * @param name 
 * @param location 
 * @param inhabitants 
 */
  void addCity(std::string key, std::string name, std::string region, std::string location, uint32_t inhabitants)
{
  if (addedTxt.length() > 0)
  {
    addedTxt.append(", ");
  }
  addedTxt.append(key);
  
  // add city with region & key as id
  std::string id = cities.makeIdFromArgs(region, key);
  cities.addObjWithId(id, key, name, region, location, inhabitants);
}


/**
 * @brief callback function to print a stored object with id and object members
 * 
 * @param id 
 * @param obj a city object 
 * @return true (as we do not want to stop iterarion)
 */
bool printCity(const std::string &id, const city &obj)
{
  printf("id: %s, key: %s, name: %s, region: %s, location: %s, inhabitants: %i\n", id.c_str(), obj._key.c_str(), obj._name.c_str(), obj._region.c_str(), obj._location.c_str(), obj._inhabitants);
  return true;
}


/**
 * create id callback funcs
 */
std::string createId_1(const city &obj)
{
  return cities.makeIdFromArgs(obj._region, obj._name);
}
std::string createId_2(const city &obj)
{
  return cities.makeIdFromArgs(obj._name);
}
std::string createId_3(const city &obj)
{
  return cities.makeIdFromArgs(obj._inhabitants, obj._key);
}


/**
 * @brief our main function
 * 
 */
int main(int argc, char *argv[])
{

  // for Windows console codepage issues, comment out if not needed
  SetConsoleOutputCP(CP_UTF8);

  // change separator
  cities.setIdSeparator("$$$");

  // add cities
  addCity("IEV", "Kyiv", "Europe", "50°27'00\"N 30 31'24\"E", 2952301); 
  addCity("PAR", "Paris", "Europe", "48°51'24\"N 2°21'8\"E", 2102650);
  addCity("TYO", "Tokyo", "Asia", "35°41'23\"N 139°41'32\"E", 14094034);
  addCity("LAX", "Los Angeles", "America", "34°03'N 118°15'W", 3898747);
  addCity("LON", "London", "Europe", "51°30'26\"N 0°7'39\"W", 8799800);
  addCity("BKK", "Bangkok", "Asia", "13°45'09\"N 100°29'39\"E", 8305218);
  addCity("MUC", "München", "Europe", "48°08'15\"N 11°34'30\"E", 1512491);
  printf("added: %s\n", addedTxt.c_str());

  // print all cities
  printf("cities known: %d\n", cities.getSize());
  cities.forEach(&printCity);


  printf("done\n");

}
