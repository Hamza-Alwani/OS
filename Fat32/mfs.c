// The MIT License (MIT)
//
// Copyright (c) 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/*
HAMZA ALWANI
1001638905
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#define MAX_NUM_ARGUMENTS 4

#define WHITESPACE " \t\n" // We want to split our command line up into tokens \
                           // so we need to define what delimits our tokens.   \
                           // In this case  white space                        \
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

int16_t BPB_BytesPerSec;
int16_t BPB_SecPerClus;
int16_t BPB_RsvdSecCnt;
int16_t BPB_NumFATS;
int16_t BPB_FATSz32;

/*
TODO:
OPEN  -done 
CLOSE -done
info  -done
STAT  -done
get   -done
cd    -done
ls    -done
read  -done

*/

struct __attribute__((packed)) DirectoryEntry
{
  char DIR_Name[11];
  uint8_t DIR_Attr;
  uint8_t Unused1[8];
  uint16_t DIR_FirstClusterHigh;
  uint8_t Unused2[4];
  uint16_t DIR_FirstClusterLow;
  uint32_t DIR_FileSize;
};

struct DirectoryEntry dir[16]; //array of directorys
int32_t root; //perma holds root
int32_t curdir; //hold current dir

int LBAToOffset(int32_t sector)
{ 
  //took from presentation
  if (sector == 0) //if secotor is 0 make it two due to the exepction
    sector = 2;
  return ((sector - 2) * BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt) + (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec); //got from the pres.
}
/*
in: needle= name of the file we are looking for
out: index of where needle is in the dir array. 
does://given a name of the directory it returns its index in the dir array
*/
int get_DIR_num(char *needle) 
{
  int i = 0;
  char filename[12]; //soters file names given from dir
  char in[12]; //sotrs the needle.

  int k = 30; //k is a flag to see if incase index isnt found
  for (int i = 0; i < 16; i++)
  {
    strncpy(&filename[0], &dir[i].DIR_Name[0], 11); //copy each name to filename
    filename[11] = '\0'; //make it null terminated 

  
    if(needle== NULL) //if needle is empty
    {
      return -1;

    }
    strncpy(&in[0], needle, 11); //copy the needle 

    int ln = strlen(in); 
    if ((ln > 0) && (in[ln - 1] == '\n')) //make the needle null terminated
      in[ln - 1] = '\0';

    
    else if (strncmp(in, filename, ln) == 0) // check if equal
    {
      k = i; // sets k as i to switch of flag
      return i; // if they are equal return 
    }
  }

  if (k == 30) //if k is still 30 we didnt find the needle
  {
    return -1; //return -1 if not found
  }
  return 0; //will never get here but it throws a warning so
}


