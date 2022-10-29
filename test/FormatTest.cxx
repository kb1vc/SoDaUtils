#include "../include/Format.hxx"
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <random>

template <typename T> bool badDiff(T val, T newval, int p) {
  T diff = val - newval;
  diff = (diff > 0.0) ? diff : -diff;

  T norm = 5 * pow(10.0, T(-p)); // accommodate rounding? 

  T err = diff / val;

  bool flag = err > norm;

  if(flag) {
    std::cerr << "err = " << err << " diff = " << diff << " norm = " << norm << " val = " << val << "\n";
  }

  return flag;
  
}

template <typename T> bool checkVal(T val, const std::string & fmtstr, int precision) {
  std::stringstream ss(fmtstr);

  T newval;

  ss >> newval;

  bool ret = badDiff(val, newval, precision);
  if(ret) {
    std::cerr << val << " came out as " << newval << " string (" << fmtstr << ")\n";
  }

  return ret;
}

bool doTest(double v, int p, int w, char fmt) {
  std::string fs = SoDa::Format("%0").addF(v, fmt, w, p).str();
  //  std::cerr << fs << "]\n";
  return checkVal<double>(v, fs, p);
}

int runBigTest() {

  std::uniform_real_distribution<double> unif(0, 1.1);
  std::default_random_engine re;
  int errcount = 0;
  
  double test_v = 64.0;
  std::string formats("esg");
  int itcount = 50000; // 100000; 
  for(int i = 0; i < itcount; i += 13) {
    double test_iv = i; 
    for(double e = -18; e < 18; e++) {
      double v = test_v * pow(10.0, e);
      double iv = test_iv * pow(10.0, e);
      for(int p = 0; p < 5; p++) {
	for(int w = 0; w < 10; w++) {
	  for(auto fmt : formats) {
	    bool vt = false;
	    bool ivt = false;
	    if((vt = doTest(v, p, w, fmt)) || (ivt = doTest(iv, p, w, fmt))) {
	      errcount++;
	      std::cerr << "vt = " << ((char) (vt ? 'T' : 'F')) << " ivt = " << ((char) (ivt ? 'T' : 'F')) << " ";
	      std::cerr << "e = " << e << " v = " << v << " iv = " << iv << " p = " << p << " w = " << w << " fmt = " << fmt << "\n";
	      if(errcount > 10) {
		std::cerr << "Too many errors, time to quit.\n";
		exit(-1);
	      }
	    }
	  }
	}
      }
    }      
    test_v = unif(re);
    if(i % 1000 == 0) {
      std::cerr << i << "\n";
    }
  }
  std::cerr << "Done\n"; 

  return errcount;
}

int main(int argc, char * argv[]) {

  std::cerr << SoDa::Format("This is what 0 looks like F[%0] U[%1] I[%2] U16[%3] \n")
    .addF(0.0, 'e', 0, 0)
    .addU(0)
    .addI(0)
    .addU(0, 'X');

  std::cerr << SoDa::Format("Testing adjacent markers [%0%1] should be [foobar]\n")
    .addS("foo")
    .addS("bar");
  
  if(argc < 5) {
    int rval; 
    try {
      rval = runBigTest(); 
    }
    catch(SoDa::Exception & e) {
      std::cerr << e.what() << "\n";
      exit(-1);
    }
    if(rval > 0) {
      exit(-1);
    }
    else {
      exit(0);
    }

  }
  else {
    double v = atof(argv[1]);
    int p = atoi(argv[2]);
    int w = atoi(argv[3]);
    char fmt = argv[4][0];
    std::cerr << "Debug test v = " << v << " p = " << p << " w = " << w << " fmt = " << fmt << "\n";

    doTest(v, p, w, fmt);
  }
}
