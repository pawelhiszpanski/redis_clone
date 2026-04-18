#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

void showTutorial() {
	std::cout << "Tutorial for using Redis Clone" << std::endl;
	std::cout << "1. To set a key-value pair, use command -> SET" << std::endl;
	std::cout << "   Example: SET key value" << std::endl;
	std::cout << "2. To get the value of a key, use command -> GET" << std::endl;
	std::cout << "   Example: GET key" << std::endl;
	std::cout << "3. To delete a key, use command -> DEL" << std::endl;
	std::cout << "   Example: DEL key" << std::endl;
	std::cout << "4. To exit the program, use command -> EXIT" << std::endl;
	std::cout << "5. To clear the view, use command -> CLEAR" << std::endl;
}

int main() {
	std::unordered_map<std::string, std::string> db;
	std::cout << "Welcome to Redis Clone!" << std::endl;
	std::string command = "";
	bool end = false;

	while (!end) {
		std::cout << std::endl;
		std::cout << "Press 'T' to show tutorial" << std::endl;
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
			std::cout << "Closing Redis Clone db! Thank you for using our services!" << std::endl;
		}
		else if (cmd == "SET") {
			std::string key, val;
			if (!(ss >> key)) {
				std::cout << "ERROR -> to insert into db you have to provide key!" << std::endl;
				continue;
			}
			if (db.count(key)) {
				std::string current = db[key];
				std::cout << "The value assigned to this key already exists -> " << "| " << current << " | Do you want to replace it ? (Y/N) : ";
				std::string response;
				std::getline(std::cin, response);
				for (auto& r : response) r = toupper(r);

				if (response == "Y" || response == "YES" || response == "YE") {
					if (!(std::getline(ss >> std::ws, val))) {
						std::cout << "ERROR -> to insert into databse you have to provide value!" << std::endl;
						continue;
					}
					db[key] = val;
					std::cout << "| " << db[key] << " |" << " is correctly added into database!" << std::endl;
				}
				else if(response == "N" || response=="NO") {
					std::cout << "No changes made!" << std::endl;
				}
				else {
					std::cout << "Unknown command! Try again!" << std::endl;
				}
				continue;
			}

			if (!(std::getline(ss >> std::ws, val))) {
				std::cout << "ERROR -> to insert into databse you have to provide value!" << std::endl;
				continue;
			}
			db[key] = val;
			std::cout << "| " << db[key] << " |" << " is correctly added into database!" << std::endl;

		}
		else if (cmd == "GET") {
			std::string key;
			if (!(ss >> key)) {
				std::cout << "ERROR -> key must be provided!" << std::endl;
				continue;
			}
			if (!db.count(key)) {
				std::cout << "ERROR -> key do not exist in our database!" << std::endl;
			}
			else {
				std::cout << "| " << db[key] << " |" << std::endl;
			}
		}
		else if (cmd == "DEL") {
			std::string key;
			if (!(ss >> key)) {
				std::cout << "ERROR -> key must be provided!" << std::endl;
			}
			else {
				if (db.count(key)) {		// if key exist in db
					std::string removed = db[key];
					db.erase(key);
					std::cout << "| " << removed << " |" << " was correctly erased from database!" << std::endl;
				}
				else {
					std::cout << "Cannot erase key! It does not exist!" << std::endl;
				}
			}
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T" << std::endl;
		}
	}
	return 0;
}