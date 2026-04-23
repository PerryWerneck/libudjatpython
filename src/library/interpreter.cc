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
 #include <udjat/tools/memory.h>
 #include <udjat/tools/logger.h>
 
 #include <Python.h>

 using namespace std;

 namespace Udjat {

	std::recursive_mutex Python::Interpreter::guard;

	Python::Interpreter::Interpreter() {
		
		lock_guard<recursive_mutex> lock(guard);

		Logger::String{"Initializing python " PY_VERSION " interpreter"}.trace();

		// 1. Initialize the config with default Python settings
		PyConfig_InitPythonConfig(&config);

		// 2. Set the program name (Replacement for Py_SetProgramName)
		// This implicitly handles decoding the string
		Application::Name name{true};

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
		
		lock_guard<recursive_mutex> lock(guard);

 		PyConfig_Clear(&config);
		Py_Finalize();
	}

	int Python::Interpreter::run(const char *script_text) {
		
		lock_guard<recursive_mutex> lock(guard);

		int rc = PyRun_SimpleString(script_text);

		if (PyErr_Occurred()) {
			auto exc = make_handle(PyErr_GetRaisedException(),Py_DECREF);
			if (exc.get()) {
                // 3. Convert the exception to a string (equivalent to str(e))
                auto exc_str = make_handle(PyObject_Str(exc.get()),Py_DECREF);
                if (exc_str.get()) {
                    string msg = PyUnicode_AsUTF8(exc_str.get());
					throw runtime_error(msg);
                }
            }			
		}

		return rc;

	}

	int Python::Interpreter::run(const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress) {
		
		lock_guard<recursive_mutex> lock(guard);

		int rc;

		rc = PyRun_SimpleString("import sys, io\nsys.stdout = io.StringIO()");
    	rc = PyRun_SimpleString(script_text);

    	auto sys_module = make_handle(PyImport_ImportModule("sys"),Py_DECREF);
    	auto stdout_obj = make_handle(PyObject_GetAttrString(sys_module.get(), "stdout"),Py_DECREF);
    	auto result = make_handle(PyObject_CallMethod(stdout_obj.get(), "getvalue", NULL),Py_DECREF);

    	const char * output = PyUnicode_AsUTF8(result.get());

		progress(0,strlen(output),output,strlen(output));

		PyRun_SimpleString("import sys\nsys.stdout = sys.__stdout__");

	}

 }

