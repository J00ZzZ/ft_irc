#pragma once
#ifndef _server_replies_hpp_
# define _server_replies_hpp_

# define ERR_NEEDMOREPARAMS(hostname, nick, command) \
    (":" + hostname + " 461 " + nick + " " + command + " :Not enough parameters\r\n")

# define ERR_PASSWDMISMATCH(hostname, nick) \
    (":" + hostname + " 464 " + nick + " :Password incorrect\r\n")

# define ERR_NOTREGISTERED(hostname, nick) \
    (":" + hostname + " 451 " + nick + " :You have not registered\r\n")

# define ERR_ALREADYREGISTRED(hostname, nick) \
	(":" + hostname + " 462 " + nick + " :You may not reregister\r\n")

# define ERR_BADCHANMASK(hostname, nick, channel) \
	(":" + hostname + " 476 " + nick + " :" + channel + " :Bad channel mask\r\n")

# define ERR_USERNOTINCHANNEL(hostname, nick, channel) \
	(":" + hostname + " 441 " + nick + " " + channel + " :They aren't on that channel\r\n")

# define ERR_CHANOPRIVSNEEDED(hostname, nick, channel) \
	(":" + hostname + " 482 " + nick + " :" + channel + " :You're not channel operator\r\n")

# define ERR_NOTONCHANNEL(hostname, nick, channel) \
	(":" + hostname + " 442 " + nick + " : " + channel + " :You're not on that channel\r\n")

# define RPL_WELCOME(server, nick, user, host) \
    (":" + server + " 001 " + nick + " :Welcome to the 42 IRC network " + nick + "! " + user + "@" + host + "\r\n")

# define ERR_INVITEONLYCHAN(server, nick, channel) \
	(":" + server + " 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n")\

# define ERR_BANNEDFROMCHAN(server, nick, channel) \
	(":" + server + " 474 " + nick + " " + channel + " :Cannot join channel (+b)\r\n")

# define ERR_BADCHANNELKEY(server, nick, channel) \
	(":" + server + " 475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n")

# define ERR_CHANNELISFULL(server, nick, channel) \
    (":" + server + " 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")

# define RPL_TOPIC(server, nick, channel, topic) \
    (":" + server + " 332 " + nick + " " + channel + " :" + topic + "\r\n")

# define RPL_NAMREPLY(server, nick, symbol, channel, userList) \
    (":" + server + " 353 " + nick + " " + symbol + " " + channel + " :" + userList + "\r\n")

# define RPL_ENDOFNAMES(server, nick, channel) \
    (":" + server + " 366 " + nick + " " + channel + " :End of /NAMES list.\r\n")

#define ERR_NOSUCHCHANNEL(server, nick, channel) \
	(":" + server + " 403 " + nick + " " + channel + " :No such channel\r\n")

#define ERR_TOOMANYCHANNELS(server, nick, channel) \
	(":" + server + " 405 " + nick + " " + channel + " :You have joined too many channels\r\n")

#define ERR_NORECIPIENT(server, nick, command) \
	(":" + server + " 411 " + nick + " :No recipient given (" + command + ")\r\n")

#define ERR_NOTEXTTOSEND(server, nick) \
	(":" + server + " 412 " + nick + " :No text to send\r\n")

#define ERR_CANNOTSENDTOCHAN(server, nick, channel) \
	(":" + server + " 404 " + nick + " " + channel + " :Cannot send to channel\r\n")

#define ERR_NOTOPLEVEL(server, nick, mask) \
	(":" + server + " 413 " + nick + " " + mask + " :No toplevel domain specified\r\n")

#define ERR_WILDTOPLEVEL(server, nick, mask) \
	(":" + server + " 414 " + nick + " " + mask + " :Wildcard in toplevel domain\r\n")

#define ERR_TOOMANYTARGETS(server, nick, target) \
	(":" + server + " 407 " + nick + " " + target + " :Duplicate recipients. No message delivered\r\n")

#define ERR_NOSUCHNICK(server, nick, target) \
	(":" + server + " 401 " + nick + " " + target + " :No such nick/channel\r\n")

#define ERR_USERONCHANNEL(server, nick, user, channel) \
	(":" + server + " 443 " + nick + " " + user + " " + channel + " :is already on channel\r\n")

#define RPL_INVITING(server, nick, target, channel) \
	(":" + server + " 341 " + nick + " " + target + " " + channel + " :Invitation sent\r\n")

#define CMD_INVITE(invitor, target, channel) \
	(":" + invitor + " INVITE " + target + " :" + channel + "\r\n")

#define RPL_NOTOPIC(server, nick, channel) \
	(":" + server + " 331 " + nick + " " + channel + " :No topic is set\r\n")

#define CMD_TOPIC(nick, user, host, channel, topic) \
	(":" + nick + "!" + user + "@" + host + " TOPIC " + channel + " :" + topic + "\r\n")

#define ERR_UNKNOWNMODE(hostname, nick, modechar) \
	(":" + hostname + " 472 " + nick + " " + modechar + " :is unknown mode char to me\r\n")

#define RPL_MODE_CHANGE(prefix, channel, mode) \
	(":" + prefix + " MODE " + channel + " " + mode + "\r\n")

#define ERR_UNKNOWNCOMMAND(hostname, nick, command) \
	(":" + hostname + " 421 " + nick + " " + command + " :Unknown command\r\n")

#define RPL_PONG(servername, token) \
	("PONG :" + token + "\r\n")

#define RPL_KICK(kicker_id, channel, target, reason) \
	(":" + kicker_id + " KICK " + channel + " " + target + ":" + reason + "\r\n")

#define RPL_FILERECEIVED(server, nick, filename) \
(":" + server + " 69420 " + nick + " :File \"" + filename + "\" successfully received\r\n")

#define ERR_FILENOTFOUND(server, nick, filename) \
(":" + server + " 2137 " + nick + " :File \"" + filename + "\" does not exist\r\n")

#define ERR_MSGTOOLONG(hostname, nick) \
    (":" + hostname + " NOTICE " + nick + " :Input line too long (max 512 bytes)\r\n")

#define RPL_CENSORSHIP(server, nick, word) \
    (":" + server + " NOTICE " + nick + " :Your message was censored for containing a banned word: \"" + word + "\"\r\n")

#endif
