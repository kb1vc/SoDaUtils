#include "../include/Property.hxx"
#include "../include/PropertyIO_JSON.hxx"
#include <string>
#include <iostream>
#include <sstream>

int main(int argc, char * argv[]) {
  auto ptree = SoDa::makePropertyTree();
  
  ptree->addProperty(SoDa::makeProperty("Flintstones", ""));

  ptree->addProperty(SoDa::makeProperty("Barney", "Dad"), "/Neighbors/Rubbles");

  ptree->print(std::cout, "   ");
  
  SoDa::PropertyIO_JSON propio; 
  
  if(argc > 1) {
    auto json_tree = propio.read(argv[1]);
    
    json_tree->print(std::cout, ">>> ");
  }
}
