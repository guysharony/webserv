#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SERVER_PORT		8082

#define TRUE             1
#define FALSE            0

int	main(int argc, char *argv[])
{
	int					len;
	int					rc;
	int					on = 1;
	int					listen_sd = -1;
	int					new_sd = -1;
	int					desc_ready;
	int					end_server = FALSE;
	int					compress_array = FALSE;
	int					close_conn;
	char					buffer[80];
	struct sockaddr_in		addr;
	std::vector<pollfd> 	fds;
	int					timeout;
	int					nfds = 1;
	int					current_size = 0;
	int					i;
	int					j;

	if (argc != 3)
	{
		perror("IP address or Port not specified.");
		exit(-1);
	}

	/*************************************************************/
	/* Create an AF_INET stream socket to receive incoming      */
	/* connections on                                            */
	/*************************************************************/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	/*************************************************************/
	/* Allow socket descriptor to be reuseable                   */
	/*************************************************************/
	if (setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	if (ioctl(listen_sd, FIONBIO, (char *)&on) < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}


	/*************************************************************/
	/* Bind the socket                                           */
	/*************************************************************/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port        = htons(atoi(argv[2]));
	if (bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Set the listen back log                                   */
	/*************************************************************/
	rc = listen(listen_sd, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Initialize the pollfd structure                           */
	/*************************************************************/
	struct pollfd new_pollfd;
	new_pollfd.fd = listen_sd;
	new_pollfd.events = POLLIN;
	fds.push_back(new_pollfd);
	fcntl(listen_sd, F_SETFL, O_NONBLOCK);

	/*************************************************************/
	/* Initialize the timeout to 3 minutes. If no                */
	/* activity after 3 minutes this program will end.           */
	/* timeout value is based on milliseconds.                   */
	/*************************************************************/
	timeout = (3 * 60 * 1000);

	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	while (end_server == FALSE) {
		/***********************************************************/
		/* Call poll() and wait 3 minutes for it to complete.      */
		/***********************************************************/
		printf("Waiting on poll()...\n");
		rc = poll(fds.data(), fds.size(), timeout);

		/***********************************************************/
		/* Check to see if the poll call failed.                   */
		/***********************************************************/
		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}

		/***********************************************************/
		/* Check to see if the 3 minute time out expired.          */
		/***********************************************************/
		if (rc == 0)
		{
			printf("  poll() timed out.  End program.\n");
			break;
		}

		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/
			if (it->revents == 0)
				continue;

			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
			if(it->revents != POLLIN)
			{
				printf("  Error! revents = %d\n", it->revents);
				end_server = TRUE;
				break;
			}

			std::cout << std::endl << "pollfd#" << it->fd << "->revents: " << it->revents << std::endl;

			if (it->fd == listen_sd)
			{
				/*******************************************************/
				/* Listening descriptor is readable.                   */
				/*******************************************************/
				printf("  Listening socket is readable\n");

				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do
				{
					/*****************************************************/
					/* Accept each incoming connection. If               */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other              */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					/*****************************************************/
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0)
					{
						break;
					}

					/*****************************************************/
					/* Add the new incoming connection to the            */
					/* pollfd structure                                  */
					/*****************************************************/
					printf("  New incoming connection - %d\n", new_sd);;

					struct pollfd new_pollfd;
					new_pollfd.fd = new_sd;
					new_pollfd.events = POLLIN;
					fds.push_back(new_pollfd);
					fcntl(new_sd, F_SETFL, O_NONBLOCK);

					/*****************************************************/
					/* Loop back up and accept another incoming          */
					/* connection                                        */
					/*****************************************************/
				} while (new_sd != -1);
			}

			/*********************************************************/
			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			/*********************************************************/

			else
			{
				printf("  Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do {
					/*****************************************************/
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					/*****************************************************/
					rc = recv(it->fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						break;
					}

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					len = rc;
					printf("  %d bytes received\n", len);

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					rc = send(it->fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("  send() failed");
						close_conn = TRUE;
						break;
					}

				} while(TRUE);

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn)
				{
					close(it->fd);
					it->fd = -1;
					compress_array = TRUE;
				}

			}	/* End of existing connection is readable             */
		}	/* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < fds.size(); i++)
			{
				if (fds[i].fd == -1)
				{
					for (j = i; j < fds.size() - 1; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}

					i--;
					fds.pop_back();
				}
			}
		}

	} /* End of serving running.    */

	/*************************************************************/
	/* Clean up all of the sockets that are open                 */
	/*************************************************************/
	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}