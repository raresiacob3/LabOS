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

    switch (file_type)
    {
    case 'r':

        printf("Reg file menu: \n");
        printf("-n: Name of the file \n");
        printf("-d: File size \n");
        printf("-h: Counter of the hard links \n");
        printf("-m: Time of the last modification \n");
        printf("-a: Access rights \n");
        printf("-l: Create the symbolic link \n");
        break;

    case 'd':
        printf("Directory menu: \n");
        printf("-n: Name of the directory \n");
        printf("-a: Access rights \n");
        printf("-c: Total number of files with .c extension \n");
        break;
    case 's':
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

char get_file_type(const char *path)
{
    struct stat sb;
    if (lstat(path, &sb) == -1)
    {
        // Error occurred while getting file information
        perror("lstat");
        return '\0';
    }

    if (S_ISREG(sb.st_mode))
    {
        return 'r'; // Regular file
    }
    else if (S_ISDIR(sb.st_mode))
    {
        return 'd'; // Directory
    }
    else if (S_ISLNK(sb.st_mode))
    {
        return 's'; // Symbolic link
    }
    else
    {
        return '\0'; // Unknown file type
    }
}

void reg_handle(char *path, char type, char *opt)
{
    do
    {
        printf("\nFile : %s --%c-- \n", path, type);
        menu(type);

        printf("\nEnter options: ");
        scanf("%s", opt);

        // now we check for the valid options

        int flag = 1;
        for (int i = 1; i < strlen(opt); i++)
        {
            if (type != 'r' && opt[i] != 'l')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }

            if (opt[i] != 'n' && opt[i] != 'd' && opt[i] != 'h' && opt[i] != 'm' && opt[i] != 'a' && opt[i] != 'l')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            struct stat info;
            stat(path, &info);
            switch (type)
            {
            case 'r':
                if (strchr(opt, 'n'))
                {
                    FILE *f = fopen(path, "r");
                    printf("Name of the file: %s\n", path);
                    fclose(f);
                }
                if (strchr(opt, 'd'))
                {
                    // print file size
                    printf("File size: %ld bytes\n", info.st_size);
                }
                if (strchr(opt, 'h'))
                {
                    // print hard link count
                    printf("Hard link count: %ld\n", info.st_nlink);
                }
                if (strchr(opt, 'm'))
                {
                    // print time of last modification
                    char time[100];
                    strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&info.st_mtime));
                    printf("Time of last modification: %s\n", time);
                }
                if (strchr(opt, 'l'))
                {
                    // create a symbolic link
                    if (symlink)
                    {
                        char symlink_name[256];
                        printf("Enter the name of the symbolic link: ");
                        scanf("%s", symlink_name);

                        if (symlink(path, symlink_name) < 0)
                        {
                            printf("Error: Could not create symbolic link.\n");
                        }
                        else
                        {
                            printf("Symbolic link created successfully.\n");
                        }
                    }
                }
            }
            if (strchr(opt, 'a'))
            {
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
    }while (opt != NULL);
}

    


void dir_handle(char *path, char type, char *opt)
{

    struct stat info;

    do
    {
        printf("\nDirectory : %s --%c-- \n", path, type);
        menu(type);

        printf("\nEnter options: ");
        scanf("%s", opt);

        // now we check for the valid options

        int flag = 1;
        for (int i = 1; i < strlen(opt); i++)
        {
            if (type != 'd')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }

            if (opt[i] != 'n' && opt[i] != 'd' && opt[i] != 'a' && opt[i] != 'c')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }

            switch (type)
            {
            case 'd':
                if (strchr(opt, 'n'))
                {
                    DIR *dir = opendir(path);
                    printf("Name of the directory: %s\n", path);
                    closedir(dir);
                }
                if (strchr(opt, 'a'))
                {
                    // print access rights
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
                if (strchr(opt, 'c'))
                {
                    DIR *dir = opendir(path);
                    int count = 0;
                    struct dirent *entry;
                    while ((entry = readdir(dir)) != NULL)
                    {
                        if (entry->d_type == DT_REG)
                        {
                            char *ext = strrchr(entry->d_name, '.');
                            if (ext != NULL && strcmp(ext, ".c") == 0)
                            {
                                count++;
                            }
                        }
                    }

                    closedir(dir);

                    printf("Number of .c files: %d\n", count);
                }
                //     if (count && fnmatch("*.c", entry->d_name, 0) == 0) {
                // ocurr++;
                //     }
                // printf("\n");
                //  if (count) {
                //     printf("Total number of .c files: %d\n", count);
            }
            break;
        }

        //     if (size) {
        //         printf("%ld\t", info.st_size);
        //     }

        //     if (access) {
        //         printf((S_ISDIR(info.st_mode)) ? "d" : "-");
        //         printf((info.st_mode & S_IRUSR) ? "r" : "-");
        //         printf((info.st_mode & S_IWUSR) ? "w" : "-");
        //         printf((info.st_mode & S_IXUSR) ? "x" : "-");
        //         printf((info.st_mode & S_IRGRP) ? "r" : "-");
        //         printf((info.st_mode & S_IWGRP) ? "w" : "-");
        //         printf((info.st_mode & S_IXGRP) ? "x" : "-");
        //         printf((info.st_mode & S_IROTH) ? "r" : "-");
        //         printf((info.st_mode & S_IWOTH) ? "w" : "-");
        //         printf((info.st_mode & S_IXOTH) ? "x" : "-");
        //         printf("\t");
        //     }

    } while (opt != NULL);
}

