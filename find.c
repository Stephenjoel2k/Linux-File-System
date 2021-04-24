/**
 * Recursively prints the files in a directory.
 * */

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


/**
 * Lists the directories in the same format as the Linux find
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 **/
void listdir(const char *name)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;

    //check if file or directory. Return if file
    if (!(dir = opendir(name)))     
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {        
        //If it is a directory
        if (entry->d_type == DT_DIR) {              
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)    
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);     
            printf("%s/%s\n", name, entry->d_name);                         //This prints the path upto this direction and adds the name of the current dir followed by a "/"
            listdir(path);                                                  //Recuresively call the path until it explores all directories/files.
        }
        //If it is a directory 
        else {                                   
            printf("%s/%s\n", name, entry->d_name);                         //Print the path to the file.
        }
    }
    closedir(dir);
}

/**
 * -name
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * char *dirToFind -> The name of the file we want to find
*/
void finddirName(const char *name, char *dirToFind)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirName(path, dirToFind);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //print if path matches
            if(strcmp(entry->d_name, dirToFind) == 0){
              printf("%s\n", path);
            }
        }
        
    }
    closedir(dir);
}


/**
 * -mmin
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * long *minTime -> The amount of time, it can be positive or negative
 * char *exact -> This is acting like a boolean. This is usually null unless the user is looking for an exact match of modified time.
 * */
void finddirTime(const char *name, long minTime, char *exact)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    struct tm dt;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            //recursively call the function
            finddirTime(path, minTime, exact);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //Get the stats to access the mmin
            if(stat(path,&buf)==0) {
              time_t now = time(0);
              dt = *(localtime(&buf.st_mtime));
              double seconds = difftime(now,mktime(&dt));
              int minutes = (int)seconds/60;
              //If the mintime is negative (i.e -> Files modified in the last N minutes)
              //minutes + minTime -> Is the time passed between now and last modfied. If it is hasn't been over N minutes then print file
              //Example lets say a file was modified 5 mins ago and N is -10 then 5 + -10 = -5 which is less than 0 so we print it.
             
              //if modified exactly at the minTime
              if(exact && minutes == (int)minTime)
              {
                  printf("File: %s, was modified %d minutes ago.\n", path, minutes);
              }
              //if modified before the minTime
              else if(!exact && minTime < 0 && (minutes + minTime) <= 0)
              {
                  printf("File: %s, was modified %d minutes ago.\n", path, minutes);
              } 
              //if modified after the minTime
              else if (!exact && minTime > 0 && minutes > minTime)  
              {
                  printf("File: %s, was modified %d minutes ago.\n", path, minutes);
              }
              
            }
            
        }
        
    }
    closedir(dir);
}

/**
 * -inum
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * long dirNum -> This contains the i_node num that we the user wants to match while finding.
*/
void finddirNumber(const char *name, long dirNum)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;
    struct stat buf;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirNumber(path, dirNum);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //Get the stats to access the Inum
            if(stat(path,&buf)==0) {
                long num = buf.st_ino;
                if(num == dirNum){
                    printf("File: %s, has the i-node number %d.\n", path, dirNum);
                }
            }
        }
        
    }
    closedir(dir);
}


/***********
 * 
 * DELETE FUNCTIONS
 * 
 * **********/

/**
 * -name -delete
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * char *dirToFind -> The name of the file we want to find
*/
void finddirNameAndDelete(const char *name, char *dirToFind)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            //recursively call on the directory
            finddirNameAndDelete(path, dirToFind);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //delete if the name matches the search of the user
            if(strcmp(entry->d_name, dirToFind) == 0){
                int del = remove(path);
                if(!del){
                    printf("\n\nFollowing path has been deleted : %s\n", path);
                }else{
                    printf("Failed to delete file : %s", path);
                }
            }
        }
        
    }
    closedir(dir);
}


/**
 * -mmin -delete
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * long *minTime -> The amount of time, it can be positive or negative
 * char *exact -> This is acting like a boolean. This is usually null unless the user is looking for an exact match of modified time.
 * */
