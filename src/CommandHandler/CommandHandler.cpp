#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"

CommandHandler::CommandHandler(Server &server) : m_Server(server) { }

CommandHandler::CommandHandler(const CommandHandler &other) :m_Server(other.m_Server) { }  

CommandHandler::~CommandHandler(void) { }

const CommandHandler	&CommandHandler::operator = (const CommandHandler &other) {
	(void) other;
	return (*this);
}

void CommandHandler::_setClientNickname(int fd, std::vector<std::string> & cmd) {
	if (cmd.size() > 1) {
		m_Server.getClientByFd(fd)->setNick(cmd[1]);
		m_Server.getClientByFd(fd)->setNickReceived();
	}
}

void CommandHandler::_setClientUsername(int fd, std::vector<std::string> & cmd) {
	if (cmd.size() > 1) {
		m_Server.getClientByFd(fd)->setUsername(cmd[1]);
		m_Server.getClientByFd(fd)->setUnameReceived();
	}
}

std::vector<std::string>	CommandHandler::_splitCmd(std::string& cmd) {
	std::vector<std::string> res;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		res.push_back(token);
		token.clear();
	}
	return (res);
}

/**
 * Parses & processes a command input from a client.
 *
 * @param cmd The command string received from the client.
 * @param fd The file descriptor representing the client connection.
 * 
 **/
void CommandHandler::processInput(std::string &cmd, int fd) {
	if (cmd.empty())
		return;

	std::size_t start = cmd.find_first_not_of("\t\v ");
	if (start != std::string::npos)
		cmd = cmd.substr(start);

	std::vector<std::string> cmd_vec = _splitCmd(cmd);
	if (cmd_vec.empty())
		return;

	Client *client = m_Server.getClientByFd(fd);
	if (!client)
		return;

	std::string cmd_name = cmd_vec[0];

	if (client->getAuth() == false) {
		if (cmd_name == "PASS" || cmd_name == "pass")
			m_Server.handlePasswordAuth(fd, cmd_vec);
		else if (cmd_name == "NICK" || cmd_name == "nick")
			_setClientNickname(fd, cmd_vec);
		else if (cmd_name == "USER" || cmd_name == "user")
			_setClientUsername(fd, cmd_vec);
		else if (cmd_name == "CAP" || cmd_name == "cap")
			m_Server.sendResponse(m_Server.getHostname() + " CAP * LS :\r\n", fd);
		else
			m_Server.sendResponse(ERR_NOTREGISTERED(m_Server.getHostname(), "*"), fd);
		m_Server.tryRegister(fd);
	} else {
		if (cmd_name == "PASS" || cmd_name == "pass")
			m_Server.sendResponse(ERR_ALREADYREGISTRED(m_Server.getHostname(), client->getNick()), fd);
		else if (cmd_name == "USER" || cmd_name == "user")
			m_Server.sendResponse(ERR_ALREADYREGISTRED(m_Server.getHostname(), client->getNick()), fd);
		else if (cmd_name == "NICK" || cmd_name == "nick")
			_setClientNickname(fd, cmd_vec);
		else if (cmd_name == "JOIN" || cmd_name == "join")
			_handleJoinCmd(fd, cmd_vec);
		else if (cmd_name == "MSG" || cmd_name == "msg" || cmd_name == "PRIVMSG" || cmd_name == "privmsg")
			_handleMsgCmd(fd, cmd_vec);
		else if (cmd_name == "INVITE" || cmd_name == "invite")
			_handleInviteCmd(fd, cmd_vec);
		else if (cmd_name == "TOPIC" || cmd_name == "topic")
			_handleTopicCmd(fd, cmd_vec);
		else if (cmd_name == "MODE" || cmd_name == "mode")
			_handleModeCmd(fd, cmd_vec);
		else if (cmd_name == "KICK" || cmd_name == "kick")
			_handleKickCmd(fd, cmd_vec);
		else if (cmd_name == "PING" || cmd_name == "ping") {
				if (cmd_vec.size() > 2)
					m_Server.sendResponse(RPL_PONG("ft_irc", cmd_vec[2]), fd);
				else {
					m_Server.sendResponse(RPL_PONG("ft_irc", std::string("")), fd);
				}
			}
		else if (cmd_name == "WHO" || cmd_name == "who") // -> for now
			return ;
		else if (cmd_name == "SENDFILE" || cmd_name == "sendfile") {
			_handleSendFile(fd, cmd_vec);
		} else if (cmd_name == "RECFILE" || cmd_name == "recfile") {
			_handleRecFile(fd, cmd_vec);
		}
		else {
			m_Server.sendResponse(ERR_UNKNOWNCOMMAND(m_Server.getHostname(), client->getNick(), cmd_name), fd);
		}
	}
}
