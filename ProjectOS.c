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
#include <fcntl.h>

#define options 20
#define MAX_ARGS 100
#define MAX_CMD_LENGTH 200
#define MAX_FILE_EXT 10

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

            // break;
        }
    } while (opt != NULL);
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
            }
            break;
        }

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

    } while (opt != NULL);
}

int execute_script(char *filename)
{
    // Execute script to compile the file and print number of errors and warnings
    char cmd[MAX_CMD_LENGTH];
    sprintf(cmd, "gcc -Wall -Werror %s 2>&1 | grep -iE '(warning|error)'", filename);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        perror("Error executing script");
        exit(EXIT_FAILURE);
    }

    char line[MAX_CMD_LENGTH];
    int num_warnings = 0;
    int num_errors = 0;
    while (fgets(line, MAX_CMD_LENGTH, fp) != NULL)
    {
        if (strstr(line, "warning") != NULL)
        {
            num_warnings++;
        }
        else if (strstr(line, "error") != NULL)
        {
            num_errors++;
        }
    }

    int exit_code = pclose(fp);
    if (exit_code == -1)
    {
        perror("Error closing pipe");
        exit(EXIT_FAILURE);
    }

    printf("Compile result for %s: %d errors, %d warnings\n", filename, num_errors, num_warnings);
    return (num_errors << 8) | num_warnings;
}

void reg_script(const char *path, const char *opt)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int num_lines = 0;
    char c;
    while (read(fd, &c, 1) > 0)
    {
        if (c == '\n')
        {
            num_lines++;
        }
    }

    printf("%s: %d lines\n", path, num_lines);
    close(fd);
}

void dir_script(const char *path, const char *opt)
{
    char filename[100];
    sprintf(filename, "%s_file.txt", path);
    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1)
    {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
    }

    char *message = "This file was created by the program\n";
    write(fd, message, strlen(message));

    close(fd);
}

void sym_script(const char *path, const char *opt)
{
    if (chmod(path, S_IRUSR | S_IWUSR | S_IXUSR | S_IWGRP | S_IRGRP) == -1)
    {
        perror("Failed to change permissions");
        exit(EXIT_FAILURE);
    }
}

// int compute_score(int exit_code) {
//    int num_errors = exit_code >> 8;
//     int num_warnings = exit_code & 0xFF;
//     int score = 0;
//     if (num_errors == 0 && num_warnings == 0) {
//         score = 10;
//     }
//     else if (num_errors > 0) {
//         score = 1;
//     }
//     else if (num_warnings > 10) {
//         score = 2;
//     }
//     else {
//         score = 2 + 8 * (10 - num_warnings) / 10;
//     }
//     return score;
// }

int has_c_extension(const char *filename)
{
    const char *extension = strrchr(filename, '.');
    if (extension == NULL)
    {
        return 0;
    }
    return strcmp(extension, ".c") == 0;
}

