//USED LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>
#include<fcntl.h>

//DEFINE FREQUENTLY USED CONST
#define MAXLEN 200
#define MAXNUM 15
#define SPACE " "


//DECLARING BACKGROUND FLAG AND LOG FILE NAME GLOBALY
int backGroundFlag=0;
char* fname;


//CHECKING PRESENCE OF "&" IN THE INPUT
int backGroundCheck(char* sec[])
{
    int index=0;
    //LOOP TILL "&" IS FOUND
    while(sec[index]!=NULL)
    {
        if(strcmp(sec[index],"&")==0)
        {

            backGroundFlag=1;
            return index;
        }
        index++;
    }
    //RETURN INDEX WHICH "&" IS FOUND IN
    return index;
}



void scanInput(char inputLine[])
{

    //SCANNING THE INPUT LINE FROM CONSOL AND PUTTING IT IN INPUTLINE ARRAY
    fgets(inputLine,MAXLEN,stdin);

    int i=0;

    //REMOVING THE EXTRA NEW LINE THAT SCANNED WITH THE INPUT
    while(inputLine [i]!= '\n')
    {
        i++;
    }

    inputLine[i]='\0';


    //HANDELING EXIT CASE (IF USER INPUT IS EXIT) THEN TERMINATE THE SHELL
    if(strcasecmp(inputLine,"exit")==0)
    {
        exit(0);
    };


}

//FUNCTION TO PROCCES THE INPUT TO BE AS(ARRAY OF STRINGS)
int processInput(char* sec[],char input[])
{

    int i=0;

    //REPLACING FIRST SPACE WITH STRING TERMINATOR AND RETURN POINTER TO THE BEGIN OF STRING
    sec[i]=strtok(input,SPACE);

    //IF END OF INPUT REACHED THEN RETURN
    if(sec[i] == NULL)
    {
        return 1;
    }

    //REPLACING EACH SPACE WITH STRING TERMINATOR AND RETURN POINTER TO THE BEGIN OF STRING
    while (sec[i] != NULL)
    {

        sec[++i] = strtok(NULL,SPACE);

    }
    return 1;
};

int lineParser(char* ptrs[],char input[])
{
    int loopCount=0;

    char *sec[MAXLEN];

   //CALLING FUNCTIONS TO SCAN THE INPUT AND PROCCES IT AS DESIRED SHAPE (ARRAY OF STRINGS)

    scanInput(input);

    processInput(sec,input);

    //CHECKKING IF "&" IS FOUND (BACKGROUND PROCCES) AND CATICHING THE INDEX THAT FOUND IN

    int index= backGroundCheck(sec);

    //REFOURMULATING THE ARRAY (COMBINING THE PART BEFORE "&" AND PART AFTER )TO REMOVE "&"

    while (loopCount<index)
    {

        ptrs[loopCount]=sec[loopCount];
        loopCount++;

    }
    if(backGroundFlag==1)
    {
        loopCount++;
    }
    while (sec[loopCount]!=NULL)
    {

        ptrs[loopCount-1]=sec[loopCount];
        loopCount++;
    }

    if(backGroundFlag==1)
    {
        ptrs[loopCount-1]=NULL;
    }
    else
    {
        ptrs[loopCount]=NULL;
    }

    return 1;

}

void proc_exit()
{
    //OPENING LOG FILE TO APPEND OR INFORM THAT CHILD PROCCES TERMINATED
    FILE * fp;
    fp = fopen(fname, "a");
    fprintf(fp,"%s","Child proccess was terminated\n");
    fclose(fp);

    //KILLING ZOMBIE PROCCESES
    int wstat;

		pid_t	pid;

		while (1) {
			pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
			if (pid == 0)
				return;
			else if (pid == -1)
				return;

		}
}

int main()
{

   //DECLARING INPUT ARRAY AND ARRAY OF POINTERS (ARRAY OF STRINGS)

    char  input[MAXNUM];
    char* ptrs[MAXLEN];

   //CREATING LOG FILE AND NAMING IT BY THE CURRENT TIME

    time_t currentTime;
    time(&currentTime);
    fname=ctime(&currentTime);
    char ftype[]=".txt";
    strcat(fname,ftype);

    FILE * fp;
    fp = fopen(fname, "w+");
    fclose(fp);

    //SIGNAL OF TERMINATED PROCCES
    signal (SIGCHLD, proc_exit);

    // INFINITE INPUT LOOP
    while(1)
    {
        //TAKING USER INPUT
        lineParser(ptrs,input);

        int i=0;

        //FORK FOR CHILD PROCCES
        int pid=fork();

        if(pid==0)
        {
            //EXECTUE COMMAND IF THIS IS CHILD PROCCES
            if(execvp(ptrs[0],ptrs)<0)
                printf("ERROR : No Such a command \n");
            exit(0);

        }
        else
        {
            //IF IT'S PARENT PROCCES AND THE COMMAND IS NOT BACK GROUND PROCCES
            if(backGroundFlag==0)
            {
                //MAKE THE PARENT WAIT FOR CHILD TERMINATION (STUCKING THE SHELL)
                waitpid(pid,NULL,0);
            }
            //IF IT'S PARENT PROCCES AND THE COMMAND IS NOT BACK GROUND PROCCES
            else
            {
                //IGNORE WATING OF PARENT ON CHILD PROCCCES
                backGroundFlag=0;
            }
        }
    };
    return 0;
}
