#pragma once
#ifndef _command_handler_hpp_
# define _command_handler_hpp_

# include <string>
# include <cstddef>
# include <vector>
# include <map>

# include "Channel.hpp"

class Server;

class CommandHandler {
private:
	Server								&m_Server; // -> Core Server Object Reference

	/* SECTION: Private utils */
	std::vector<std::string>			_splitCmd(std::string& cmd);
	std::map<std::string, std::string> _createChannelKeyMap(std::vector<std::string> &);

	/* SECTION: Commands */
	void 								_handleJoinCmd(int, std::vector<std::string> &);
	void 								_handleKickCmd(int, std::vector<std::string> &);
	void								_handleMsgCmd(int, std::vector<std::string> &);
	void 								_handleInviteCmd(int, std::vector<std::string> &);
	void 								_handleTopicCmd(int, std::vector<std::string> &);
	void 								_handleModeCmd(int, std::vector<std::string> &);
	void								_handleRecFile(int, std::vector<std::string> &);
	void								_handleSendFile(int, std::vector<std::string> &);

	/* SECTION: MODE command togglers (utils) */
	void 								_toggleI_mode(Channel *, Client *, std::string &);
	void 								_toggleT_mode(Channel *, Client *, std::string &);
	void 								_toggleK_mode(Channel *, Client *, std::vector<std::string> &);
	void 								_toggleO_mode(Channel *, Client *, std::vector<std::string> &);
	void 								_toggleL_mode(Channel *, Client *, std::vector<std::string> &);

	/* SECTION: Client object access */
	void 								_setClientNickname(int fd, std::vector<std::string> & cmd);
	void 								_setClientUsername(int fd, std::vector<std::string> & cmd);

public:
	CommandHandler(Server &);
	CommandHandler(const CommandHandler &);
	~CommandHandler(void);
	const CommandHandler				&operator=(const CommandHandler &);

	/* CORE FN */
	void								processInput(std::string &, int);
};

#endif
