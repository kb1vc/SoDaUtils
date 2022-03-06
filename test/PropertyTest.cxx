#include "../include/Property.hxx"
#include <string>
#include <iostream>
#include <sstream>

int main(int argc, char * argv[]) {
  auto ptree = SoDa::makePropertyTree();
  
  ptree->addProperty(SoDa::makeProperty("Flintstones", ""));

  ptree->addProperty(SoDa::makeProperty("Barney", "Dad"), "/Neighbors/Rubbles");

  ptree->print(std::cout, "   ");
}
