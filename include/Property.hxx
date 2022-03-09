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
 * @date Mar 6, 2022
 *
 * Contains types defining property holders and values for property trees. 
 * 
 * Note this file contains multiple class definitions. To do otherwise
 * would really clutter up the header files. 
 */

namespace SoDa {

  
  class Property : public std::enable_shared_from_this<Property> {
  public: 
    class Value; 
    Property(const std::string name, 
	     const std::string value, 
	     std::shared_ptr<Property> parent = nullptr);
    Property(const std::string name, 
	     const Property::Value & v, 
	     std::shared_ptr<Property> parent = nullptr);
    Property(const std::string name, 
	     std::shared_ptr<Property> parent = nullptr);
    Property(std::shared_ptr<Property> parent = nullptr); 
  
    /**
     *
     * The place where we store the content of an attribute/property
     *
     * Property values may be of string, or numeric type. String values
     * are parsed as long values, if possible, otherwise as doubles, then 
     * as bool (either 'T' or 'F') and finaly we give up and call it a string. 
     *
     * In parsing long, double, or bool, the input string is split and trimmed. 
     * If there is more than one element in the split list, then this value is
     * a string. 
     * 
     * A get method on a numeric property can be used to get a long or a double. 
     * All get string operations return the original string. 
     */
    class Value : public std::enable_shared_from_this<Property::Value> {
      // stored property value
    public:
      enum PrimitiveType { LONG, DOUBLE, BOOL, STRING, OTHER };

      Value(const std::string value = std::string(""));
      Value(long v);
      Value(double v);
      Value(bool v);
      
      bool get(long & v, bool throw_exception = false) const;
      bool get(double & v, bool throw_exception = false) const;
      bool get(std::string & v, bool throw_exception = false) const;
      bool get(bool & v, bool throw_exception = false) const;
      std::string getRaw(); 

      void set(const std::string & v);
      void set(long v);
      void set(double v);
      void set(bool v);
      
      PrimitiveType getType() const { return vtype; }
      
      bool operator==(const Value & other) const; 

      std::string str_value;
      double dv; 
      long lv; 
      bool bv; 
      PrimitiveType vtype;
      
      std::shared_ptr<Value> get();

      std::ostream & print(std::ostream & os) const;
      
      friend std::ostream & operator<<(std::ostream & os, const Value & v);
      friend std::ostream & operator<<(std::ostream & os, std::shared_ptr<Value> vp);      
      
    protected:
      static std::map<std::string, bool>  initBooleanMap();

      static std::map<std::string, bool> boolean_map; 
    }; 


    
    bool get(long & v, bool throw_exception = false) const;
    bool get(double & v, bool throw_exception = false) const;
    bool get(bool & v,  bool throw_exception = false) const;
    bool get(std::string & v, bool throw_exception = false) const;

    void set(const std::string & v);
    void set(long v);
    void set(double v);
    void set(bool v);
      
    const Value &  get();  
    
    bool hasValue() { return has_value; }
    
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

    std::string & getName() { return name; }
    
    void setAttribute(const std::string name, const std::string value);

    const std::list<std::string> & getAttributeNames() const;

    const Value & getAttribute(const std::string & name, bool throw_exception = false);

    const std::list<std::string> getChildNames() const;

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
    bool has_value; 
    std::string name; 

    std::map<std::string, Value> attributes;
    std::list<std::string> attribute_names; 
    
    std::map<std::string, std::shared_ptr<Property>> children;

    std::shared_ptr<Property> parent;

    static Value null_value; 
  };

  class PropertyTree : public Property {
  public:
    PropertyTree() : Property("") { }
  };


  std::shared_ptr<Property> makeProperty(const std::string name, const std::string value);

  std::shared_ptr<PropertyTree> makePropertyTree();

}
    
std::ostream & operator<<(std::ostream & os, const SoDa::Property & p);
std::ostream & operator<<(std::ostream & os, const SoDa::Property::Value & v);
std::ostream & operator<<(std::ostream & os, const std::shared_ptr<SoDa::Property::Value> vp); 
