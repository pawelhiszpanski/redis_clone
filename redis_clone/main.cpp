#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

void showTutorial() {
	std::cout << "Tutorial for using Redis Clone\n";
	std::cout << "1. To set a key-value pair, use command -> SET\n";
	std::cout << "   Example: SET key value\n";
	std::cout << "2. To get the value of a key, use command -> GET\n";
	std::cout << "   Example: GET key\n";
	std::cout << "3. To delete a key, use command -> DEL\n";
	std::cout << "   Example: DEL key\n";
	std::cout << "4. To exit the program, use command -> EXIT\n";
	std::cout << "5. To clear the view, use command -> CLEAR\n";
}

int main() {
	std::unordered_map<std::string, std::string> db;
	std::cout << "Welcome to Redis Clone!\n";
	std::string command = "";
	bool end = false;

	while (!end) {
		std::cout << std::endl;
		std::cout << "Press 'T' to show tutorial\n";
		std::cout << "Enter command: ";

		if (!(std::getline(std::cin, command))) {
			break;
		}
		std::cout << std::endl;
		std::stringstream ss(command);
		
		std::string cmd;
		ss >> cmd;
		for (auto& c : cmd) c = toupper(c);		

		if (cmd == "T") {
			showTutorial();
		}
		else if (cmd == "CLEAR") {
			system("cls");
		}
		else if (cmd == "EXIT" || cmd == "QUIT" || cmd =="Q") {
			end = true;
			std::cout << "Closing Redis Clone db! Thank you for using our services!\n";
		}
		else if (cmd == "SET") {
			std::string key, val;
			if (!(ss >> key)) {
				std::cout << "ERROR -> to insert into db you have to provide key!\n";
				continue;
			}

			if (!(std::getline(ss >> std::ws, val))) {
				std::cout << "ERROR -> to insert into databse you have to provide value!\n";
				continue;
			}
			db[key] = val;
			std::cout << "| " << db[key] << " |" << " is correctly added into database!\n";

		}
		else if (cmd == "GET") {
			std::string key;
			if (!(ss >> key)) {
				std::cout << "ERROR -> key must be provided!\n";
				continue;
			}
			auto it = db.find(key);
			if (it == db.end()) {
				std::cout << "ERROR -> key do not exist in our database!\n";
			}
			else {
				std::cout << "| " << it->second << " |\n";
			}
		}
		else if (cmd == "DEL") {
			std::string key;
			if (!(ss >> key)) {
				std::cout << "ERROR -> key must be provided!\n";
			}
			else {
				auto it = db.find(key);
				if (it != db.end()) {		// if key exist in db
					std::string removed = db.at(it->first);
					db.erase(it);
					std::cout << "| " << removed << " |" << " was correctly erased from database!\n";
				}
				else {
					std::cout << "Cannot erase key! It does not exist!\n";
				}
			}
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T\n";
		}
	}
	return 0;
}