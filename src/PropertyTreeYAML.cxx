#include "PropertyTreeYAML.hxx"
#include <iostream>
#include <sstream>
#include <yaml-cpp/yaml.h>

/*
BSD 2-Clause License

Copyright (c) 2022, Matt Reilly - kb1vc
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


namespace SoDa {
  PropertyTreeYAML::PropertyTreeYAML() {
    // do nothing
  }
  
  PropertyTreeYAML::PropertyTreeYAML(const std::string & filename) {
    readFile(filename); 
  }

  void PropertyTreeYAML::readFile(const std::string & filename) {
    YAML::Node TopNode; ///< the root node for the current file. 
    
    try {
      TopNode = YAML::LoadFile(filename);
    }
    catch(YAML::BadFile & e) {
      throw PropertyTree::FileNotFound(e.what());
    }
    
    // now recursively create the tree
    traverse(TopNode, "");
    root = buildRecurse(TopNode, nullptr);
  }

  std::string ntypestr(const YAML::Node & node) {
    std::string ret; 
    if(node.IsDefined()) ret = ret + "Defined ";
    if(node.IsNull()) ret = ret + "Null ";
    if(node.IsScalar()) ret = ret + "Scalar ";
    if(node.IsSequence()) ret = ret + "Sequence ";
    if(node.IsMap()) ret = ret + "Map ";

    return ret;
  }

  void PropertyTreeYAML::traverse(const YAML::Node & node, std::string indent) {
    //    std::cerr << indent << "kind [" << ntypestr(node) << "] size = " << node.size() << "\n";

    if(node.IsScalar()) {
      std::cerr << indent << "Scalar value [" << node.as<std::string>() << "]\n";
    }
    else if(node.IsSequence()) {
      std::cerr << indent << "Starting sequence\n";

      for(auto it : node) {
	traverse(it, indent + "  ");
      }
    }
    else if(node.IsMap()) {
      //for(auto it = node.begin(); it != node.end(); ++it) {
      for(auto it : node) {
	if(it.second.size() == 0) {
	  std::cerr << indent << "[" << it.first << "] = (" << it.second.as<std::string>() << ")\n";	  
	}
	else {
	  std::cerr << indent << "Map[" << it.first << "]  !size = " << it.second.size() << "\n";	  
	  traverse(it.second, indent + "  ");
	}
      }
    }
  }
  unsigned int level = 0;
  
  PropertyTree::PropNode * 
  PropertyTreeYAML::buildRecurse(const YAML::Node & node,
				 PropertyTree::PropNode * badparent) {

    PropertyTree::PropNode * ret;
    level++;
    
    if(node.IsScalar()) {
      ret = new PropertyTree::PropNode(nullptr, node.as<std::string>(), true);
    }
    else if(node.IsSequence()) {
      // build the tree for each item in the sequence
      std::cerr << level << " Got sequence named [" 
		<< node.as<std::string>("SEQ???") << "]\n";
    
      // create a new sequence tree...
      ret = new PropertyTree::PropNode(nullptr, node.as<std::string>("SEQ???"),
				       false);
      for(auto it : node) {
	ret->prop_list.push_back(buildRecurse(it, nullptr));
      }
    }
    else if(node.IsMap()) {
      // this is either a dictionary
      // or it is a KVP.
      // A KVP will be one element long.
      ret = new PropertyTree::PropNode(nullptr, 
				       node.as<std::string>("ANON"),
				       false);
      
      // We can only know by testing its children
      for(auto it : node) {
	if(it.second.IsScalar()) {
	  // this was a KVP	  
	  ret->dictionary[it.first.as<std::string>()] = 
	    buildRecurse(it.second, nullptr);
	}
	else {
	  ret->dictionary[it.first.as<std::string>()] = 
	    buildRecurse(it.second, nullptr);
	}
      }
    }
    level --; 
    return ret;
  }

  void PropertyTreeYAML::writeFile(const std::string & filename) {
  }
  
  
  
}
