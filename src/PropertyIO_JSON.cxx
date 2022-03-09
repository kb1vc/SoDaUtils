#include "PropertyIO_JSON.hxx"
#include "../external/include/json.hpp"
#include <stack>
#include "Property.hxx"
#include <iostream>
#include <fstream>

// This is inside baseball -- not for export to users.  This keeps
// the library from having to export the actual json reader header files.

// taken from https://github.com/nlohmann/json/blob/develop/README.md


namespace SoDa {

  using json = nlohmann::json;

  class SAX : public nlohmann::json_sax<json> {
  public:
    SAX(std::shared_ptr<SoDa::Property> prop_tree) : prop_tree(prop_tree) {
      property_stack.push(prop_tree); 
    }

    // called when null is parsed
    bool null() {
      return true; 
    }

    template<typename JT, typename T> bool setVal(JT v) {
      // we get here because we're setting a value.
      // we should have seen the key by now.
      auto cur_prop = property_stack.top();
      cur_prop->set(T(v));
      // now pop the stack.
      property_stack.pop();
      // how does it look?
      return true; 
    }

    // called when a boolean is parsed; value is passed
    bool boolean(bool val) {
      return setVal<bool, bool>(val);
    }

    // called when a signed or unsigned integer number is parsed; value is passed
    bool number_integer(json::number_integer_t val) {
      return setVal<json::number_integer_t, long>(val);    
    }

    bool number_unsigned(json::number_unsigned_t val) {
      return setVal<json::number_unsigned_t, long>(val);        
    }


    // called when a floating-point number is parsed; value and original string is passed
    bool number_float(json::number_float_t val, const string_t& s) {
      return setVal<json::number_float_t, double>(val);            
    }

    // called when a string is parsed; value is passed and can be
    // safely moved away
    bool string(json::string_t& val)  {
      return setVal<json::string_t, std::string>(val);            
    }

    // called when a binary value is parsed; value is passed and can
    // be safely moved away
    bool binary(json::binary_t& val) {
      // no idea what this is.
      return false; 
    }

    // called when an object or array begins or ends, resp. The
    // number of elements is passed (or -1 if not known)
    bool start_object(std::size_t elements) {
      // don't need to do anything???
      return true; 
    }

    bool end_object() {
      // pop the stack. 
      property_stack.pop();
      return true; 
    }
  
    // we don't support this right now
    bool start_array(std::size_t elements) {
      return false; 
    }

    bool end_array() {
      return false; 
    }
  
    // called when an object key is parsed; value is passed and can be safely moved away
    bool key(json::string_t& val) {
      // each time we see a key, we create a new property
      auto cur_prop = property_stack.top();
      auto new_prop = std::make_shared<SoDa::Property>(std::string(val), cur_prop);
      cur_prop->addChild(new_prop);
      property_stack.push(new_prop); 
      return true; 
    }

    // called when a parse error occurs; byte position, the last token, and an exception is passed
    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::detail::exception& ex) {
      std::cerr << "I have no idea what happened at position " << position 
		<< "\nBut the last token was [" << last_token << "]"
		<< "\nParser says: [" << ex.what() << "\n";
      return false; 
    }
  
    std::shared_ptr<SoDa::Property> prop_tree; 
    std::stack<std::shared_ptr<SoDa::Property>> property_stack; 
  };


  PropertyIO_JSON::PropertyIO_JSON() {
    depth = 0; 
  }
    
  std::shared_ptr<Property> PropertyIO_JSON::read(const std::string & in_filename) {
    std::ifstream istr(in_filename); 
    return read(istr);
  }

  std::shared_ptr<Property> PropertyIO_JSON::read(std::istream & is) {
    prop_tree = std::make_shared<PropertyTree>();    
    // now call sax parser.
    SAX parser(prop_tree);
    if(json::sax_parse(is, &parser)) {
      std::cerr << "Parsing worked!\n";
    }
    else {
      std::cerr << "Parser failed.  Wonder why?\n";
    }
    return prop_tree; 
  }

  void PropertyIO_JSON::write(const std::shared_ptr<Property> & p, const std::string & out_filename) {
    std::ofstream os(out_filename);
    write(p, os); 
    return; 
  }

  void PropertyIO_JSON::write(const std::shared_ptr<Property> & p, std::ostream & os) {
    privateWrite(p, os, false, true);
    os << "\n";
    return; 
  }

  void PropertyIO_JSON::privateWrite(const std::shared_ptr<Property> & p, std::ostream & os, bool prefix_comma, bool is_outer) {
    if(!is_outer) {
      if(prefix_comma) os << ",\n";

      // first print the value, if any.
      printPrefixSpaces(os);

      // print our name. 
      os << "\"" << p->getName() << "\" : ";
      os.flush();
    }
    
    // do we have a value? 
    if(p->hasValue()) {
      if(p->get().getType() == Property::Value::PrimitiveType::STRING) {
	os << "\""; 
	p->get().print(os); 
	os << "\"";
      }
      else {
	p->get().print(os);
      }
    }
    else {
      os << "{\n";
      depth = depth + 1;     
      bool pfx_comma = false; 
      for(auto & n : p->getChildNames()) {
	privateWrite(p->getProperty(n, false), os, pfx_comma, false); 
	pfx_comma = true; 
      }
      depth = depth - 1; 
      os << "\n";
      printPrefixSpaces(os) << "}";
    }
    return; 
  }
  
  std::ostream &  PropertyIO_JSON::printPrefixSpaces(std::ostream & os) {
    for(int i = 0; i < depth; i++) {
      os << "  "; 
    }
    os.flush();
    
    return os;
  }
}
