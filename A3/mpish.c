#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mpi.h>

const int MAX_STRING = 300;

int main(void) {
   int  comm_sz;               
   int  my_rank;               
   char *command;
   char *file;
   char * response;
   int process = 0;
   int flag = 0;
   int position = 1;
   MPI_Init(NULL, NULL); 
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   if (my_rank == 0)
   {
      while (1)
      { 
         command = "\n";
         file = "\n";
         process = 0;
         char buffer[1000];
         char * arr;
         // Get user input
         fgets(buffer, 1000,stdin);
         arr = strtok(buffer," ");
         command = arr;
         //validation
         arr = strtok(NULL," ");
         if (arr != NULL){
            file = arr;
            arr = strtok(NULL,"\n");
            if (arr !=NULL){
               flag = 0;
               for (int i=0; i<strlen(arr); i++){
                  if (isdigit(arr[i])==0){
                     flag = 1;
                  }
               }
               if (flag == 0 )
               {
                  process = atoi(arr);
               }else{
                  command = "invaild";
               }
            }else{
               file = strtok(file,"\n");
            }
         }else{
            command = strtok(command, "\n");
         }
         //commands checking
         if (strcmp(command, "put")==0){
            if (file!="\n")
            { 
               if (position>3){
                  position = 1;
               }
               FILE * fp = fopen(file,"r");
               if (fp!=NULL){
                  char back[MAX_STRING];
                  strcpy(back,"");
                  if (process == 0)
                  {
                     for (int i = 1; i < comm_sz; i++)
                     {
                        char tmpfile[100];
                        sprintf(tmpfile, "p%d/", i);
                        strcat(tmpfile, file);
                        MPI_Send(tmpfile, strlen(tmpfile) + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
                        MPI_Recv(back, MAX_STRING, MPI_CHAR, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        if (strcmp(back, "not") != 0)
                        {
                           process = i;
                           break;
                        }
                     }
                  }
                  char* line = malloc(sizeof(char)*MAX_STRING);
                  while (fgets(line, MAX_STRING,fp))
                  {
                     if (process != 0)
                     {
                        //reads in for binary files
                        if (strstr(file, ".bin") != NULL)
                        { 
                           rewind(fp);
                           unsigned char * lineb = malloc(sizeof(unsigned char)*MAX_STRING);
                           while(fread(lineb, sizeof(lineb), 1, fp)){
                              MPI_Send(file, strlen(file) + 1, MPI_CHAR, process, 1, MPI_COMM_WORLD);
                              MPI_Send(lineb, sizeof(lineb) + 1, MPI_UNSIGNED_CHAR, process, 1, MPI_COMM_WORLD);
                           }
                           free(lineb);
                           break;
                        }else{
                           MPI_Send(file, strlen(file) + 1, MPI_CHAR, process, 1, MPI_COMM_WORLD);
                           MPI_Send(line, strlen(line) + 1, MPI_CHAR, process, 1, MPI_COMM_WORLD);
                        }
                        }
                     else
                     {
                        //reads in for binary files
                        if (strstr(file, ".bin") != NULL)
                        {
                           rewind(fp);
                           unsigned char * lineb = malloc(sizeof(unsigned char)*MAX_STRING);
                           while(fread(lineb, sizeof(lineb), 1, fp)){
                              MPI_Send(file, strlen(file) + 1, MPI_CHAR, position, 1, MPI_COMM_WORLD);
                              MPI_Send(lineb, sizeof(lineb) + 1, MPI_UNSIGNED_CHAR, position, 1, MPI_COMM_WORLD);
                           }
                           free(lineb);
                           break;
                        }else{
                           MPI_Send(file, strlen(file) + 1, MPI_CHAR, position, 1, MPI_COMM_WORLD);
                           MPI_Send(line, strlen(line) + 1, MPI_CHAR, position, 1, MPI_COMM_WORLD);
                        }
                     }
                  }
                  if (process == 0){
                     position++;
                  }
                  free(line);
                  fclose(fp);
               }else{
                  printf("Please enter a valid file.\n");
               }
            }else{
               printf("Please enter a valid command.\n");
            }
         }else if(strcmp(command,"ls")==0){
            if (file == "\n")
            {
               int rank = 1;
               char send[10];
               for (rank = 1; rank < comm_sz; rank++)
               {
                  sprintf(send, "ls p%d", rank);
                  MPI_Send(send, strlen(send) + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD);
               }
               //get reply from MPI and print out results of ls
               for (rank = 1; rank < comm_sz; rank++)
               {
                  char * back = malloc(sizeof(char) * MAX_STRING);
                  MPI_Recv(back, MAX_STRING, MPI_CHAR, rank, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                  printf("%s\n", back);
                  free(back);
               }
            }else{
               printf("Please enter a valid command.\n");
            }
         }
         else if (strcmp(command, "lls") == 0)
         {
            if (file == "\n")
            {
               system("ls");
            }
            else
            {
               printf("Please enter a valid command.\n");
            }
         }
         else if (strcmp(command, "get") == 0)
         {
            int rank = 0;
            FILE *fp;
            MPI_Status status2;
            char data[MAX_STRING];
            //finds the file in MPI
            for (int i =1; i < comm_sz; i++){
               char *tmpfile = malloc(sizeof(char) * MAX_STRING);
               sprintf(tmpfile, "p%d/", i);
               strcat(tmpfile, file);
               MPI_Send(tmpfile, strlen(tmpfile) + 1, MPI_CHAR, i, 4, MPI_COMM_WORLD);
               char *data= malloc(sizeof(char)*MAX_STRING);
               MPI_Recv(data, MAX_STRING, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
               free(data);
               if(status2.MPI_TAG == 3){
                  rank = i;
                  break;
               }
            }

            if (rank != 0){
               char * tmpfile = malloc(sizeof(char)*MAX_STRING);
               sprintf(tmpfile, "p%d/", rank);
               strcat(tmpfile, file);
               MPI_Send(tmpfile, strlen(tmpfile) + 1, MPI_CHAR, rank, 3, MPI_COMM_WORLD);
               free(tmpfile);
               remove(file);
               while (1)
               {
                  if (strstr(file, ".bin") != NULL)
                  {
                     fp = fopen(file, "ab");
                     unsigned char *data = malloc(sizeof(unsigned char) * MAX_STRING);
                     MPI_Recv(data, MAX_STRING, MPI_UNSIGNED_CHAR, rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
                     if (status2.MPI_TAG == 3)
                     {
                        fwrite(data, sizeof(data), 1, fp);
                        fclose(fp);
                        free(data);
                     }else{
                        fclose(fp);
                        free(data);
                        break;
                     }
                  }
                  else
                  {
                     fp = fopen(file, "a");
                     char *data = malloc(sizeof(char) * MAX_STRING);
                     MPI_Recv(data, MAX_STRING, MPI_CHAR, rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
                     if (status2.MPI_TAG == 3)
                     {
                        fputs(data, fp);
                        fclose(fp);
                        free(data);
                     }
                     else
                     {
                        fclose(fp);
                        free(data);
                        break;
                     }
                  }
               }
            }else{
               printf("file cound not be found.\n");
            }
         }
         else if (strcmp(command, "lrm") == 0)
         {
            if (file != "\n")
            {
               sprintf(command, "rm %s", file);
               system(command);
            }
            else
            {
               printf("Please enter a valid command.\n");
            }
         }
         //exits out of the while loop and ends the program
         else if (strcmp(command, "exit") == 0)
         {
            if (file == "\n")
            {
               char send[10];
               for (int rank = 1; rank < comm_sz; rank++)
               {
                  sprintf(send, "exit", rank);
                  MPI_Send(send, strlen(send) + 1, MPI_CHAR, rank, 66, MPI_COMM_WORLD);
               }
               break;
            }
            else
            {
               printf("Please enter a valid command.\n");
            }
         }
         else
         {
            printf("Please enter a valid command.\n");
         }
      }
   }
   else
   {
      //creates the folders for the ranks
      char folder[10];
      sprintf(folder, "p%d", my_rank);
      mkdir(folder, 0777);
         while(1){
         response =malloc(sizeof(char)*MAX_STRING);
         MPI_Status status;
         MPI_Recv(response, MAX_STRING, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

         //checks if its ls and then does the command
         if (status.MPI_TAG == 0){
            char *send = malloc(sizeof(char)*MAX_STRING);
            char output[100];
            sprintf(send, "p%d\n", my_rank);
            FILE *open = popen(response, "r");
            while (fgets(output, sizeof(output), open) != NULL)
            {
               strcat(send, output);
            }
            pclose(open);
            MPI_Send(send, strlen(send) + 1, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
            free(send);
         }
         //does put
         else if(status.MPI_TAG == 1){
            FILE* fp;
            char path[20];
            sprintf(path, "p%d/", my_rank);
            strcat(path, response);
            if (strstr(path, ".bin") != NULL)
            {
               fp = fopen(path, "ab");
               unsigned char *data = malloc(sizeof(unsigned char) * MAX_STRING);
               MPI_Recv(data, MAX_STRING, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
               fwrite(data,sizeof(data),1, fp);
               fclose(fp);
               free(data);
            }
            else
            {
               fp = fopen(path, "a");
               char *data = malloc(sizeof(char) * MAX_STRING);
               MPI_Recv(data, MAX_STRING, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
               fputs(data, fp);
               fclose(fp);
               free(data);
            }
            //checks if the file exists and if so deletes and gives the folder
         }else if (status.MPI_TAG == 2){
            FILE * fp;
            if (strstr(response, ".bin") != NULL){
               fp = fopen(response, "rb");
            }else{
               fp = fopen(response, "r");
            }
            char delete[100];
            if (fp!=NULL){
               fclose(fp);
               sprintf(delete, "rm %s",response);
               system(delete);
               MPI_Send(&my_rank,MAX_STRING , MPI_CHAR, 0, 2, MPI_COMM_WORLD);
            }else{
               strcpy(delete,"not");
               MPI_Send(delete, MAX_STRING, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
            }
         }
         // does get
         else if (status.MPI_TAG == 3)
         {
            FILE *fp ;
            if (strstr(response, ".bin") != NULL)
            {
               fp = fopen(response, "rb");
            }else{
               fp = fopen(response, "r");
            }
            char* line = malloc(sizeof(char)*MAX_STRING);
            while (fgets(line, MAX_STRING,fp)){
               //reads in for binary files
               if (strstr(response, ".bin") != NULL)
               {
                  rewind(fp);
                  unsigned char *lineb = malloc(sizeof(unsigned char) * MAX_STRING);
                  while (fread(lineb, sizeof(lineb), 1, fp))
                  {
                     MPI_Send(lineb, sizeof(lineb) + 1, MPI_UNSIGNED_CHAR, 0, 3, MPI_COMM_WORLD);
                  }
                  MPI_Send(lineb, sizeof(lineb) + 1, MPI_UNSIGNED_CHAR, 0, 4, MPI_COMM_WORLD);
                  free(lineb);
                  break;
               }
               else
               {
                  MPI_Send(line, strlen(line) + 1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
               }
            }
            if (strstr(response, ".bin") == NULL){
               MPI_Send(line, strlen(line) + 1, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
            }
            free(line);
            fclose(fp);
         }else if(status.MPI_TAG == 4){
            FILE *fp;
            if (strstr(response, ".bin") != NULL)
            {
               fp = fopen(response, "rb");
            }
            else
            {
               fp = fopen(response, "r");
            }
            char * back = malloc(sizeof(char)*MAX_STRING);
            if (fp != NULL)
            {
               fclose(fp);
               strcpy(back, "found");
               MPI_Send(back, strlen(back) + 1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
            }
            else
            {
               strcpy(back, "not");
               MPI_Send(back, strlen(back) +1 , MPI_CHAR, 0, 4, MPI_COMM_WORLD);
            }
            free(back);
         }
         //exit case
         else if (status.MPI_TAG == 66)
         {
            free(response);
            break;
         }
      }
   }
   //shutdown mpi
   MPI_Finalize();
   return 0;
}