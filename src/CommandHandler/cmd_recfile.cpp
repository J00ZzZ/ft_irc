#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/*
	Allows clients to receive the files uploaded to the server in b64 format.
	Notifies on file not found.
*/
void CommandHandler::_handleRecFile(int fd, std::vector<std::string>& cmd) {
	if (cmd.size() != 2) {
		m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "RECFILE"), fd);
		return ;
	}
	std::ifstream in(("received_files/" + cmd[1]).c_str(), std::ios::in | std::ios::binary);
	if (!in) {
		m_Server.sendResponse(ERR_FILENOTFOUND(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), cmd[1]), fd);
		return ;
	}

	std::ostringstream contents;
	contents << in.rdbuf();
	m_Server.sendResponse(RPL_FILERECEIVED(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), contents.str()), fd);
}