int main()
{

  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  FILE *fp = NULL;
  

  while (1)
  {
    // Print out the mfs prompt
    printf("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your FAT32 functionality

    //over HERE

    if (!token[0]) //if input is empty aka "\n" just do nothing and let it reprint mfs>
    {
    }
    //OPEN
    else if (strcmp(token[0], "open") == 0) // if user types open
    {

      if (token_count == 3) //if count ==3 "open" "filename" "\n" 
      {
        fp = fopen(token[1], "r"); //open the file read only
      }
      if (fp == NULL) //file didnt open 
        perror("Something went wrong");
      else //if file opend successfully set up all the stuff like BPB
      {
        fseek(fp, 11, SEEK_SET); //seek to 11 get  BPB_BytesPerSec
        fread(&BPB_BytesPerSec, 2, 1, fp);

        fseek(fp, 13, SEEK_SET);//seek to 13 get  BPB_SecPerClus
        fread(&BPB_SecPerClus, 1, 1, fp);

        fseek(fp, 14, SEEK_SET);//seek to 14 get  BPB_RsvdSecCnt
        fread(&BPB_RsvdSecCnt, 2, 1, fp);

        fseek(fp, 16, SEEK_SET);//seek to 15 get  BPB_NumFATS
        fread(&BPB_NumFATS, 1, 1, fp);

        fseek(fp, 36, BPB_FATSz32);//seek to 136get  BPB_FATSz32
        fread(&BPB_FATSz32, 4, 1, fp);

        root = (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec); //set up root dir
        curdir = root; //make cur dir root
      }
    }
    else if(fp ==NULL) //if file isnt open dont let them do anything else
    {
      printf("FILE IS NOT OPEN\n");
    }
    //CLOSE
    else if (strcmp(token[0], "close") == 0)// if user types close
    {
      fclose(fp); //close file
      fp=NULL; //set pointer to null so it can be caught on 211 (else if above this one)
    }
    //info
    else if (strcmp(token[0], "info") == 0)// if user types info
    {
      //prints all the info we stored during open.
      printf("BPB_BytesPerSec: %d, %x\n", BPB_BytesPerSec, BPB_BytesPerSec);
      printf("BPB_SecPerClus: %d, %x\n", BPB_SecPerClus, BPB_SecPerClus);
      printf("BPB_RsvdSecCnt: %d, %x\n", BPB_RsvdSecCnt, BPB_RsvdSecCnt);
      printf("BPB_NumFATS: %d, %x\n", BPB_NumFATS, BPB_NumFATS);
      printf("BPB_FATSz32: %d, %x\n", BPB_FATSz32, BPB_FATSz32);
    }
    //ls
    else if (strcmp(token[0], "ls") == 0)// if user types ls
    {
      fseek(fp, curdir, SEEK_SET); //get the current dir
      fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read it and set it to dir
      int i;
      for (i = 0; i < 16; i++) //itrerate over the dir array
      {
        char filename[12]; //simple string to temp hold the name 
        strncpy(&filename[0], &dir[i].DIR_Name[0], 11); //copy the file name from dir[i] to filename
        filename[11] = '\0'; //null terminate it
        if ((dir[i].DIR_Name[0] != (char)0xe5) && (dir[i].DIR_Attr == 0x1 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)) //if its is a viable/valid file print it
          printf("%s\n", filename);

      }
    }
    //cd
    else if (strcmp(token[0], "cd") == 0)// if user types cd
    {

      if (token_count == 3) //if count ==3 "cd" "folder/directory" "\n" 
      {
        fseek(fp, curdir, SEEK_SET); //get current directory incase we havent yet
        fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read and set it incase we havent

        int i = get_DIR_num(token[1]); //get the index number of the directroy

        if (i >= 0) //if found
        {
          int cluster = dir[i].DIR_FirstClusterLow; //get cluster num
          int offset = LBAToOffset(cluster); //find offset
          fseek(fp, offset, SEEK_SET); //get to it
          fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read it
          curdir = offset; //update current dirrectory
        }
        else//if not found give error
          printf("DIR not found\n");
      }
      else //if token count!= 3 file name wasnt given
        printf("ERROR: Too few or Too many arrguments \n");
    }
    //get
    else if (strcmp(token[0], "get") == 0) // if user types cd
    {

      if (token_count == 3) //if count ==3 "get" "file" "\n" 
      {
        fseek(fp, curdir, SEEK_SET); //get current directory incase we havent yet
        fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read and set it incase we havent
        int i = get_DIR_num(token[1]); //get the index number of the directroy

        if (i >= 0)  //if found
        {
          FILE *target; //make new file
          char name[14]; //make a simple string 
          strcpy(name, token[1]); //copy the name the user typed in for eg "DEADBEEF"
          name[strlen(token[1])] = '.'; //add the extenstion by using the last the chars in the dir
          name[strlen(token[1]) + 1] = tolower(dir[i].DIR_Name[8]);   // gets the ext makes it lower case in the eg : "t"
          name[strlen(token[1]) + 2] = tolower(dir[i].DIR_Name[9]);   // gets the ext makes it lower case in the eg : "x"
          name[strlen(token[1]) + 3] = tolower(dir[i].DIR_Name[10]);  // gets the ext makes it lower case in the eg : "t"
          //the file name would now be "DEADBEEF.txt"
          target = fopen(name, "ab+");

          int cluster = dir[i].DIR_FirstClusterLow; //get cluster of file
          int offset = LBAToOffset(cluster); //get offset of file
          fseek(fp, offset, SEEK_SET); //go to the point in file
          int size = dir[i].DIR_FileSize; //get size from file that will be copied
          char *ptr = (char *)malloc(size); //simple array
          fread(ptr, size, 1, fp); //read file to be copied, copy it to ptr
          fwrite(ptr, size, 1, target);//copy ptr to target file 
          fclose(target);//close target
          free(ptr); //free the ptr 
        }
        else //if not file is not found 
        {
          printf("DIR not found\n");
        }
      }
      else //if token count!= 3 file name wasnt given
        printf("ERROR: Too few or Too many arrguments \n");
    }
    //read
    else if (strcmp(token[0], "read") == 0) // if user types read
    {

      if (token_count == 4) //if token count 4 = "read" "file" "postion" "size" 
      {
        fseek(fp, curdir, SEEK_SET); //get current directory incase we havent yet
        fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read and set it incase we havent
        int i = get_DIR_num(token[1]); //get the index number of the directroy
        if (i >= 0) //if found
        {
          int cluster = dir[i].DIR_FirstClusterLow; //get cluster of file
          int offset = LBAToOffset(cluster); //get offset of file
          offset += atoi(token[2]); //add postion to the offset 
           fseek(fp, offset, SEEK_SET); //go to the point in file
          int till = atoi(token[3]); //read till 
          for (int c = 0; c < till; c++) //iterate over the whole file and print each number char by char
          {
            printf("%d\t", fgetc(fp)); //print each byte
          }
          printf("\n"); //att the end add a \n
        }
        else// if not found
          printf("DIR not found\n");
      }
      else //if token is less than 4 missing some 
      {
        printf("missing inputs\n");
      }
    }
    else if (strcmp(token[0], "stat") == 0) //user typed stat
    {
      if (token_count == 3) //if token count 4 = "stat" "file" "\n" 
      {
        fseek(fp, curdir, SEEK_SET); //get current directory incase we havent yet
        fread(&dir, sizeof(struct DirectoryEntry), 16, fp); //read and set it incase we havent

        int i = get_DIR_num(token[1]); //get the index number of the directroy
        if (i >= 0) //if found
        {
          printf("File Attribute \t %d\nsize \t %d\nStarting clustuer Number \t %d\n", dir[i].DIR_Attr, dir[i].DIR_FileSize, dir[i].DIR_FirstClusterLow); //print it 
        }
        else //if not found 
          printf("DIR not found\n");
      }
      else //if not eough arrgs in the command file name wassnt given
      {
        printf("ERROR: Too few or Too many arrguments \n");
      }
    }
    else //if nothing hit yet it is not a valid input throw error
    {
      printf("Error command not found\n");
    }

    free(working_root);
  }
  return 0;
}
//end
