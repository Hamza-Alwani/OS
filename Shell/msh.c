/*
Name: Hamza Alwani
ID: 1001638905
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

//buffer size for input to keep everything consistant
#define buffer 360

// 2d array to store history
char history[15][buffer];
// array of ints to store pids
int pids[15];

/*
name:
input:
output: 
modifes: 
use: 
how to get here: 
*/

/*
name:display_histroy
input:none
output: none
modifes: none
use: used to print the the array : history
how to get here: by user typing "history"
*/
void dislay_history()
{

  int i = 0;                         //counter i.
  for (i = 0; i < 15; i++)           //iterates from 0 to 14
    if (strcmp(history[i], "") != 0) //doesnt print if it is empty
    {
      printf("%d: %s\n", i, history[i]); //prints non-empty strings
    }
}

/*
name:display_pids
input:none
output: none
modifes: none
use: used to print the the array : pids
how to get here: by user typing "listpids"
*/
void display_pids()
{
  int i = 0;               //counter i.
  for (i = 0; i < 15; i++) //iterates from 0 to 14
    if (pids[i] != 0)      //doesnt print if pid is 0
    {
      printf("%d: %d\n", i, pids[i]); //prints non-zero pid numbers
    }
}

/*
name:execute
input:char*arg[] where arg[0] is command name arg[1] to arg[n] are arguments like -a -b and arg[last]=NULL
output: int- child pid
modifes: none
use: gets a char *arg[] uses c function execvp to execute command like shell. uses forks and wait
how to get here: by user typing and shell commands like ls
*/
int execute(char *arg[])
{

  pid_t pid;    //creates pid
  pid = fork(); // starts for and saves value of child
  if (pid < 0)
  {
    //comes here if there is a error
    printf("A fork error has occurred.\n");
    exit(-1);
  }
  else if (pid == 0) /* We are in the child. */
  {
    //child fnction

    execvp(arg[0], arg); //runs the command line along with all arguments using execvp. where where arg[0] is command name arg[1] to arg[n] are arguments like -a -b and arg[last]=NULL
    /*  If execlp() is successful, we should not reach this next line. */
    printf("%s: Command not found.\n", arg[0]); //only reach here if command was not a real shell command
    fflush(NULL);                               //flush
    exit(EXIT_SUCCESS);                         //exit child
  }
  else /* We are in the parent. */
  {
    int status;
    // Force the parent process to wait until the child process
    // exits
    waitpid(pid, &status, 0); //waits for child
    fflush(NULL);             //flush
  }
  return pid; //returns pid of child set at line 75 by fork
}

/*
name:execute
input:char *input which is input by the user
output: none
modifes: history[]
use: takes the input and adds in to the history array like a queue 
how to get here: by user typing any command other than empty enter
*/
void add_history(char *input)
{
  int c;                   //
  for (c = 14; c > 0; c--) // itterteates from 14 to 1 and moves all items down the list one, getting rid of number 15 if avalialbe
  {
    strcpy(history[c], history[c - 1]); //moves all items down one to make space for ne input at the top history[0]
  }
  strcpy(history[0], input); //adds new input to the top of the list at history[0]
}

/*
name: prep
input: char *in this is the input typed in by the user
output: char**. this returns arg[] for execute(). where arg[0] is command name arg[1] to arg[n] are arguments like -a -b and arg[last]=NULL
modifes: char input and and return arg[] for execution 
use: used to prepare the input, tokenaizes and splits the string into usable content for execvp
how to get here: called by program if user's input is not  "!n" || "history" || "listpids" || "cd" || "quit" || "exit" || "\n" 
*/
char **prep(char *in)
{
  char *token;                         //creates a poiner called token
  char **args;                         //creates a 2d array (array of string) called args which will be returned later
  args = malloc(12 * sizeof(char **)); //gives arg size of 12 since first is command line and last is NULL remaining 10 are possible arguments
  token = strtok(in, " ");             //tokenaizes in at first avaible space. therfore ls -a would cause token to be ls
  args[0] = token;                     //arg[0]  is set as token which will be the command line like ls
  int counter = 1;                     //begins counter for for-loop starts at one since arg[0] is already set
  while (token != NULL)                //while there is something left
  {
    token = strtok(NULL, " "); //tokenize at space again if og input was ls -a token would now be -a. if it was ls -ab it would now be -ab a
    args[counter] = token;     //each argument is added to the args[] array.
    counter++;                 //counter is incremented to not override previos wirte.
  }
  args[counter] = NULL; //arg[] has to be null terminated for execvp
  return args;          //returns args noww ready for execvp
}

