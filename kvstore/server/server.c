#include "server.h"

#define PORT 9899

B_tree *tree;
static pthread_mutex_t mutex;
static pthread_mutex_t wrt;
static int rcount = 0;

threadpool_t *pool;

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
	init_server(true);

    /* master file descriptor list for select() */
    fd_set master;
    /* temp file descriptor list for select() */
    fd_set read_fds;
    /* server address */
    struct sockaddr_in serveraddr;
    /* client address */
    struct sockaddr_in clientaddr;
    /* maximum file descriptor number */
    int fdmax;
    /* listening socket descriptor */
    int listener;
    /* newly accept()ed socket descriptor */
    int newfd;
    /* buffer for client data */
    char buf[BUF_LEN];
    int nbytes;
    /* for setsockopt() SO_REUSEADDR, below */
    int yes = 1;
    int addrlen;
    int i, j = 0;
    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    /* get the listener */
    
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    printf("(info) server socket is OK...\n");
    /*address already in use" error message */
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    /* bind */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);
    if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    printf("(info) server bind is OK...\n");
    /* listen */
    if (listen(listener, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("(info) server listen is OK...\n");
    /* add the listener to the master set */
    FD_SET(listener, &master);
    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/
    
    /*create thread pool*/
    pool = threadpool_create(THREAD, SIZE, 0);

    /* loop */
    for (;;)
    {
        /* copy it */
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(1);
        }
        /* printf("select() is OK...\n");*/
        /*run through the existing connections looking for data to be read*/
        for (i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { /* we got one... */
                if (i == listener)
                {
                    /* handle new connections */
                    addrlen = sizeof(clientaddr);
                    if ((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        printf("(info) accept is OK...\n");
                        FD_SET(newfd, &master); /* add to master set */
                        if (newfd > fdmax)
                        { /* keep track of the maximum */
                            fdmax = newfd;
                        }
                        printf("New connection from %s on socket %d\n",  inet_ntoa(clientaddr.sin_addr), newfd);
                    }
                }
                else 
                {
                    char* buf = (char*)malloc(sizeof(char)*BUF_LEN);
                    if ((nbytes = recv(i, buf, BUF_LEN*sizeof(char), 0)) <= 0)
                    {
                        /* got error or connection closed by client */
                        if (nbytes == 0){
                            /* connection closed */
                            printf("(info) client %d is closed\n", i);
                        }
                        else{
                            /*perror("recv");*/
                            printf("(info) client %d is disconnected\n",i);
                        }
                        /* remove from master set */
                        FD_CLR(i, &master);
                        /* close it... */
                        close(i);
                        /* free buffer*/
                        free(buf);
                    }
                    else{
                        buf[nbytes] = '\0';
                        threadpool_add(pool, &handle_client, (void*)init_param(i, buf), 0);
                    }
                }
            }
        }
    }
    for(int i = 0; i < fdmax; i++){
        if(FD_ISSET(i, &master)){
            close(i);
        }
    }
    threadpool_destroy(pool, 0);
    close_server();
    return 0;
}


void handle_client(void* args)
{
    param_t* p = (param_t*)args;
    char* buf = p->buf, *msg;
    /* handle data from a client */
    msg = process_request(buf);
    if (send(p->fd, msg, BUF_LEN *sizeof(char), 0) <= 0){
        perror("send");
    }
    free(msg);
    free(buf);
    free(p);
}


char *process_request(char *request)
{
	char *response = (char *)malloc(BUF_LEN * sizeof(char));
	char p1[ARG_L], p2[KEY_L], p3[VAL_L];
	sscanf(request, "%s %s %s", p1, p2, p3);
	if (!strcmp(p1, "set"))
	{
		thread_safe_insert(tree, response, p2, p3);
	}
	else if (!strcmp(p1, "get"))
	{
		thread_safe_search(tree, response, p2);
	}
	else if (!strcmp(p1, "del"))
	{
		thread_safe_delete(tree, response, p2);
	}
	else
	{
		strcpy(response, "error");
	}
	return response;
}

void thread_safe_insert(B_tree *tree, char *response, char *key, char *value)
{
	pthread_mutex_lock(&wrt);
	insert0(tree, key, value);
	pthread_mutex_unlock(&wrt);
	strcpy(response, "OK");
}

void thread_safe_search(B_tree *tree, char *response, char *key)
{
	Kv_pair *ret;
	pthread_mutex_lock(&mutex);
	rcount = rcount + 1;
	if (rcount == 1)
		pthread_mutex_lock(&wrt);
	pthread_mutex_unlock(&mutex);
	ret = search(tree, key);
	pthread_mutex_lock(&mutex);
	rcount = rcount - 1;
	if (rcount == 0)
		pthread_mutex_unlock(&wrt);
	pthread_mutex_unlock(&mutex);
	if (ret != NULL)
	{
		strcpy(response, ret->val);
	}
	else
	{
		strcpy(response, "not found!");
	}
}

void thread_safe_delete(B_tree *tree, char *response, char *key)
{
	bool ret;
	pthread_mutex_lock(&wrt);
	ret = remove0(tree, key);
	pthread_mutex_unlock(&wrt);
	if (ret == true)
	{
		strcpy(response, "OK");
	}
	else
	{
		strcpy(response, "not exist!");
	}
}

void signal_handler(int num)
{
	printf("\n(info) server terminate");
	close_server();
	exit(num);
}

void init_server(bool is_load)
{
	if (is_load)
	{
		tree = create_loading_btree(CONFIG_FILE);
	}
	else
	{
		tree = create_empty_btree(STORAGE_FILE);
	}

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&wrt, NULL);
}

void close_server()
{
	pthread_mutex_destroy(&wrt);
	pthread_mutex_destroy(&mutex);
	destroy_btree(tree, CONFIG_FILE);
}
