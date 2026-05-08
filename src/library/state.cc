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
 #include <udjat/tools/xml.h>
 #include <private/object.h>
 #include <private/state.h>
 #include <private/tools.h>
 #include <cstdint>

 using namespace Udjat;
 using namespace std;

 // ---------------------------------------------------------------------------------------
 // C++ Object
 // ---------------------------------------------------------------------------------------

 namespace Udjat {

	Python::State::State(const XML::Node &node, const Type type) : Udjat::Abstract::State{node} {

		lock_guard<recursive_mutex> lock(Python::guard);

		switch(type) {
		case Type::Numeric:
			current_value = PyLong_FromLong(
				XML::AttributeFactory(node,"value").as_int()
			);
			break;

		case Type::String:
			current_value = PyUnicode_FromString(
				XML::AttributeFactory(node,"value").as_string()
			);
			break;

		}

	}

	Python::State::~State() {
		if(current_value) {
			lock_guard<recursive_mutex> lock(Python::guard);
			Py_DecRef(current_value);
			current_value = NULL;
		}
	}

	std::string Python::State::value() const {
		return Python::to_string(current_value);
	}

	std::shared_ptr<PyObject> Python::State::factory(Abstract::State *st) {

		lock_guard<recursive_mutex> lock(guard);

		debug("Building state...");
		auto state = make_handle(PyObject_CallFunction((PyObject*)&state_type, "O", Py_None));

		if(!state) {
			debug("Failed building python state");
			throw runtime_error(exception());
		}

		// Store State in the object.
		{
			pyAbstractObject *native = ((pyAbstractObject *) state.get());
			native->handler = dynamic_cast<Udjat::Abstract::Object *>(st);
		}

		return state;
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE PyObject	* state_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__);
	if (PyErr_Occurred()) {
        return NULL; // Exit early if something else already failed
    }

	PyObject *self = type->tp_alloc(type,0);

	if(self) {
		((pyAbstractObject *) self)->handler = NULL;
	}

	return self;
 }

 UDJAT_PRIVATE void state_dealloc(PyObject * self) {
	Py_TYPE(self)->tp_free(self);
 } 
 
 UDJAT_PRIVATE int state_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	return 0;

 }

 UDJAT_PRIVATE void state_finalize(PyObject *self) {

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Abstract::State &state)> &callback) {

	try {

		return callback(get_private<Udjat::Abstract::State>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;
	
 }