/*
name: start
input: char* input. input form user
output: int. 0 or 1; 1 for exit 0 for continue
modifes:  na
use: see what input is and decide what to do with it
how to get here: program come here natrually. 
*/
int start(char *input)
{
  //first case: if user inputs quit or exit
  if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) //if input is exit or quit
  {
    //return 1 so main can exit
    return 1; //exit condition
  }
  //case 2: if user enters cd
  //since cd is cannot be handled by execvp we have to handle it our self
  else if (input[0] == 'c' && input[1] == 'd') //cd
  {
    if (chdir(prep(input)[1]) != 0) //  use chdir. send the input into prep. since arg[0] would be cd arg[1] would be the directory we nee to go to
    {
      printf("%s: command not found\n", prep(input)[1]); //if failed we come here and say command not found
    }
    add_history(input); //no matter success or failure he we add to history
    return 0;           //return 0 so we dont exit in main
  }
  //case 3: user enters history
  else if (strcmp(input, "history") == 0) //if input == history
  {
    add_history(input); //first add to history
    dislay_history();   //call display history to print history
    return 0;           //return 0 so we dont exit in main
  }
  //case 4: Picking from history. if user enter !n n being a number from 0 to 14 we
  else if (input[0] == '!' && isdigit(input[1])) //if first char is ! and the second char is a digit
  {
    int number = input[1] - '0'; //get first char and conver it to int. since int number = input[1] return number plus 48 since its ascii
    if (isdigit(input[2]))       //if there is a second number since n can be from 0 to 15
    {
      number = number * 10; //multiply number by 10 and add second number and minus 48 since c garuntees 0-9 will be consecutive but not 10.
      number = number + input[2] - '0';
    }
    if (number > 15 || number < 0) //if number is greater than 14 or less than 0 it not possible hince history is from 0-14
    {
      printf("command not in history\n"); //we say not possible
    }
    else if (strcmp(history[number], "") != 0) //if  0<n<15 and possible we ececute
    {
      int r = execute(prep(history[number])); // send the history[number] to prep and send its return to execute; the execute returns a pid number we store it in r
      int i;                                  //counter
      for (i = 14; i > 0; i--)                //itterteates from 14 to 1 and moves all items down the list one, getting rid of number 15 if avalialbe
      {
        pids[i] = pids[i - 1]; //moves all items down one to make space for ne input at the top pids[0]
      }
      pids[0] = r; //new pid is added at head
      return 0;    //return 0 so we dont exit in main
    }
    else //if  0<n<15 but possible that history is not there yet so we say not possible
    {
      printf("command not in history\n");
    }
    add_history(input); //once done fail or success we add to history
    return 0;           //return 0 so we dont exit in main
  }
  //case 5: user enter listpids
  else if (strcmp(input, "listpids") == 0) //if input is listpids
  {
    display_pids();     // call display pids
    add_history(input); // add call to history
    return 0;           //return 0 so we dont exit in main
  }
  //case 6: if user's input is not  "!n" || "history" || "listpids" || "cd" || "quit" || "exit" || "\n"  then its ether a shell command or a worng commadn which will both be handled by eceute.
  else
  {
    int r = execute(prep(input)); //input is sent to prep. prep returns a arg[] which is sent to execute. execute returns r which is the pid of the procces.
    int i;                        //counter
    for (i = 14; i > 0; i--)      //itterteates from 14 to 1 and moves all items down the list one, getting rid of number 15 if avalialbe
    {
      pids[i] = pids[i - 1]; //moves all items down one to make space for ne input at the top pids[0]
    }
    pids[0] = r;        //new pid is added at head
    add_history(input); //adeed to history
    return 0;           //return 0 so we dont exit in main
  }
}

/*
name:main
input:none
output: int- returns o on exit
modifes: none
use: -
how to get here: on run naturally.
*/

int main()
{
  char input[buffer]; //create input string for user
  int i = 0;          // exit flag.
  while (i == 0)      //while 0 keep looping. only 1 when start returns 1 .
  {
    printf("msh> ");             //print msh>
    fgets(input, buffer, stdin); //gets user input.
    if (input[0] != '\n')        //if user input isnt empty. since fgets we als oget user's 'n' when they press enter
    {
      input[strcspn(input, "\n")] = 0; //since fgets we need to remove end "\n"
      i = start(input);                //take input and send to start. start returns 0 or 1. if one we exit. if 0 we keep going
    }
  }
  return 0; //return 0 on
}
