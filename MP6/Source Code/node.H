#include "utils.H"
#include "thread.H"


/* implement a queue */

class Node {
  
private:
  Thread* thread;
  Node* next;

public:
  Node(){
    thread = NULL;
    next = NULL;
  }

  Node(Thread* t){
    thread = t;
    next = NULL;
  }

  Node(Node& n){
    thread = n.thread;
    next = n.next;
  }

  void enqueue(Thread* t){
    // when the queue is empty
    if (thread == NULL){
      thread = t;
    }
    // when he queue is not empty
    else
      if (next == NULL){
        next = new Node(t); // when there is one node in the queue
      }else{
        next->enqueue(t); // when there is more than one node in the queue
      }
    
  }

  Thread* dequeue(){
    // when the queue is empty
    if (thread == NULL) {
      return NULL;
    }
    // when there are more than one node in the queue
    if (next != NULL) {
      Thread* poped = thread;
      thread = next->thread;
      Node* redundant = next;
      next = next->next;
      delete redundant;
      return poped;
    }
    //when there is only one node in the queue
    Thread* temp = thread;
    thread = NULL;
    return temp;

  }



};