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
 #include <udjat/tools/request.h>
 #include <private/value.h>
 #include <private/request.h>
 #include <udjat/tools/logger.h>
 #include <private/tools.h>

 using namespace Udjat;
 using namespace std;

 // ---------------------------------------------------------------------------------------
 // C++ Object
 // ---------------------------------------------------------------------------------------

 namespace Udjat {

	 UDJAT_PRIVATE std::shared_ptr<PyObject> Python::factory(Udjat::Request &request) {

		Python::Guard guard;

		auto self = make_handle(PyObject_CallFunction((PyObject*)&request_type, "O", Py_None));

		if(!self) {
			debug("Failed building object");
			throw runtime_error(exception());
		}

		// Store Request in the object.
		{
			pyValue *native = ((pyValue *) self.get());
			native->handler = &request;
		}

		return self;
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE PyObject	* request_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
 	return type->tp_alloc(type,0);
 }

 UDJAT_PRIVATE void request_dealloc(PyObject * self) {
	debug(__FUNCTION__);
	Py_TYPE(self)->tp_free(self);
 } 

 UDJAT_PRIVATE int request_init(PyObject *self, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
	return 0;

 }

 UDJAT_PRIVATE void request_finalize(PyObject *self) {
 }

 UDJAT_PRIVATE int request_setattr(PyObject *self, PyObject *attr, PyObject *value) {


	return value_setattr(self,attr,value);
 }

 UDJAT_PRIVATE PyObject * request_getattr(PyObject *self, PyObject *attr) {


	return value_getattr(self,attr);
 }
