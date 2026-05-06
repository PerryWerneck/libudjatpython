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

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <private/object.h>
 #include <private/tools.h>
 #include <private/xml.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/abstract/object.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/intl.h>
 #include <memory>

 #include <Python.h>

 using namespace Udjat;
 using namespace std;

 static PyObject * call(PyObject *self,const std::function<PyObject *(Abstract::Object &object)> &callback) {

	try {

		return callback(get_private<Abstract::Object>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;

 }

 UDJAT_PRIVATE PyObject * object_str(PyObject *self) {

	return call(self,[](Abstract::Object &object) -> PyObject * {
		return PyUnicode_FromString(
			object.to_string().c_str()
		);
	});

 }

 UDJAT_PRIVATE PyObject * object_getattr(PyObject *self, PyObject *attr) {

	const char *attrname = PyUnicode_AsUTF8(attr);

	debug(__FUNCTION__,"(",attrname,")");

	if(attrname && *attrname != '_' && ((pyAbstractObject *) self)->handler) {

		return call(self,[attr](Abstract::Object &object) -> PyObject * {

			const char *name = "";
			if(!PyArg_ParseTuple(attr, "s", &name)) {
				throw runtime_error("Invalid argument");
			}

			return PyUnicode_FromString(
				object.getProperty(name).c_str()
			);

		});

	}

	return PyObject_GenericGetAttr(self, attr);

 }
 
 UDJAT_PRIVATE int object_setattr(PyObject *self, PyObject *attr, PyObject *value) {

	try {

		const char *name = "";
		if(!PyArg_ParseTuple(attr, "s", &name)) {
			throw runtime_error("Invalid argument");
		}

		const char *val = "";
		if(!PyArg_ParseTuple(attr, "s", &val)) {
			throw runtime_error("Invalid argument");
		}

		get_private<Abstract::Object>(self).setProperty(name,val);

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return 0;

 }

 static PyObject * write_log(const Logger::Level level, PyObject *self, PyObject *arg) {

	if(PyTuple_Size(arg) != 1) {
		PyErr_SetString(PyExc_RuntimeError, _("Too many arguments"));
		return NULL;
	}

	try {

		const char *name = "python";
		pyAbstractObject *object = ((pyAbstractObject *) self);
		
		if(object->handler) {
			name = object->handler->name();
		}
		
		const char *msg = "";
		if (!PyArg_ParseTuple(arg, "s", &msg))
            throw system_error(ENOENT,system_category(),_("The log message should be a string"));

		Logger::Message{msg}.write(level,name);
		return Py_None;

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python logger."));

	}

	return NULL;
 }

 UDJAT_PRIVATE PyObject * object_trace(PyObject *self, PyObject *msg) {
	return write_log(Logger::Trace,self,msg);
 }

 UDJAT_PRIVATE PyObject * object_error(PyObject *self, PyObject *msg) {
	return write_log(Logger::Error,self,msg);
 }

 UDJAT_PRIVATE PyObject * object_warning(PyObject *self, PyObject *msg) {
	return write_log(Logger::Warning,self,msg);
 }

 UDJAT_PRIVATE PyObject * object_info(PyObject *self, PyObject *msg) {
	return write_log(Logger::Info,self,msg);
 }

 UDJAT_PRIVATE bool object_setup(PyObject *self, const char *name, const XML::Node &node) {

	lock_guard<recursive_mutex> lock(Python::guard);

	auto response = Python::make_handle(Python::call(self, "setup", node));
	if(!response.get()) {
		throw runtime_error(Python::exception());
	}

	if(Py_IsNone(response.get())) {
		Logger::Message{"Setup method on {} returned PyNone when I was expecting a boolean",Py_TYPE(self)->tp_name}.warning(name);
		return false;
	}

	if(!PyBool_Check(response.get())) {
		Logger::String{"Setup method on {} returned value when I was expecting a boolean",Py_TYPE(self)->tp_name}.warning(name);
		return false;
	}		

	int result = PyObject_IsTrue(response.get());
	if (result == -1) {
		throw logic_error(Python::exception());
	}
	
	return (result != 0);

 }

