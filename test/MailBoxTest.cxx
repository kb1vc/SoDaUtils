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
  // now wait
  while(waiters < waitcount) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return;
  
}

class MyMsg {
public:
  MyMsg(int from, int v) : from(from), v(v) {
  }

  ~MyMsg() { 

  }

  static std::shared_ptr<MyMsg> makeMsg(int from, int v) {
    return std::make_shared<MyMsg>(from, v);
  }

  int from, v;
};


int msgMailBoxTest(std::shared_ptr<SoDa::MailBox<MyMsg>> mailbox, int num_msgs, int num_threads) {
  //! [subscribe and wait]

  int me = mailbox->subscribe();

  barrier(num_threads);
  //! [subscribe and wait]  

  // push some messages
  //! [send messages]
  for(int i = 0; i < num_msgs; i++) {
    auto msg = MyMsg::makeMsg(me, i);
    mailbox->put(msg);
  }
  //! [send messages]

  // now look for messages
  int totmsgs = num_msgs * num_threads;
  // let's sum the content of all the messages
  int msg_sum = 0;
  int sender_sum = 0; 
  for(int i = 0; i < num_msgs * num_threads;) {
    //! [get message]
    auto p = mailbox->get(me);
    if(p != nullptr) {
      i++;
      msg_sum += p->v;
      if(0)
      {
	mtx.lock();
	std::cerr << SoDa::Format("from %0 to %1 v %2  sum %3\n")
	  .addI(p->from)
	  .addI(me)
	  .addI(p->v)
	  .addI(msg_sum);
	mtx.unlock();
      }

      sender_sum += p->from; 
      //std::this_thread::sleep_for(std::chrono::milliseconds(10));      
    }
    //! [get message]    
  }

  int N = num_msgs - 1; 
  int expected_sum = num_threads * (N * (N+1))/2;
  
  if(expected_sum != msg_sum) {
  std::cout << SoDa::Format("subscriber %0 got all %1 expected messages sum was %2 (should have been %4) sender sum was %3\n")
    .addI(me)
    .addI(totmsgs)
    .addI(msg_sum)
    .addI(sender_sum)
    .addI(expected_sum);
  }
  
  std::shared_ptr<MyMsg> p; 
  while((p = mailbox->get(me)) != nullptr) {
    std::cout << SoDa::Format("subscriber %0 got extra message from subscriber %1 : %2\n")
      .addI(me)
      .addI(p->v);
  }
  return 0; 
}

int vectorMailBoxTest(std::shared_ptr<SoDa::MailBox<std::vector<int>>> mailbox, int num_msgs, int num_threads) {
  //! [subscribe and wait]

  int me = mailbox->subscribe();

  barrier(num_threads);
  //! [subscribe and wait]  

  // push some messages
  //! [send messages]
  for(int i = 0; i < num_msgs; i++) {
    auto msg = std::make_shared<std::vector<int>>(100000);
    (*msg)[2] = me;
    (*msg)[3] = i; 
    mailbox->put(msg);
  }
  //! [send messages]

  // now look for messages
  int totmsgs = num_msgs * num_threads;
  // let's sum the content of all the messages
  int msg_sum = 0;
  for(int i = 0; i < num_msgs * num_threads;) {
    //! [get message]
    auto p = mailbox->get(me);
    if(p != nullptr) {
      i++;
      auto & rp = *p;
      msg_sum += rp[3]; 
    }
    //! [get message]    
  }

  std::cout << SoDa::Format("subscriber %0 got all %1 expected messages sum was %2\n")
    .addI(me)
    .addI(totmsgs)
    .addI(msg_sum);
  
  std::shared_ptr<std::vector<int>> p; 
  while((p = mailbox->get(me)) != nullptr) {
    std::cout << SoDa::Format("subscriber %0 got extra message from subscriber %1 : %2\n")
      .addI(me)
      .addI((*p)[2])
      .addI((*p)[3]);
  }
  return 0; 
}


int test1(int msg_count, int num_threads) {
  // we're going to use a really really simple barrier.   
  waiters = 0; 
  //! [create a mailbox]
  auto mailbox_p = SoDa::makeMailBox<std::vector<int>>("TestMail");
  //! [create a mailbox]
  
  std::cout << "Creating threads\n";
  
  std::list<std::thread *> threads;
  std::cout << SoDa::Format("Spawning %0 threads\n").addI(num_threads);
  std::cout.flush();

  //! [create threads]
  for(int i = 0; i < num_threads; i++) {
    threads.push_back(new std::thread(vectorMailBoxTest, 
				      mailbox_p, 
				      msg_count, 
				      num_threads));
  }
  //! [create threads]  
  std::cout << SoDa::Format("Waiting to join threads\n");
  std::cout.flush();
  
  for(auto t : threads) {
    t->join();
  }

  std::cout << "Joined all threads\n";

  return 0;
}

  
int test2(int msg_count, int num_threads) {
  // we're going to use a really really simple barrier. 
  waiters = 0;   
  //! [create a mailbox]
  auto mailbox_p = SoDa::makeMailBox<MyMsg>("MessageMailbox");
  //! [create a mailbox]
  
  std::cout << "Creating threads\n";
  
  std::list<std::thread *> threads;
  std::cout << SoDa::Format("Spawning %0 threads\n").addI(num_threads);
  std::cout.flush();

  //! [create threads]
  for(int i = 0; i < num_threads; i++) {
    threads.push_back(new std::thread(msgMailBoxTest,
				      mailbox_p, 
				      msg_count, 
				      num_threads));
  }
  //! [create threads]  
  std::cout << SoDa::Format("Waiting to join threads\n");
  std::cout.flush();
  
  for(auto t : threads) {
    t->join();
  }

  std::cout << "Joined all threads\n";

  return 0;
}

int main(int argc, char ** argv) {
  // create a mailbox
  SoDa::Options cmd;

  int msg_count, num_threads; 
  cmd.add<int>(&msg_count, "msgs", 'm', 1, "Number of messages to send from each thread")
    .add<int>(&num_threads, "th", 't', 2, "Number of threads in test.");

  if(!cmd.parse(argc, argv)) exit(-1);

  // std::cerr << "test 1\n";
  // test1(msg_count, num_threads);
  
  std::cerr << "test 2\n";
  test2(msg_count, num_threads);
}
