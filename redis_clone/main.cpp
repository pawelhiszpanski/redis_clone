#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <ctime>

class RedisValue {
public:
	std::string value;
	long long expireAt;
};

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
	std::cout << "11. To set a time-to-live for an existing key in seconds, use EXPIRE\n";
	std::cout << "    Example: EXPIRE key seconds\n";
}

bool isExpired(std::unordered_map<std::string, RedisValue>& db, std::string& key) {
	auto it = db.find(key);
	if (it == db.end()) return false;	// key not in database

	long long currentTime = std::time(nullptr);

	if (it->second.expireAt > 0 && currentTime > it->second.expireAt) {
		db.erase(it);
		return true;		// our data from database expired so we remove
	}
	return false;			// data is still valid
}

void expireAll(std::unordered_map<std::string, RedisValue>& db) {
	//check all keys -> mainly for db.size()
	auto it = db.begin();
	while (it != db.end()) {
		long long currentTime = std::time(nullptr);
		if (it->second.expireAt > 0 && currentTime >= it->second.expireAt) {	// if expiring time is set and current time is greater
			it = db.erase(it);		// it becomes pointer to next valid element
		}
		else {
			it++;
		}
	}
	return;
}

void saveToFile(std::unordered_map<std::string, RedisValue>& db) {
	expireAll(db);
	std::ofstream file;			
	file.open("database.txt");	// ofstream clear the file but database is already in RAM cause of loadFromFile() in main
	if (file.is_open()) {
		for (const auto& [key, val] : db) {
			file << key << " " << val.expireAt << " " << val.value << "\n";
		}
		file.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void appendOnlyFile(std::string key, std::string value, long long expireAt) {
	std::ofstream file;
	file.open("database.txt", std::ios::app);
	if (file.is_open()) {
		file << key << " " << expireAt << " " << value << "\n";
		file.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void loadFromFile(std::unordered_map<std::string, RedisValue>& db) {
	std::ifstream file;
	file.open("database.txt");	// read only
	if (file.is_open()) {
		std::string str;
		while (std::getline(file, str)) {
			if (str.empty()) continue;
			std::stringstream ss(str);
			std::string key, val;
			long long time;
			if (ss >> key >> time) {
				std::getline(ss >> std::ws, val);
				db[key] = { val, time };
			}
		}
		file.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void clearFile(std::unordered_map<std::string, RedisValue>& db) {
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

void setHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss) {
	std::string key, val;
	if (!(ss >> key)) {
		std::cout << "ERROR -> to insert into db you have to provide key!\n";
		return;
	}
	if (!(std::getline(ss >> std::ws, val))) {
		std::cout << "ERROR -> to insert into databse you have to provide value!\n";
		return;
	}
	db[key] = { val, 0 };	//  at first data never expire
	std::cout << "| " << db[key].value << " |" << " is correctly added into database!\n";
	saveToFile(db);
}

void getHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
		return;
	}
	auto it = db.find(key);
	if (it == db.end() || isExpired(db, key)) {
		std::cout << "ERROR -> key does NOT exist in our database!\n";
	}
	else {
		std::cout << "| " << it->second.value << " |\n";
	}
}

void delHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
	}
	else {
		auto it = db.find(key);
		if (it == db.end() || isExpired(db, key)) {		// if key exist in db
			std::cout << "Cannot erase key! It does NOT exist!\n";
		}
		else {
			RedisValue removed = db.at(it->first);
			db.erase(it);
			std::cout << "| " << removed.value << " |" << " was correctly erased from database!\n";
		}
	}
	saveToFile(db);
}

void exitHandler(std::unordered_map<std::string, RedisValue>& db, bool&end) {
	end = true;
	std::cout << "Closing Redis Clone! Thank you for using our services!\n";
}

void existHandler(std::unordered_map<std::string, RedisValue>& db , std::stringstream& ss) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
	}
	else {
		if (db.count(key) && !isExpired(db, key)) {
			std::cout << "Key exist\n";
			return;
		}
		std::cout << "Key do NOT exist\n";
	}
}

void dbsizeHandler(std::unordered_map<std::string, RedisValue>& db) {
	expireAll(db);
	std::cout << "Database hold " << db.size() << " elements\n";
}

void keysHandler(std::unordered_map<std::string, RedisValue>& db) {
	expireAll(db);
	int i = 1;
	std::cout << "Keys in database:\n";
	for (const auto& [key, val] : db) {
		std::cout << i++ << ". " << key << "\n";
	}
	if (i == 1) std::cout << "-----\n";
}

void renameHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss) {
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
	if (it == db.end() || isExpired(db, old_key)) {
		std::cout << "ERROR -> provided key does NOT exist!";
	}
	else {
		RedisValue tmp = db.at(it->first);		// copy of redis structure of key 'it'
		db.erase(it);
		db[new_key] = tmp;
		std::cout << "Key changed succesfully\n";
		saveToFile(db);
	}
}

void expireHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss) {
	std::string key;
	long long time;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided\n";
		return;
	}
	if (!(ss >> time)) {
		std::cout << "ERROR -> expire time must be provided\n";
		return;
	}
	auto it = db.find(key);
	if (it == db.end() || isExpired(db, key)) {		// key may expire till we check
		std::cout << "ERROR -> provided key does NOT exist\n";
	}
	else {
		it->second.expireAt = std::time(nullptr) + time;
		std::cout << "Expire time set succesfully!\n";
		saveToFile(db);		// if user will set expire date then computer crash could not save the changes
	}
	return;
}

int main() {
	std::cout << "Welcome to Redis Clone!\n";
	std::unordered_map<std::string, RedisValue> db;
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

		if (cmd == "T" || cmd == "TUTORIAL") {
			showTutorial();
		}
		else if (cmd == "CLEAR" || cmd == "C") {
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
		else if (cmd == "EXISTS" || cmd == "EXIST") {
			existHandler(db, ss);
		}
		else if (cmd == "DBSIZE" || cmd == "SIZE") {
			dbsizeHandler(db);
		}
		else if (cmd == "KEYS") {
			keysHandler(db);
		}
		else if (cmd == "RENAME") {
			renameHandler(db, ss);
		}
		else if (cmd == "EXPIRE" || cmd == "EXPIRES") {
			expireHandler(db, ss);
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T\n";
		}
	}
	return 0;
}