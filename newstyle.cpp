#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>

#define PORT "1974"

/**********************************************************
  struct addrinfo {
               int              ai_flags;		multiple flags with OR
               int              ai_family;		specifies IP4/IP6 or auto	
               int              ai_socktype;	Stream or diagram == TCP or UDP
               int              ai_protocol;	protocoll or 0 for any
               socklen_t        ai_addrlen;		|
               struct sockaddr *ai_addr;		|>
               char            *ai_canonname;	|>		must be 0 or NULL
               struct addrinfo *ai_next;		|
           };
**********************************************************/
static
void	handle(int num, siginfo_t *inf, void *ctxt)
{
	if (num && (ctxt || !ctxt) && inf)
			std::cout << "Good Bye" << std::endl;
	exit(0);
}


int	main(void)
{
		struct sockaddr_storage		their_addr;
		socklen_t					addr_size;
		struct addrinfo 			hints, *res;
		int							sockfd, newfd;
		struct sigaction			sgl;

		sgl.sa_flags = SA_SIGINFO;
		sgl.sa_sigaction = &handle;
		sigaction(SIGINT, &sgl, NULL);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;		// AF_INET or AF_INET6 to force version
		hints.ai_socktype = SOCK_STREAM;	// TCP
		hints.ai_flags = AI_PASSIVE;		//system enters IP

		getaddrinfo(NULL, PORT, &hints, &res);

		//get socket fd
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
		{
				std::cout << "Error: could not open socket" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "Success: Socket fd is : " << sockfd << std::endl;

		//bind port to address
		if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
		{
				std::cout << "Error: failed to bind port to addr" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "Success: bound to Port #" << PORT << std::endl;

		//listen to incoming calls
		if (listen(sockfd, 5) < 0)
		{
				std::cout << "Error: failed to listen on port #" << PORT << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "Success: listen on Port #" << PORT << std::endl;

		//accept incoming calls and and assign new fd to the caller
		addr_size = sizeof(their_addr);
		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		if (newfd < 0)
		{
				std::cout << "Error: accept failed" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "Success: connection established, new fd is: " << newfd << std::endl;

		//read in a loop until client closes connection
		int		send_data, buflen;
		char	msg[1024];

		buflen = 1024;
		send_data = 1;
		while (1) //send_data > 0)
		{
			send_data = recv(newfd, (char *)msg, buflen, 0);
			std::cout << ":" << send_data << " >>> " << msg;
			send(newfd, ">>> OK <<<\n", 11, 0);
			memset(msg, 0, buflen);
		}
		if (send_data < 0)
			std::perror("receiv failed");
		freeaddrinfo(res);
		close(sockfd);
		close(newfd);
		return (0);
}

