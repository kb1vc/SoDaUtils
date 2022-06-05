#include "../include/MailBox.hxx"
#include <memory>
#include "../include/Format.hxx"
#include "../include/Options.hxx"

#include <iostream>
#include <thread>
#include <functional>
#include <chrono>

std::mutex mtx;
int waiters = 0; 
void barrier(int waitcount) {
  // increment the waiters
  {
    mtx.lock();
    waiters++;
    mtx.unlock();
  }
  std::cerr << "." << waiters << "\n";
  // now wait
  while(waiters < waitcount) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return;
  
}

int test(std::shared_ptr<SoDa::MailBox<std::vector<int>>> mailbox, int num_msgs, int num_threads) {
    int me = mailbox->subscribe();

    barrier(num_threads);
    std::cerr << me << " out of barrier\n";
    
    // push some messages
    for(int i = 0; i < num_msgs; i++) {
      auto msg = std::make_shared<std::vector<int>>(1000);
      (*msg)[2] = me;
      (*msg)[3] = i; 
      mailbox->put(msg);
    }
  
    // now look for messages
    std::vector<int> msg_count(num_threads);
    int totmsgs = num_msgs * num_threads; 
    for(int i = 0; i < num_msgs * num_threads;) {
      auto p = mailbox->get(me);
      if(p != nullptr) {
	msg_count.at((*p)[2])++; 
	i++;
	std::cerr << SoDa::Format("me %0 other %1 msg %2\n")
	  .addI(me)
	  .addI((*p)[2])
	  .addI((*p)[3]);
      }
    }

    std::cout << SoDa::Format("subscriber %0 got all %1 expected messages\n")
      .addI(me)
      .addI(totmsgs);
  
    std::shared_ptr<std::vector<int>> p; 
    while((p = mailbox->get(me)) != nullptr) {
      std::cout << SoDa::Format("subscriber %0 got extra message from subscriber %1 : %2\n")
	.addI(me)
	.addI((*p)[2])
	.addI((*p)[3]);
    }
    return 0; 
  }


int main(int argc, char ** argv) {
  // create a mailbox
  SoDa::Options cmd;

  int msg_count, num_threads; 
  cmd.add<int>(&msg_count, "msgs", 'm', 1, "Number of messages to send from each thread")
    .add<int>(&num_threads, "th", 't', 2, "Number of threads in test.");

  if(!cmd.parse(argc, argv)) exit(-1);
  
  auto mailbox = std::make_shared<SoDa::MailBox<std::vector<int>>>("TestMailBox"); 
  

  std::cout << "Creating threads\n";
  
  std::list<std::thread *> threads;
  std::cout << SoDa::Format("Spawning %0 threads\n").addI(num_threads);
  std::cout.flush();
  
  for(int i = 0; i < num_threads; i++) {
    threads.push_back(new std::thread(test, mailbox, msg_count, num_threads));
  }
  
  std::cout << SoDa::Format("Waiting to join threads\n");
  std::cout.flush();
  
  for(auto t : threads) {
    t->join();
  }

  std::cout << "Joined all threads\n";
}
