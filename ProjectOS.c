#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fnmatch.h>

#define options 20

void menu(char file_type)
{
    switch(file_type){
        case 'reg':
            printf("Reg file menu: \n");
             printf("-n: Name of the file \n");
              printf("-d: File size \n");
               printf("-h: Counter of the hard links \n");
                printf("-m: Time of the last modification \n");
                 printf("-a: Access rights \n");
                  printf("-l: Create the symbolic link \n");
                  break;
        case 'dir':
             printf("Directory menu: \n");
             printf("-n: Name of the directory \n");
              printf("-a: Access rights \n");
               printf("-c: Total number of files with .c extension \n");
               break;
        case 'sym':
             printf("SymLink menu: \n");
             printf("-n: Name of the link \n");
              printf("-d: Link size \n");
               printf("-l: Delete link \n");
                printf("-t: Size of the target file \n");
                 printf("-a: Access rights \n");
                 break;
        default:
        printf("Unknown type\n");                
               


    }
}

void reg_handle(char *path, char type, char* opt)
{
    printf("File : %s --%c-- \n", path, type);
    menu(type);
    printf("Enter options: ");
    scanf("%s", opt);

    //now we check for the valid options

    int flag = 1;
    for (int i = 1; i< strlen(opt); i++)
    {
        if (type != 'reg' && opt[i] != 'l')
        {
            flag = 0;
            break;
        }
        
        if (opt[i] != 'n' && opt[i] != 'd' && opt[i] != 'h' && opt[i] != 'm' && opt[i] != 'a' && opt[i] != 'l') {
            flag = 0;
            break;

    }


}
        if (flag)
        {
            struct stat info;
            stat(path, &info);
            switch(type){
                case 'reg':
                    if (strchr(opt, 'n')) {
                        FILE *f = fopen(path, "r");
                        printf("Name of the file: %s", path);
                        fclose(f);

                    }         
                    if (strchr(opt, 'd')) {
                    // print file size
                    printf("File size: %ld bytes\n", info.st_size);
                }
                if (strchr(opt, 'h')) {
                    // print hard link count
                    printf("Hard link count: %ld\n", info.st_nlink);
                }
                if (strchr(opt, 'm')) {
                    // print time of last modification
                    char time[100];
                    strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&info.st_mtime));
                    printf("Time of last modification: %s\n", time);
                }
                if (strchr(opt, 'a')) {
                    // print access rights
                    printf("User:\n");
                    printf("Read - %s\n", (info.st_mode & S_IRUSR) ? "yes" : "no");
                    printf("Write - %s\n", (info.st_mode & S_IWUSR) ? "yes" : "no");
                    printf("Exec - %s\n", (info.st_mode & S_IXUSR) ? "yes" : "no");
                    printf("Group:\n");
                    printf("Read - %s\n", (info.st_mode & S_IRGRP) ? "yes" : "no");
                    printf("Write - %s\n", (info.st_mode & S_IWGRP) ? "yes" : "no");
                    printf("Exec - %s\n", (info.st_mode & S_IXGRP) ? "yes" : "no");
                    printf("Others:\n");
                    printf("Read - %s\n", (info.st_mode & S_IROTH) ? "yes" : "no");
                    printf("Write - %s\n", (info.st_mode & S_IWOTH) ? "yes" : "no");
                    printf("Exec - %s\n", (info.st_mode & S_IXOTH) ? "yes" : "no");
                    

         
            }

            break;

        
        }
        
}
}


void dir_handle(char *path, int name, int size, int access, int count)
{
    struct dirent *entry;
    struct stat info;
    DIR *dir = opendir(path);
    int ocurr = 0;

    if (dir == NULL) {
        printf("Error: Could not open directory %s.\n", path);
        return;
    }

    printf("Directory contents:\n");

    while ((entry = readdir(dir)) != NULL) {
        char file_path[512];
        sprintf(file_path, "%s/%s", path, entry->d_name);

        if (stat(file_path, &info) < 0) {
            printf("Error: Could not get information for file %s.\n", entry->d_name);
            continue;
        }

        if (name) {
            printf("%s\t", entry->d_name);
        }

        if (size) {
            printf("%ld\t", info.st_size);
        }

        if (access) {
            printf((S_ISDIR(info.st_mode)) ? "d" : "-");
            printf((info.st_mode & S_IRUSR) ? "r" : "-");
            printf((info.st_mode & S_IWUSR) ? "w" : "-");
            printf((info.st_mode & S_IXUSR) ? "x" : "-");
            printf((info.st_mode & S_IRGRP) ? "r" : "-");
            printf((info.st_mode & S_IWGRP) ? "w" : "-");
            printf((info.st_mode & S_IXGRP) ? "x" : "-");
            printf((info.st_mode & S_IROTH) ? "r" : "-");
            printf((info.st_mode & S_IWOTH) ? "w" : "-");
            printf((info.st_mode & S_IXOTH) ? "x" : "-");
            printf("\t");
        }

        if (count && fnmatch("*.c", entry->d_name, 0) == 0) {
            ocurr++;
        }

        printf("\n");
    }

    closedir(dir);

    if (count) {
        printf("Total number of .c files: %d\n", count);
    }
}

void sym_handle(const char *path, int name, int delete, int size, int target_size, int access)
{
struct stat info;

    if (lstat(path, &info) < 0) {
        printf("Error: Could not get information for symbolic link %s.\n", path);
        return;
    }

    if (delete) {
        if (unlink(path) < 0) {
            printf("Error: Could not delete symbolic link %s.\n", path);
            return;
        } else {
            printf("Symbolic link %s deleted.\n", path);
            return;
        }
    }

    if (name) {
        printf("Symbolic link name: %s\n", path);
    }

    if (size) {
        printf("Symbolic link size: %ld bytes\n", info.st_size);
    }

    if (target_size) {
        struct stat target_info;

        if (stat(path, &target_info) < 0) {
            printf("Error: Could not get information for target file of symbolic link %s.\n", path);
        } else {
            printf("Target file size: %ld bytes\n", target_info.st_size);
        }
    }

    if (access) {
        printf((S_ISLNK(info.st_mode)) ? "l" : "-");
        printf((info.st_mode & S_IRUSR) ? "r" : "-");
        printf((info.st_mode & S_IWUSR) ? "w" : "-");
        printf((info.st_mode & S_IXUSR) ? "x" : "-");
        printf((info.st_mode & S_IRGRP) ? "r" : "-");
        printf((info.st_mode & S_IWGRP) ? "w" : "-");
        printf((info.st_mode & S_IXGRP) ? "x" : "-");
        printf((info.st_mode & S_IROTH) ? "r" : "-");
        printf((info.st_mode & S_IWOTH) ? "w" : "-");
        printf((info.st_mode & S_IXOTH) ? "x" : "-");
        printf("\n");
    }

}

int main(int argc, char *argv[])
{

 int file;   

if (argc < 3)
{
    perror("Error for number of arguments");

}

menu(file);


}