#ifndef MESSAGE_H
# define MESSAGE_H

# include "lib.hpp"
# include <vector>

class Target
{
	protected:
		Target();
		~Target();

	public:
		bool	isMask(const std::string& target);
		bool	isNickname(const std::string& target);
		bool	isChannel(const std::string& target);
};

class Message : public Target
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
