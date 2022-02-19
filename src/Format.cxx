#include "Format.hxx"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctype.h>
#include <regex>

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
  char Format::separator = '.';

  Format::FmtStringSeg::FmtStringSeg(const std::string & str) {
    seg_type = LITERAL;
    val = str; 
  }
  
  Format::FmtStringSeg::FmtStringSeg(unsigned int _idx) {
    seg_type = FMT_VAL;
    idx = _idx;
  }

  Format::Format(const std::string & fmt_string) {
    // save the format string
    orig_fmt_string = fmt_string; 
    // build the format list
    initialScan(fmt_string);
    // we're looking for the first argument
    cur_arg_number = 0;
  }


  void Format::initialScan(const std::string & fmt_string) {
    format_string_segments.clear();    
    // scan the format string
    enum ScanState { NORM, SAW_PC, ACC_FLDNUM };
    ScanState s_state = NORM;

    std::string cur_str;
    unsigned int cur_fldnum = 0; 
    for(auto c : fmt_string) {
      switch (s_state) {
      case NORM:
	if(c == '%') {
	  // we might be looking at a field specifier
	  s_state = SAW_PC;
	}
	else {
	  cur_str.push_back(c); 
	}
	break; 
      case SAW_PC:
	// are we looking at a field number?
	if(isdigit(c)) {
	  // we should push the current string
	  if(!cur_str.empty()) {
	    format_string_segments.push_back(FmtStringSeg(cur_str));
	    cur_str.clear();
	  }

	  // and indicate that we're scanning for the rest of the field number
	  s_state = ACC_FLDNUM; 
	  cur_fldnum = (unsigned int) (c - '0');
	}
	else {
	  // push a % and the current char onto the current string
	  cur_str.push_back('%');
	  // if the input was %%, we just send one %, otherwise, include the next char
	  if(c != '%') cur_str.push_back(c);
	  // and now we're normal
	  s_state = NORM;
	}
	break; 
      case ACC_FLDNUM:
	if(isdigit(c)) {
	  cur_fldnum = cur_fldnum * 10 + ((unsigned int) (c - '0'));
	}
	else {
	  // we scanned an fld, now push the fmt specifier
	  format_string_segments.push_back(FmtStringSeg(cur_fldnum));
	  // set the state
	  s_state = NORM;
	  // clear the current string
	  cur_str.clear();
	  // and save the character
	  cur_str.push_back(c); 
	}
	break; 
      }
    }

    switch (s_state) {
    case SAW_PC:
      // we got to end-of-string with a % at the end.
      cur_str.push_back('%');
      // fall through. 
    case NORM:
      if(!cur_str.empty()) {
	format_string_segments.push_back(FmtStringSeg(cur_str));
      }
      break;
    case ACC_FLDNUM:
      format_string_segments.push_back(FmtStringSeg(cur_fldnum));
      break; 
    }
  }

  Format & Format::addI(int v, unsigned int w) {
    std::stringstream ss;
    if(w != 0) {
      ss << std::setw(w); 
    }
    ss << v; 
    insertField(ss.str()); 
    return * this; 
  }

  Format & Format::addU(unsigned int v, char fmt, unsigned int w) {
    std::stringstream ss;
    if(fmt == 'x') ss << "0x"; 

    if(w != 0) {
      ss << std::setw(w); 
    }
    
    if(fmt == 'd') {
      ss << v; 
    }
    else {
      ss << std::setfill('0') << std::hex << v; 
    }
    insertField(ss.str());     
    return *this;     
  }
  
  Format & Format::addF(double v, char fmt, unsigned int width, unsigned int frac_precision) {
    std::stringstream ss;    
    
    switch (fmt) {
    case 'f':
      // fixed floating point format
      if(width) ss << std::setw(width); 
      ss << std::fixed << std::setprecision(frac_precision) << v;  
      break; 
    case 's':
      // scientific (who cares what the exponent is? format)
      if(width) ss << std::setw(width);       
      ss << std::scientific << std::setprecision(frac_precision) << v;
      break; 
    case 'g':
      // general (who cares what the exponent is format, or how this looks)
      ss.unsetf(std::ios::fixed | std::ios::scientific);
      if(width) ss << std::setw(width);             
      ss << std::setprecision(frac_precision) << v;  				    
      break; 
    case 'e':
      // now this is a tough one.
      // the object is to print this number as xxx.fffeN where N is a multiple of 3.
      // (engineering notation).  It is beyond me how C and C++ have continued to print
      // crap floating point formats that are only suited to imperial units (black and
      // white TV) or astronomers.  It is 2021 -- time to use the metric system, even
      // in backwaters that still cling to the 16th century.

      // it is possible that the value is 0.  if so, just jump skip the rest of this.
      if(v == 0.0) {
	ss << "   0" << separator << std::setw(frac_precision) << std::setfill('0') << 0 << "e0";
      }
      else {
	// get the sign
	bool is_neg = (v < 0.0);
	
	double av = fabs(v);
	// first find the log base 10.	
	double log_10 = log10(av);
	// now remember the sign
	double exp_sign_cor = (log_10 < 0.0) ? -1.0 : 1.0; 
	// truncate it toward 0
	log_10 = floor(fabs(log_10));
	double scale = pow(10, -1.0 * exp_sign_cor * log_10);
	// scale the "mantissa" part
	double mant = av * scale; 
	// now get the integer version of the exponent
	int ilog_10 = rint(log_10);
	// now we want to adjust ilog_10 until it is a multiple of 3
	// we're always going to *decrease* the exponent, so we should
	// always multiply the mantissa by 10
	ilog_10 = ilog_10 * ((exp_sign_cor < 0.0) ? -1 : 1);
	while(((ilog_10 % 3) != 0) || (mant < 1.0)) {
	  mant = 10.0 * mant;
	  ilog_10 -= 1;
	}
	// now we have a mantissa in the range 1 to 999.99...
	int whole = rint(floor(mant));
	int pow_mul = (int) (pow(10.0, frac_precision));
	int frac = rint((mant - floor(mant)) * pow(10.0, frac_precision));
	if(frac >= pow_mul) {
	  whole = whole + (frac / pow_mul);
	  frac = frac % pow_mul;
	}
	ss << (is_neg ? '-' : ' ') << std::setw(3) << whole << separator << std::setfill('0') << std::setw(frac_precision) << frac << 'e' << ilog_10 ; 
      }
    }

    insertField(ss.str());
    return *this;     
  }

  Format & Format::addS(const std::string & v, int width) {
    std::string fstr;
    if(width < 0)  {
      for(auto c : v) {
	fstr.push_back(c);
      }
      int spaces = (- width) - v.size();
      for(int i = 0; i < spaces; i++) {
	fstr.push_back(' ');
      }
      insertField(fstr);      
    }
    else if (width > 0) {
      int spaces = width - v.size();
      for(int i = 0; i < spaces; i++) {
	fstr.push_back(' ');
      }
      for(auto c : v) {
	fstr.push_back(c);
      }
      insertField(fstr);
    }
    else {
      insertField(v);
    }
    return *this;     
  }

  Format & Format::addC(char c) {
    std::stringstream ss;
    ss << c; 
    insertField(ss.str());
    return *this;
  }

  void Format::insertField(const std::string & s) {
    for (auto & fld : format_string_segments) {
      if((fld.seg_type == FmtStringSeg::FMT_VAL) && (fld.idx == cur_arg_number)) {
	fld.val = s; 
	fld.seg_type = FmtStringSeg::LITERAL; 
      }
    }

    cur_arg_number++;
  }

  
  Format & Format::reset() {
    initialScan(orig_fmt_string);
    cur_arg_number = 0; 
    return *this;
  }
  

  std::string Format::str(bool check_for_filled_out) const {
    std::string ret_string;
    // assemble the string, warts and all
    int unfilled_count = 0; 
    for(auto & fld : format_string_segments) {
      if(fld.seg_type == FmtStringSeg::LITERAL) {
	ret_string = ret_string + fld.val;
      }
      else if(fld.seg_type == FmtStringSeg::FMT_VAL) {
	std::stringstream ss;
	ss << '%' << fld.idx;
	ret_string = ret_string + ss.str();
	unfilled_count++; 
      }
    }

    if(check_for_filled_out && (unfilled_count != 0)) {
      throw BadFormat("Unfilled argument string [" + ret_string + "]", *this);
    }

    return ret_string; 
  }

  double Format::roundToSigDigs(double v, int sig_digits) {
    double ret = v;
    double significance_threshold = pow(10.0, sig_digits) - 0.5;
    double shift_correction = 1.0;

    if(ret > significance_threshold) {
      // divide down until we're ready to round
      while(ret > significance_threshold) {
	ret = ret * 0.1;
	shift_correction *= 10.0;
      }
    }
    else if(ret < significance_threshold) {
      while(ret < significance_threshold) {
	ret = ret * 10.0;
	shift_correction *= 0.1;
      }
      ret = ret * 0.1;
      shift_correction *= 10.0;
    }

    // now calculate the round increment.
    double fpart = ret - floor(ret);
    if(fpart >= 0.5) ret = ret + 0.5;
      
    ret = trunc(ret);

    return ret * shift_correction;
  }
}



std::ostream & operator<<(std::ostream & os, const SoDa::Format & f) {
  os << f.str(false);
  return os; 
}
