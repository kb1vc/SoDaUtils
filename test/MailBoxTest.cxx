#include "../include/MailBox.hxx"
#include <memory>
#include "../include/Format.hxx"
#include "../include/Options.hxx"
#include "../include/Barrier.hxx"

#include <iostream>
#include <thread>
#include <functional>
#include <chrono>

std::mutex mtx;

class MyMsg {
public:
  MyMsg(int from, int v) : from(from), v(v) {
    std::lock_guard<std::mutex> lock(mtx);
    tot_active++; 
  }

  ~MyMsg() { 
    std::lock_guard<std::mutex> lock(mtx);
    tot_active--; 
  }

  static std::shared_ptr<MyMsg> makeMsg(int from, int v) {
    return std::make_shared<MyMsg>(from, v);
  }

  static unsigned int tot_active;
  int from, v;
};

unsigned int MyMsg::tot_active = 0;

int objMailBoxTest(std::shared_ptr<SoDa::MailBox<MyMsg>> mailbox_p, 
		   int num_msgs, int num_threads, 
		   std::shared_ptr<SoDa::Barrier> barrier_p) {
  //! [subscribe and wait]
  int me = mailbox_p->subscribe();
  barrier_p->wait();
  //! [subscribe and wait]  

  // push some messages
  //! [send messages]
  for(int i = 0; i < num_msgs; i++) {
    auto msg = MyMsg::makeMsg(me, i);
    mailbox_p->put(msg);
  }
  //! [send messages]

  // now look for messages
  int totmsgs = num_msgs * num_threads;
  // let's sum the content of all the messages
  unsigned long msg_sum = 0;
  unsigned long sender_sum = 0; 
  for(int i = 0; i < num_msgs * num_threads;) {
    //! [get message]
    auto p = mailbox_p->get(me);
    if(p != nullptr) {
      i++;
      msg_sum += p->v;
      sender_sum += p->from;
    }
    //! [get message]    
  }

  unsigned long N = num_msgs - 1; 
  unsigned long expected_sum = num_threads * (N * (N+1))/2;
  
  if(expected_sum != msg_sum) {
    std::cerr << SoDa::Format("subscriber %0 got all %1 expected messages sum was %2 (should have been %4) sender sum was %3\n")
    .addI(me)
    .addI(totmsgs)
    .addU(msg_sum)
    .addU(sender_sum)
    .addU(expected_sum);
    return -1;
  }
  
  std::shared_ptr<MyMsg> p; 
  while((p = mailbox_p->get(me)) != nullptr) {
    std::cerr << SoDa::Format("subscriber %0 got extra message from subscriber %1 : %2\n")
      .addI(me)
      .addI(p->v);
    return -1; 
  }
  return 0; 
}

int vectorMailBoxTest(std::shared_ptr<SoDa::MailBox<std::vector<int>>> mailbox_p, 
		      int num_msgs, int num_threads,
		      std::shared_ptr<SoDa::Barrier> barrier_p) {
  //! [vec subscribe and wait]
  int me = mailbox_p->subscribe();
  barrier_p->wait();
  //! [vec subscribe and wait]  

  // push some messages
  //! [vec send messages]
  for(int i = 0; i < num_msgs; i++) {
    auto msg = std::make_shared<std::vector<int>>(100000);
    (*msg)[2] = me;
    (*msg)[3] = i; 
    mailbox_p->put(msg);
  }
  //! [vec send messages]

  // now look for messages
  int totmsgs = num_msgs * num_threads;
  // let's sum the content of all the messages
  int msg_sum = 0;
  for(int i = 0; i < num_msgs * num_threads;) {
    //! [vec get message]
    auto p = mailbox_p->get(me);
    if(p != nullptr) {
      i++;
      auto & rp = *p;
      msg_sum += rp[3]; 
    }
    //! [vec get message]    
  }

  std::cerr << SoDa::Format("subscriber %0 got all %1 expected messages sum was %2\n")
    .addI(me)
    .addI(totmsgs)
    .addI(msg_sum);
  
  std::shared_ptr<std::vector<int>> p; 
  while((p = mailbox_p->get(me)) != nullptr) {
    std::cerr << SoDa::Format("subscriber %0 got extra message from subscriber %1 : %2\n")
      .addI(me)
      .addI((*p)[2])
      .addI((*p)[3]);
  }
  return 0; 
}


int testVectorMsg(int msg_count, int num_threads) {
  //! [t1 create a mailbox]
  auto mailbox_p = SoDa::makeMailBox<std::vector<int>>("TestMail");
  //! [t1 create a mailbox]

  //! [t1 create a barrier]
  // each thread will wait at the barrier until everyone has finished
  // subscribing.
  auto barrier_p = SoDa::makeBarrier("subscription barrier", num_threads);
  //! [t1 create a barrier]  
  
  std::cerr << "Creating threads\n";
  
  std::list<std::thread *> threads;
  std::cerr << SoDa::Format("Spawning %0 threads\n").addI(num_threads);

  //! [t1 create threads]
  for(int i = 0; i < num_threads; i++) {
    threads.push_back(new std::thread(vectorMailBoxTest, 
				      mailbox_p, 
				      msg_count, 
				      num_threads, 
				      barrier_p));
  }
  //! [t1 create threads]  
  std::cerr << SoDa::Format("Waiting to join threads\n");
  
  for(auto t : threads) {
    t->join();
  }

  std::cerr << "Joined all threads\n";

  return 0;
}

  
int testObjMessage(int msg_count, int num_threads) {
  //! [create a mailbox]
  auto mailbox_p = SoDa::makeMailBox<MyMsg>("MessageMailbox");
  //! [create a mailbox]
 
  //! [create a barrier]
  // each thread will wait at the barrier until everyone has finished
  // subscribing.
  auto barrier_p = SoDa::makeBarrier("subscription_barrier", num_threads);
  //! [create a barrier]  
  
  std::cerr << "Creating threads\n";
  
  std::list<std::thread *> threads;
  std::cerr << SoDa::Format("Spawning %0 threads\n").addI(num_threads);

  //! [create threads]
  for(int i = 0; i < num_threads; i++) {
    threads.push_back(new std::thread(objMailBoxTest,
				      mailbox_p, 
				      msg_count, 
				      num_threads,
				      barrier_p));
  }
  //! [create threads]  
  std::cerr << SoDa::Format("Waiting to join threads\n");
  
  for(auto t : threads) {
    t->join();
  }

  std::cerr << "Joined all threads\n";

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
  // testVectorMsg(msg_count, num_threads);
  
  std::cerr << "test 2\n";
  testObjMessage(msg_count, num_threads);
  
  if(MyMsg::tot_active > 0) {
    std::cerr << "There may be a leak in allocating messages: " << 
      MyMsg::tot_active << " still outstanding.\n";
    exit(-1);
  }
}
