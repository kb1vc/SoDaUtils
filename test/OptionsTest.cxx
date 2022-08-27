#include "../include/Format.hxx"
#include "../include/Options.hxx"
#include <iostream>


int a_val;
float f_val; 
bool b_val;
std::string s_val;
std::vector<std::string> s_val_list;
std::vector<int> a_val_list;
bool pres_val;

bool testPosArgs(SoDa::Options cmd) {
  a_val = 0; 
  std::list<std::string> arglist = { 
    "--ava", "3", 
    "po0", "po1", "po2" };

  bool is_good = true; 
  if(!cmd.parse(arglist)) {
    std::cerr << "testPosArgs bad parse\n";
    is_good = false; 
  }

  // check the aval.
  if(a_val != 3) {
    std::cerr << "testPosArgs bad a_val\n";
    is_good = false;
  }
  
  if(cmd.numPosArgs() != 3) {
    std::cerr << "testPosArgs got bad arg count\n";
    is_good = false; 
  }
  
  auto pags = cmd.getPosArgs();
  if(pags.size() != 3) {
    std::cerr << "testPosArgs got bad arglist count\n";
    is_good = false; 
  }

  arglist.pop_front(); arglist.pop_front();
  int i = 0;
  for(auto refa : arglist) {
    auto pa = cmd.getPosArg(i); 

    if(pa.compare(refa) != 0) {
      std::cerr << SoDa::Format("testPosArgs getPosArg got [%0] should be [%1]\n")
	.addS(pa)
	.addS(refa);
    is_good = false; 
    }
    if(pags[i].compare(refa) != 0) {
      std::cerr << SoDa::Format("testPosArgs getPosArgs[%2] got [%0] should be [%1]\n")
	.addS(pa)
	.addS(refa)
	.addI(i);
      is_good = false; 
    }
    i++; 
  }
  
  return is_good; 
}

int main(int argc, char ** argv) {
  // create a mailbox
  SoDa::Options cmd;

  cmd.addP(&pres_val, "pres", 'p')
    .add<bool>(&b_val, "boo", 'b')
    .add<int>(&a_val, "ava", 'a')
    .add<float>(&f_val, "fva", 'f')
    .add<std::string>(&s_val, "sva", 's')
    .addV<std::string>(&s_val_list, "sla", 'l')
    .addV<int>(&a_val_list, "ala", 'L');

  bool is_good = true; 
  is_good = is_good && testPosArgs(cmd); 
  
  if(is_good) std::cout << "PASS\n";
  else std::cout << "FAIL\n";
}
