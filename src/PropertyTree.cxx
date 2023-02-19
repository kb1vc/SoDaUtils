
#include <iostream>
#include <sstream>
#include "Utils.hxx"
#include "Format.hxx"
#include "PropertyTree.hxx"
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
  PropertyTree::PropertyTree() {
  }

  std::list<std::string> 
  PropertyTree::getChildNames(const std::string & pathname, 
			      bool throw_exception) {
    std::list<std::string> ret; 
    auto pathlist = SoDa::split(pathname, ":");
    auto pn = internalGet(&root, pathlist, false, pathname, throw_exception);
    if(pn != nullptr) {
      for(auto ce : pn->children) {
	ret.push_back(ce.first);
      }
    }
    return ret; 
  }

  std::ostream & 
  PropertyTree::recursiveDump(PropNode * pn, std::ostream & os,
			      const std::string & pathname) {
    std::string pathnamebase = pathname + ":";
    if(pathname.size() == 0) {
      pathnamebase = "";
    }
    if(pn->value.size() != 0) {
      os << pathname << "  [" << pn->value << "]\n";
    }
    
    for(auto cn : pn->children) {
      recursiveDump(cn.second, os, pathnamebase + cn.first);
    }
    return os; 
  }

  std::ostream & PropertyTree::dump(std::ostream & os) {
    return recursiveDump(&root, os, ""); 
  }
  
  PropertyTree::PropNode * 
  PropertyTree::internalGet(PropertyTree::PropNode * node, 
			    std::list<std::string> & pathlist, 
			    bool create, 
			    const std::string & orig_pathname, 
			    bool throw_exception) {
    // look in the current node. Is the front of the pathlist in
    // the children map? 
    if(pathlist.size() == 0) return node;
    
    if(node == nullptr) return nullptr; 
    
    std::string key = pathlist.front();
    pathlist.pop_front();
    if(node->children.find(key) != node->children.end()) {
      return internalGet(node->children[key], pathlist, create, 
			 orig_pathname, throw_exception);
    }
    else if(throw_exception) {
      throw PropertyNotFound(orig_pathname);
    }
    else if(create) {
      node->children[key] = new PropNode("");
      return internalGet(node->children[key], pathlist, create, 
			 orig_pathname, throw_exception);
    }
    else {
      return nullptr; 
    }
  }

  std::list<std::string> 
  PropertyTree::makePathList(const std::string & pathname) {
    return SoDa::split(pathname, ":");
  }
  
  PropertyTree::FileNotFound::FileNotFound(const std::string & str) :
    Exception(SoDa::Format("PropertyTree::PropNode::FileNotFound \"%0\"")
	      .addS(str).str())
  {
  }

  PropertyTree::PropertyNotFound::PropertyNotFound(const std::string & str) :
    Exception(SoDa::Format("PropertyTree::PropNode::PropertyNotFound \"%0\"")
	      .addS(str)
	      .str())
  {
  }

  PropertyTree::BadPropertyType::BadPropertyType(const std::string & path_name, 
							   const std::string & type_name, 
							   const std::string & val_string) :
    Exception(SoDa::Format("PropertyTree::PropNode::BadPropertyType at node name \"%0\" with value string \"%1\" which cannot be converted to type \"2\"")
	      .addS(path_name)
	      .addS(val_string)
	      .addS(type_name)
	      .str()) {
  }
}
