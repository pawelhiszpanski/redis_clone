#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <list>
#include <algorithm>
#define MAX_SIZE 200

class RedisValue {
public:
	std::string value;
	long long expireAt;
	std::list<std::string>::iterator lru_ptr;		// faster system of searching for ptr
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
	std::cout << "    Example: EXIT\n";
	std::cout << "5. To clear the view, use command -> CLEAR\n";
	std::cout << "    Example: CLEAR\n";
	std::cout << "6. To clear whole database, use command -> NEW\n";
	std::cout << "    Example: NEW\n";
	std::cout << "7. To check if key exist in database, use command -> EXISTS\n";
	std::cout << "   Example: EXISTS key\n";
	std::cout << "8. To size of database, use command -> DBSIZE\n";
	std::cout << "    Example: DBSIZE\n";
	std::cout << "9. To show all keys in database, use command -> KEYS\n";
	std::cout << "    Example: KEYS\n";
	std::cout << "10. To change key value, use command -> RENAME\n";
	std::cout << "    Example: RENAME old_key new_key\n";
	std::cout << "11. To set a time-to-live for an existing key in seconds, use EXPIRE\n";
	std::cout << "    Example: EXPIRE key seconds\n";
	std::cout << "12. To compress the database file and free up disk space, use REWRITE\n";
	std::cout << "    Example: REWRITE\n";
}

bool isExpired(std::unordered_map<std::string, RedisValue>& db, std::string& key, std::list<std::string>& lru) {
	auto it = db.find(key);
	if (it == db.end()) return false;				// key not in database

	long long currentTime = std::time(nullptr);

	if (it->second.expireAt > 0 && currentTime > it->second.expireAt) {
		lru.erase(it->second.lru_ptr);
		db.erase(it);
		return true;								// our data from database expired so we remove
	}
	return false;									// data is still valid
}

void expireAll(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	//check all keys -> mainly for db.size()
	auto it = db.begin();
	long long currentTime = std::time(nullptr);									// before the loop to make the program faster, such time difference should not change so much
	while (it != db.end()) {
		//long long currentTime = std::time(nullptr);
		if (it->second.expireAt > 0 && currentTime >= it->second.expireAt) {	// if expiring time is set and current time is greater
			lru.erase(it->second.lru_ptr);
			it = db.erase(it);													// it becomes pointer to next valid element	
		}
		else {
			it++;
		}
	}
	return;
}

