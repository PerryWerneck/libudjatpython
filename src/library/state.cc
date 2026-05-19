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

	static const char * const kwlist[Python::State::PropertyCount+2] = {

		// Object properties
		"name",
		"label",
		"summary",
		"body",
		"icon",
		"url",

		// Extra values
		"level",
		NULL

	};

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

		for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
			Udjat::Abstract::State::getProperty(kwlist[ix],properties[ix]);
		}

	}

	Python::State::~State() {
		lock_guard<recursive_mutex> lock(Python::guard);
		if(this->current_value) {
			Py_DecRef(this->current_value);
			this->current_value = NULL;
		}
	}

	std::string Python::State::value() const {
		return std::to_string(current_value);
	}

	PyObject * Python::State::factory(std::shared_ptr<Abstract::State> st) {

		debug(__FUNCTION__);

		lock_guard<recursive_mutex> lock(guard);

		if (PyErr_Occurred()) {
			debug(__FUNCTION__,": PyErr_Occurred()")
			throw runtime_error(exception());
	    }

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

	void Python::State::set(PyObject *value) {

		lock_guard<recursive_mutex> lock(Python::guard);

		if(this->current_value) {
			Py_DecRef(this->current_value);
			this->current_value = NULL;
		}

		this->current_value = value;

		if(this->current_value) {
			Py_IncRef(this->current_value);
		}
	}

	static void load(PyObject *args, std::string props[Python::State::PropertyCount+1]) {

		debug("Building state object with ",Py_TYPE(args)->tp_name," arguments");

		props[0] = "python";
		props[Python::State::PropertyCount] = "unimportant";

		if(PyDict_Check(args)) {

			// It's a dict, use it.
			for(size_t ix = 0; ix < Python::State::PropertyCount+1; ix++) {

				PyObject* pValue = PyDict_GetItemString(args, kwlist[ix]);

				if(pValue && pValue != Py_None) {

					props[ix] = std::to_string(pValue);
					debug(kwlist[ix],"= '",props[ix].c_str(),"'");

				}

			}

		} else {

			throw runtime_error(_("Invalid argument"));
		}

	}

	std::shared_ptr<Python::State> Python::State::factory(PyObject *args) {

		std::string props[Python::State::PropertyCount+1];
		load(args,props);

		auto state = make_shared<Python::State>(props[Python::State::PropertyCount].c_str());

		for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
			state->setProperty(kwlist[ix],props[ix].c_str());
		}

		return state;
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE int state_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	if(kwds) {

		try {

			std::string props[Python::State::PropertyCount+1];
			load(kwds,props);

			auto state = make_shared<Python::State>(props[Python::State::PropertyCount].c_str());

			for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
				state->setProperty(kwlist[ix],props[ix].c_str());
			}

			pyAbstractObject *object = ((pyAbstractObject *) self);
			if(!object->pvt->object) {
				object->pvt->object = state;
			} else {
				Logger::String{"Python State object double initialization detected"}.error("python");
			}

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());
			return -1;
		}

	}
#ifdef DEBUG 
	else {
		debug("Building empty state");
	}
#endif

	debug(__FUNCTION__," is complete");
	return 0;

 }

 bool Python::State::setProperty(const char *key, const char *value) {

	if(!strcasecmp(key,"level")) {
		Abstract::State::properties.level = LevelFactory(value);
		return true;
	}

	for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
		if(!strcasecmp(key,kwlist[ix])) {

			debug(__FUNCTION__," ",key,"='",value,"'");

			// Store value.
			properties[ix] = value;

			// Update object.
			switch(ix) {
				case Python::State::Name:
					rename(properties[ix].c_str());
					break;

				case Python::State::Label:
					Udjat::Object::properties.label = properties[ix].c_str();
					break;

				case Python::State::Summary:
					Udjat::Object::properties.summary = properties[ix].c_str();
					break;

				case Python::State::Body:
					Abstract::State::properties.body = properties[ix].c_str();
					break;

				case Python::State::Icon:
					Udjat::Object::properties.icon = properties[ix].c_str();
					break;

				case Python::State::Url:
					Udjat::Object::properties.url = properties[ix].c_str();
					break;

			}

			return true;
		}
	}

	return Udjat::Abstract::State::setProperty(key,value);
 }

 bool Python::State::getProperty(const char *key, std::string &value) const {

	if(!strcasecmp(key,"level")) {
		value = std::to_string(Abstract::State::properties.level);
		return true;
	}

	for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
		if(!strcasecmp(key,kwlist[ix])) {
			value = properties[ix];
			return true;
		}
	}

	return Udjat::Abstract::State::getProperty(key,value);	
 }

 UDJAT_PRIVATE void state_finalize(PyObject *self) {	
	debug(__FUNCTION__);

 }

 /*
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
 */

 UDJAT_PRIVATE int state_setattr(PyObject *self, PyObject *attr, PyObject *value) {
	if(!strcmp(PyUnicode_AsUTF8(attr),"value")) {
		if(!object_has_private(self	)) {
			PyErr_SetString(PyExc_RuntimeError, _("Object is empty"));
			return -1;
		}
		auto state = object_get_private<Python::State>(self);
		state->set(value);
		return 0;
	}
	return object_setattr(self, attr, value);
 }

 UDJAT_PRIVATE PyObject * state_getattr(PyObject *self, PyObject *attr) {
	if(!strcmp(PyUnicode_AsUTF8(attr),"value")) {
		if(!object_has_private(self)) {
			PyErr_SetString(PyExc_RuntimeError, _("Object is empty"));
			return NULL;
		}
		return object_get_private<Python::State>(self)->get();
	}
	return object_getattr(self, attr);
 }

