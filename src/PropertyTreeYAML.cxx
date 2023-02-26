#include "PropertyTreeYAML.hxx"
#include <iostream>
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

/*
  BSD 2-Clause License

  Copyright (c) 2023, Matt Reilly - kb1vc
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
    buildRecurse(TopNode, "");
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
  

  void PropertyTreeYAML::buildRecurse(const YAML::Node & node,
				      const std::string & pathname) {

    std::cerr << "buildRecurse at pathname [" << pathname << "]\n";
    std::string pathnamebase = pathname + ":";
    if(pathname.size() == 0) {
      pathnamebase = "";
    }
    if(node.IsScalar()) {
      // we're at the end 
	put(pathname, node.as<std::string>(), false);
    }
    else if(node.IsMap()) {

      // if the map list is 1 long, then we call create path with a KVP.
      std::cerr << "At pathname \"" << pathname << "\" with node.size() = " << node.size() << "\n";
      for(auto it : node) {
	buildRecurse(it.second, pathnamebase + it.first.as<std::string>());
      }
    }
  }

  void PropertyTreeYAML::writeFile(const std::string & filename) {
    // we'll walk through the property tree and build a YAML tree.
    // then we'll write it out.
#if 1
    YAML::Node root;
    root["THIS"] = "this node";
    root["THAT"] = YAML::Node(YAML::NodeType::Map);
    // root["THAT"]["THERE"];
    YAML::Node sub = root["THAT"];
    sub["THERE"] = "that there value";
    root["THAT"]["ANOTHER"] = "another that";

    root["THAT_SEQ"] = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node s2;
    s2["first"] = "first val";
    s2["second"] = "second val";
    YAML::Node s3;
    s3["third"] = "third val";
    s3["first"] = "fourth val";
    
    root["THAT_SEQ"].push_back(s2);
    root["THAT_SEQ"].push_back(s3);

    root["AGAIN"]["SO"] = "there";
    root["AGAIN"]["WHAT"] = "say again?";
      
    std::cerr << "root[THAT_SEQ][0][second] = "
	      << root["THAT_SEQ"][0]["second"] << "\n";

    std::ofstream os(filename);
    os << root;
    os.close();
#else    
    // Root of our file
    YAML::Node root;
    // Create a node listing some values
    root["MyNode"] = YAML::Node(YAML::NodeType::Map);
    // We now will write our values under root["MyNode"]
    YAML::Node wrnode = root["MyNode"];
    // Write some values
    wrnode["seed"] = 3.14;
    wrnode["name"] = "Glados";
    wrnode["isTurret"] = false;
    // Populate emitter
    YAML::Emitter emitter;
    emitter << root;
    // Write to file
    std::ofstream fout(filename);
    fout << emitter.c_str();
#endif
  }
}
