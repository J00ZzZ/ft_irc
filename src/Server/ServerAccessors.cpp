#include "../../inc/Server.hpp"
#include "../../inc/utils.hpp"

int Server::getServerFd(void) const {
	return (this->_serverFd);
}

const std::vector<struct pollfd> &Server::getPollFds(void) const {
	return (this->_pollFds);
}

Client* Server::getClientByFd(int fd) const {
	for (std::size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getFd() == fd)
			return (_clients[i]);
	}
	return (NULL);
}

Client* Server::getClientByNick(std::string& nick) const {
	for (std::size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getNick() == nick)
			return (_clients[i]);
	}
	return (NULL);
}

const std::string & Server::getPassword(void) const {
	return (_password);
}
		
const std::vector<Channel *>	Server::getChannels(void) const {
	return (this->_channels);
}

const std::string & Server::getHostname(void) const {
	return (_hostname);
}

Bot * Server::getBot(void) const {
	return (_bot);
}

const std::string & Server::getBotName(void) const {
	return (_botName);
}

void Server::_rmClient(int fd) {
	Client * client = getClientByFd(fd);

	for (std::size_t i = 0; i < _channels.size(); ++i) {
		for (std::size_t j = 0; j < _channels[i]->getClients().size(); ++j) {
			if (_channels[i]->getClients()[j] == client) {
				_channels[i]->delClient(client);
				break;
			}
		}
	}

	for (std::size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == fd) {
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	for (std::size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->getFd() == fd) {
			delete _clients[i];
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
	_capDone.erase(fd);
}

Channel * Server::getChannel(std::string& channel_name) const {
	for (std::size_t i = 0; i < _channels.size(); i++) {
		if (channel_name == _channels[i]->getName())
			return (_channels[i]);
	}
	return (NULL);
}

void Server::addChannel(Channel* channel) {
	_channels.push_back(channel);
}