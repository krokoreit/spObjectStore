/**
 * example code for spObjectStore library
 * 
 * 
 */
#include <stdio.h>
#include <string>
#include <filesystem>

#include <spObjectStore.h>


class city
{
  public:
    std::string _code = "";
    std::string _name = "";
    std::string _continent = "";
    std::string _location = "";
    uint32_t _population = 0;
    city(std::string code, std::string name, std::string continent, std::string location, uint32_t population);


};

city::city(std::string code, std::string name, std::string continent, std::string location, uint32_t population)
{
  _code = code;
  _name = name;
  _continent = continent;
  _location = location;
  _population = population;
}

// separatot between continent & city
char sep[] = "#/#";


/**
 * @brief compare the key based on continent and city value
 * 
 * @param A 
 * @param B 
 * @return int <0 = A has lower value, 0 = same, >0 = A has higher value 
 */
int compareByContinentAndCity(std::string A, std::string B)
{
  size_t sPos = A.find(sep);
  std::string A1 = A.substr(0, sPos);
  std::string A2 = A.substr(sPos + 1);
  sPos = A.find(sep);
  std::string B1 = B.substr(0, sPos);
  std::string B2 = B.substr(sPos + 1);

  int cmp = strcmp(A1.c_str(), B1.c_str());
  if (cmp == 0)
  {
    cmp = strcmp(A2.c_str(), B2.c_str());
  }
  return cmp;
}


// the store
spObjectStore<city> cities(compareByContinentAndCity);


// for print
bool isFirst = true;


/**
 * @brief print key and add object to store
 * 
 * @param code 
 * @param name 
 * @param continent 
 * @param location 
 * @param population 
 */
void addCity(std::string code, std::string name, std::string continent, std::string location, uint32_t population)
{
  std::string key = continent + sep + name;
  // print key
  if (isFirst)
  {
    printf("adding ");
    isFirst = false;
  }
  else
  {
    printf(", ");
  }
  printf("%s", key.c_str());
  // add city
  cities.addObj(key, code, name, continent, location, population);
}


/**
 * @brief callback function to print a stored object with key and object members
 * 
 * @param pObj pointer to city object 
 * @return true (as we do not want to stop iterarion)
 */
bool printCity(city* pObj)
{
  printf("continent: %s, name: %s, code: %s, location: %s, population: %i\n", pObj->_continent.c_str(), pObj->_name.c_str(), pObj->_code.c_str(), pObj->_location.c_str(), pObj->_population);
  return true;
}



/**
 * @brief print content of store
 * 
 */
void printCities()
{
  printf("cities known: %d\n", cities.getSize());
  // loop output
  cities.forEach(&printCity);
}



/**
 * @brief our main function
 * 
 */
int main(int argc, char *argv[])
{
    std::string a = argv[0];
    printf("running %s\n", a.substr(a.rfind(std::filesystem::path::preferred_separator) + 1).c_str());

    // add cities
    addCity("IEV", "Kyiv", "Europe", "50°27′00″N 30°31′24″E", 2952301); 
    addCity("PAR", "Paris", "Europe", "48°51′24″N 2°21′8″E", 2102650);
    addCity("TYO", "Tokyo", "Asia", "35°41′23″N 139°41′32″E", 14094034);
    addCity("LAX", "Los Angeles", "America", "34°03′N 118°15′W", 3898747);
    addCity("LON", "London", "Europe", "51°30′26″N 0°7′39″W", 8799800);
    addCity("BKK", "Bangkok", "Asia", "13°45′09″N 100°29′39″E", 8305218);
    addCity("MUC", "München", "Europe", "48°08′15″N 11°34′30″E", 1512491);
    
    printf("\n");

    // print all cities
    printCities();

    printf("done\n");
}
