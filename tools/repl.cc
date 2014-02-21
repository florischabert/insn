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
#include <functional> 
#include <map>

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

struct quit : exception {};

repl::repl() {
	cmds["help"] = &repl::help_cmd;
	cmds["quit"] = &repl::quit_cmd;
	cmds["arch"] = &repl::arch_cmd;
	cmds["asm"] = &repl::asm_cmd;
}

void repl::loop() {
	rl_bind_key('\t', rl_insert);

	while (true) {
		char *cmd_str = readline("> ");
		if (!cmd_str) {
			cout << endl;
			break;
		}

		try {
			istringstream tokens(cmd_str);
			string token;

			tokens >> token;
			void (repl::*handler)(std::istringstream&) = cmds[token];
			if (handler) {
				(this->*handler)(tokens);
			}
			else {
				throw runtime_error("Unknown command.");
			}

			add_history(cmd_str);
		}
		catch (quit) {
			break;
		}
		catch (runtime_error& e) {
			cout << e.what() << endl;
		}
	}

	cout << "Done." << endl;
}

void repl::help_cmd(std::istringstream& cmd) {
	cout << "Help:" << endl;
}

void repl::quit_cmd(std::istringstream& cmd) {
	throw quit();
}

void repl::arch_cmd(std::istringstream& cmd) {
	if (!cmd) {
		throw runtime_error("Usage: arch <arch_name>");
	}

	string arch_name;
	cmd >> arch_name;
	cout << "Setting arch to " << arch_name << "." << endl;
}

void repl::asm_cmd(std::istringstream& cmd) {

}
