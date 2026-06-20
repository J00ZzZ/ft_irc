#pragma once
#ifndef _server_hpp_
# define _server_hpp_

# include <iostream>
# include <vector>
# include <map>
# include <cstdlib>
# include <stdexcept>
# include <string>
# include <cstring>
# include <cstdio>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <sstream>
# include <ctime>
# include <csignal>

# include "CommandHandler.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Bot.hpp"

/* SECTION: Macros */
# define YELLOW "\033[33m"
# define PURPLE "\033[35m"
# define CYAN "\033[36m"
# define BLUE "\033[34m"
# define GREEN "\033[32m"
# define RESET "\033[0m"

# define IRC_MESSAGE_BUFFERSIZE 512

class Server {
private:
	CommandHandler							m_commandHandler;

	int 									_port;
	std::string 							_password;
	int 									_serverFd; // > listening socket
	struct sockaddr_in 						_serverAddress;
	std::string 							_hostname;
	std::vector<struct pollfd> 				_pollFds; // -> active socket list
	std::vector<Client *> 					_clients; // -> registered clients 
	std::vector<Channel *>					_channels; // -> vector of registered channels on the server
	std::map<int, bool> 					_capDone; // -> handshake indicator boolean map
	Bot										*_bot;
	std::string								_botName;

	/* Event handlers */
	void 									_handleClientDisconnection(int processed_fd);
	void 									_handleNewConn(void);
	void 									_handleClientMessage(std::size_t index);

	
	/* Cleanup utils */
	void 									_rmClient(int fd);
	
public:
	Server(int port, std::string password);
	~Server(void);
	void 									shutdown(void);
	
	/* Accessors */
	int										getServerFd(void) const;
	const std::vector<struct pollfd>		&getPollFds(void) const;
	Client * 								getClientByFd(int fd) const;
	Client *								getClientByNick(std::string& nick) const;
	Channel *								getChannel(std::string& channel_name) const;
	const std::string & 					getHostname(void) const;
	const std::string & 					getPassword(void) const;
	const std::vector<Channel *>			getChannels(void) const;
	Bot *									getBot(void) const;
	const std::string &						getBotName(void) const;
	void									addChannel(Channel *);
	
	/* Accessible from CommandParser */
	void 									handlePasswordAuth(int processed_fd, std::vector<std::string> cmd_vec);
	void 									tryRegister(int fd);
	void 									dispatchToChannel(Channel *, std::string, Client *);
	void 									sendNameReply(Client* client, Channel* channel);
	void 									sendResponse(std::string response, int fd);
	
	/* Main server loop */
	void 									startServ(void);
};

#endif