int count_lines(char *filename)
{
    FILE *fp;
    int count = 0;
    char ch;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        return -1;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            count++;
        }
    }

    fclose(fp);

    return count;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t child_pid, wait_pid;
    int status;

    for (int i = 1; i < argc; i++)
    {
        char file_type = get_file_type(argv[i]);
        char opt[options];

        if (file_type == 'r')
        {
            child_pid = fork();
            if (child_pid == -1)
            {
                printf("Failed to fork child process\n");
                return EXIT_FAILURE;
            }
            else if (child_pid == 0)
            {
                reg_handle(argv[i], file_type, opt);
                return EXIT_SUCCESS;
            }
        }
        else if (file_type == 'd')
        {
            child_pid = fork();
            if (child_pid == -1)
            {
                printf("Failed to fork child process\n");
                return EXIT_FAILURE;
            }
            else if (child_pid == 0)
            {
                dir_handle(argv[i], file_type, opt);
                return EXIT_SUCCESS;
            }
        }
        else if (file_type == 's')
        {
            child_pid = fork();
            if (child_pid == -1)
            {
                printf("Failed to fork child process\n");
                return EXIT_FAILURE;
            }
            else if (child_pid == 0)
            {
                sym_handle(argv[i], file_type, opt);
                return EXIT_SUCCESS;
            }
        }

        // Handle regular files with .c extension in a separate child process
        if (file_type == 'r' && has_c_extension(argv[i]))
        {
            child_pid = fork();
            if (child_pid == -1)
            {
                printf("Failed to fork child process\n");
                return EXIT_FAILURE;
            }
            else if (child_pid == 0)
            {
                // Execute script and get output
                char cmd[100];
                sprintf(cmd, "gcc -Wall -Wextra %s 2>&1 | grep -E '(warning|error)' | tee /dev/tty", argv[i]);
                FILE *script_output = popen(cmd, "r");

                // Parse output to count errors and warnings
                int num_errors = 0;
                int num_warnings = 0;
                char line[100];
                while (fgets(line, sizeof(line), script_output))
                {
                    if (strstr(line, "error"))
                    {
                        num_errors++;
                    }
                    else if (strstr(line, "warning"))
                    {
                        num_warnings++;
                    }
                }
                printf("%s: %d errors, %d warnings\n", argv[i], num_errors, num_warnings);
                // Calculate score based on errors and warnings
                int score = 0;
                if (num_errors == 0 && num_warnings == 0)
                {
                    score = 10;
                }
                else if (num_errors >= 1)
                {
                    score = 1;
                }
                else if (num_warnings > 10)
                {
                    score = 2;
                }
                else
                {
                    score = 2 + 8 * (10 - num_warnings) / 10;
                }

                // Write score to file
                char filename[100];
                sprintf(filename, "grades.txt");
                FILE *grades_file = fopen(filename, "a");
                fprintf(grades_file, "%s: %d\n", argv[i], score);
                fclose(grades_file);

                return EXIT_SUCCESS;
            }
        }
    }

    for (int i = 1; i < argc; i++)
    {
        char file_type = get_file_type(argv[i]);

        if (file_type == 'r' && !has_c_extension(argv[i]))
        {
            child_pid = fork();
            if (child_pid == -1)
            {
                printf("Failed to fork child process\n");
                return EXIT_FAILURE;
            }
            else if (child_pid == 0)
            {
                reg_script(argv[i], NULL);
                return EXIT_SUCCESS;
            }
        }
    }
    for (int i = 1; i < argc; i++) {
    char file_type = get_file_type(argv[i]);

    if (file_type == 'd') {
        child_pid = fork();
        if (child_pid == -1) {
            printf("Failed to fork child process\n");
            return EXIT_FAILURE;
        } else if (child_pid == 0) {
            dir_script(argv[i], NULL);
            return EXIT_SUCCESS;
        }
    }
}

// Handle symbolic links in a separate child process
for (int i = 1; i < argc; i++) {
    char file_type = get_file_type(argv[i]);

    if (file_type == 's') {
        child_pid = fork();
        if (child_pid == -1) {
            printf("Failed to fork child process\n");
            return EXIT_FAILURE;
        } else if (child_pid == 0) {
            sym_script(argv[i], NULL);
            return EXIT_SUCCESS;
        }
    }
}
    // int num_processes = 0;
    // char pids[1000];
    //  for (int i = 1; i < argc; i++) {
    //     char file_type = get_file_type(argv[i]);

    //     if (file_type == 'r' && !has_c_extension(argv[i])) {
    //         child_pid = fork();
    //         if (child_pid == -1) {
    //             perror("Failed to create child process");
    //             exit(EXIT_FAILURE);
    //         } else if (child_pid == 0) {
    //             // Child process
    //             int num_lines = count_lines(argv[i]);
    //             printf("File %s has %d lines\n", argv[i], num_lines);
    //             exit(EXIT_SUCCESS);
    //         } else {
    //             // Parent process
    //             pids[num_processes++] = child_pid;
    //         }
    //     }

    // Wait for child processes to finish
    while ((wait_pid = wait(&status)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Child process %d exited with status %d\n", wait_pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process %d was terminated by signal %d\n", wait_pid, WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("Child process %d was stopped by signal %d\n", wait_pid, WSTOPSIG(status));
        }
        else
        {
            printf("Unexpected status received for child process %d\n", wait_pid);
        }
    }
    return EXIT_SUCCESS;
}
