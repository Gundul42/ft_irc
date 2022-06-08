#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

#define PORT "1974"

int	main(void)
{
		struct sockaddr_storage		their_addr;
		socklen_t					addr_size;
		struct addrinfo 			hints, *res;
		int							sockfd, newfd;

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
				std::cout << "Success: connection established new fd is: " << newfd << std::endl;

		return (0);
}
