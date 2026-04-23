/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <config.h>

 #include <udjat/defs.h>
 #include <udjat/tools/application.h>
 #include <private/interpreter.h>
 #include <stdexcept>
 
 #include <Python.h>

 using namespace std;

 namespace Udjat {

	Python::Interpreter & Python::Interpreter::Instance() {
		static Interpreter instance;
		return instance;
	}
	
	Python::Interpreter::Interpreter() {
		lock_guard<recursive_mutex> lock(*this);

		// 1. Initialize the config with default Python settings
		PyConfig_InitPythonConfig(&config);

		// 2. Set the program name (Replacement for Py_SetProgramName)
		// This implicitly handles decoding the string
		// Application::Name name{true};
		string name{"/home/perry/project/udjat/module/python/.build/udjatpython"};

		debug("Program_name=",name.c_str());

		status = PyConfig_SetBytesString(&config, &config.program_name, name.c_str());
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			throw runtime_error("Unable to set python application name");
		}

		// 3. Initialize the interpreter from this config
		status = Py_InitializeFromConfig(&config);
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			throw runtime_error("Unable to initialize python interpreter");
		}

		Py_Initialize();

		// Import libraries
		// PyRun_SimpleString("import sys");
		// PyRun_SimpleString("sys.path.append('/path/to/your/library_folder')")

	}

	Python::Interpreter::~Interpreter() {
		lock_guard<recursive_mutex> lock(*this);
 		PyConfig_Clear(&config);
		Py_Finalize();
	}

	void Python::Interpreter::run(const char *script_text) {
		lock_guard<recursive_mutex> lock(*this);
		PyRun_SimpleString(script_text);
	}

 }

