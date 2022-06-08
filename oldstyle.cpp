#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#define PORT 1974

int	main(void)
{
		int		sfd;			//socket_fd
		int		nRet = 0;		//bind
		struct sockaddr_in srv;

		//get socket fd : domain, type, protocol
		sfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sfd < 0)
		{
				std::cout << "Socket not opened" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
		{
				std::cout << "socket succesfully opened" << std::endl;
				std::cout << "socket number is " << sfd << std::endl;
		}
		
		//init the sockaddr structure
		//set all to zero
		memset(&srv, 0, sizeof(srv));
		srv.sin_family = AF_INET;
		srv.sin_port = htons(PORT);			//convert from host byte order 2 network byte order
		srv.sin_addr.s_addr = inet_addr("127.0.0.1"); //localhost
		//srv.sin_addr.s_addr = INADDR_ANY; //macro for localhost equals the above

		//bind socket to local port
		//"assigning a name to a socket"
		nRet = bind(sfd, (sockaddr*)&srv, sizeof(sockaddr));
		if (nRet < 0)
		{
				std::cout << "Failed to bind to local port" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "SUCCESS bound to local port" << std::endl;
		// 
		// listen to request from client
		nRet = listen(sfd, 5);
		if (nRet < 0)
		{
				std::cout << "Failed to listen to local port" << std::endl;
				exit(EXIT_FAILURE);
		}
		else
				std::cout << "SUCCESS listening to local port" << std::endl;
		//
		// Keep waiting for new requests and proceed as per the request
		return (0);
}
