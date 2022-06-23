#ifndef MESSAGE_H
# define MESSAGE_H

# include "lib.hpp"
# include <vector>

class Message
{
public:
	Message(std::string input);
	~Message();

	//returns input read from buf from socket from client
	const std::string&				getInput();
	const std::string&				getPrefix();
	const std::string&				getCommand();
	const std::vector<std::string>&	getParam();
	const std::string&				getTrailing();

private:
	std::string					_input;
	std::string					_prefix;
	std::string					_command;
	std::vector<std::string>	_param;
	std::string					_trailing;

	void				parse(const std::string& buf);
};

#endif
