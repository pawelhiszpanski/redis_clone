#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>

using namespace std;

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
	std::cout << "7. To check if key exist in database, use command -> EXISTS\n";
	std::cout << "   Example: EXISTS key\n";
	std::cout << "8. To size of database, use command -> DBSIZE\n";
	std::cout << "9. To show all keys in database, use command -> KEYS\n";
	std::cout << "10. To change key value, use command -> RENAME\n";
	std::cout << "    Example: RENAME old_key new_key\n";
}

void saveToFile(std::unordered_map<std::string, std::string>& db) {
	std::ofstream plik;
	plik.open("database.txt");
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
	std::ifstream plik;
	plik.open("database.txt");	// read only
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

void clearFile(std::unordered_map<std::string, std::string>& db) {
	std::cout << "This operation will delete all data from database. Continue? (Y/N): ";
	std::string str;
	std::getline(std::cin, str);
	for (auto& s : str) s = toupper(s);

	if (str == "YES" || str == "Y") {
		db.clear();
		saveToFile(db);
		std::cout << "Database cleared!\n";
	}
	else {
		std::cout << "Operation cancelled!\n";
	}
	
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

void exitHandler(std::unordered_map<std::string, std::string>& db, bool&end) {
	end = true;
	std::cout << "Closing Redis Clone! Thank you for using our services!\n";
}

void existHandler(std::unordered_map<std::string, std::string>& db , std::stringstream& ss) {
	string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
	}
	else {
		if (db.count(key)) {
			cout << "Key exist\n";
			return;
		}
		cout << "Key do NOT exist\n";
	}
}

void dbsizeHandler(std::unordered_map<std::string, std::string>& db) {
	cout << "Database hold " << db.size() << " elements\n";
}

void keysHandler(std::unordered_map<std::string, std::string>& db) {
	int i = 1;
	cout << "Keys in database:\n";
	for (auto [key, val] : db) {
		cout << i++ << ". " << key << "\n";
	}
	if (i == 1) std::cout << "-----\n";
}

void renameHandler(std::unordered_map<std::string, std::string>& db, std::stringstream& ss) {
	std::string old_key, new_key;
	if (!(ss >> old_key)) {
		std::cout << "ERROR -> old key must be provided!\n";
		return;
	}
	if (!(ss >> new_key)) {
		std::cout << "ERROR -> new key must be provided!\n";
		return;
	}
	auto it = db.find(old_key);
	if (it == db.end()) {
		std::cout << "ERROR -> provided key does NOT exist!";
	}
	else {
		string tmp = db.at(it->first);		// copy of value of key 'it'
		db.erase(it);
		db[new_key] = tmp;
		std::cout << "Key changed succesfully\n";
		saveToFile(db);
	}
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
			exitHandler(db, end);
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
			clearFile(db);
		}
		else if (cmd == "EXISTS") {
			existHandler(db, ss);
		}
		else if (cmd == "DBSIZE") {
			dbsizeHandler(db);
		}
		else if (cmd == "KEYS") {
			keysHandler(db);
		}
		else if (cmd == "RENAME") {
			renameHandler(db, ss);
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T\n";
		}
	}
	return 0;
}