void saveToFile(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	expireAll(db, lru);
	std::ofstream file;			
	file.open("database.txt");									// ofstream clear the file but database is already in RAM cause of loadFromFile() in main
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

void rewriteAofHandler(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	saveToFile(db, lru);										// deleting all double DEL or SET occurring
	std::cout << "File succesfully compressed!\n";
}

void appendOnlyFile(std::string key, std::string value, long long expireAt) {
	std::ofstream file;
	file.open("database.txt", std::ios::app);					// std::ios::app for appending at the end of the file 
	if (file.is_open()) {
		file << key << " " << expireAt << " " << value << "\n";
		file.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void loadFromFile(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	std::ifstream file;
	file.open("database.txt");										// read only
	if (file.is_open()) {
		std::string str;
		while (std::getline(file, str)) {
			if (str.empty()) continue;
			std::stringstream ss(str);
			std::string key, val;
			long long time;
			if (ss >> key) {
				if (key == "DELETE") {
					std::string to_delete;
					if (ss >> to_delete) {
						auto it = db.find(to_delete);
						if (it != db.end()) {						// checking for eventually errors
							lru.erase(it->second.lru_ptr);			// first deleting it->... then it itself 
							db.erase(it);
						}	
					}
					continue;
				}
			}
			if (ss >> time) {
				std::getline(ss >> std::ws, val);
				auto it = db.find(key);
				if (it != db.end()) {								// if key already exist in database, e.g in RENAME or SET
					if (it->second.lru_ptr != lru.end()) {			// to make sure its safe
						lru.erase(it->second.lru_ptr);
					}
					lru.push_front(key);
																	// overwriting elements at key
					it->second.lru_ptr = lru.begin();
					it->second.value = val;
					it->second.expireAt = time;
				}
				else {												// key is completely new
					if (lru.size() >= MAX_SIZE) {
						std::string key_to_delete = lru.back();
						lru.pop_back();
						db.erase(key_to_delete);
					}
					lru.push_front(key);
					db[key] = { val, time, lru.begin() };
				}
			}
		}
		file.close();
	}
}

void clearFile(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	std::cout << "This operation will delete all data from database. Continue? (Y/N): ";
	std::string str;
	std::getline(std::cin, str);
	for (auto& s : str) s = toupper(s);

	if (str == "YES" || str == "Y") {
		db.clear();
		saveToFile(db, lru);						// clearing whole .txt file
		lru.clear();								// clearing the list as well
		std::cout << "Database cleared!\n";
	}
	else {
		std::cout << "Operation cancelled!\n";
	}
}

void appendDelete(std::string key) {
	std::ofstream file;
	file.open("database.txt", std::ios::app);
	if (file.is_open()) {
		file << "DELETE " << key << "\n";
		file.close();
	}
	else {
		std::cout << "ERROR -> unable to open database file!\n";
	}
}

void setHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss, std::list<std::string>& lru) {
	std::string key, val;
	if (!(ss >> key)) {
		std::cout << "ERROR -> to insert into db you have to provide key!\n";
		return;
	}
	if (!(std::getline(ss >> std::ws, val))) {
		std::cout << "ERROR -> to insert into databse you have to provide value!\n";
		return;
	}													
	auto it = db.find(key);								// list handle
	if (it != db.end()) {								// if key exist
		if (it->second.lru_ptr != lru.end()) {			// to make sure its safe
			lru.erase(it->second.lru_ptr);
		}
		lru.push_front(key);
														// overwriting elements at key
		it->second.lru_ptr = lru.begin();
		it->second.value = val;
		it->second.expireAt = 0;						// set reset the expiring time
	}
	else {												// key is completely new
		if (lru.size() >= MAX_SIZE) {
			std::string key_to_delete = lru.back();
			lru.pop_back();
			db.erase(key_to_delete);
			appendDelete(key_to_delete);
		}
		lru.push_front(key);
		db[key] = { val, 0, lru.begin() };				// making new database element
	}

	appendOnlyFile(key, val, 0);
	std::cout << "| " << val << " |" << " is correctly added into database!\n";
	//saveToFile(db);
}

void getHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss, std::list<std::string>& lru) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
		return;
	}
	auto it = db.find(key);
	if (it == db.end() || isExpired(db, key, lru)) {
		std::cout << "ERROR -> key does NOT exist in our database!\n";
	}
	else {
		if (it->second.lru_ptr != lru.end()) {
			lru.erase(it->second.lru_ptr);
		}
		lru.push_front(key);
		it->second.lru_ptr = lru.begin();						// set the pointer to the beggining of the list
		std::cout << "| " << it->second.value << " |\n";
	}
}

void delHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss, std::list<std::string>& lru) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
	}
	else {
		auto it = db.find(key);
		if (it == db.end() || isExpired(db, key, lru)) {			// if key exist in database
			std::cout << "Cannot erase key! It does NOT exist!\n";
		}
		else {
			RedisValue removed = db.at(it->first);
			if (it->second.lru_ptr != lru.end()) {					// deleting from lru LIST first
				lru.erase(it->second.lru_ptr);
			}
			db.erase(it);											// then deleting from MAP

			appendDelete(key);										// write at the end of file that we have to delete such key instead of saving whole file
			std::cout << "| " << removed.value << " |" << " was correctly erased from database!\n";
		}
	}
	//saveToFile(db);
}

void exitHandler(std::unordered_map<std::string, RedisValue>& db, bool&end) {
	end = true;
	std::cout << "Closing Redis Clone! Thank you for using our services!\n";
}

