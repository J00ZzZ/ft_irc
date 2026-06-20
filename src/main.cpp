#include <iostream>
#include "../inc/Server.hpp"

Server* g_server = NULL; // -> global instance ptr for setting up signals

void handle_signal(int signum) {
	std::cout << YELLOW << "\n[INFO] Signal " << signum << " received. Cleaning up..." << RESET << std::endl;
	if (g_server)
		g_server->shutdown();
	std::exit(0);
}

int main(int ac, char **av) {

	if (ac != 3) {
		std::cout << "Valid exec.: ./ircserv <port> <password>" << std::endl;
		std::exit(1);
	}

	int port = std::atoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Invalid port number. Available ports range from 0 to 65535. Suggested port: 6667" << std::endl;
		std::exit(1);
	}
	
	std::string password = std::string(av[2]);
	if (password.empty()) {
		std::cout << "Password cannot be empty." << std::endl;
		std::exit(1);
	} else if (password.length() > 510) {
		std::cout << "Password must be at maxium 510 chars long (UTF-8)" << std::endl;
		std::exit(1);
	}

	try {
		Server serv = Server(port, password);
		g_server = &serv;

		std::signal(SIGINT, handle_signal);
		std::signal(SIGTERM, handle_signal);

		serv.startServ();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	return (0);
}
