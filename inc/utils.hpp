#ifdef __cplusplus
# pragma once
#endif
#ifndef _ft_irc_hpp_
# define _ft_irc_hpp_

/* SECTION: includes */
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
# include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

/* SECTION: Utility functions */
char*               get_time_string(void);
std::string         generate_cache_name(const std::string& name);

#endif
