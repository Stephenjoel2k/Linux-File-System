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
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%s/%s\n", name, entry->d_name);
            listdir(path);
        } else {
            printf("%s/%s\n", name, entry->d_name);
        }
    }
    closedir(dir);
}

/**
 * -name
*/
void finddirName(const char *name, char *dirToFind)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirName(path, dirToFind);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            if(strcmp(entry->d_name, dirToFind) == 0){
              printf("%s\n", path);
            }
        }
        
    }
    closedir(dir);
}


/**
 * -mmin
 * */
void finddirTime(const char *name, long minTime, char *exact)
{
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    struct tm dt;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirTime(path, minTime, exact);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            if(stat(path,&buf)==0) {
              time_t now = time(0);
              dt = *(localtime(&buf.st_mtime));
              double seconds = difftime(now,mktime(&dt));
              int minutes = (int)seconds/60;
              //If the mintime is negative (i.e -> Files modified in the last N minutes)
              //minutes + minTime -> Is the time passed between now and last modfied. If it is hasn't been over N minutes then print file
              //Example lets say a file was modified 5 mins ago and N is -10 then 5 + -10 = -5 which is less than 0 so we print it.
              if(exact && minutes == (int)minTime)
              {
                  printf("File: %s, was modified %d minutes ago.\n", path, minutes);
              }
              else if(!exact && minTime < 0 && (minutes + minTime) <= 0)
              {
                  printf("File: %s, was modified %d minutes ago.\n", path, minutes);
              } 
              else if (!exact && minTime > 0 && minutes > minTime)  //if modified after the minTime
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
*/
void finddirNumber(const char *name, long dirNum)
{
    DIR *dir;
    struct dirent *entry;
    struct stat buf;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirNumber(path, dirNum);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
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
*/
void finddirNameAndDelete(const char *name, char *dirToFind)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirNameAndDelete(path, dirToFind);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
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
*/
void finddirTimeAndDelete(const char *name, long minTime, char *exact)
{
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    struct tm dt;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirTimeAndDelete(path, minTime, exact);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            if(stat(path,&buf)==0) {
              time_t now = time(0);
              dt = *(localtime(&buf.st_mtime));
              double seconds = difftime(now,mktime(&dt));
              int minutes = (int)seconds/60;
              //If the mintime is negative (i.e -> Files modified in the last N minutes)
              //minutes + minTime -> Is the time passed between now and last modfied. If it is hasn't been over N minutes then print file
              //Example lets say a file was modified 5 mins ago and N is -10 then 5 + -10 = -5 which is less than 0 so we print it.
              if(exact && minutes == (int)minTime)
              {
                  int del = remove(path);
                  if(!del){
                    printf("File Deleted: %s, was modified %d minutes ago.\n", path, minutes);
                  }else{
                    printf("Failed to delete file : %s", path);
                  }
              }
              else if(!exact && minTime < 0 && (minutes + minTime) <= 0)
              {
                  int del = remove(path);
                  if(!del){
                    printf("File Deleted: %s, was modified %d minutes ago.\n", path, minutes);
                  }else{
                    printf("Failed to delete file : %s", path);
                  }
              } 
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
*/
void finddirNumberAndDelete(const char *name, long dirNum)
{
    DIR *dir;
    struct dirent *entry;
    struct stat buf;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        //if folder
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            finddirNumberAndDelete(path, dirNum);
        //if file only then do we need to find a match
        } else {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);   //Get absolute path from the relative path
            if(stat(path,&buf)==0) {
                long num = buf.st_ino;
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
    if (argc == 1)
    {
        listdir(".");
    }
    if (argc == 2)
    {
      listdir(argv[1]);
    }
    else if(argc == 4)
    {
      if (strcmp("-name", argv[2]) == 0){
        finddirName(argv[1], argv[3]);
      }else if (strcmp("-mmin", argv[2]) == 0){
        char symbol = argv[3][0];
        long arg = strtol(argv[3], NULL, 10);
        if(symbol == '-') 
        {
            printf("\nThe files below have been modified in the last %ld minutes.\n\n\n", arg);
            finddirTime(argv[1], arg, NULL);
        }
            
        else if(symbol == '+') 
        {
            printf("\nThe files below have been modified before %ld minutes.\n\n\n", arg);
            finddirTime(argv[1], arg, NULL);
        }
            
        else
            finddirTime(argv[1], arg, "exact");
      }
      else if (strcmp("-inum", argv[2]) == 0){
        long arg = strtol(argv[3], NULL, 10);
        finddirNumber(argv[1], arg);
      }
    }
    //DELETE
    else if(argc == 5)
    {
        if (strcmp("-name", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            finddirNameAndDelete(argv[1], argv[3]);
        }
        if (strcmp("-mmin", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            char symbol = argv[3][0];
            long arg = strtol(argv[3], NULL, 10);
            if(symbol == '-') 
            {
                printf("\nThe files below have been modified in the last %ld minutes.\n\n\n", arg);
                finddirTimeAndDelete(argv[1], arg, NULL);
            }   
            else if(symbol == '+') 
            {
                printf("\nThe files below have been modified before %ld minutes.\n\n\n", arg);
                finddirTimeAndDelete(argv[1], arg, NULL);
            }    
            else
            {
                finddirTimeAndDelete(argv[1], arg, "exact");
            }
                
            }
        if (strcmp("-inum", argv[2]) == 0 && strcmp("-delete", argv[4]) == 0){
            long arg = strtol(argv[3], NULL, 10);
            finddirNumberAndDelete(argv[1], arg);
        }
    }   
}
