/*
Name: Hamza Alwan
ID: 1001638905
If you dont have g++: sudo apt install g++
To build: g++ thread.cpp -o thread -lpthread
To run: ./thread yourfile
yourfile is the file you want to check. dont put your file it wont work. unless your files name is yourfile. it 2am pls. i dont want to comment anymore
*/
#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <sys/time.h>
#include <vector>
#include <fstream>
#include <chrono>

#define MAX 5000000

int total = 0;

int n1, n2;
std::string s1;
std::string s2;


/*
name: readf
input: filename
output:returns 0 if works fine
modifes: s1 s2 n1 n2
use: sets string 1 and 2 and there sizes to n1 and n2 respectivl
how to get here: main calls it by it self (organicly)
*/
int readf(char *filename)
{
  std::ifstream myfile; 
  myfile.open(filename); //opens file
  if (myfile.is_open()) //if file is opened
  {
    getline(myfile, s1); //gets line 1 sets to  s1
    getline(myfile, s2); //gets line 2 sets to  s2
  }
  if (s1.empty()) //if s1 is not in something went wrong throw error and get out
  {
    printf("ERROR: Out of memory!\n");
    return -1; // get out
  }

  if (s2.empty()) //if s2 is not in something went wrong throw error and get out
  {
    printf("ERROR: Out of memory\n");
    return -1; // get out
  }

  /*read s1 s2 from the file*/

  n1 = s1.length(); /*length of s1*/
  n2 = s2.length(); /*length of s2*/

  if (s1.empty() || s2.empty() || n1 < n2) // if s1 is smaller than s2 some thing is wrong get out
  {
    return -1; 
  }

  return 0;// return 0 if everthing went well
}

/*
name: num_substring
input: sub-string from s1 called input 
output: nothing
modifes: total
use: finds all s2 in input 
how to get here: main calls it by it self (organicly)
*/
void num_substring(std::string input) //finds the number of subtrings in the string input  
{

  std::vector<size_t> postion; 
  size_t pos= input.find(s2,0); //find s2 in input from index 0
  while (pos!= std::string::npos) //for alll s2 in s1
  {
    total++; //if found adds
    postion.push_back(pos); //in case more than 1
    pos=input.find(s2,pos+1); //starts over from new point
    
  }
}


/*
name: main
input: argv which is the file name   
output: 0 if all works well 
modifes: none
use: finds all s2 in s1 
how to get here: organicly
*/
 int main(int argc, char *argv[])
{
   
  int count = 0; //sets the count to zero

  if (argc < 2)  //checks if file is passed in the arg line
  {
    printf("Error: You must pass in the datafile as a commandline parameter\n");
  }

  readf(argv[1]); //reads the file 
  

    
  int numberofthread=4;  //sets the nunmber of threads  

    auto start = std::chrono::high_resolution_clock::now(); //starts clock
    int r_point=0; //right side of the string we serch 
    int l_point=0; //left side of the string we serch
    int size = n1/numberofthread; //sets size to size of s1/ number of threrds
    int buffer= n2-1; //since the string is getting devided s2 might get devided into two sections so be put in a extra buffer fo that it gets the whole word
    /*
    eg:
    s1= xabcde
    s2:ab
    numthreads= 3
    it would get devided like   xa|bc|de therefor it would find 0 "ab"'s
    with a buffer of n2-1 it would get devided like
    t1:xab
    t2:bcd
    t3:de
    therfore all are found 
    */
   
    std::vector <std::thread> threads(numberofthread); //makes a vector of threads of length numberofthreads
    for(int i=0; i<numberofthread; i++) //for each thread
    {  
      if(i==numberofthread-1) //if last thread
      {
        l_point=r_point;
        int leftoversize= n1-l_point;
        threads[i]= std::thread(num_substring,s1.substr(l_point,leftoversize) );
      }
      else //for all threads but last
      {
        l_point=r_point; //make left point now right
        r_point= l_point+ size;// move up the right pointer up by size chars  
        threads[i]= std::thread(num_substring,s1.substr(l_point,size+buffer) ); //makes a thread and sends the string from left_point by size +buffer 
      }

    }
      for(auto &th : threads) // for all threads
      {
          th.join(); //wait for them to finish and then join them
      }

      
    auto end = std::chrono::high_resolution_clock::now(); //end time
    auto dura= std::chrono::duration_cast<std::chrono::microseconds>(end-start); //get duration
    printf ("\nThe number of substrings is : %d\n" , total) ; //print result
    std::cout<<"Elapsed time is : "<< dura.count() << "microseconds\n";//print time
    

   

  return 0; //safely exit
}
