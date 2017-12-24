#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define READ 0
#define WRITE 1
#define MAXLINE 1024

void handler();

int main() {

	char amper;
	char str[MAXLINE];
	char *command1, *command2;
	int fd[2], pid;
	int status;

	signal(SIGINT, handler); // Ctrl + C 처리

	while (1) {

		printf("[shell]");

		fgets(str, sizeof(str), stdin); // 명령 입력

		str[strlen(str) - 1] = '\0';

		if (strcmp(str, "exit") == 0) { // exit
			exit(0);
		}

		amper = str[strlen(str) - 1];

		// 후면 처리
		if (amper == '&') {
			str[strlen(str) - 1] = ' ';
		}

		pid = fork();

		if (pid == 0) {

			if (strchr(str, '|') != NULL) { 	 // 파이프 사용하는 경우
				command1 = strtok(str, "|");
				command2 = strtok(NULL, "|");

				pipe(fd);

				if (fork() == 0) { // 자식 프로세
					char *tmp[MAXLINE];
					int i = 0;
					tmp[i] = strtok(command1, " ");
					i++;

					while (1) {
						tmp[i] = strtok(NULL, " ");
						if (tmp[i] == NULL) {
							break;
						}

						if (strcmp(tmp[i], ">") == 0) { // 입력 재지정
							tmp[i] = strtok(NULL, " ");
							int fd = open(tmp[i], O_CREAT | O_TRUNC | O_WRONLY,
									0600);
							tmp[i] = NULL;
							dup2(fd, 1);   // 파일을 표준 출력에 복제
							close(fd);
							continue;
						}

						else if (strcmp(tmp[i], "<") == 0) { // 출력 재지정
							tmp[i] = strtok(NULL, " ");
							int fd = open(tmp[i], O_RDONLY, 0600);
							tmp[i] = NULL;
							dup2(fd, 0);   // 파일을 표준 출력에 복제
							close(fd);
							continue;
						}

						i++;
					}

					close(fd[READ]);
					dup2(fd[WRITE], 1); // 쓰기용 파이프를 표준 출력에 복제
					close(fd[WRITE]);
					execvp(tmp[0], tmp);
					perror("pipe");
				}

				else { // 부모 프로세스
					char *tmp[MAXLINE];
					int i = 0;
					tmp[i] = strtok(command2, " ");
					i++;

					while (1) {
						tmp[i] = strtok(NULL, " ");
						if (tmp[i] == NULL) {
							break;
						}

						if (strcmp(tmp[i], ">") == 0) {
							tmp[i] = strtok(NULL, " ");
							int fd = open(tmp[i], O_CREAT | O_TRUNC | O_WRONLY,
									0600);
							tmp[i] = NULL;
							dup2(fd, 1);   // 파일을 표준 출력에 복제
							close(fd);
							continue;
						}

						else if (strcmp(tmp[i], "<") == 0) {
							tmp[i] = strtok(NULL, " ");
							int fd = open(tmp[i], O_RDONLY, 0600);
							tmp[i] = NULL;
							dup2(fd, 0);   // 파일을 표준 출력에 복제
							close(fd);
							continue;
						}

						i++;
					}

					close(fd[WRITE]);
					dup2(fd[READ], 0); // 읽기용 파이프를 표준 입력에 복제
					close(fd[READ]);
					execvp(tmp[0], tmp);
					perror("pipe");
				}
			}

			else { // 파이프를 사용하지 않는 경우
				char *tmp[MAXLINE];
				int i = 0;
				tmp[i] = strtok(str, " ");
				i++;

				while (1) {
					tmp[i] = strtok(NULL, " ");
					if (tmp[i] == NULL) {
						break;
					}

					if (strcmp(tmp[i], ">") == 0) {
						tmp[i] = strtok(NULL, " ");
						int fd = open(tmp[i], O_CREAT | O_TRUNC | O_WRONLY,
								0600);
						tmp[i] = NULL;
						dup2(fd, 1); // 파일을 표준 출력에 복제
						close(fd);
						continue;
					}

					else if (strcmp(tmp[i], "<") == 0) {
						tmp[i] = strtok(NULL, " ");
						int fd = open(tmp[i], O_RDONLY, 0600);
						tmp[i] = NULL;
						dup2(fd, 0); // 파일을 표준 출력에 복제
						close(fd);
						continue;
					}

					i++;
				}
				execvp(tmp[0], tmp);
			}
		}

		else { // 후면 실행 시 부모의 처
			if (amper != '&')
				wait(&status);
		}

	}

}

void handler() {

}
