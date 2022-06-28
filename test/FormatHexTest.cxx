#include "../include/Format.hxx"
#include <string>
#include <iostream>
#include <map>

bool testFmt(unsigned long v, int w, const std::string & pat, char sep) {
  std::string res = SoDa::Format("%0").addU(v, 'X', w, sep).str(); 
  
  if (res != pat) {
    std::cerr << SoDa::Format("Bad match: v = %0, w = %1, res = [%2] pattern = [%3]\n")
      .addU(v, 'X')
      .addI(w)
      .addS(res)
      .addS(pat);
    return false; 
  }
  
  return true; 
}
int main(int argc, char * argv[]) {

  
  std::map<std::string, std::pair<unsigned long, int>> check_map = 
    { { "0x0", { 0, 0 } }, 
      { "0x000", { 0, 3} }, 
      { "0x100", { 0x100, 0} },
      { "0x0100", { 0x100, 4 } },
      { "0x00000100", { 0x100, 8 } },
      { "0x0000000000000100", { 0x100, 16 } }            
    };

  std::map<std::string, std::pair<unsigned long, int>> under_check_map = 
    { { "0x0", { 0, 0 } }, 
      { "0x000", { 0, 3} }, 
      { "0x100", { 0x100, 0} },
      { "0x0100", { 0x100, 4 } },
      { "0x0000_0100", { 0x100, 8 } },
      { "0x0000_0000_0000_0100", { 0x100, 16 } }            
    };
  
  bool all_ok = true; 
  for(auto e : check_map) {
    all_ok = all_ok && testFmt(e.second.first, e.second.second, e.first, '\000');
  }
  for(auto e : under_check_map) {
    all_ok = all_ok && testFmt(e.second.first, e.second.second, e.first, '_');
  }
    
  if(all_ok) exit(0);
  else exit(-1);
}
