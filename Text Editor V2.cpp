// Text Editor V2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

INPUT_RECORD lpBuff[128];
DWORD fdwSaveOldMode, cNumRead, i;
HANDLE handle; // handle > hStdin
BOOL existingFile;
std::string fileName;
std::string input;

// This is a simple text editor I made to learn some more stuff, like ReadConsoleInput and Message boxes
// I need to learn arrow keys so you can edit opened files
// This is my first bigger C++ project so the code is a bit messy

// TODO:
//		- More keybinds
//		- Arrow keys eventually
//		- Command line arguments

class FileManager {
public:
	static void saveFile(std::string input, std::string name) {
		std::ofstream file(name);
		if (!file) {
			std::cout << "File doesn't exist so it's unable to be saved\n";
			exit(0);
		}
		file << input;

		file.close();
	}

	static void openFile(std::string name) {
		std::ifstream file(name);
		if (!file) {
			std::cout << "File doesn't exist!\n";
			exit(1);
		}

		std::string content;
		while (std::getline(file, content)) {
			input += content + '\n';
		}
		std::cout << input;

		file.close();
	}
};

void keyEvent(KEY_EVENT_RECORD event) {
	//std::cout << event.uChar.AsciiChar;
	if (event.bKeyDown) {
		switch (event.wVirtualKeyCode) {
		case VK_BACK:
			if (!input.empty()) {

				std::cout << '\b' << " " << '\b';
				input.pop_back(); // we erase the last character of the string
			}
			break;
		case VK_RETURN:
			input += '\n'; // we add the newline character to the string
			std::cout << '\n';
			break;
		case 'D':
			if (event.dwControlKeyState == LEFT_CTRL_PRESSED || event.dwControlKeyState == RIGHT_CTRL_PRESSED) {
				int messageBox = MessageBox(NULL, (LPCWSTR)L"Are you sure you want to exit without saving?", (LPCWSTR)L"Text Editor", MB_OKCANCEL);
				switch (messageBox) {
				case IDOK:
					SetConsoleMode(handle, fdwSaveOldMode); // we set our console mode to the previous one here as well
					exit(0); // we exit the program with a code 0 (code 0 means that everything was successful)
					break;
				case IDCANCEL:
					break;
				}
			}
		case 'S':
			if (event.dwControlKeyState == LEFT_CTRL_PRESSED || event.dwControlKeyState == RIGHT_CTRL_PRESSED) {
				int messageBox = MessageBox(NULL, (LPCWSTR)L"Are you sure you want to save and exit?", (LPCWSTR)L"Text Editor", MB_OKCANCEL);
				switch (messageBox) {
				case IDOK:
					SetConsoleMode(handle, fdwSaveOldMode); // we set our console mode to the previous one here as well
					system("cls");
					
					if (existingFile) {
						FileManager::saveFile(input, fileName);
					} else {
						std::cout << "File name: ";
						std::string file;
						std::cin >> file;

						FileManager::saveFile(input, file);
					}

					exit(0);
					break;
				}
			}
		default:
			std::cout << event.uChar.AsciiChar;
			input += event.uChar.AsciiChar; // we add the character to the string
			break;
		}
	}
}

void init() {
	SetConsoleTitle((LPCWSTR)L"Text Editor");
	handle = GetStdHandle(STD_INPUT_HANDLE); // we simply get our consoles handle and check if it exists

	if (handle == INVALID_HANDLE_VALUE) {
		std::cout << "Something went wrong and the program could not get your console input handle!\n";
		exit(1);
	}

	GetConsoleMode(handle, &fdwSaveOldMode); // we save our previous console mode
	SetConsoleMode(handle, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT); // we set our new console mode

	if (existingFile) {
		FileManager::openFile(fileName);
	}

	while (true) {
		ReadConsoleInput(handle, lpBuff, 128, &cNumRead); // we read the console input and then handle it with our keyEvent function
		for (i = 0; i < cNumRead; i++) {
			switch (lpBuff[i].EventType) {
			case KEY_EVENT:
				keyEvent(lpBuff[i].Event.KeyEvent);
				break;
			}
		}
	}

	SetConsoleMode(handle, fdwSaveOldMode); // we set our console mode to the previous one
	std::cout << input << '\n'; // TODO: remove this
}

int main() {
	std::cout << "Would you like to create a new file or open an existing file? (1/2): ";
	int choice;
	std::cin >> choice;

	if (choice == 1) {
		system("cls");
		init();
	} else if (choice == 2) {
		std::cout << "File name (must be in this directory): ";
		std::string name;
		std::cin >> name; // TODO: change to std::getline
		existingFile = true;
		fileName = name;

		system("cls");
		init();
	}
}
