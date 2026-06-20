#include "../../inc/Server.hpp"
#include "../../inc/utils.hpp"

/**
 * Handles new incoming client connections.
 *
 * Accepts a new client connection on the server's listening socket.
 * If the connection is successful, it creates a new Client object for the client,
 * inits its file descriptor and hostname, and adds it to the list of clients.
 * Also registers the client's socket for polling and logs the new connection.
 * If the acceptance of the client fails, it logs an error.
 */
void Server::_handleNewConn(void) {
	struct sockaddr_in client_addr;
	
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &client_len);

	if (client_fd < 0) {
		perror("accept");
		return;
	}

	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;

	_clients.push_back(new Client());
	_clients.back()->setFd(client_fd);
	_clients.back()->setHostname(inet_ntoa(client_addr.sin_addr));
	_pollFds.push_back(client_pollfd);
	_capDone[client_fd] = false;

	char * time_str = get_time_string();
	std::cout << GREEN << "[" << time_str << "]" << RESET
	    	<< " NEW CLIENT CONNECTED, client fd: " << client_fd << std::endl;
}

/**
 * Handles client disconnections.
 *
 * Prints a message to the console with the client file descriptor and
 * removes the client from the active client list.
 *
 * @param processed_fd The file descriptor representing the client connection.
 */
void Server::_handleClientDisconnection(int processed_fd) {
	char * time_str = get_time_string();

	std::cout << GREEN << "[" << time_str << "]" << RESET
		<< " CLIENT DISCONNECTED, client fd: " << processed_fd << std::endl;
	_rmClient(processed_fd);
	close(processed_fd);
}

/**
 * Processes incoming messages from a client.
 *
 * Reads data from the client's file descriptor into a buffer. If data is received,
 * it is added to the client's buffer. When a complete command is detected (terminated
 * by a newline), the command is processed by the command handler. If no data is received
 * (client disconnected), the client is removed. Handles any errors in receiving data.
 *
 * @param index The index in the poll file descriptor list corresponding to the client.
 */
void Server::_handleClientMessage(std::size_t index) {
	char* time_str;
	int fd = _pollFds[index].fd;
	Client* client = getClientByFd(fd);
	if (!client)
		return;

	char buf[IRC_MESSAGE_BUFFERSIZE];
	std::memset(buf, 0, IRC_MESSAGE_BUFFERSIZE);

	ssize_t bytes = recv(fd, buf, sizeof(buf) - 1, 0); // MSG_PEEK for testing chunks sent
	if (bytes < 0) {
		perror("recv");
		return;
	}
	if (bytes == 0) {
		_handleClientDisconnection(fd);
		return;
	}

	client->setBuf(std::string(buf, bytes));
	std::string& buffer = client->getBuf();

	std::size_t pos;
	while ((pos = buffer.find("\r\n")) != std::string::npos) {
		std::string line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);

		if (!line.empty()) {
			time_str = get_time_string();
			std::cout << GREEN << "[" << time_str << "]" << RESET
					  << " Received data from fd " << fd << ": " << line << std::endl;

			m_commandHandler.processInput(line, fd);
		}
	}
}
