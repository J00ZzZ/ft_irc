#pragma once
#ifndef _chan_bot_hpp_
# define _chan_bot_hpp_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Server;
class Channel;

class Bot {

private:
    Server                       &_server;

    std::string                  _name;
    std::vector<std::string>     _bannedWords;

    void                        _loadBannedWords(void);

public:
    Bot(Server &server);
    ~Bot(void);

    int                         notifyOnCensorship(const std::string& msg, int fd);
};


#endif