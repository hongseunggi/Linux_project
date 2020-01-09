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


void cmd_help()// 도움말을 보여주는 함수
{
	printf("*******Seung Gi's Simple Shell*******\n");
	printf("You can use it just as the conventional shell\n");
	printf("Some examples of the built-in commands\n");
	printf("cd\t: change drectory\n");
	printf("exit\t: exit this shell\n");
	printf("help\t: show this help\n");
	printf("*************************************\n");
}
bool cmd_cd(int token_count, char* tokens[])// 내부 명령어인 cd 수행 함수
{
	if (token_count == 1) chdir(getenv("HOME")); //cd만 입력 되면 홈 디렉터리로 이동
	else if (token_count == 2) chdir(tokens[1]); //두개의 인자가 입력되었으면 뒤에 입력된 디렉
	터리로 이
	else printf("ERROR\n");
	return true;
}
int tokenizer(char* buf, char* delims, char* tokens[], int maxTokens)//토큰을 분할하고 토큰의
갯수를 구하는 함수
{
	char* token;
	int token_count = 0;

	token = strtok(buf, delims); //strtok를 이용하여 첫번째 토큰 추출
	while (token != NULL && token_count < maxTokens)//두번째 이상 토큰 추출
	{
		tokens[token_count++] = token;
		token = strtok(NULL, delims);
	}

	tokens[token_count] = NULL;
	return token_count;//토큰 갯수 리턴


}

bool ex(char* line)
{

	char* tokens[1024];
	char* tmp[1024];
	char* delims = " \n";
	bool back = false; //백그라운드 상태를 체크할 bool형 변수
	pid_t child;
	int status;
	int token_count = tokenizer(line, delims, tokens, sizeof(tokens) / sizeof(char*));
	int fd;
	int flag = 0; //리다이렉션 상태를 체크할 flag
	int i, j;

	for (i = 0;i < token_count;i++)//리다이렉션과 백그라운드 상태 체크
	{

		if (!strcmp(tokens[i], RE_DIR))//">"가 있는지 체크 한후 flag값 변경
		{
			flag = 1;
			break;
		}
		else if (!strcmp(tokens[i], APPEND)) //">>"가 있는지 체크 한후 flag값 변경
		{
			flag = 2;
			break;
		}
		else if (!strcmp(tokens[i], R_DIR)) //"<"가 있는지 체크 한후 flag값 변경
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

	if ((child = fork()) < 0)//fork 실패할 경우
	{
		printf("failed to fork");
		return false;
	}


	else if (child == 0)//자식 프로세스
	{
		for (j = 0; j < i; j++)
		{
			tmp[j] = tokens[j];//tmp배열에 token을 i만큼 복사 i는 리다이렉션 연산자나 백
			그라운드 연산자를 만나면 위에 체크 과정에서 연산자 이전 만큼의 크기를 가진다
		}

		if (flag == 1)//flag가 1일때
		{
			fd = open(tokens[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0664);
			if (fd < 0) error("파일을 여는데 실패하였습니다.");
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);//입출력 재지정
			close(fd);
		}
		else if (flag == 2)//flag가 2일때
		{
			fd = open(tokens[i + 1], O_APPEND | O_WRONLY | O_CREAT, 0664);
			if (fd < 0) error("파일을 여는데 실패하였습니다.");
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (flag == 3)//flag가 3일때
		{
			fd = open(tokens[i + 1], O_RDONLY);
			if (fd < 0) error("파일을 여는데 실패하였습니다.");
			close(STDIN_FILENO);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		execvp(tokens[0], tmp);//token의 첫번째 명령어로 tmp를 exec
		exit(0);
	}
	else
	{
		if (back == false) waitpid(child, &status, 0);//back이 false이면 wait
		else
			waitpid(WNOHANG, &status, 0); //WNOHANG은 wait하지 않겠다는 플래그 즉back이 true이면 wait하지 않는다


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
