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
 #include <udjat/tools/value.h>
 #include <private/value.h>
 #include <udjat/tools/logger.h>
 #include <private/tools.h>

 using namespace Udjat;
 using namespace std;

 // ---------------------------------------------------------------------------------------
 // C++ Object
 // ---------------------------------------------------------------------------------------

 namespace Udjat {

	 UDJAT_PRIVATE std::shared_ptr<PyObject> Python::factory(Udjat::Value &value) {

		Python::Guard guard;

		auto self = make_handle(PyObject_CallFunction((PyObject*)&value_type, "O", Py_None));

		if(!self) {
			debug("Failed building object");
			throw runtime_error(exception());
		}

		// Store Request in the object.
		{
			pyValue *native = ((pyValue *) self.get());
			native->handler = &value;
		}

		return self;
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE PyObject	* value_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
 	return type->tp_alloc(type,0);
 }

 UDJAT_PRIVATE void value_dealloc(PyObject * self) {
	debug(__FUNCTION__);
	Py_TYPE(self)->tp_free(self);
 } 

 UDJAT_PRIVATE int value_init(PyObject *self, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
	return 0;

 }

 UDJAT_PRIVATE void value_finalize(PyObject *self) {
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
		case Udjat::Value::Type::Object:
		case Udjat::Value::Type::Report:
			{
				// Return another value object.
				auto ptr = Python::factory(object);
				Py_IncRef(ptr.get());
				return ptr.get();
			}

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

		}

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected value type in python callback."));

		
	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Value &object)> &callback) {

	try {

		return callback(Python::value_get_private<Udjat::Value>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;

 }

 UDJAT_PRIVATE PyObject * value_str(PyObject *self) {

	return call(self,[](Udjat::Value &object) -> PyObject * {
		return PyUnicode_FromString(
			object.to_string().c_str()
		);
	});

 }
