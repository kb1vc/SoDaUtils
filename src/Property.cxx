#include "Property.hxx"
#include <sstream>
#include "Utils.hxx"

/*
BSD 2-Clause License

Copyright (c) 2020, Matt Reilly - kb1vc
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

  Property::Value Property::null_value;
  
  Property::Value::Value(const std::string value) : value(value) {
  }

  std::ostream & operator<<(std::ostream & os, std::list<std::string> l) {
    bool first = true; 
    for(auto a : l) {
      if(!first) os << ", ";
      first = false; 
      os << a; 
    }
    return os; 
  }

  template<typename T> bool translate(const std::string v, T & rv, 
				      bool throw_exception,
				      const std::string tname) {
    std::istringstream ss(v);
    bool retval; 
    if(ss >> rv) {
      retval = !(ss.fail());
    }
    else {
      retval = true; 
    }

    if(throw_exception && !retval) {
      throw Property::GetValueException(tname, v);
    }
    
    return retval;
  }

  bool Property::Value::getL(long & v, bool throw_exception) const {
    return translate<long>(value, v, throw_exception, "long");
  }

  bool Property::Value::getUL(unsigned long & v, bool throw_exception) const {
    return translate<unsigned long>(value, v, throw_exception, "unsigned long");
  }

  bool Property::Value::getD(double & v, bool throw_exception) const {
    return translate<double>(value, v, throw_exception, "double");
  }


  std::string Property::Value::getS(bool throw_exception) const {
    return value; 
  }

  
  Property::Property(const std::string name, const std::string value) : 
    name(name), value(Value(value))
  {
    parent = nullptr; 
  }
  
  bool Property::getL(long & v, bool throw_exception) const {
    return value.getL(v, throw_exception);
  }

  bool Property::getUL(unsigned long & v, bool throw_exception) const {
    return value.getUL(v, throw_exception);    
  }

  bool Property::getD(double & v, bool throw_exception) const {
    return value.getD(v, throw_exception);        
  }

  
  std::string Property::getS(bool throw_exception) const {
    return value.getS(throw_exception);
  }

  
  std::string buildPath(std::list<std::string> plist) {
    std::string ret;
    for(auto v : plist) {
      std::cerr << "plist el " << v << "\n";
    }
    for(auto v : plist) {
      ret = ret + "/" + v;
      std::cerr << "ret = \"" << ret << "\"\n";      
    }
    return ret; 
  }

  std::shared_ptr<Property> Property::addChild(std::shared_ptr<Property> child, 
					       bool merge_property) {
    auto cname = child->name; 
    if(children.count(cname) == 0) {
      children[cname] = child; 
    }
    else {
      if(merge_property) {
	mergeProperty(child, children[cname]);
      }

      children[cname] = child; 
    }
    
    return child; 
  }
  
  std::shared_ptr<Property> Property::addProperty(std::shared_ptr<Property> child, 
						  const std::string & path,
						  bool merge_property) {
    std::cerr << "AP strpath\n";
    
    auto path_list = SoDa::split(path, "/");

    std::cerr << "In addProperty <strpath> with path_list " << path_list << "\n";

    // strip off empty leader.
    if(path_list.front().size() == 0) path_list.pop_front();

    return addProperty(child, path_list, merge_property);
  }

  void Property::mergeProperty(std::shared_ptr<Property> dest, std::shared_ptr<Property> src) {
    for(auto c : src->children) {
      if(dest->children.count(c.first) == 0) {
	dest->children[c.first] = c.second; 
      }
      else {
	// don't replace stuff on merge
      }
    }

    for(auto a : src->attributes) {
      if(dest->attributes.count(a.first) == 0) {
	dest->attributes[a.first] = a.second; 
      }
      else {
	// don't replace stuff on merge	
      }
    }

    // we should be pointing to the same parent
    dest->parent = src->parent; 
  }


  std::shared_ptr<Property> Property::addProperty(std::shared_ptr<Property> prop, 
					       std::list<std::string> & path_list,
					       bool merge_property) {


    std::cerr << "AP pathlist with pathlist " << path_list << "\n";
    
    // does the parent node already exist?
    auto parent_p = getProperty(path_list, false);
    
    if(parent_p == nullptr) {
      std::cerr << "Couldn't find node along path list " << path_list << "\n";
      // nope -- create the parent path
      std::cerr << "Need to create path : " << path_list << "\n";      
      parent_p = createPath(path_list); 
    }

    // now we have a parent. 
    std::cerr << "Adding prop name " << prop->name << " to parent at path " << parent_p->getPathToHere() << "\n";
    return parent_p->addChild(prop, merge_property); 
  }  

  void Property::setValue(const std::string & v) {
    value = v; 
  }

  void Property::setParent(std::shared_ptr<Property> p) {
    parent = p; 
  }

  std::shared_ptr<Property> Property::getParent() const { 
    return parent; 
  }

  void Property::addAttribute(const std::string name, const std::string value) {
    attribute_names.push_back(name); 
    attributes[name] = Value(value); 
  }

  const std::list<std::string> & Property::getAttributeNames() const {
    return attribute_names; 
  }

  const Property::Value & Property::getAttribute(const std::string & name, bool throw_exception) {
    if(attributes.count(name) == 0) {
      if(throw_exception) throw Property::BadAttributeNameException(this, name);
      else return Property::null_value; 
    }
    else {
      return attributes[name]; 
    }
  }

  const std::list<std::string> & Property::getChildNames() const {
    return child_names; 
  }

  std::shared_ptr<Property> Property::getProperty(const std::string & path,
						  bool throw_exception) {
    auto path_list = SoDa::split(path, "/");
    if(path_list.front().size() == 0) path_list.pop_front();

    return getProperty(path_list, throw_exception);
  }

  
  std::shared_ptr<Property> Property::getProperty(std::list<std::string> & path_list,
						  bool throw_exception) {
    auto name = path_list.front(); path_list.pop_front();
      
    if(path_list.size() == 0) {
      // we're looking for the child
      if(children.count(name) == 0) {	
	if(throw_exception) throw Property::BadPathException(this, name);
	else return nullptr; 
      }
      else {
	return children[name]; 
      }
    }
    else {
      // we have more to chase
      return getProperty(path_list, throw_exception);
    }
  }


  std::shared_ptr<Property> Property::createPath(std::list<std::string> & path_list) {
    // recursively create a path, if necessary.  Return a pointer to the leaf. 
    if(path_list.size() == 0) {
      std::cerr << "!" << "\n";
      return shared_from_this();
    }
    else {
      auto cname = path_list.front(); path_list.pop_front();
      
      // are do we need to create an intermediate path node? 
      if(children.count(cname) == 0) {
	// I don't have one of these.  I need to make a property node. 
	// I will be its parent. 
	children[cname] = makeProperty(cname, "");
	children[cname]->setParent(shared_from_this());
      }
      
      // now I've got a child at cname (one way or the other)
      return children[cname]->createPath(path_list);
    }
  }

    
  std::string Property::getPathToHere() const {
    std::string ret; 
    if(parent != nullptr) {
      ret = parent->getPathToHere(); 
    }
    
    return ret + "/" + name; 
  }


  std::ostream & Property::print(std::ostream & os, const std::string indent) const {
    os << indent << "Name: \"" << name << "\"  Value \"" << value.getS() << "\"\n";
    std::string my_indent = indent + "    ";

    for(auto & a : attributes) {
      os << my_indent << "Attribute: \"" << a.first << "\" Value \"" << a.second.getS() << "\"\n";
    }
    
    for(auto & c : children) {
      c.second->print(os, my_indent);
    }

    return os; 
  }

  std::shared_ptr<Property> makeProperty(const std::string name, const std::string value) {
    return std::make_shared<Property>(name, value);
  }

  std::shared_ptr<PropertyTree> makePropertyTree() {
    return std::make_shared<PropertyTree>();
  }
}

std::ostream & operator<<(std::ostream & os, const SoDa::Property & p) {
  return p.print(os, "");
}
