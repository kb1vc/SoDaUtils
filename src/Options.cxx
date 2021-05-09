#include "Options.hxx"

/*
BSD 2-Clause License

Copyright (c) 2021, Matt Reilly - kb1vc
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file Options.cxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 10, 2021
 */

#include <cctype>

namespace SoDa {

  Options::Options() {
    // not much to do. 
  }

  Options & Options::addInfo(const std::string & info) {
    info_list.push_back(info); 
    return * this; 
  }

  std::ostream &  Options::printHelp(std::ostream & os) {
    // print the description
    for(auto istr : info_list) {
      os << istr << "\n";
    }

    // now print the arguments
    for(auto ele : long_map) {
      auto arg_p = ele.second;
      os << "\t--" << arg_p->long_name << "\t-" << arg_p->ab_name << "\t";
      arg_p->printHelp(os);
      os << "\n";
    }

    return os; 
  }

  std::string Options::getPosArg(int idx) {
    if(idx > pos_arg_vec.size()) return std::string("");
    else return pos_arg_vec[idx]; 
  }

  std::list<std::string> Options::buildTokenList(int argc, char * argv[]) {
    std::list<std::string> ret; 
    if(argc < 2) return ret;

    for(int i = 1; i < argc; i++) {
      ret.push_back(std::string(argv[i]));
    }
    
    return ret; 
  }

  std::list<std::string> Options::buildTokenList(const std::string & s) {
    std::list<std::string> ret;
    std::string cur_tkn("");
    bool in_quote = false; 
    for(auto c : s) {
      if(isspace(c)) {
	if(in_quote) {
	  cur_tkn.push_back(c);
	}
	else if(cur_tkn.length() > 0) {
	  ret.push_back(cur_tkn);
	  cur_tkn.clear();
	}
      }
      else if(c == '\"') {
	if(in_quote) {
	  // push the token. 
	  ret.push_back(cur_tkn);
	  cur_tkn.clear();
	  in_quote = false; 
	}
	else {
	  // start of a quoted string...
	  in_quote = true; 
	}
      }
      else {
	cur_tkn.push_back(c);
      }
    }

    if(cur_tkn.length() > 0) {
      ret.push_back(cur_tkn);
    }

    return ret; 
  }

  int Options::isSwitch(const std::string & tkn) {
    if(tkn.length() < 2) return 0;

    // does it start with anything other than - ? 
    if(tkn[0] != '-') {
      return 0; 
    }
    
    // it starts with a -.  Is that all there is? 
    if(tkn.length() < 2) return 0;
    
    // it starts with a -. It is at least two instrs long.
    // does it start with -- ? 
    if(tkn[1] == '-') {
      // it starts with -- .  If it isn't at least 3 long, then this is just --
      if(tkn.length() < 3) return 0; 

      // it starts with -- and is followed by something
      return 2; 
    }

    // it starts with - and is followed by something. 
    return 1; 
  }

  bool Options::parse(int argc, char * argv[]) {
    std::list<std::string> tokens = buildTokenList(argc, argv);

    return parse(tokens);
  }

  bool Options::parse(const std::string & s) {
    std::list<std::string> tokens = buildTokenList(s);

    return parse(tokens);
  }

  bool Options::parse(std::list<std::string> tokens) {
    OptBase * arg_p = nullptr;
    
    try {
    while(!tokens.empty()) {
      std::string tkn = tokens.front();
      tokens.pop_front();

      int sw_len;
      if((arg_p == nullptr) || (arg_p->hasDefault())) {
	sw_len = isSwitch(tkn);      	
      }
      else {
	// we have an argpointer and there is no default
	// we need to eat the next token. 
	sw_len = 0; 
      }


      if(sw_len == 0) {
	// we know this is either a positional argument, or 
	// a value for the previous arg_p;
	if(arg_p != nullptr) {
	  arg_p->setVal(tkn);
	  arg_p = nullptr; 
	}
	else {
	  // it is a positional argument.
	  pos_arg_vec.push_back(tkn);
	}
      }
      else {
	if(arg_p != nullptr) {
	  arg_p->setPresent(); 
	}
	
	arg_p = nullptr;
	
	if(sw_len == 1) {
	  if(tkn[1] == 'h') {
	    printHelp(std::cerr); 
	    return false; 
	  }
	  arg_p = findOpt(tkn[1]);
	}
	else {
	  if(tkn == "--help") {
	    printHelp(std::cerr);
	    return false; 
	  }
	  arg_p = findOpt(tkn.substr(2));
	}

	if(arg_p == nullptr) {
	  throw BadOptionNameException(tkn); 
	}

	if(arg_p->isPresentOpt()) {
	  arg_p->setPresent();
	  arg_p = nullptr; 
	}
      }
    }
    }
    catch (BadOptValueException & exc) {
      std::cerr << exc.what() << "\n";
      return false; 
    }
    catch (BadOptionNameException & exc) {
      std::cerr << exc.what() << "\n";
      printHelp(std::cerr);
      return false; 
    }
    
    return true; 
  }

  Options::OptBase * Options::findOpt(const std::string & long_name) {
    if(long_map.find(long_name) == long_map.end()) {
      return nullptr;
    }
    return long_map[long_name];
  }

  Options::OptBase * Options::findOpt(char ab_name) {
    if(ab_map.find(ab_name) == ab_map.end()) {
      return nullptr;
    }
    return ab_map[ab_name];
  }


  bool Options::isPresent(const std::string & long_name) {
    if(long_map.find(long_name) == long_map.end()) {
      return false; 
    }
    return long_map[long_name]->isPresent();
  }

  bool Options::isPresent(char ab_name) {
    if(ab_map.find(ab_name) == ab_map.end()) {
      return false; 
    }
    return ab_map[ab_name]->isPresent();
  }

  void Options::registerOpt(OptBase * arg_p, 
		  const std::string & long_name, 
			    char ab_name) {
    arg_p->setNames(long_name, ab_name); 
    long_map[long_name] = arg_p;
    ab_map[ab_name] = arg_p;       
  }  

  
  std::ostream & Options::OptBase::printHelp(std::ostream & os) {
    os << doc_str;
    return os; 
  }
}
