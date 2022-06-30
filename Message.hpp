#ifndef MESSAGE_H
# define MESSAGE_H

# include "lib.hpp"

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
		Message();
		Message(std::string input);
		Message(const Message& cpy);
		~Message();
		Message& operator=(const Message& cpy);

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
		void				split(void);
};

#endif
