#include <iostream>
#include <string>
#include <pthread.h>
using namespace std;
struct node
{
  int ID;
  int size;
  int index;
  node *next;
  node() : next(NULL) {}
  node(int _ID, int _size, int _index, node* _next) : ID(_ID), size(_size), index(_index), next(_next) {}
};
class linkedList
{
  public:
  linkedList() : head(NULL) {}
  ~linkedList()
  {
    node * itr = head;
    while(itr != NULL)
    {
      node * temp = itr;
      itr = itr->next;
      delete temp;
    }
  }
  bool isEmpty()
  {
    if(head == NULL)
    {return true;}
    else
    {return false;}
  }
  node * find(int index) // for id=-1
  {
    node* itr = head;
    node* ptr;
    while(itr != NULL &&itr->next != NULL && (itr->index <= index || itr->ID != -1)) //until find requested node
    {
      ptr = itr;
      itr= itr->next;
    }
    return itr;
  }
  node *find(int id, int index)
  {
    node* itr = head;
    while(itr!= NULL && itr->next != NULL && (itr->index != index || itr->ID != id)) //until find requested node
    {
      itr= itr->next;
    }
    return itr;
  }
  void insert(int id, int size, int index)
  {
    if(isEmpty())
    {
      head = new node(id, size, index, NULL);
      return;
    }
    node* itr = head;
    while(itr->next != NULL && itr->next->index < index)
    {
      itr=itr->next;
    }
    if(itr == head && itr->ID == -1)
    {
      head = new node(id, size, index, head);
      return;
    }
    itr->next = new node(id, size, index, itr->next);
    
  }
  void remove(int index)
  {
    if(!isEmpty())
    {
      node* itr = head;
      if(itr->index == index)
      {
        head = head->next;
        delete itr;
        return;
      }
      while(itr->next != NULL && itr->next->index != index)
      {
        itr=itr->next;
      }
      //found
      if(itr->next !=NULL)
      {
        node * temp = itr->next;
        itr->next = itr->next->next;
        delete temp;
      }
    }
  }
  void print()
  {
    node * itr = head;
    while(itr != NULL)
    {
      cout << "[" << itr->ID << "][" << itr->size << "][" << itr->index << "]";
      if(itr->next != NULL)
      {
        cout << "___";
      }
      itr=itr->next;
    }
    cout << endl;
  }
  node * getHead()
  {
    return head;
  }
  private:
  node * head;
  
};
class HeapManager
{
  public:
  HeapManager() {}
  ~HeapManager() {}
  int initMemory(int size)
  {
    if(size>0)
    {
      //initialize heap as requested size
      list.insert(-1, size, 0);
      listSize = size;
      pthread_mutex_init(&mutex, NULL);
      //print initial heap layout
      return 1;
    }
  }
  int myMalloc(int ID, int inputSize)
  {
    pthread_mutex_lock(&mutex);
    node * candidate = list.find(-1);
    while(candidate != NULL && candidate->size < inputSize)
    {
      candidate = list.find(candidate->index);
    }
    if(inputSize > list.getHead()->size)
    {
      cout << "Can not allocate, requested size "<< inputSize<< " for thread "<< ID <<" is bigger than remaining size\n";
      print();
      pthread_mutex_unlock(&mutex);
      return -1;
    }
    if(candidate == NULL)
    {
      //no candidate
      pthread_mutex_unlock(&mutex);
      return -1;
    }
    int address = candidate->index;
    list.insert(ID, inputSize, address);
    list.find(address-1)->size = list.find(address-1)->size - inputSize;
    list.find(address-1)->index = list.find(address-1)->index + inputSize;
    int searchIndex = address+ inputSize -1;
    if(list.find(searchIndex)->size == 0)
    {
      list.remove(searchIndex+1);
    }
    cout << "Allocated for thread "<< ID << endl;
    print();
    pthread_mutex_unlock(&mutex);
    return address;
  }
  int myFree(int ID, int inputIndex)
  {
    pthread_mutex_lock(&mutex);
    if(list.find(ID, inputIndex)==NULL)
    {
      //cannot freed thread
      pthread_mutex_unlock(&mutex);
      return -1;
    }
    list.find(ID, inputIndex)->ID = -1;
    node * itr = list.getHead();
    while(itr->next != NULL)
    {
      if(itr->ID == -1 && itr->next->ID == -1)
      {
        itr->size = itr->size + itr->next->size;
        list.remove(itr->next->index);
      }
      else
      {
        itr= itr->next;
      }
    }
    cout << "Freed for thread "<< ID<< endl;
    print();
    pthread_mutex_unlock(&mutex);
    return 1;
    
  }
  void print()
  {
    list.print();
  }
  private:
  linkedList list;
  int listSize;
  pthread_mutex_t mutex;
};

