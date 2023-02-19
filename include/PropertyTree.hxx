#pragma once
#include <string>
#include <list>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <ios>
#include <map>
#include <vector>
#include <typeinfo>
#ifdef LIB_BUILD_IMPL
#include "Exception.hxx"
#else
#include <SoDa/Exception.hxx>
#endif
/*
  BSD 2-Clause License

  Copyright (c) 2022 Matt Reilly - kb1vc
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
 * @file PropertyTree.hxx
 * @author Matt Reilly (kb1vc)
 * @date December 31, 2020
 */

/**
 * @page SoDa::PropTree A simple property tree widget with
 * documentation. 
 * 
 * This is meant to be subclassed for file formats like XML and YAML.
 *
 * So, here is SoDa::PropertyTree -- 
 */

/**
 * @namespace SoDa
 * 
 * Not much else is spelled that way, so we're probably not going to
 * have too many collisions with code written by other people.
 *
 * SoDa is the namespace I use in code like <a
 * href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> (The S D
 * and R stand for Software Defined Radio.  The o,a,d,i,o don't stand
 * for anything in particular.)  But this code has nothing to do with
 * software defined radios or any of that stuff.
 */
namespace SoDa {

  class PropertyTree {
    /**
     * @class PropertyTree
     *
     * A property tree is a hierarchical store of key-value pairs. Each node in the hierarchy
     * can contain a single value (string) and/or a map of child nodes indexed by key. 
     *
     * A property tree may be built from YAML, XML, or other input formats. However, it does
     * not support value lists or sequences. Each node in the tree has a unique name and a unique value. 
     */

  private:
    class PropNode;

  public:    
    /**
     * @brief The constructor.
     *
     * Create a property tree.  There won't be anything in it. 
     */
    PropertyTree();

    /**
     * @brief Read a property file
     *
     * Add the stuff in "filename" to the property tree in this
     * object. 
     *
     * @param filename the name of the property file to be read
     * @throws SoDa::FileNotFound (a subclass of SoDa::Exception)
     * @throws SoDa::PropertyTree::BadSyntax
     */
    virtual void readFile(const std::string & filename) = 0;

    /**
     * @brief Write a property file
     *
     * Write the stuff in "filename" to the property tree in this
     * object. 
     *
     * @param filename the name of the file to be written
     * @throws SoDa::NoFileCreated (a subclass of SoDa::Exception)     
     * @throws SoDa::PropertyTree::BadSyntax
     */
    virtual void writeFile(const std::string & filename) = 0;
    
    /**
     * @brief dump the tree in a somewhat readable format. 
     *
     */
    std::ostream & dump(std::ostream & os); 
    
    std::list<std::string> getChildNames(const std::string & pathname, 
					bool throw_exception = false); 

    
    /**
     * @brief add a property (node) to the property tree
     *
     * @tparam T the value string will be coerced from this type.
     * The type *must* provide an ostream output function.
     *
     * @param pathname the heirarchical path name to the attribute.
     * Levels in the tree are separated with ":" as in "top:next:leaf"     
     * @param v a reference to the value we're storing. 
     * @param throw_exception throws an exception if the pathname is not
     * found or if the type of the value cannot be read from the property
     * string at that node.
     * @returns true if the property is found and the input value can
     * be converted to a string. 
     * @throws SoDa::PropertyTree::PropertyNotFound 
     * @throws SoDa::PropertyTree::BadPropertyType -- T has has no
     * ostream output function
     */
    template<typename T> bool put(const std::string & pathname, 
				  const T & v, 
				  bool throw_exception = false) {
      std::stringstream ss;
      ss << v;
      if(ss.fail()) {
	if(throw_exception) {
	  throw PropertyTree::BadPropertyType(" while setting " + pathname, typeid(v).name(), "");
	}
	else {
	  return false; 
	}
      }

      auto pathlist = makePathList(pathname);
      PropNode * pn = internalGet(&root, pathlist, true, pathname, throw_exception);

      if(pn == nullptr) {
	return false; 
      }
      else {
	pn->value = ss.str();
	return true; 
      }
    }

    /**
     * @brief Get the value from "pathname" and translate it
     * (if possible) into a value of a specified type.
     *
     * @tparam T the value string will be coerced into this type.
     * However, the type *must* provide an istream input function.
     *
     * @param pathname the location of the value
     * @param v reference to the value we'll set
     * @param throw_exception if true we'll throw an exception when
     * bad things happen. 
     * @returns true if we found a valid value. false if the value
     * string did not contain a value of type "T"
     * @throws SoDa::PropertyTree::BadPropertyType
     * @throws SoDa::PropertyTree::PropertyNotFound 
     */
    template<typename T> bool get(const std::string & pathname, 
				  T & v, 
				  bool throw_exception = false) {
      auto pathlist = makePathList(pathname);
      PropNode * pn = internalGet(&root, pathlist, false, 
				  pathname, 
				  throw_exception);

      if(pn == nullptr) {
	if(throw_exception) {
	  throw PropertyNotFound(pathname);	  
	}
	else {
	  return false; 
	}
      }

      std::stringstream ss(pn->value, std::ios_base::in);      
      ss >> v; 
      if(ss.fail()) {
	if(throw_exception) {
	  throw PropertyTree::BadPropertyType(pathname, typeid(v).name(), pn->value);
	}
	else return false; 
      }
      return true; 
    }


  protected:
    /**
     * @brief get a property node from the tree
     *
     * @param node a pointer to the "starting" property node
     * @param pathlist the heirarchical path name to the attribute.
     * Levels in the tree are separated with ":" as in "top:next:leaf"
     * Note that the pathlist will empty upon return. 
     * @param create build all the nodes on the path if they don't exist
     * @param orig_pathname the full pathname, used for signalling a "not found" exception. 
     * @param throw_exception throws an exception if the pathname is not
     * found or if the type of the value cannot be read from the property
     * string at that node.
     * @returns a pointer to a PropNode that can be used to get a list of properties, keys, or the value of this property. 
     * @throws SoDa::PropertyTree::PropertyNotFound 
     * @throws SoDa::PropertyTree::BadPropertyType
     */
    PropNode * internalGet(PropNode * node, 
			   std::list<std::string> & pathlist, 
			   bool create, 
			   const std::string & orig_pathname, 
			   bool throw_exception = false);

    std::list<std::string> makePathList(const std::string & pathname);

  private:
    
    std::ostream & recursiveDump(PropNode * pn, std::ostream & os, 
				 const std::string & pathname);
    
    class PropNode {
    public:
      /**
       * @brief create a key-value pair
       *
       * @param parent This node will be entered into the parent's
       * child vector at the end of the vector 
       * @param key the label for the value. If this is empty, the
       * newly created node will be treated as a list element. 
       * @param val duh
       */
      PropNode(const std::string & val = "") : value(val) { }

      std::string value; 
      std::map<std::string, PropNode*> children; 
    };

  public:
    class PropertyNotFound : public SoDa::Exception {
    public:	
      PropertyNotFound(const std::string & path); 
    }; 

    class BadPropertyType : public SoDa::Exception {
    public:	
      BadPropertyType(const std::string & path_name, 
		      const std::string & type_name, 
		      const std::string & val_string); 
    }; 

    class FileNotFound : public SoDa::Exception {
    public:
      FileNotFound(const std::string & fname);
    }; 

  protected:
    PropNode root; 
  };
    
}



