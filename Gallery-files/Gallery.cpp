#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include "DatabaseAccess.h"
#include <chrono> 
#include <ctime> 
#include "AlbumManager.h"


int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}
void Print_dev_name_and_curr_time()
{
	auto currtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << "by adam shamis version 1.0.3, current time : " << ctime(&currtime) << std::endl;
}
int main(void)
{
	// initialization data access
	DatabaseAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);

	std::string albumName;
	std::cout << " Welcome to Gallery! " << std::endl;
	std::cout << " Welcome to Gallery! ";
	Print_dev_name_and_curr_time();
	std::cout << "===================" << std::endl;
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;
	
	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}


