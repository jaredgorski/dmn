#include <errno.h>
#include <inttypes.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main
(int argc, char* argv[])
{
	FILE *fp;
	pid_t process_id = 0;
	pid_t sid = 0;

	/* Parse arguments */
	if (argc < 2) {
		printf("Please provide arguments.\n");
		exit(1);
	}

	int opt;
	char *command;
	char *logfile;
	int iterations;

	int infinite = 1;
	int log = 0;

	while ((opt = getopt(argc, argv, ":c:f:i:")) != -1) {
		switch (opt) {
			case 'c':
				command = optarg;
				break;
			case 'f':
				log = 1;
				logfile = optarg;
				break;
			case 'i':
				{
					infinite = 0;
					long num = strtoimax(optarg, NULL, 10);
					if (num == UINTMAX_MAX && errno == ERANGE) {
						printf("Could not convert -i to number.");
						exit(1);
					}

					iterations = num;
				}
				break;
			case ':':
				{
					printf("Value not provided for option: %c\n", optopt);
					exit(1);
				}
				break;
			case '?':
				{
					printf("Unknown option: %c\n", optopt);
					exit(1);
				}
				break;
		}
	}

	/* Fork a child process for the daemon */
	process_id = fork();

	if (process_id < 0) {
		printf("[dmn] INTERNAL ERR: Fork failure.\n");
		exit(1);
	}

	if (process_id > 0) {
		printf("[dmn] Initiated with pid: %d \n", process_id);
		exit(0);
	}

	umask(0);

	sid = setsid();

	if (sid < 0) {
		printf("[dmn] INTERNAL ERR: setsid failure.\n");
		exit(1);
	}

	/* Change working dir to root */
	chdir("/");

	/* Close IO */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	if (log) {
		/* Open file pointer to logfile */
		fp = fopen(logfile, "w+");
	}

	/* Set run flag */
	int run = 1;

	while (run) {
		/* Throttle 1s */
		sleep(1);

		if (log) {
			fprintf(fp, "[dmn] New iteration. Command: %s\n", command);
			fflush(fp);
		}

		FILE *pp;
		char path[1035];

		/* Execute given command and pipe output to stream */
		pp = popen(command, "r");

		if (pp == NULL) {
			fprintf(fp, "[dmn] Failed to run command: %s\n", command);
			exit(1);
		}

		if (log) {
			/* Log command output */
			while (fgets(path, sizeof(path), pp) != NULL) {
				fprintf(fp, "%s", path);
			}
		}

		pclose(pp);

		if (!infinite) {
			if (!--iterations) {
				run = 0;
			}
		}
	}

	if (log) {
		fclose(fp);
	}

	return 0;
}
