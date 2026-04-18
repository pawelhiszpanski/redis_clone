#include <iostream>
#include <string>
#include <unordered_map>

void showTutorial() {
	std::cout << "Tutorial for using Redis Clone" << std::endl;
	std::cout << std::endl;
	std::cout << "1. To set a key-value pair, use command -> SET" << std::endl;
	std::cout << "   Example: SET key value" << std::endl;
	std::cout << std::endl;
	std::cout << "2. To get the value of a key, use command -> GET" << std::endl;
	std::cout << "   Example: GET key" << std::endl;
	std::cout << std::endl;
	std::cout << "3. To delete a key, use command -> DEL" << std::endl;
	std::cout << "   Example: DEL key" << std::endl;
	std::cout << std::endl;
	std::cout << "4. To exit the program, use command -> EXIT" << std::endl;
	std::cout << std::endl;
}

int main() {
	std::unordered_map<std::string, std::string> dataBase;
	std::cout << "Welcome to Redis Clone!" << std::endl;
	std::cout << "Press 'T' to show tutorial" << std::endl;
	



	return 0;
}