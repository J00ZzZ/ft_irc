#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/* SENDFILE <filename> <base64>
	Handles sendfile command, allows to send a base64 encoded file data
	and saves it to the server files directory
 */
void CommandHandler::_handleSendFile(int fd, std::vector<std::string>& cmd) {
	if (cmd.size() != 3) {
		m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "SENDFILE"), fd);
		return ;
	}

	std::string filename = "received_files/" + cmd[1];
	std::string file_b64 = cmd[2];

	std::ofstream file(filename.c_str());
	if (file) {
		file << file_b64;
	} else {
		std::cerr << "ERROR: failed to save base64 file named: " << filename << std::endl;
		return ;
	}

	m_Server.sendResponse(RPL_FILERECEIVED(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), filename), fd);
}
