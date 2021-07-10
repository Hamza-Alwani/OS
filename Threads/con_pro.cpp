/*
Name: Hamza Alwan
ID: 1001638905
If you dont have g++: sudo apt install g++
To build: g++ con_pro.cpp  -o con_pro -lpthread
To run: ./con_pro
*/
#include <condition_variable> // std::condition_variale
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
using namespace std;

/*
Use condition variables to implement the producer-consumer algorithm. 
Assume two threads: one producer and one consumer. The producer reads characters one by one from a string stored in a file named “message.txt”, 
then writes sequentially these characters into a circular queue. Meanwhile, the consumer reads sequentially from the queue and prints them in the same order. 
Assume a buffer (queue) size of 5 characters. 
Write a threaded program using condition variables.
*/

std::mutex g_mutex; //mutex
std::condition_variable conditonal_var; //conditional var
ifstream myfile; //file
bool fineshed = false; //used for locking and waiting
bool keepgoing=true; //used for consumer 

int pointer=0; //points to the postion in then queue
char que[5]; //circular queue


/*
name:  
input: none
output: none
modifes: myfile
use: opens myfile
how to get here: main calls it by it self (organicly)
*/

/*
name:  open_file
input: none
output: none
modifes: myfile
use: opens myfile
how to get here: main calls it by it self (organicly)
*/
int  open_file() //used to open file
{
   myfile.open("message.txt");
   if (myfile.is_open()) //if file is opened
      {
        return 0;
      }
      std::cout<<"file \"message.txt\" not found\n";
    return -1;
  
}

/*
name:  push
input: char c. the thing thats geting pushed onto the que
output: none
modifes: que. adds a char to the que
use: adds a char to the que
how to get here: main calls it but it self (organicly)
*/
void push(char c)
{
  if(pointer>=5) // if pointer is over or equal to 5 reset count
  {
    pointer=0; //resets pointer to 0; 
  }
  que[pointer]=c; //sets c to index pointer 
}

/*
name:  pop
input: int data=> the index of the que that is to be popped
output: temp =>char that just got popped
modifes: que. removes a char from the que
use:  removes a char from the que and returns it
how to get here: main calls it by it self (organicly)
*/
char pop(int data)
{
  char temp=que[data]; //temporaraly holds index
  que[data]='-'; //deletes index by replacing data with a "-"
  return temp; //returns temp
}

/*
name:  consumer
input: none
output: none
modifes: conditonal_var fineshed
use: pops(pointer -1)  and calls conditional varriable
how to get here: consumerThread calls it by it self (organicly)
*/
void consumer() {
    std::unique_lock<std::mutex> ul(g_mutex); //makes a lock called ul
    conditonal_var.wait(ul, []() { return fineshed; }); //waits for producver to finish.
    cout<<pop(pointer-1)<<"\n"; //pops 1 less than the golbal pointer
    fineshed = false; //sets fineshed to false for producer
    ul.unlock(); //unlocks ul
    conditonal_var.notify_one (); //conditonal_var is notifies producer
}


/*
name: producer 
input: char p. => the char that is sent by producerthread();
output: none
modifes: pointer.  fineshed  ul and conditonal_var
use: pushes char p. incerments pointer and calls conditional varriable
how to get here: producerThread calls it by it self (organicly)
*/
void producer(char p) //reads chars from file char by char and sets to buffer 
{
    std::unique_lock<std::mutex> ul(g_mutex);  //makes a lock called ul
    push(p); // pushes the char it got from producerThread to the circular queue
    pointer++; // add 1 to pointer 
    fineshed = true; // sets fineshed to ture so that consumer can start
    ul.unlock(); // unlocks ul for consumer 
    conditonal_var.notify_one(); //conditonal_var is notifies consumer
    ul.lock(); //ul gets locked 
    conditonal_var.wait(ul, []() { return fineshed == false; }); //waits for conusemer to reutrn false so that producer can start again.
}

/*
name:  consumerThread
input: none
output: nonenone
use: calls consumer() while keepgoing is ture
how to get here: main calls it by it self (organicly)
*/
void consumerThread() 
{
  while(keepgoing) //while keepgoing is ture 
    consumer(); //calls conusmer
}


/*
name:  producerThread
input: none
output: none
modifes:keepgoing => sets it to false on last iteration so consumer knows to stop
use: pulls data from file and sends it to producer() for each char in file
how to get here: main calls it by it self (organicly)
*/
void producerThread() 
{
 char temp; //temp will hold each char as it gets pulled from file
 while(myfile.get(temp)) //while there is something (char) left in the file
 {
   int c= myfile.peek(); //checks next char in the file
  if(c==EOF) //if the next char is EOF (end of file)
  {
    keepgoing=false; // change keepgoing flag t0 flase to stop consumer thread
  }
   producer(temp); //send char to producer function.
  
 }
}
    



/*
name:  main
input: none
output: 0 if all works well 
modifes: none
use: makes a consumer and a producer thread and waits for them to end and join. ALso opens and closes file
how to get here: organicly
*/
int main() {
 
  if(open_file()==-1) //opens  massage file
    return -1;
  
  std::thread t1(consumerThread); // creates consumer threads 
  std::thread t2(producerThread); // creates producer threads
  t2.join(); //waits for producer thread to finish and join
  t1.join(); //waits for consumer  thread to finish and join
  myfile.close(); //closes file
  return 0; //edn program
}





