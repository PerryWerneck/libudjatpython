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
 #include <udjat/tools/logger.h>
 #include <private/object.h>
 #include <private/state.h>
 #include <private/tools.h>
 #include <udjat/agent/state.h>
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
	}

	std::string Python::State::value() const {
		return Python::to_string(current_value);
	}

	PyObject * Python::State::factory(std::shared_ptr<Abstract::State> st) {

		lock_guard<recursive_mutex> lock(guard);

		debug("Building state from native object...");
		auto state = PyObject_CallFunction((PyObject*)&state_type, "O", Py_None);

		if(!state) {
			debug("Failed building python state");
			throw runtime_error(exception());
		}

		// Store State in the object.
		((pyAbstractObject *) state)->pvt->object = st;

		return state;
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE int state_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	if(kwds) {

		// Building state with a list of properties.
		static const char * const kwlist[] = {
			"name",
			"level",
			"label",
			"summary",
			"body",
			"icon",
			"url",
			NULL
		};

		PyObject *prop_list = NULL;
		// Format String Breakdown:
		// '|' -> Makes everything after it optional
		// 'O' -> PyObject* (Properties List)
		// 's' -> const char* (Host String)
		// 'i' -> int (Port)
		// 'i' -> int (Timeout)
		// 'p' -> int/boolean predicate (Verbose)
		// 'i' -> int (Retry)
		const char *props[] = {
			"python", // name
			"unimportant", // level
			"", // label
			"", // summary
			"", // body
			"", // icon
			"", // url
		};

		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|sssssss", kwlist, &props[0], &props[1], &props[2], &props[3], &props[4], &props[5])) {
        	return -1; 
    	}

#ifdef DEBUG
		for(size_t ix = 0; ix < sizeof(props)/sizeof(props[0]); ix++) {
			debug(kwlist[ix],"=",props[ix]);
		}
#endif
		/*
		pyAbstractObject *object = ((pyAbstractObject *) self);
		if(!object->pvt->object) {
			object->pvt->object = make_shared<State>(prop_list);
		}
		*/

	}
#ifdef DEBUG 
	else {
		debug("Building empty state");
	}
#endif

	return 0;

 }

 UDJAT_PRIVATE void state_finalize(PyObject *self) {	
	debug(__FUNCTION__);

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Abstract::State &state)> &callback) {

	try {

		return callback(*object_get_private<Udjat::Abstract::State>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;
	
 }

