#include "Property.hxx"
#include <sstream>
#include "Utils.hxx"
#include <cmath>
#include <iomanip>
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

  Property::Value Property::null_value;
  
  std::map<std::string, bool> Property::Value::boolean_map = Property::Value::initBooleanMap();

  std::map<std::string, bool> Property::Value::initBooleanMap() {
    std::map<std::string, bool> new_map; 
    std::string truestring("TRUE/True/T/1/true");
    std::string falsestring("FALSE/False/false/f/0");

    std::cerr << "In initBooleanMap with truestring " << truestring << "\n";
    
    auto tv = SoDa::split(truestring, "/");
    auto fv = SoDa::split(falsestring, "/");    
    
    for(const auto & v : tv) {
      std::cerr << "Adding true value " << v << "\n";
      new_map[v] = true; 
    }
    for(const auto & v : fv) {
      new_map[v] = false;
    }
    return new_map;
  }

  Property::Value::Value(const std::string value) : str_value(value) {
    set(value);
  }
  Property::Value::Value(bool value)  {
    set(value);
    str_value = value ? "T" : "F";
    
  }

  Property::Value::Value(double value) {
    str_value = "";
    set(value);
  }

  Property::Value::Value(long value) {
    str_value = "";    
    set(value);
  }

  
  std::list<std::string> cdr(std::list<std::string> & l) {
    return std::list<std::string>{std::next(l.begin()), l.end()};
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

  bool Property::Value::get(long & v, bool throw_exception) const {
    if((vtype == LONG) || (vtype == DOUBLE)) {
      v = lv;
    }
    else {
      if(throw_exception) {
	throw Property::GetValueException("long", str_value);
      }
      else {
	return false; 
      }
    }
    return true; 
  }

  bool Property::Value::get(double & v, bool throw_exception) const {
    if((vtype == LONG) || (vtype == DOUBLE)) {
      v = dv;
    }
    else {
      if(throw_exception) {
	throw Property::GetValueException("double", str_value);
      }
      else {
	return false; 
      }
    }
    return true;     
  }

  bool Property::Value::get(bool & v, bool throw_exception) const {
    if(vtype == BOOL) {
      v = bv;
    }
    else {
      if(throw_exception) {
	throw Property::GetValueException("bool", str_value);
      }
      else {
	return false; 
      }
    }
    return true;     
  }

  bool Property::Value::get(std::string & v, bool throw_exception) const {
    v = str_value; 
    return true; 
  }

  
  void Property::Value::set(const std::string & v) {
    if(Property::Value::boolean_map.size() == 0) {
      initBooleanMap();
    }
    std::cerr << "In property value set with [" << v << "]\n";
    auto v_lis = SoDa::split(v, ", /");
    
    for(auto v : v_lis) {
      std::cerr << "..." << v << "\n";
    }

    if(v_lis.size() != 1) return;
    
    
    auto trval = v_lis.front();
    trval = SoDa::squashSpaces(trval);
    
    // is it a bool? 
    if(boolean_map.count(trval) != 0) {
      std::cerr << "It was a bool...\n";
      set(boolean_map[trval]); 
      return; 
    }

    // Now it must be numeric (double or long) or a string. 
    {
      // check for double
      std::istringstream is(v);       
      if(is >> dv) {
	if(!is.fail()) {
	  set(dv);
	  std::cerr << "It was a double...\n";	  
	  return; 
	}
      }
    }
    {
      // check for long
      std::istringstream is(v);       
      if(is >> lv) {
	if(!is.fail()) {
	  set(lv);
	  std::cerr << "It was a long...\n";	  	  
	  return; 
	}
      }
    }

    // Here's where we'd check for an extended type...
    
    // if we got here, it is just a string. 
    vtype = STRING;
    str_value =v; 
  }

  void Property::Value::set(long v) {
    vtype = LONG;
    lv = v; 
    dv = double(lv);
  }

  void Property::Value::set(double v) {
    vtype = DOUBLE;
    dv = v; 
    lv = lround(dv);
  }
  
  void Property::Value::set(bool v) {
    vtype = BOOL;
    bv = v; 
  }    

  std::shared_ptr<Property::Value> Property::Value::get() {
    return shared_from_this();
  }

  bool Property::Value::operator==(const Value & other) const {
    if(vtype != other.vtype) return false; 
    else {
      switch(vtype) {
      case LONG: 
	return lv == other.lv; 
	break; 
      case DOUBLE: 
	return dv == other.dv; 
	break; 
      case BOOL: 
	return bv == other.bv; 
	break; 
      case STRING:
	return str_value == other.str_value; 
	break; 
      default: 
	return true; 
      }
    }
  }

  std::ostream & Property::Value::print(std::ostream & os) const {
    int old_width = os.width();
    int old_precision = os.precision();
    switch (vtype) {
    case LONG:
      os << lv; 
      break; 
    case DOUBLE:
      os << std::setprecision(std::numeric_limits<double>::max_digits10 + 1);
      os << dv; 
      break; 
    case BOOL:
      if(bv) {
	os << "True"; 
      }
      else {
	os << "False";
      }
      break; 
    case STRING: 
      os << str_value; 
      break; 
    default: 
      break; 
    }
    os.width(old_width); 
  os.precision(old_precision);

    return os; 
  }
  
  Property::Property(const std::string name, 
		     const std::string value, 
		     std::shared_ptr<Property> parent) : 
    name(name), value(Value(value)), parent(parent)
  {
    has_value = true; 
  }

  Property::Property(const std::string name, 
		     const Property::Value & v,
		     std::shared_ptr<Property> parent) : 
    name(name), 
    value(v), 
    parent(parent) 
  {
    has_value = true; 
  }

  Property::Property(const std::string name, 
		     std::shared_ptr<Property> parent) : 
    name(name), parent(parent), value(Property::null_value)
  {
    has_value = false; 
  }
  
  Property::Property(std::shared_ptr<Property> parent) :
    name(""), value(Property::null_value), parent(parent)  {
    has_value = false; 
  }    

  bool Property::get(long & v, bool throw_exception) const {
    if(has_value) return value.get(v, throw_exception);
    else return false; 
  }


  bool Property::get(double & v, bool throw_exception) const {
    if(has_value) return value.get(v, throw_exception);        
    else return false;
  }

  
  bool Property::get(bool & v, bool throw_exception) const {
    if(has_value) return value.get(v, throw_exception);
    else return false;    
  }

  bool Property::get(std::string & v, bool throw_exception) const {
    if(has_value) return value.get(v, throw_exception);
    else return false; 
  }

  void Property::set(const std::string & v) {
    has_value = true; 
    value.set(v);
  }

  void Property::set(long v) {
    has_value = true;     
    value.set(v);
  }

  void Property::set(double v) {
    has_value = true;     
    value.set(v);
  }

  void Property::set(bool v) {
    has_value = true;     
    value.set(v);
  }
  
  const Property::Value & Property::get() { 
    return value;
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

  void Property::setParent(std::shared_ptr<Property> p) {
    parent = p; 
  }

  std::shared_ptr<Property> Property::getParent() const { 
    return parent; 
  }

  void Property::setAttribute(const std::string name, const std::string value) {
    if(attributes.count(name) == 0) {
      attribute_names.push_back(name); 
    }
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

  const std::list<std::string>  Property::getChildNames() const {
    std::list<std::string> child_names; 
    for(auto v : children) {
      child_names.push_back(v.first);
    }
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

    auto name = path_list.front();       

    if(path_list.size() == 1) {
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
      auto rest = cdr(path_list);
      return getProperty(rest, throw_exception);
    }
  }


  std::shared_ptr<Property> Property::createPath(std::list<std::string> & path_list) {
    // recursively create a path, if necessary.  Return a pointer to the leaf. 
    if(path_list.size() == 0) {
      std::cerr << "!" << "\n";
      return shared_from_this();
    }
    else {
      auto cname = path_list.front();
      
      // are do we need to create an intermediate path node? 
      if(children.count(cname) == 0) {
	// I don't have one of these.  I need to make a property node. 
	// I will be its parent. 
	children[cname] = makeProperty(cname, "");
	children[cname]->setParent(shared_from_this());
      }
      
      // now I've got a child at cname (one way or the other)
      auto rest = cdr(path_list);
      return children[cname]->createPath(rest);
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
    os << indent << "Name: \"" << name << "\"  Value \"" ;
    value.print(os) << "\"\n";
    std::string my_indent = indent + "    ";

    for(auto & a : attributes) {
      os << my_indent << "Attribute: \"" << a.first << "\" Value \""; 
      a.second.print(os) << "\"\n";
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

std::ostream & operator<<(std::ostream & os, const SoDa::Property::Value & v) {
  return v.print(os);
}

std::ostream & operator<<(std::ostream & os, std::shared_ptr<SoDa::Property::Value>  vp) {
  return vp->print(os);
}
