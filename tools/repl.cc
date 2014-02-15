/**
 * Copyright (c) 2014, Floris Chabert. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "repl.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

struct quit : exception {};
struct help : exception {};

repl_cmd::repl_cmd() {
	name = "repl";
	description = "Assembly REPL.";
}

void repl_cmd::run(int args_num, char const *args[]) {
	while (true) {
		string cmd = readline("> ");

		try {
			handle_cmd(cmd);
		}
		catch (help) {
			cout << "Help:" << endl;
		}
		catch (quit) {
			break;
		}
		catch (runtime_error& e) {
			cout << e.what() << endl;
		}

		add_history(cmd.c_str());
	}

	cout << "Done." << endl;
}

void repl_cmd::handle_cmd(std::string cmd) {
	istringstream tokens(cmd);
	string token;

	tokens >> token;
	
	if (token == "help" || token == "?") {
		throw help();
	}
	else if (token == "quit" || token == "exit") {
		throw quit();
	}
	else if (token == "arch") {
		tokens >> token;

		if (!tokens) {
			throw runtime_error("Usage: arch <arch_name>");
		}

		cout << "Setting arch to " << token << "." << endl;
	}
	else {
		throw runtime_error("Unknown command.");
	}
}
