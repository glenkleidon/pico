#include "testmethods.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>


char *testname;
int totalTestCount=0;
int testCount=0;
int lastTestCount=-1;

void checkCounts()
{
    if (lastTestCount==testCount) 
    {
       skip_message;
       printf("No tests for %s\r\n",testname);
    }
}

void new_test(char *new_test_name)
{
    
    checkCounts();
    lastTestCount=testCount;
    testname = new_test_name;
}

void new_set(char * new_set_name)
{
   checkCounts();
   printf("\r\n%s\r\n",new_set_name);
   testCount=0;
   lastTestCount=-1;
}

void end_run()
{
   checkCounts();
}



void check_int(int expected, int actual, char *message)
{
  testCount++;
  totalTestCount++;
   char *msg = "OK";
   int result=1;
   if (expected!=actual) 
   {
      result=0;
      msg=message; 
   } 
   if (result) 
   {
      pass_message; 
   } else
   {
      fail_message;
   }   
   if (!msg) 
   { 
        printf(" Expected %u, actual %u\r\n", expected, actual);
   }else 
   {
       printf("%s-%s\r\n",testname,msg);
   }
}
void check_str(char *expected, char *actual, char *message)
{ 
  testCount++;
  totalTestCount++;
   int result=1;
   char *msg = "OK";
   if (expected==NULL && actual==NULL)
   {
       // this is ok - still a pass;
   } else if ( (expected==NULL && actual!=NULL) || (expected!=NULL && actual==NULL))
   {
      result=0;
      msg=message; 
   } else if (strncmp(expected,actual, strlen(expected)))
   {
      result=0;
      msg=message; 
   } 
   if (result) 
   {
      pass_message; 
   } else
   {
      fail_message;
   }
   if (!msg) 
   { 
      printf("%s\r\nExpected <%s>\r\nactual   <%s>\r\n", testname, expected, actual);
   }else 
   {
      printf("%s-%s\r\n",testname,msg);
   }
}
