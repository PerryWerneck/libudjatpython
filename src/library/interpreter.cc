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

 #include <Python.h>

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <udjat/tools/application.h>
 #include <stdexcept>
 #include <udjat/tools/memory.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/memory.h>
 
 #include <private/interpreter.h>
 #include <private/modules.h>

 using namespace std;

 namespace Udjat {

	Python::Interpreter & Python::Interpreter::getInstance() {
		static Interpreter instance;
		return instance;
	}

	Python::Interpreter::Interpreter() {

		Logger::String{"Initializing python " PY_VERSION " interpreter"}.info();
		
		lock_guard<recursive_mutex> lock(guard);

		// Initialize the config with default Python settings
		PyConfig_InitPythonConfig(&config);

		// Set the program name (Replacement for Py_SetProgramName)
		// This implicitly handles decoding the string
		Application::Name name{true};

		debug("Program_name=",name.c_str());

		status = PyConfig_SetBytesString(&config, &config.program_name, name.c_str());
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			throw runtime_error("Unable to set python application name");
		}

		// Add built-in modules, before Py_Initialize
		if (PyImport_AppendInittab("udjat", PyInit_udjat) == -1) {
			throw runtime_error("Error: could not extend in-built modules table");
		}

		//if (PyImport_AppendInittab("logger", PyInit_logger) == -1) {
		//	throw runtime_error("Error: could not extend in-built modules table");
		//}

		//if (PyImport_AppendInittab("config", PyInit_config) == -1) {
		//	throw runtime_error("Error: could not extend in-built modules table");
		//}

		// Initialize the interpreter from this config
		status = Py_InitializeFromConfig(&config);
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			throw runtime_error("Unable to initialize python interpreter");
		}

	}

	Python::Interpreter::~Interpreter() {
		Logger::String{"Deinitializing python " PY_VERSION " interpreter"}.info();

		lock_guard<recursive_mutex> lock(guard);

		PyConfig_Clear(&config);
		Py_Finalize();
	}

	int Python::Interpreter::run(const char *, const char *script_text) {
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

	int Python::Interpreter::run(const char *, const char *script_text, const std::function<bool(uint64_t current, uint64_t total, const void *data, size_t len)> &progress) {
		
		lock_guard<recursive_mutex> lock(guard);

		int rc;

		run(
			"import sys, io\n"
			"sys.stdout = io.StringIO()\n"
			"sys.stderr = io.StringIO()"
		);

    	rc = run(script_text);

    	auto sys_module = make_handle(PyImport_ImportModule("sys"),Py_DECREF);
    	auto stdout_obj = make_handle(PyObject_GetAttrString(sys_module.get(), "stdout"),Py_DECREF);
    	auto result = make_handle(PyObject_CallMethod(stdout_obj.get(), "getvalue", NULL),Py_DECREF);

    	const char * output = PyUnicode_AsUTF8(result.get());

		// TODO: Split the captured output into individual lines 
		// and invoke the progress callback for each.
		progress(0,strlen(output),output,strlen(output));

		run(
			"import sys\n"
			"sys.stdout = sys.__stdout__\n"
			"sys.stderr = sys.__stderr__"
		);

		return rc;
	}

	PyObject * Python::Interpreter::import(const char *pysource) {

		lock_guard<recursive_mutex> lock(guard);

		PyObject *pName = PyUnicode_FromString(pysource);
 		PyObject* pModule = PyImport_Import(pName);
    	Py_DECREF(pName); // Clean up the name object immediately

		if(!pModule) {
			exception(true);
			throw runtime_error(Logger::String{"Unable to load '",pysource,"'"});
		}

		return pModule;
	}

	PyObject * Python::Interpreter::module(const char *module_name) {

		lock_guard<recursive_mutex> lock(guard);
		return PyImport_ImportModule(module_name);

	}

	std::string Python::Interpreter::exception(bool write_to_log) {

		lock_guard<recursive_mutex> lock(guard);

		string response;

		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback); // Clears the global error state
    
    	if (pvalue == nullptr) 
			return _("Unknown Python Error");

		// Normalize the exception (essential for proper error strings)
		PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

		// Convert the exception value to a Python string object
		PyObject* pstr = PyObject_Str(pvalue);
		if (pstr) {
			response += PyUnicode_AsUTF8(pstr); // Extract C-string from Python string
			Py_DECREF(pstr);
		}

		if(write_to_log) {
			Logger::String{response.c_str()}.error("python");
		}
		
		// TODO: Extract line number from traceback if available
		/*
		PyObject* pModule = PyImport_ImportModule("traceback");
		if (pModule) {
			// 2. Get the 'format_exception' function
			PyObject* pFunc = PyObject_GetAttrString(pModule, "format_exception");
			if (pFunc && PyCallable_Check(pFunc)) {
				// 3. Call format_exception(ptype, pvalue, ptraceback)
				// format_exception returns a list of strings
				PyObject* pList = PyObject_CallFunctionObjArgs(pFunc, ptype, pvalue, ptraceback, NULL);

				if (pList && PyList_Check(pList)) {
					Py_ssize_t size = PyList_Size(pList);
					for (Py_ssize_t i = 0; i < size; i++) {
						PyObject* pItem = PyList_GetItem(pList, i); // Borrowed reference
						Logger::String{PyUnicode_AsUTF8(pItem)}.error("python");
					}
					Py_DECREF(pList);
				}
				Py_XDECREF(pFunc);
			}
			Py_DECREF(pModule);
		}
		*/

		Py_XDECREF(ptype);
		Py_XDECREF(pvalue);
		Py_XDECREF(ptraceback);
		
		return response;
		
	}

	PyObject * Python::Interpreter::factory(const char *pysource, const char *method, const XML::Node &node) {
	
		lock_guard<recursive_mutex> lock(guard);

		debug("Creating object from ",pysource," using ",method,"(settings)");
		auto module = make_handle(PyImport_ImportModule(pysource),Py_DECREF);
		if(!module) {
			exception(true);
			throw runtime_error(Logger::String{"Unable to load '",pysource,"'"});
		}
		
		debug("Searching for '",method,"'");
		auto func = make_handle(PyObject_GetAttrString(module.get(), method),Py_DECREF);
		if(!func) {
			throw logic_error(Logger::Message{_("The method {}(settings) is required on '{}"),method,pysource});
		}

		if(!PyCallable_Check(func.get())) {
			throw logic_error(Logger::Message{_("The method {}(settings) is not callable on '{}"),method,pysource});
		}

		// FIX-ME: Build an empty settings object for node.
		auto args = make_handle(PyTuple_Pack(1, PyLong_FromLong(10)),Py_DECREF);

		debug("Calling object...");
		PyObject *response = PyObject_CallObject(func.get(), args.get());

		if(!response) {
			throw runtime_error(this->exception());
		}
		
		return response;
	}

 }

