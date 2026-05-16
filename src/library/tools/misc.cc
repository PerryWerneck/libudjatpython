/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2026 Perry Werneck <perry.werneck@gmail.com>
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
 #include <private/tools.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/memory.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/logger.h>
 #include <memory>

 using namespace std;

 namespace Udjat {

	UDJAT_PRIVATE std::recursive_mutex Python::guard;

	static void decref(PyObject *self) {
		lock_guard<recursive_mutex> lock(Python::guard);
		Py_DecRef(self);
	}

	std::shared_ptr<PyObject> Python::make_handle(PyObject *self) {
		lock_guard<recursive_mutex> lock(guard);
		return Udjat::make_handle(self,decref);
	}

	std::string Python::exception(bool write_to_log) {

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

	bool Python::compare(PyObject *a, PyObject *b) {

		if(a == b) {
			return true;
		}

		if(!a || !b) {
			return false;
		}

		lock_guard<recursive_mutex> lock(Python::guard);

		int result = PyObject_RichCompareBool(a, b, Py_EQ);

		if (result == 1) {
			
			// Objects are equal
			return true;
			
		} else if (result == 0) {

			// Objects are NOT equal
			return false;

		} else {

			// An error occurred (result == -1)
			throw runtime_error(Python::exception());

		}
		
	}

 }

 namespace std {

 	string to_string(PyObject *value) noexcept {

		if(!value) {
			return "";
		}

		lock_guard<recursive_mutex> lock(Udjat::Python::guard);

		if (PyUnicode_Check(value)) {
    		// It is a Python string (str)		
			return PyUnicode_AsUTF8(value);
		}

		if (PyLong_Check(value)) {
			// It is a Python int (or a subclass of int)
			return std::to_string(PyLong_AsLong(value));
		}

		// It's an object, convert it.
		auto pyStr = Udjat::Python::make_handle(PyObject_Str(value));
		if(!pyStr) {
			return "";
		}

		return PyUnicode_AsUTF8(pyStr.get());

	}

}