void existHandler(std::unordered_map<std::string, RedisValue>& db , std::stringstream& ss, std::list<std::string>& lru) {
	std::string key;
	if (!(ss >> key)) {
		std::cout << "ERROR -> key must be provided!\n";
	}
	else {
		auto it = db.find(key);									// O(1) because of hash
		if (it != db.end() && !isExpired(db, key, lru)) {
			if (it->second.lru_ptr != lru.end()) {				// if we have this pointer in our list -> it should be its just for being safe
				lru.erase(it->second.lru_ptr);
			}
			lru.push_front(key);								// last recently used to the front
			it->second.lru_ptr = lru.begin();
			std::cout << "Key exist\n";
			return;
		}
		std::cout << "Key do NOT exist\n";
	}
}

void dbsizeHandler(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	expireAll(db, lru);
	std::cout << "Database hold " << db.size() << " elements\n";
}

void keysHandler(std::unordered_map<std::string, RedisValue>& db, std::list<std::string>& lru) {
	expireAll(db, lru);
	int i = 1;
	std::cout << "Keys in database:\n";
	for (const auto& [key, val] : db) {
		std::cout << i++ << ". " << key << "\n";
	}
	if (i == 1) std::cout << "-----\n";
}

void renameHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss, std::list<std::string>& lru) {
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
	if (it == db.end() || isExpired(db, old_key, lru)) {
		std::cout << "ERROR -> provided key does NOT exist!";
	}
	else {
		RedisValue tmp = it->second;									// copy of redis structure of key 'it'
		if (it->second.lru_ptr != lru.end()) {
			lru.erase(it->second.lru_ptr);								// deleting from current pos
		}
		db.erase(it);													// deleting from MAP
		//auto it_key = find(lru.begin(), lru.end(), old_key);			// not important anymore cause of new structure lru_ptr
		lru.push_front(new_key);
		tmp.lru_ptr = lru.begin();
		db[new_key] = tmp;
		std::cout << "Key changed succesfully\n";
		appendDelete(old_key);											// to avoid returning old key after restart
		appendOnlyFile(new_key, tmp.value, tmp.expireAt);
		//saveToFile(db);
	}
}

void expireHandler(std::unordered_map<std::string, RedisValue>& db, std::stringstream& ss, std::list<std::string>& lru) {
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
	if (it == db.end() || isExpired(db, key, lru)) {					// key may expire till we check
		std::cout << "ERROR -> provided key does NOT exist\n";
	}
	else {
		it->second.expireAt = std::time(nullptr) + time;
		std::cout << "Expire time set succesfully!\n";
		//saveToFile(db);
		appendOnlyFile(key, it->second.value, it->second.expireAt);
	}
	return;
}

int main() {
	std::cout << "Welcome to Redis Clone!\n";
	std::unordered_map<std::string, RedisValue> db;
	std::list<std::string> lru;											// monitoring last recently used element in case of too big size of db
	loadFromFile(db, lru);
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
			setHandler(db, ss, lru);
		}
		else if (cmd == "GET") {
			getHandler(db, ss, lru);
		}
		else if (cmd == "DEL") {
			delHandler(db, ss, lru);
		}
		else if (cmd == "NEW") {
			clearFile(db, lru);
		}
		else if (cmd == "EXISTS" || cmd == "EXIST") {
			existHandler(db, ss, lru);
		}
		else if (cmd == "DBSIZE" || cmd == "SIZE") {
			dbsizeHandler(db, lru);
		}
		else if (cmd == "KEYS") {
			keysHandler(db, lru);
		}
		else if (cmd == "RENAME") {
			renameHandler(db, ss, lru);
		}
		else if (cmd == "EXPIRE" || cmd == "EXPIRES") {
			expireHandler(db, ss, lru);
		}
		else if (cmd == "REWRITE" || cmd == "REWRITEAOF") {
			rewriteAofHandler(db, lru);
		}
		else {
			std::cout << "Command unknown, check tutorial by typing T\n";
		}
	}
	return 0;
}