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

#include <iostream>
#include <stdexcept>
#include <vector>
#include <iomanip>

#include "repl.h"
#include "run.h"
#include "retarget.h"

using namespace std;

struct usage_error : exception {};

vector<command*> commands {
	new repl_cmd(),
	new run_cmd(),
	new retarget_cmd(),
};

void handle_args(int args_num, char const *args[]) {
	if (args_num == 0) {
		throw usage_error(); 
	}

	command *cmd = nullptr;
	for (command *c : commands) {
		if (c->name == args[0]) {
			cmd = c;
			break;
		}
	}

	if (!cmd) {
		throw usage_error();
	}

	cmd->run(args_num - 1, args + 1);
}

int main(int argc, char const *argv[]) {
	try {
		handle_args(argc - 1, argv + 1);
	}
	catch (usage_error& e) {
		cerr << "Usage: " << argv[0] << " <command> [options...]" << endl;
		for (command *c : commands) {
			cerr << "   " << left << setw(10) << c->name;
			cerr << c->description << endl;
		}
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}