#include "Server.hpp"

bool is_offline = false;
Server	irc;

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "./ircserv <port> <password>" << std::endl;
		return (-1);
	}
	
	irc.createServer("127.0.0.1", atoi(argv[1]), std::string(argv[2]));
	
	int	client_socket;
	while (42)
	{
		struct timeval timeout;
    	timeout.tv_sec = 1;
	
		if (is_offline == true)
			break ;
		client_socket = 0;
	
		memcpy(&irc.temp_read_fds, &irc._read_fds, sizeof(irc.temp_read_fds));
		memcpy(&irc.temp_write_fds, &irc._write_fds, sizeof(irc.temp_write_fds));
		int res = select(irc.get_max_fd() + 1, &irc.temp_read_fds, &irc.temp_write_fds, NULL, NULL);
		if (res == -1)
		{
			std::cerr << "Error: select failed" << std::endl;
			return -1;
		}
		else if (res == 0)
			continue ;
		if ((client_socket = irc.waitClient()) != -1)
			irc.addClient(client_socket);
		irc.listenClients();
	}
	return (0);
}