void sym_handle(const char *path, char type, char *opt)
{
    struct stat info;

    if (lstat(path, &info) < 0)
    {
        printf("Error: Could not get information for symbolic link %s.\n", path);
        return;
    }

    do
    {
        printf("\nLink : %s --%c-- \n", path, type);
        menu(type);

        printf("\nEnter options: ");
        scanf("%s", opt);

        // now we check for the valid options

        int flag = 1;
        for (int i = 1; i < strlen(opt); i++)
        {
            if (type != 's')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }

            if (opt[i] != 'n' && opt[i] != 'd' && opt[i] != 'a' && opt[i] != 'l' && opt[i] != 't')
            {
                printf("\nUnknown type of command\n");
                flag = 0;
                break;
            }
        }
         switch (type)
            {
            case 's':
                if (strchr(opt, 'n'))
                {
                    printf("Symbolic link name: %s\n", path);
                }
                if (strchr(opt, 'a'))
                {
                    // print access rights
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
                if (strchr(opt, 'd'))
                {
                     printf("Symbolic link size: %ld bytes\n", info.st_size);
                }

                if (strchr(opt, 't'))
                {
                    struct stat target_info;

                    if (stat(path, &target_info) < 0)
                    {
                        printf("Error: Could not get information for target file of symbolic link %s.\n", path);
                    }
                    else
                    {
                        printf("Target file size: %ld bytes\n", target_info.st_size);
                    }
                }

                if (strchr(opt, 'l'))
                {
                    if (unlink(path) < 0)
                {
                    printf("Error: Could not delete symbolic link %s.\n", path);
                    return;
                }
                else
                {
                    printf("Symbolic link %s deleted.\n", path);
                    return;
                }
                }

    } 
    // if (delete)
    // {
    //     if (unlink(path) < 0)
    //     {
    //         printf("Error: Could not delete symbolic link %s.\n", path);
    //         return;
    //     }
    //     else
    //     {
    //         printf("Symbolic link %s deleted.\n", path);
    //         return;
    //     }
    // }

    // if (name)
    // {
    //     printf("Symbolic link name: %s\n", path);
    // }

    // if (size)
    // {
    //     printf("Symbolic link size: %ld bytes\n", info.st_size);
    // }

    // if (target_size)
    // {
    //     struct stat target_info;

    //     if (stat(path, &target_info) < 0)
    //     {
    //         printf("Error: Could not get information for target file of symbolic link %s.\n", path);
    //     }
    //     else
    //     {
    //         printf("Target file size: %ld bytes\n", target_info.st_size);
    //     }
    // }

    // if (access)
    // {
    //     printf((S_ISLNK(info.st_mode)) ? "l" : "-");
    //     printf((info.st_mode & S_IRUSR) ? "r" : "-");
    //     printf((info.st_mode & S_IWUSR) ? "w" : "-");
    //     printf((info.st_mode & S_IXUSR) ? "x" : "-");
    //     printf((info.st_mode & S_IRGRP) ? "r" : "-");
    //     printf((info.st_mode & S_IWGRP) ? "w" : "-");
    //     printf((info.st_mode & S_IXGRP) ? "x" : "-");
    //     printf((info.st_mode & S_IROTH) ? "r" : "-");
    //     printf((info.st_mode & S_IWOTH) ? "w" : "-");
    //     printf((info.st_mode & S_IXOTH) ? "x" : "-");
    //     printf("\n");
    // }
}while (opt != NULL);
}

int main(int argc, char *argv[])
{
    char type;
    // char *path[1000];
    // char type ;
    // char *opt ;

    // if (argc < 3)
    // {
    //     perror("Error for number of arguments");
    //     exit(-1);

    // }

    for (int i = 1; i < argc; i++)
    {

        type = get_file_type(argv[i]);
        if (type == 'r')
        {
            char opt[options];
            reg_handle(argv[i], type, opt);
        }
        if (type == 'd')
        {
            char opt[options];
            dir_handle(argv[i], type, opt);
        }
        if (type == 's')
        {
            char opt[options];
            sym_handle(argv[i], type, opt);
        } 
    }

    return 0;

    // reg_handle(path, type, opt );
    //  menu(type);
}