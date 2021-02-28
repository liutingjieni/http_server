/*************************************************************************
	> File Name: clent.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月24日 星期五 14时52分04秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <assert.h>

#define IP "127.0.0.1"

#define PORT 8888
    char pack[100];

void *run(void *clifd)
{
    while(1) {
        recv(*(int *)clifd, &pack, sizeof(pack), 0);
        printf("%s\n", pack);
    }
}

int main()
{
    int cli_fd;
    struct sockaddr_in serv_addr;
    
    cli_fd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(IP);
        
    /*struct timeval timeout;
    timeout.tv_sec = time(NULL);
    timeout.tv_usec = 0;
    socklen_t len = sizeof(timeout);
    int ret = setsockopt(cli_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
    assert(ret!=-1);
*/
    int ret = connect(cli_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)    
    {
        perror("connect 出现问题");
        exit(-1);            
    }
    printf("客户端启动成功");
    pthread_t tid;
    pthread_create(&tid, NULL, run, (void *)&cli_fd);
    while(1) {
        scanf("%s", pack);
        send(cli_fd, &pack, sizeof(pack),0);
    }
}
