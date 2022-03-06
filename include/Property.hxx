#pragma once
#include <map>
#include <memory>
#include <string>
#include <list>
#include <iostream>

/*
BSD 2-Clause License

Copyright (c) 2020, 2021, Matt Reilly - kb1vc
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
 * @file Property.hxx
 * @author Matt Reilly (kb1vc)
 * @date December 31, 2020
 */

namespace SoDa {

  
  class Property : public std::enable_shared_from_this<Property> {
  public: 
    Property(const std::string name, const std::string value);
  
    class Value {
      // stored property value
    public:
      Value(const std::string value = std::string("")); 
      
      bool getL(long & v, bool throw_exception = false) const;
      
      bool getUL(unsigned long & v, bool throw_exception = false) const;
      
      bool getD(double & v, bool throw_exception = false) const;
      
      std::string getS(bool throw_exception = false) const;

      std::string value;
    }; 
    
    bool getL(long & v, bool throw_exception = false) const;
    
    bool getUL(unsigned long & v, bool throw_exception = false) const;

    bool getD(double & v, bool throw_exception = false) const;

    std::string getS(bool throw_exception = false) const;

    void setValue(const std::string & v);

    std::shared_ptr<Property> addChild(std::shared_ptr<Property> child, 
				       bool merge_property = false);

    std::shared_ptr<Property> addProperty(std::shared_ptr<Property> child, 
					  bool merge_property = false) {
      std::cerr << "AP to AC!\n";
      if(merge_property) std::cerr << "MERGE\n";
      else std::cerr << "NOMERGE\n";
      return addChild(child, merge_property); 
    }

    // C++ converts string literals to char* which looks like a bool.  Thanks committee.
    std::shared_ptr<Property> addProperty(std::shared_ptr<Property> child, 
					  const char * path_c, 
					  bool merge_property = false) {
      return addProperty(child, std::string(path_c), merge_property);
    }
    
    std::shared_ptr<Property> addProperty(std::shared_ptr<Property> child, 
				       const std::string & path, 
				       bool merge_property = false);

    std::shared_ptr<Property> addProperty(std::shared_ptr<Property> child, 
				       std::list<std::string> & path, 
				       bool merge_property = false);

    void setParent(std::shared_ptr<Property> p);

    std::shared_ptr<Property> getParent() const;

    void addAttribute(const std::string name, const std::string value);

    const std::list<std::string> & getAttributeNames() const;

    const Value & getAttribute(const std::string & name, bool throw_exception = false);

    const std::list<std::string> & getChildNames() const;

    std::shared_ptr<Property> getProperty(const std::string & path, 
					  bool throw_exception = false);

    std::shared_ptr<Property> getProperty(std::list<std::string> & path_list, 
					  bool throw_exception = false);     
    
    std::shared_ptr<Property> createPath(std::list<std::string> & path);
    
    std::string getPathToHere() const; 

    class GetValueException : public std::runtime_error {
    public:
      GetValueException(const std::string tname, const std::string valstring) : 
	std::runtime_error("SoDa::Property::GetValueException - string \"" + 
			   valstring + "\" doesn't look like a " + tname + " value") {
      }
    };

    class BadPathException : public std::runtime_error {
    public:
      BadPathException(const Property * p, const std::string downstream) : 
	std::runtime_error("SoDa::Property::BadPathException - Property \"" + 
			   p->getPathToHere() + "\" find descendent property on path \"" + 
			   downstream + "\"") {
      }
    };

    class BadAttributeNameException : public std::runtime_error {
    public:
      BadAttributeNameException(const Property * p, const std::string attrname) : 
	std::runtime_error("SoDa::Property::BadAttributeException - Property \"" + 
			   p->getPathToHere() + "\" has no attribute named \"" + 
			   attrname + "\"") {
      }
    };


    std::ostream & print(std::ostream & os, const std::string indent) const;
    
    friend std::ostream & operator<<(std::ostream & os, const Property & p);
    
  protected:
    void mergeProperty(std::shared_ptr<Property> dest, std::shared_ptr<Property> src);
  private:
    Value value;
    std::string name; 

    std::map<std::string, Value> attributes;
    std::list<std::string> attribute_names; 
    
    std::map<std::string, std::shared_ptr<Property>> children;
    std::list<std::string> child_names; 

    std::shared_ptr<Property> parent;

    static Value null_value; 
  };

  class PropertyTree : public Property {
  public:
    PropertyTree() : Property("", "") { }
  };


  std::shared_ptr<Property> makeProperty(const std::string name, const std::string value);

  std::shared_ptr<PropertyTree> makePropertyTree();

}
    
std::ostream & operator<<(std::ostream & os, const SoDa::Property & p);

