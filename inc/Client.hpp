#pragma once
#ifndef _client_hpp_
# define _client_hpp_

#include <string>
#include <vector>

class Channel;

class Client {
private:

    int                     _fd;
    bool                    _pass_received;
    bool                    _nick_received;
    bool                    _uname_received;
    bool                    _is_auth;
    std::string             _nickname;
    std::string             _username;
    std::string             _ipv4_addr;
    std::string             _buffer;
    std::vector<Channel *>  _activeChannels;

public:

    Client(void) : _pass_received(false), _nick_received(false), _uname_received(false), _is_auth(false) {}
    ~Client(void) {}

    /* Getters */
    int             getFd(void) const { return (this->_fd); }
    // For read-write access
    std::string& getBuf() { return _buffer; }

    // For read-only access on const objects
    const std::string&      getBuf(void) const { return _buffer; }
    const std::string&      getHostname(void) const { return (_ipv4_addr); }
    bool                    getAuth(void) const { return (_is_auth); }
    const std::string&      getNick(void) const { return (_nickname); }
    const std::string&      getUsername(void) const { return (_username); }
    const std::string       getFullId(void) const { return (_nickname + "!" + _username + "@" + _ipv4_addr); }
    bool                    getPassReceived(void) const { return (_pass_received); }
    bool                    getNickReceived(void) const { return (_nick_received); }
    bool                    getUnameReceived(void) const { return (_uname_received); }

    /* Setters */
    void                    setHostname(char * hostname) { _ipv4_addr = std::string(hostname); }
    void                    setFd(int fd) { _fd = fd; }
    void                    setPassReceived(void) { _pass_received = true; }
    void                    setNickReceived(void) { _nick_received = true; }
    void                    setUnameReceived(void) { _uname_received = true; }
    void                    setAuth(void) { _is_auth = true; }
    void                    setNick(std::string& nick) { _nickname = nick; }
    void                    setUsername(std::string& username) { _username = username; }
    void                    setIpv4Addr(std::string ipv4_addr) { _ipv4_addr = ipv4_addr; }
    void                    setBuf(std::string buffer) { _buffer += buffer; }
    void                    addChannel(Channel* channel) { _activeChannels.push_back(channel); }

    /* Utils */
    void            clearBuf(void) { _buffer.clear(); }
};

#endif