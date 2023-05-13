

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
#include <sys/wait.h>

#define OPTIONS 20


int count_lines(const char *filename)
{
    FILE *file = fopen(filename, "r");
    int ch, lines = 0;

    if (file == NULL)
    {
        perror("Failed to use count_lines function");
        return -1;
    }

    while (!feof(file))
    {
        ch = fgetc(file);
        if (ch == '\n')
        {
            lines++;
        }
    }

    fclose(file);
    return lines;
}
int has_c_extension(const char *path)
{
    // Get the length of the path
    int len = strlen(path);

    // Check if the last two characters are ".c"
    if (len > 2 && path[len - 2] == '.' && path[len - 1] == 'c')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void handle_regular_file(const char *path, int write_fd)
{
    printf("Regular file: %s\n", path);

    char options[OPTIONS];
    printf("Enter option: ");
    scanf("%s", options);

    // Now we check for the valid options
    int flag = 1;
    for (int i = 1; i < strlen(options); i++)
    {
        if (options[i] != 'n' && options[i] != 'd' && options[i] != 'h' && options[i] != 'm' && options[i] != 'a' && options[i] != 'l')
        {
            printf("Unknown type of command\n");
            flag = 0;
            break;
        }
    }

    if (flag)
    {
        struct stat info;
        stat(path, &info);
        if (strchr(options, 'n'))
        {
            printf("Name of the file: %s\n", path);
        }
        if (strchr(options, 'd'))
        {
            printf("File size: %ld bytes\n", info.st_size);
        }
        if (strchr(options, 'h'))
        {
            printf("Hard link count: %ld\n", info.st_nlink);
        }
        if (strchr(options, 'm'))
        {
            char time[100];
            strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&info.st_mtime));
            printf("Time of last modification: %s\n", time);
        }
        if (strchr(options, 'a'))
        {
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
        if (strchr(options, 'l'))
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

    // Write options to the pipe
    write(write_fd, options, strlen(options));

    if (has_c_extension(path))
    {
        int num_warnings = 0;
        int num_errors = 0;

        char cmd[500];
        sprintf(cmd, "gcc -Wall -Wextra %s 2>&1", path);

        FILE *fp = popen(cmd, "r");
        if (fp == NULL)
        {
            printf("Failed to run script\n");
            return;
        }

        char line[500];
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            if (strstr(line, "warning"))
            {
                num_warnings++;
            }
            else if (strstr(line, "error"))
            {
                num_errors++;
            }
        }

        pclose(fp);

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

        // Write score to a file
        char filename[500];
        sprintf(filename, "grades.txt");
        printf("Warnings: %d, Errors: %d\n", num_warnings, num_errors);
        FILE *grades_file = fopen(filename, "a");
        if (grades_file != NULL)
        {
            fprintf(grades_file, "%s: %d\n", path, score);
            fclose(grades_file);
        }
        else
        {
            printf("Failed to open grades file\n");
        }
    }
    else
    {
        int num_lines = count_lines(path);

        // Write the result to the pipe
        char result[1000];
        sprintf(result, "%s: %d lines", path, num_lines);
        printf("%s\n", result); // print the result
        ssize_t num_bytes = write(write_fd, result, strlen(result));
        if (num_bytes == -1)
        {
            perror("Failed to write to pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void handle_directory(const char *path, int write_fd)
{
    printf("Directory menu: \n");
    printf("-n: Name of the directory \n");
    printf("-a: Access rights \n");
    printf("-c: Total number of files with .c extension \n");
    printf("Directory: %s\n", path);

    char options[OPTIONS];
    printf("Enter option: ");
    scanf("%s", options);

    // Now we check for the valid options
    int flag = 1;
    for (int i = 1; i < strlen(options); i++)
    {
        if (options[i] != 'n' && options[i] != 'd' && options[i] != 'a' && options[i] != 'c')
        {
            printf("Unknown type of command\n");
            flag = 0;
            break;
        }
    }

    if (flag)
    {
        struct stat info;
        stat(path, &info);
        if (strchr(options, 'n'))
        {
            printf("Name of the directory: %s\n", path);
        }
        if (strchr(options, 'd'))
        {
            printf("Directory size: %ld bytes\n", info.st_size);
        }
        if (strchr(options, 'a'))
        {
            printf("Access rights:\n");
            printf((info.st_mode & S_IRUSR) ? "User: Read - yes\n" : "User: Read - no\n");
            printf((info.st_mode & S_IWUSR) ? "User: Write - yes\n" : "User: Write - no\n");
            printf((info.st_mode & S_IXUSR) ? "User: Exec - yes\n" : "User: Exec - no\n");
            printf((info.st_mode & S_IRGRP) ? "Group: Read - yes\n" : "Group: Read - no\n");
            printf((info.st_mode & S_IWGRP) ? "Group: Write - yes\n" : "Group: Write - no\n");
            printf((info.st_mode & S_IXGRP) ? "Group: Exec - yes\n" : "Group: Exec - no\n");
            printf((info.st_mode & S_IROTH) ? "Others: Read - yes\n" : "Others: Read - no\n");
            printf((info.st_mode & S_IWOTH) ? "Others: Write - yes\n" : "Others: Write - no\n");
            printf((info.st_mode & S_IXOTH) ? "Others: Exec - yes\n" : "Others: Exec - no\n");
        }
        if (strchr(options, 'c'))
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
        char filename[1000];
        sprintf(filename, "%s_file.txt", path);

        // Create the file with .txt extension
        FILE *file = fopen(filename, "w");
        if (file == NULL)
        {
            perror("Failed to create file");
            exit(EXIT_FAILURE);
        }

        // Write a message to the file
        fprintf(file, "This file was created by the program.\n");

        fclose(file);

        // Write options to the pipe
        write(write_fd, options, strlen(options));
        char result[10000];
        ssize_t num_bytes = write(write_fd, result, strlen(result));
        if (num_bytes == -1)
        {
            perror("Failed to write to pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void handle_symbolic_link(const char *path, int write_fd)
{
    printf("SymLink menu: \n");
    printf("-n: Name of the link \n");
    printf("-d: Link size \n");
    printf("-l: Delete link \n");
    printf("-t: Size of the target file \n");
    printf("-a: Access rights \n");

    printf("Symbolic link: %s\n", path);

    char options[OPTIONS];
    printf("Enter option: ");
    scanf("%s", options);

    // Now we check for the valid options

    for (int i = 1; i < strlen(options); i++)
    {
        if (options[i] != 'n' && options[i] != 'l' && options[i] != 'd' && options[i] != 't' && options[i] != 'a')
        {
            printf("Unknown option");
        }
    }

    if (strchr(options, 'n'))
    {
        printf("Name of the symbolic link: %s\n", path);
    }
    if (strchr(options, 'l'))
    {
        if (unlink(path) < 0)
        {
            printf("Error: Could not delete symbolic link %s.\n", path);
        }
        else
        {
            printf("Symbolic link %s deleted.\n", path);
        }
        return;
    }
    if (strchr(options, 'd'))
    {
        struct stat info;
        lstat(path, &info);
        printf("Symbolic link size: %ld bytes\n", info.st_size);
    }
    if (strchr(options, 't'))
    {
        struct stat info;
        if (lstat(path, &info) < 0)
        {
            printf("Error: Could not get information for target file of symbolic link %s.\n", path);
        }
        else
        {
            printf("Target file size: %ld bytes\n", info.st_size);
        }
    }
    if (strchr(options, 'a'))
    {
        struct stat info;
        lstat(path, &info);
        printf("Access rights:\n");
        printf((info.st_mode & S_IRUSR) ? "User: Read - yes\n" : "User: Read - no\n");
        printf((info.st_mode & S_IWUSR) ? "User: Write - yes\n" : "User: Write - no\n");
        printf((info.st_mode & S_IXUSR) ? "User: Exec - yes\n" : "User: Exec - no\n");
        printf((info.st_mode & S_IRGRP) ? "Group: Read - yes\n" : "Group: Read - no\n");
        printf((info.st_mode & S_IWGRP) ? "Group: Write - yes\n" : "Group: Write - no\n");
        printf((info.st_mode & S_IXGRP) ? "Group: Exec - yes\n" : "Group: Exec - no\n");
        printf((info.st_mode & S_IROTH) ? "Others: Read - yes\n" : "Others: Read - no\n");
        printf((info.st_mode & S_IWOTH) ? "Others: Write - yes\n" : "Others: Write - no\n");
        printf((info.st_mode & S_IXOTH) ? "Others: Exec - yes\n" : "Others: Exec - no\n");
    }

    // Change permissions of the symbolic link
    char resolved_path[PATH_MAX];
    if (realpath(path, resolved_path) == NULL)
    {
        perror("Failed to resolve symbolic link");
        return;
    }

    // Change the permissions of the actual file
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | // Read, write, and execute rights for the user
                  S_IRGRP | S_IWGRP |           // Read and write rights for the group
                  0;                            // No rights for others
    if (chmod(resolved_path, mode) == -1)
    {
        perror("Failed to change file permissions");
        return;
    }
    write(write_fd, options, strlen(options));
    // Write the result to the pipe
    char result[10000];

    ssize_t num_bytes = write(write_fd, result, 10000);
    if (num_bytes == -1)
    {
        perror("Failed to write to pipe");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_files = argc - 1;
    int pipes[num_files][2];

    // Create pipes for each file argument
    for (int i = 0; i < num_files; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("Failed to create pipe");
            return EXIT_FAILURE;
        }
    }
    pid_t child_pid;

    // Create child processes for each file argument
    for (int i = 0; i < num_files; i++)
    {
        child_pid = fork();

        if (child_pid == -1)
        {
            perror("Failed to fork child process");
            return EXIT_FAILURE;
        }
        else if (child_pid == 0)
        {
            // Child process
            close(pipes[i][0]); // Close the read end of the pipe

            // Determine the type of the file
            struct stat sb;
            if (lstat(argv[i + 1], &sb) == -1)
            {
                perror("lstat");
                return EXIT_FAILURE;
            }

            if (S_ISREG(sb.st_mode))
            {
                handle_regular_file(argv[i + 1], pipes[i][1]);
            }

            else if (S_ISDIR(sb.st_mode))
            {
                handle_directory(argv[i + 1], pipes[i][1]);
            }
            else if (S_ISLNK(sb.st_mode))
            {
                handle_symbolic_link(argv[i + 1], pipes[i][1]);
            }
            else
            {
                printf("Unknown file type: %s\n", argv[i + 1]);
            }

            close(pipes[i][1]); // Close the write end of the pipe
            return EXIT_SUCCESS;
        }
    }

    // Parent process
    for (int i = 0; i < num_files; i++)
    {
        int status;
        pid_t pid = waitpid(child_pid, &status, 0); // Wait for the child process to finish

        if (WIFEXITED(status))
        {
            printf("The process with PID %d has ended with the exit code %d\n", pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("The process with PID %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("The process with PID %d was stopped by signal %d\n", pid, WSTOPSIG(status));
        }
        else
        {
            printf("Unexpected status received for child process with PID %d\n", pid);
        }

        close(pipes[i][1]); // Close the write end of the pipe

        char options[OPTIONS];
        ssize_t num_bytes = read(pipes[i][0], options, sizeof(options) - 1);
        if (num_bytes == -1)
        {
            perror("Failed to read from pipe");
        }
        else
        {
            options[num_bytes] = '\0';
            // printf("Options for %s: %s\n", argv[i + 1], options);
        }

        close(pipes[i][0]); // Close the read end of the pipe
    }

    // Wait for child processes to finish

    return EXIT_SUCCESS;
}