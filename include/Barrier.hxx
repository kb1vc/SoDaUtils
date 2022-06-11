#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "NoCopy.hxx"
#include "Exception.hxx"
#include "Format.hxx"

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

/**
 * @file Barrier.hxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 21, 2022
 */

/**
 * @page SoDa::Barrier A simple std::thread barrier 
 *
 *
 */

/**
 * @namespace SoDa
 * 
 * Not much else is spelled that way, so we're probably not going to
 * have too many collisions with code written by other people.
 *
 * SoDa is the namespace I use in code like
 * <a href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> (The S
 * D and R stand for Software Defined Radio.  The o,a,d,i,o don't
 * stand for anything in particular.)  But this code has nothing to do
 * with software defined radios or any of that stuff.
 */
namespace SoDa {

  
  
  /**
   * @class Barrier
   * @brief All threads connected with a barrier will wait on calling
   * "wait". All will be awakened when all have called wait. 
   *
   */

  class Barrier : public NoCopy {
  public:
    /**
     * @brief constructor 
     * @param name Name of the barrier
     * @param num_waiters number of threads that will wait at this barrier.
     */    
    Barrier(const std::string & name, unsigned int num_waiters);

    class Timeout : public Exception {
    public:      
      Timeout(Barrier & b, 
	      const std::chrono::duration<long, std::milli> & timeout) :
	Exception(SoDa::Format("Barrier %0 timed out after %1 ms. Current state is dangerous\n")
		  .addS(b.name)
		  .addU(timeout.count()).str()) {
      }
    }; 

    class Corrupt : public Exception {
    public:
      Corrupt(Barrier & b) :
	Exception(SoDa::Format("Barrier %0 is corrupted.\n")
		  .addS(b.name).str()) {

      }
    }; 

    /**
     *
     * @brief wait for all waiters to arrive at the barrier. 
     *
     * @param timeout_ms thrown an exception if "timeout_ms" has
     * expired without all reaching the barrier.
     * 
     * If timeout_ms is zero, the timeout will be set to about 1000
     * days.  This is not equal to std::chrono::duration::max() as,
     * for some reason, condition_value::wait_for will trigger
     * immediately. I think it is treating count() as a signed
     * integer.. dunno.
     * 
     * @throws Barrier::Timeout if we reach the timeout limit
     * @throws Barrier::Corrupt if the barrier state is corrupted. 
     */
    void wait(unsigned long timeout_ms = 0);

    /**
     *
     * @brief wait for all waiters to arrive at the barrier. 
     *
     * The timeout should be chosen carefully. If any one of the threads
     * at a barrier "times out" then all threads will eventually time out. 
     *
     * @param timeout wait will throw an exception if "timeout" has
     * expired without all reaching the barrier.  Timeout is specified
     * as a chrono::duration. 
     * 
     * If timeout is zero, the timeout will be set to about 1000
     * days.  This is not equal to std::chrono::duration::max() as,
     * for some reason, condition_value::wait_for will trigger
     * immediately. I think it is treating count() as a signed
     * integer.. dunno.
     * 
     * @return true if all reached the barrier
     * before the timeout, false otherwise.
     */
    void wait(const std::chrono::duration<long, std::milli> & timeout); 

  private:
    std::string name;
    unsigned int num_waiters;
    unsigned int waiting;
    unsigned int barrier_count; 

    bool corrupted; 

    // mutex and condition variable
    std::mutex mtx; 
    std::condition_variable cv; 
  };


    /**
     * @brief Make a barrier and return a shared pointer to it. 
     *
     * This is a safer way of creating barrier, as it ensures that
     * the barrier will live even after it "goes out of scope" in the
     * thing that created the it and spawned the threads that use
     * it. Sure we could rely on good behavior, but why? 
     *
     * @param name Name of the barrier
     * @param num_waiters number of threads that will wait at this barrier.
     * @returns shared pointer to a barrier object
     */ 
  std::shared_ptr<Barrier> makeBarrier(const std::string & name, unsigned int num_waiters);
}



    
