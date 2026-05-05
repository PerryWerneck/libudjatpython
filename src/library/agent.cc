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
 #include <udjat/agent/abstract.h>
 #include <private/object.h>
 #include <private/agent.h>
 #include <private/tools.h>
 #include <udjat/tools/xml.h>

 using namespace Udjat;
 using namespace std;

 UDJAT_PRIVATE PyObject	* agent_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	return type->tp_alloc(type,0);
 }

 UDJAT_PRIVATE void agent_dealloc(PyObject * self) {
	Py_TYPE(self)->tp_free(self);
 } 
 
 UDJAT_PRIVATE int agent_init(PyObject *self, PyObject *args, PyObject *kwds) {

	try {


		return 0;

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, "Unexpected error in core module");

	}

	return -1;

 }

 UDJAT_PRIVATE void agent_finalize(PyObject *self) {

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Python::Agent &agent)> &callback) {

	try {

		return callback(get_private<Udjat::Python::Agent>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;
	
 }

 UDJAT_PRIVATE PyObject * agent_failed(PyObject *self, PyObject *args) {

	return call(self,[args](Udjat::Python::Agent &agent) -> PyObject * {

		const char *summary;
		const char *body;

		switch(PyTuple_Size(args)) {
		case 1:
			if(!PyArg_ParseTuple(args, "s", &summary)) {
				throw runtime_error(_("Invalid argument"));
			}
			break;

		case 2:
			if(!PyArg_ParseTuple(args, "ss", &summary,&body)) {
				throw runtime_error(_("Invalid argument"));
			}
			break;

		default:
			throw runtime_error(_("Invalid argument"));

		}

		agent.failed(summary,body);

		return Py_None;

	});

 }

 UDJAT_PRIVATE PyObject * agent_get_by_path(PyObject *self, PyObject *args) {

	return call(self,[](Udjat::Python::Agent &agent) -> PyObject * {


	});

 }

 UDJAT_PRIVATE PyObject * agent_invalidate(PyObject *self, PyObject *args) {

	return call(self,[args](Udjat::Python::Agent &agent) -> PyObject * {

		if(PyTuple_Size(args) != 1) {
			throw runtime_error(_("Invalid argument"));
		}


		return Py_None;
	});

 }

 UDJAT_PRIVATE PyObject * agent_start(PyObject *self, PyObject *args) {

	return call(self,[](Udjat::Python::Agent &agent) -> PyObject * {


	});

 }

 UDJAT_PRIVATE PyObject * agent_stop(PyObject *self, PyObject *args) {

	return call(self,[](Udjat::Python::Agent &agent) -> PyObject * {


	});

 }

 UDJAT_PRIVATE PyObject * agent_refresh(PyObject *self, PyObject *args) {

	return call(self,[](Udjat::Python::Agent &agent) -> PyObject * {


	});

 }
