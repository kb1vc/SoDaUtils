#pragma once
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

#include <string>
#include <memory>
#include <iostream>
#include "PropertyIO.hxx"

namespace SoDa {
  class Property; 
  
  class PropertyIO_JSON : public PropertyIO {
  public:
    PropertyIO_JSON(); 
    
    std::shared_ptr<Property> read(const std::string & in_filename);

    std::shared_ptr<Property> read(std::istream & is);

    void write(const std::shared_ptr<Property> & p, const std::string & out_filename);

    void write(const std::shared_ptr<Property> & p, std::ostream & os);

  protected:

    void privateWrite(const std::shared_ptr<Property> & p, std::ostream & os, 
		      bool prefix_comma = false, bool is_outer = false);    

    std::ostream &  printPrefixSpaces(std::ostream & os); 

    std::shared_ptr<Property>  prop_tree; 
    
    int depth; 
  };
}
