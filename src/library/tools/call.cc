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
 #include <private/modules.h>
 #include <private/tools.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/logger.h>
 #include <private/request.h>
 #include <private/response.h>
 #include <functional>
 #include <errno.h>
 #include <mutex>
 #include <stdexcept>

 using namespace std;

 namespace Udjat {

	PyObject * Python::call(PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept {

		try {

			return callback(args);

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

	PyObject * Python::call(int required_args, PyObject *args, const std::function<PyObject *(PyObject *args)> &callback) noexcept {

		if(PyTuple_Size(args) != required_args) {
			PyErr_SetString(PyExc_RuntimeError, strerror(EINVAL));
			return NULL;
		}

		try {

			return callback(args);

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

	PyObject * Python::call(int required_args, PyObject *args, const std::function<void (PyObject *args)> &callback) noexcept {

		if(PyTuple_Size(args) != required_args) {
			PyErr_SetString(PyExc_RuntimeError, strerror(EINVAL));
			return NULL;
		}

		try {

			callback(args);
			Py_RETURN_NONE;

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;
	}

	PyObject * Python::call(const std::function<PyObject *(void)> &callback) noexcept {

		try {

			return callback();

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

		}

		return NULL;

	}

	UDJAT_PRIVATE PyObject * Python::call(PyObject *self, const char *method_name) {

		if(!self) {
			throw logic_error(_("The object is not binded with python"));
		}
		
		debug("Calling ",Py_TYPE(self)->tp_name, ".",method_name," without any arguments");

		Python::Guard guard;

		auto func = Python::make_handle(PyObject_GetAttrString(self, method_name));

		if(!PyCallable_Check(func.get())) {
			throw logic_error(Logger::Message{_("The method {} is not callable on {}"),method_name,Py_TYPE(self)->tp_name});
		}

		auto tuple = Python::make_handle(PyTuple_New(0));

		PyObject *response = PyObject_CallObject(func.get(), tuple.get());

		if(!response) {
			throw runtime_error(Python::exception());
		}

		return response;
		
	}

	static PyObject * call_method(PyObject *self, const char *method_name, vector<PyObject *> &args) {

		debug("Calling ",Py_TYPE(self)->tp_name, ".",method_name," with ",args.size()," arguments");

		Python::Guard guard;

		auto func = Python::make_handle(PyObject_GetAttrString(self, method_name));

		if(!PyCallable_Check(func.get())) {
			throw logic_error(Logger::Message{_("The method {} is not callable on {}"),method_name,Py_TYPE(self)->tp_name});
		}

		auto tuple = Python::make_handle(PyTuple_New(args.size()));
		for(size_t i = 0; i < args.size(); i++) {
			PyTuple_SetItem(tuple.get(), i, args[i]);
		}

		PyObject *response = PyObject_CallObject(func.get(), tuple.get());

		if(!response) {
			throw runtime_error(Python::exception());
		}

		return response;
		
	}

	UDJAT_PRIVATE PyObject * Python::call(PyObject *self, const char *method_name, PyObject *arg, ...) {

		vector<PyObject *> args;

		va_list list;
		va_start(list,arg);
		while(arg) {
			args.push_back(arg);
			arg	= va_arg(list, PyObject *);
		}
		va_end(list);

		return call_method(self,method_name,args);
		
	}

	int Python::call(PyObject *self, const Udjat::Request &request, Udjat::Response &response, bool except) {

		debug(__FUNCTION__)

		auto pyReq = Python::factory(const_cast<Udjat::Request &>(request));
		auto pyResp = Python::factory(response);

		auto ret = Python::call(
				self,
				"call",
				pyReq.get(),
				pyResp.get(),
				NULL
		);

		int rc = -1;

		if (PyLong_Check(ret)) {
			rc = (int) PyLong_AsLong(ret);
		} else if(except) {
			Py_DecRef(ret);
			throw logic_error(_("Unexpected response from python method"));
		} else {
			Logger::String{"Unexpected response from method"}.error("python");
		}

		Py_DecRef(ret);

		return rc;
	}

 }

