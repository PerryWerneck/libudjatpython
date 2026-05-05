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
 #include <private/settings.h>
 #include <private/tools.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/abstract/object.h>
 #include <udjat/tools/intl.h>

 #include <Python.h>

 using namespace Udjat;
 using namespace std;

 UDJAT_PRIVATE PyObject	* settings_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	return type->tp_alloc(type,0);
 }

 UDJAT_PRIVATE void settings_dealloc(PyObject * self) {
	Py_TYPE(self)->tp_free(self);
 } 
 
 UDJAT_PRIVATE int settings_init(PyObject *self, PyObject *args, PyObject *kwds) {

	try {


		return 0;

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, "Unexpected error in core module");

	}

	return -1;

 }

 UDJAT_PRIVATE void settings_finalize(PyObject *self) {

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(const XML::Node &node)> &callback) {

	pySettings *settings = ((pySettings *) self);
	
	if(!settings->handler) {
		PyErr_SetString(PyExc_RuntimeError, _("The object is empty"));
		return NULL;
	}

	try {

		return callback(*settings->handler);

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;

 }

 UDJAT_PRIVATE PyObject * settings_str(PyObject *self) {

	return call(self,[](const XML::Node &node) -> PyObject * {

		return PyUnicode_FromString(
			node.name()
		);

	});

 }

 UDJAT_PRIVATE PyObject * settings_getattr(PyObject *self, PyObject *attr) {

	return call(self,[attr](const XML::Node &node) -> PyObject * {

		const char *attrname;
		if(!PyArg_ParseTuple(attr, "s", &attrname)) {
			throw runtime_error("Invalid argument");
		}

		return PyUnicode_FromString(
			XML::AttributeFactory(
				node, 
				attrname
			).as_string()
		);

	});

 }
 
 UDJAT_PRIVATE PyObject * settings_get(PyObject *self, PyObject *args) {

	return call(self,[args](const XML::Node &node) -> PyObject * {

		if(PyTuple_Size(args) != 2) {
			throw runtime_error(_("Method requires 2 arguments"));
		}

		const char *attrname;
		const char *def;
		if(!PyArg_ParseTuple(args, "ss", &attrname,&def)) {
			throw runtime_error(_("Invalid argument"));
		}

		return PyUnicode_FromString(
			XML::AttributeFactory(
				node, 
				attrname
			).as_string(def)
		);

	});


 }
