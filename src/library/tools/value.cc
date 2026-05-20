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
 #include <private/value.h>
 #include <private/value.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/value.h>
 #include <functional>
 #include <errno.h>
 #include <stdexcept>

 using namespace std;
 using namespace Udjat;

 namespace Udjat {


	/*
	PyObject * Python::ObjectFactory(const Udjat::Value &value) noexcept {

		Python::Guard guard;

		try {

			switch((Value::Type) value) {
			case Udjat::Value::Type::Undefined:
				return Py_None;

			case Udjat::Value::Type::Array:
				{
					PyObject* list = PyList_New(0);
    				if(!list) {
						throw runtime_error(_("Error creating python list."));
					}

					try {

						value.for_each([list](const Value &value){
							PyObject *object = ObjectFactory(value); 
							PyList_Append(list,object);
							Py_DecRef(object);
							return false;
						});

					} catch(...) {

						Py_DecRef(list);
						throw;

					}

					return list;
				}

			case Udjat::Value::Type::Object:
				{
					PyObject* dict = PyDict_New();
					if(!dict) {
						throw runtime_error(_("Error creating python dictionary."));
					}

					try {

						value.for_each([dict](const char *name, const Value &value){
							PyObject *object = ObjectFactory(value); 
							PyDict_SetItemString(dict, name, object);
							Py_DecRef(object);
							return false;
						});

					} catch(...) {

						Py_DecRef(dict);
						throw;

					}

					return dict;
				}

			case Udjat::Value::Type::String:
			case Udjat::Value::Type::Url:
			case Udjat::Value::Type::State:
			case Udjat::Value::Type::Icon:
				{
					std::string response;
					value.get(response);
					return PyUnicode_FromString(response.c_str());
				}

			case Udjat::Value::Type::Timestamp:
				{

					return Py_None;
				}

			case Udjat::Value::Type::Signed:
				{

					return Py_None;
				}

			case Udjat::Value::Type::Unsigned:
				{
					long response;
					value.get(response);
					return PyLong_FromLong(response);
				}

			case Udjat::Value::Type::Fraction:
			case Udjat::Value::Type::Real:
				{
					double response;
					value.get(response);
					return PyFloat_FromDouble(response);
				}

			case Udjat::Value::Type::Boolean:
				{
					bool response;
					value.get(response);
					return response ? Py_True : Py_False;
				}

			case Udjat::Value::Type::Report:
				throw runtime_error(_("Unexpected value type in python callback."));

			}

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected value type in python callback."));
			
		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());
		
		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));
		}

		return NULL;
	}

	Udjat::Value & Python::get(Udjat::Value &value, PyObject *obj) noexcept {

		if(!obj) {

			value = "";

		} else {

			Python::Guard guard;

			if (PyUnicode_Check(obj)) {

				// It is a Python string (str)		
				value = PyUnicode_AsUTF8(obj);

			} else if (PyLong_Check(obj)) {

				// It is a Python int (or a subclass of int)
				value = PyLong_AsLong(obj);

			} else {

				// It's an object, convert it.
				auto pyStr = make_handle(PyObject_Str(obj));
				if(pyStr) {
					value = PyUnicode_AsUTF8(pyStr.get());
				} else {
					value = "";
				}

			}

		}

		return value;

	}
	*/

 }


 UDJAT_PRIVATE int value_setattr(PyObject *self, PyObject *attr, PyObject *value) {

	try {

		Udjat::Value &object = Python::value_get_private<Udjat::Value>(self);


	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return -1;

 }

 UDJAT_PRIVATE PyObject * value_getattr(PyObject *self, PyObject *attr) {

	try {

		Udjat::Value &object = Python::value_get_private<Udjat::Value>(self);

		switch((Value::Type) object) {
		case Udjat::Value::Type::Undefined:
			return Py_None;

		case Udjat::Value::Type::Array:
			throw runtime_error("Unable to convert array to pyobject");

		case Udjat::Value::Type::Object:
			throw runtime_error("Unable to convert object to pyobject");

		case Udjat::Value::Type::String:
		case Udjat::Value::Type::Url:
		case Udjat::Value::Type::State:
		case Udjat::Value::Type::Icon:
			{
				std::string response;
				object.get(response);
				return PyUnicode_FromString(response.c_str());
			}

		case Udjat::Value::Type::Timestamp:
			{
				return Py_None;
			}

		case Udjat::Value::Type::Signed:
			{
				int response;
				object.get(response);
				return PyLong_FromLong(response);
			}

		case Udjat::Value::Type::Unsigned:
			{
				long response;
				object.get(response);
				return PyLong_FromLong(response);
			}

		case Udjat::Value::Type::Fraction:
		case Udjat::Value::Type::Real:
			{
				double response;
				object.get(response);
				return PyFloat_FromDouble(response);
			}

		case Udjat::Value::Type::Boolean:
			{
				bool response;
				object.get(response);
				return response ? Py_True : Py_False;
			}

		case Udjat::Value::Type::Report:
			throw runtime_error(_("Unexpected value type in python callback."));

		}

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected value type in python callback."));

		
	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;

 }

