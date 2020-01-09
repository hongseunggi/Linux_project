/* myshell program, by hong seung gi. tmdrl5661@naver.com. 2018-11-19*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/wait.h>
#define B_ACK "&"
#define RE_DIR ">"
#define APPEND ">>"
#define R_DIR "<"


void cmd_help()// ������ �����ִ� �Լ�
{
	printf("*******Seung Gi's Simple Shell*******\n");
	printf("You can use it just as the conventional shell\n");
	printf("Some examples of the built-in commands\n");
	printf("cd\t: change drectory\n");
	printf("exit\t: exit this shell\n");
	printf("help\t: show this help\n");
	printf("*************************************\n");
}
bool cmd_cd(int token_count, char* tokens[])// ���� ��ɾ��� cd ���� �Լ�
{
	if (token_count == 1) chdir(getenv("HOME")); //cd�� �Է� �Ǹ� Ȩ ���͸��� �̵�
	else if (token_count == 2) chdir(tokens[1]); //�ΰ��� ���ڰ� �ԷµǾ����� �ڿ� �Էµ� ��
	�͸��� ��
	else printf("ERROR\n");
	return true;
}
int tokenizer(char* buf, char* delims, char* tokens[], int maxTokens)//��ū�� �����ϰ� ��ū��
������ ���ϴ� �Լ�
{
	char* token;
	int token_count = 0;

	token = strtok(buf, delims); //strtok�� �̿��Ͽ� ù��° ��ū ����
	while (token != NULL && token_count < maxTokens)//�ι�° �̻� ��ū ����
	{
		tokens[token_count++] = token;
		token = strtok(NULL, delims);
	}

	tokens[token_count] = NULL;
	return token_count;//��ū ���� ����


}

bool ex(char* line)
{

	char* tokens[1024];
	char* tmp[1024];
	char* delims = " \n";
	bool back = false; //��׶��� ���¸� üũ�� bool�� ����
	pid_t child;
	int status;
	int token_count = tokenizer(line, delims, tokens, sizeof(tokens) / sizeof(char*));
	int fd;
	int flag = 0; //�����̷��� ���¸� üũ�� flag
	int i, j;

	for (i = 0;i < token_count;i++)//�����̷��ǰ� ��׶��� ���� üũ
	{

		if (!strcmp(tokens[i], RE_DIR))//">"�� �ִ��� üũ ���� flag�� ����
		{
			flag = 1;
			break;
		}
		else if (!strcmp(tokens[i], APPEND)) //">>"�� �ִ��� üũ ���� flag�� ����
		{
			flag = 2;
			break;
		}
		else if (!strcmp(tokens[i], R_DIR)) //"<"�� �ִ��� üũ ���� flag�� ����
		{
			flag = 3;
			break;
		}
		else if (!strcmp(tokens[i], B_ACK))
		{
			back = true;
			break;
		}
		else if (!strcmp(tokens[i], "cd"))
		{
			return cmd_cd(token_count, tokens);

		}
	}

	if ((child = fork()) < 0)//fork ������ ���
	{
		printf("failed to fork");
		return false;
	}


	else if (child == 0)//�ڽ� ���μ���
	{
		for (j = 0; j < i; j++)
		{
			tmp[j] = tokens[j];//tmp�迭�� token�� i��ŭ ���� i�� �����̷��� �����ڳ� ��
			�׶��� �����ڸ� ������ ���� üũ �������� ������ ���� ��ŭ�� ũ�⸦ ������
		}

		if (flag == 1)//flag�� 1�϶�
		{
			fd = open(tokens[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0664);
			if (fd < 0) error("������ ���µ� �����Ͽ����ϴ�.");
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);//����� ������
			close(fd);
		}
		else if (flag == 2)//flag�� 2�϶�
		{
			fd = open(tokens[i + 1], O_APPEND | O_WRONLY | O_CREAT, 0664);
			if (fd < 0) error("������ ���µ� �����Ͽ����ϴ�.");
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (flag == 3)//flag�� 3�϶�
		{
			fd = open(tokens[i + 1], O_RDONLY);
			if (fd < 0) error("������ ���µ� �����Ͽ����ϴ�.");
			close(STDIN_FILENO);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		execvp(tokens[0], tmp);//token�� ù��° ��ɾ�� tmp�� exec
		exit(0);
	}
	else
	{
		if (back == false) waitpid(child, &status, 0);//back�� false�̸� wait
		else
			waitpid(WNOHANG, &status, 0); //WNOHANG�� wait���� �ʰڴٴ� �÷��� ��back�� true�̸� wait���� �ʴ´�


	}



	return true;
}

int main(void)
{
	char line[1024];

	while (1)
	{

		printf("Seung Gi's Shell%s $", get_current_dir_name());
		fgets(line, sizeof(line) - 1, stdin);
		if (!(strcmp(line, "exit\n")))
			break;
		if (!(strcmp(line, "help\n")))
			cmd_help();
		ex(line);
	}
	return 0;
}
