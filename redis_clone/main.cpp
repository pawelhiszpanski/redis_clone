#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>

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
	std::cout << "6. To clear whole database, use command -> NEW\n";
}

void saveToFile(std::unordered_map<std::string, std::string>& db) {
	std::fstream plik;
	plik.open("database.txt", std::ios::out | std::ios::trunc);		// trunc for clearing file before saving
	if (plik.is_open()) {
		for (auto& [key, val] : db) {
			plik << key << " " << val << "\n";
		}
		plik.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void loadFromFile(std::unordered_map<std::string, std::string>& db) {
	std::fstream plik;
	plik.open("database.txt", std::ios::in);	// read only
	if (plik.is_open()) {
		std::string str;
		while (std::getline(plik, str)) {
			if (str.empty()) continue;
			std::stringstream ss(str);
			std::string key, val;
			ss >> key;
			std::getline(ss >> std::ws, val);
			db[key] = val;
		}
		plik.close();
	}
}

void clearFile() {
	std::fstream plik;
	plik.open("databse.txt", std::ios::trunc);
	if (plik.is_open()) plik.close();
}

void setHandler(std::unordered_map<std::string, std::string>&db, std::stringstream& ss) {
	std::string key, val;
	if (!(ss >> key)) {
		std::cout << "ERROR -> to insert into db you have to provide key!\n";
		return;
	}

	if (!(std::getline(ss >> std::ws, val))) {
		std::cout << "ERROR -> to insert into databse you have to provide value!\n";
		return;
	}
	db[key] = val;
	std::cout << "| " << db[key] << " |" << " is correctly added into database!\n";
	saveToFile(db);
}

void getHandler(std::unordered_map<std::string, std::string>& db, std::stringstream& ss) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
		return;
	}
	auto it = db.find(key);
	if (it == db.end()) {
		std::cout << "ERROR -> key do not exist in our database!\n";
	}
	else {
		std::cout << "| " << it->second << " |\n";
	}
}

void delHandler(std::unordered_map<std::string, std::string>& db, std::stringstream& ss) {
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
	saveToFile(db);
}

void exitHandler(std::unordered_map<std::string, std::string>& db, std::stringstream& ss, bool&end) {
	end = true;
	std::cout << "Closing Redis Clone! Thank you for using our services!\n";
}

int main() {
	std::cout << "Welcome to Redis Clone!\n";
	std::unordered_map<std::string, std::string> db;
	loadFromFile(db);
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
			exitHandler(db, ss, end);
		}
		else if (cmd == "SET") {
			setHandler(db, ss);
		}
		else if (cmd == "GET") {
			getHandler(db, ss);
		}
		else if (cmd == "DEL") {
			delHandler(db, ss);
		}
		else if (cmd == "NEW") {
			db.clear();
			clearFile();
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T\n";
		}
	}
	return 0;
}