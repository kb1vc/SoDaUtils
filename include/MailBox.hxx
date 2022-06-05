#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
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
 * @file MailBox.hxx
 * @author Matt Reilly (kb1vc)
 * @date Feb 21, 2022
 */

/**
 * @page SoDa::MailBox A buffer object that produces vectors
 * from a pool. 
 * 
 * 
 * ## Namespace
 * 
 * SoDa::MailBox is enclosed in the SoDa namespace because it is
 * inevitiable that there are lots of classes out there called
 * "BUffer."  Perhaps you have written one of them.  Naming a class "MailBox"
 * is like naming a street "Oak:" It might make lots of sense, but
 * you're going to have to reconcile yourself that there's a street 
 * with the same name one town over and sometimes your pizza is going
 * to get mis-routed. 
 * 
 * So MailBox is in the SoDa namespace.  SoDa is from 
 * <a href="https://kb1vc.github.io/SoDaRadio/">SoDaRadio</a> though the SoDa::MailBox 
 * class is a completely independent chunk 'o code. Most of the code I'll release
 * will be in the SoDa namespace, just to avoid the Oak Street problem.
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

  /**
   * @brief Catch this when you don't care why the MailBox threw an exception
   */
  class MailBoxException : public std::runtime_error {
  public:
    MailBoxException(std::string name, const std::string & problem) :
      std::runtime_error("SoDa::MailBox[" + name + "] " + problem) {
    }
  }; 
  
  /**
   * @brief The subscriber ID was invalid. 
   */
  class MailBoxMissingSubscriberException : public MailBoxException {
  public:
    MailBoxMissingSubscriberException(std::string name, const std::string & operation, int sub_id) :
      MailBoxException(name, "::" + operation + " Subscriber ID " + std::to_string(sub_id) + " not found.") {
    }
  }; 
  
  
  /**
   * @class MailBox<T>
   * @brief Accept messages and distribute them to multiple mailboxes
   *
   * @tparam T Type of message that will be found in this mailbox
   */

  template<typename T>
  class MailBox {
  public:
    /**
     * @brief Create a mailbox. All mailboxes can put and get 
     * messages from the mailbox. 
     *
     * Messages are assumed to be std::shared_ptr as a message pointer
     * will be released from the message queue when a subscriber takes
     * posesion of it.
     * 
     * Each subscriber gets  message queue.  It is up to the subscriber
     * to "read the mail"
     */
    MailBox(std::string name) : name(name) {
    }

    ~MailBox() {
      for(auto & s : message_queues) {
	while(!s.empty()) s.pop();
      }
      message_queues.clear();
    }
    
    /**
     * @brief Subscribe the caller to a mailbox.  There may be 
     * multiple subscribers to the same mailbox.  A copy of each
     * message will be reserved for each caller. 
     * 
     * @returns subscriber ID. 
     */
    int subscribe() {
      std::lock_guard<std::mutex> lock(mtx);	      
      // what will the subscriber number be? 
      int ret = message_queues.size();

      // make a subscriber queue
      message_queues.push_back(std::queue<std::shared_ptr<T>>());

      return ret; 
    }

    /**
     * @brief What is the name of this mailbox? 
     * @return the name.
     */
    const std::string & getName() const { return name; }

    /**
     * Get an object out of the mailbox for this subscriber
     * 
     * @param subscriber_id each user of a mailbox must have subscribed to the mailbox. 
     * (I know, we're mixing metaphors here... sigh.)
     * @returns The oldest object in the subscriber's mailbox. 
     */
    std::shared_ptr<T> get(int subscriber_id) {
      std::lock_guard<std::mutex> lock(mtx);	      
      if(message_queues.size() <= subscriber_id) {
	throw MailBoxMissingSubscriberException(this->name, "get()", subscriber_id);
      }
      else {
	if(message_queues[subscriber_id].empty()) {
	  return nullptr;
	}
	else {
	  auto ret = message_queues[subscriber_id].front();
	  message_queues[subscriber_id].pop();
	  return ret;
	}
      }
    }

    /**
     * @brief Place a message in every subscriber's mailbox
     *
     * @param msg The message to be sent to every subscriber. Note that this 
     * is passed by value -- each subscriber will get a copy.  Shared pointers
     * work just fine. 
     */
    void put(std::shared_ptr<T> msg) {
      std::lock_guard<std::mutex> lock(mtx);            
      for(auto & q : message_queues) {
	q.push(msg);
      }
    }

    /**
     * @brief Empty the subscriber's mailbox
     *
     * @param subscriber_id -- the owner. 
     */
    void clear(int subscriber_id) {
      std::lock_guard<std::mutex> lock(mtx);      
      if(message_queues.size() <= subscriber_id) {
	throw MailBoxMissingSubscriberException(this->name, "clear()", subscriber_id);	
      }
      else {
	while(!message_queues[subscriber_id].empty()) {
	  message_queues[subscriber_id].pop_front();
	}
      }
    }

  protected:
    std::string name;
    std::vector<std::queue<std::shared_ptr<T>>> message_queues; 

    // mutual exclusion stuff
    std::mutex mtx; 
  };

}



    
