#include "Oper.hpp"

Oper::Oper(std::string nick, std::string pass, std::string host) : _nick(nick), _pass(pass), _host(host) {}

Oper::~Oper() {}

const std::string&	Oper::getNick() const { return this->_nick; }
const std::string&	Oper::getPass() const { return this->_pass; }
const std::string&	Oper::getHost() const { return this->_host; }

OperList::OperList()
{
	int				start_flag;
	std::string		line;
	std::string		oper_info;
	std::ifstream	config_file("Oper.config");

	oper_info.clear();
	if (config_file.is_open())
	{
		while (std::getline(config_file, line))
		{
			if (!line.empty() && line[0] == '{')
				start_flag = 1;
			else if (!line.empty() && line[0] == '}')
			{
				start_flag = 0;
				getOper(oper_info);
				oper_info.clear();
			}
			else if (!line.empty() && start_flag)
				oper_info += line;
		}
		if (config_file.eof())
			config_file.close();
	}
	if (config_file.bad())
		std::cout << "1\n";
	//send to client loading config file failed
}

OperList::~OperList()
{

}

const OperList::operMap&	OperList::getOperList() const { return this->_operList; }

bool	OperList::getOper(std::string& oper_info)
{
	std::string *nick = getString(oper_info, "nickname");
	std::string *pass = getString(oper_info, "password");
	std::string *host = getString(oper_info, "hostname");

	Oper op(*nick, *pass, *host);
	this->_operList.insert(std::pair<std::string, Oper*>(op.getNick(), &op));
	delete[] nick;
	delete[] pass;
	delete[] host;
}

std::string*	OperList::getString(const std::string& from, const std::string& tofind) const
{
	std::allocator<std::string>	alloc;
	std::string					target_string;
	std::string*				ret_string;
	int							start_flag = 0;
	int							len = 0;

	std::string::size_type pos = from.find(tofind);
	if (pos != std::string::npos)
	{
		target_string = from.substr(pos);
		for (std::string::iterator it = target_string.begin(); it != target_string.end(); it++)
		{
			if (!start_flag)
			{
				pos++;
				if (*it == '"')
					start_flag = 1;
			}
			else if (*it != '"')
				len++;
			else
			{
				start_flag = 0;
				break ;
			}
		}
		ret_string = alloc.allocate(len);
		*ret_string = from.substr(pos, len);
	}
	else
	{
		ret_string = alloc.allocate(1);
		*ret_string = "";
	}
	return ret_string;
}
