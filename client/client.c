#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#define SOCKETNAME	"ddns.sock"
#define BUFFERSIZE	1024

int main(int argc, char** argv)
{
	char buf[BUFFERSIZE];
	struct sockaddr_un addr;
	int fd, rc;
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket Error");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = PF_UNIX;
	strncpy(addr.sun_path, SOCKETNAME, strlen(SOCKETNAME));
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("Connect Error");
		return 1;
	}
	while ((rc = read(STDERR_FILENO, buf, sizeof(buf))) > 0)
	{
		if (write(fd, buf, rc) != rc)
		{
			if (rc > 0) fprintf(stderr, "Partial Write\n");
			else { perror("Write Error"); return 1;}
		}
	}
	return 0;
}