void finddirTimeAndDelete(const char *name, long minTime, char *exact)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    struct tm dt;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //We don't want to recursively call on "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirTimeAndDelete(path, minTime, exact);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //get the stat to acces the mtime.
            if(stat(path,&buf)==0) {
              time_t now = time(0);
              dt = *(localtime(&buf.st_mtime));
              double seconds = difftime(now,mktime(&dt));
              int minutes = (int)seconds/60;
              //if modified exactly at minTime
              if(exact && minutes == (int)minTime)
              {
                  int del = remove(path);
                  if(!del){
                    printf("File Deleted: %s, was modified %d minutes ago.\n", path, minutes);
                  }else{
                    printf("Failed to delete file : %s", path);
                  }
              }
              //If modified before min time
              else if(!exact && minTime < 0 && (minutes + minTime) <= 0)
              {
                  int del = remove(path);
                  if(!del){
                    printf("File Deleted: %s, was modified %d minutes ago.\n", path, minutes);
                  }else{
                    printf("Failed to delete file : %s", path);
                  }
              } 
              //If modified after min time
              else if (!exact && minTime > 0 && minutes > minTime)  //if modified after the minTime
              {
                  int del = remove(path);
                  if(!del){
                    printf("File Deleted: %s, was modified %d minutes ago.\n", path, minutes);
                  }else{
                    printf("Failed to delete file : %s", path);
                  }
              }
              
            }
            
        }
        
    }
    closedir(dir);
}



/**
 * -inum -delete
 * char *name -> The path/folder name. We recursively pass the path as the argument.
 * long dirNum -> This contains the i_node num that we the user wants to match while finding.
*/
void finddirNumberAndDelete(const char *name, long dirNum)
{
    //initializing variables
    DIR *dir;
    struct dirent *entry;
    struct stat buf;

     //check if file or directory. Return if file
    if (!(dir = opendir(name)))
        return;

    //While the current entry is a directory keep looping
    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            //To prevent recursively calling on .. and .
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirNumberAndDelete(path, dirNum);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            //Get the updated path including the current directory
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            //get the stat to access inum.
            if(stat(path,&buf)==0) {
                long num = buf.st_ino;
                //If match delete
                if(num == dirNum){
                    int del = remove(path);
                    if(!del){
                        printf("\nFile: %s, with the i-node number %d has been deleted.\n", path, dirNum);
                    }else{
                        printf("Failed to delete file : %s", path);
                    }
                }
            }
        }
        
    }
    closedir(dir);
}



int main(int argc, char **argv)
{
    //If no arguments are passed then act like a find
    if (argc == 1)
    {
        listdir(".");
    }
    //If an argument is passed then find dirs/files from the argument
    if (argc == 2)
    {
      listdir(argv[1]);
    }
    //If 4 arguments then find with -name, -mmin, -inum
    else if(argc == 4)
    {
        //If name
      if (strcmp("-name", argv[2]) == 0){
        finddirName(argv[1], argv[3]);
      }
      //if -mmin
      else if (strcmp("-mmin", argv[2]) == 0){
        char symbol = argv[3][0];
        long arg = strtol(argv[3], NULL, 10);
        //Less than n
        if(symbol == '-') 
        {
            printf("\nThe files below have been modified in the last %ld minutes.\n\n\n", arg);
            finddirTime(argv[1], arg, NULL);
        }
        //Before n
        else if(symbol == '+') 
        {
            printf("\nThe files below have been modified before %ld minutes.\n\n\n", arg);
            finddirTime(argv[1], arg, NULL);
        }
        //exactly at n
        else
            finddirTime(argv[1], arg, "exact");
      }
      //-inum
      else if (strcmp("-inum", argv[2]) == 0){
        long arg = strtol(argv[3], NULL, 10);
        finddirNumber(argv[1], arg);
      }
    }
    //DELETE
    else if(argc == 5)
    {
        //name delete
        if (strcmp("-name", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            finddirNameAndDelete(argv[1], argv[3]);
        }
        //mmin delete
        if (strcmp("-mmin", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            char symbol = argv[3][0];
            long arg = strtol(argv[3], NULL, 10);
            //if less than n
            if(symbol == '-') 
            {
                printf("\nThe files below have been modified in the last %ld minutes.\n\n\n", arg);
                finddirTimeAndDelete(argv[1], arg, NULL);
            }   
            //before n
            else if(symbol == '+') 
            {
                printf("\nThe files below have been modified before %ld minutes.\n\n\n", arg);
                finddirTimeAndDelete(argv[1], arg, NULL);
            }    
            //exactly at N
            else
            {
                finddirTimeAndDelete(argv[1], arg, "exact");
            }
                
            }
        //inum delete
        if (strcmp("-inum", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            long arg = strtol(argv[3], NULL, 10);
            finddirNumberAndDelete(argv[1], arg);
        }
    }   
}
