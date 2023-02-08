#include "../include/PropertyTreeYAML.hxx"
#include "../include/Format.hxx"
#include "../include/Options.hxx"

#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <random>

int main(int argc, char * argv[]) {
  SoDa::Options cmd; 
  
  if(!cmd.parse(argc, argv) || (cmd.numPosArgs() == 0)) {
    std::cerr << "Missing input file name.\n";
    exit(-1); 
  }
  
  std::string fname = cmd.getPosArg(0); 
  
  SoDa::PropertyTreeYAML ptr(fname); 

  std::cout << "\n\n\nREADY?\n\n\n";
  ptr.dump(std::cout); 
  
  std::string propname("FLINTSTONE:FRED");
  // get the property FLINTSTONE:FRED and see that the value is "guy"
  auto pn = ptr.get(propname);
  pn->dump(std::cerr, "!\t");
  
  std::string fattr;
  bool got_it = pn->get<std::string>(fattr);
  std::cout << propname << " = [" << fattr << "]\n";
}
