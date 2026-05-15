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
 #include <private/interpreter.h>
 #include <private/object.h>
 #include <private/agent.h>
 #include <private/tools.h>
 #include <private/xml.h>
 #include <private/state.h>
 #include <private/value.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/memory.h>
 #include <string>
 #include <cstdarg>

 using namespace Udjat;
 using namespace std;

 // ---------------------------------------------------------------------------------------
 // C++ Object
 // ---------------------------------------------------------------------------------------

 namespace Udjat {

	static const char * const kwlist[Python::Agent::PropertyCount+1] = {

		// Object properties
		"name",
		"label",
		"summary",
		"icon",
		"url",

		NULL
	};

	Python::Agent::Factory::Factory(const char *name) : Abstract::Agent::Factory{name} {
	}

	Python::Agent::Factory::~Factory() {
	}

	std::shared_ptr<Abstract::Agent> Python::Agent::Factory(const char *pysource, const XML::Node &node) {
		auto agent = make_shared<Python::Agent>(pysource,node);
		((pyAbstractObject *) agent->self)->pvt->object = dynamic_pointer_cast<Abstract::Object>(agent);
		return agent;
	}

	std::shared_ptr<Abstract::Agent> Python::Agent::Factory(const char *pysource) {
		return Factory(pysource,XML::Node{});
	}

	std::shared_ptr<Abstract::Agent> Python::Agent::Factory::AgentFactory(const XML::Node &node) const {
		return Python::Agent::Factory(XML::AttributeFactory(node,"src").as_string(),node);
	}

	Python::Agent::Agent(const char *pysource,const XML::Node &node) 
		: self{Python::factory(pysource,"AgentFactory",node)} {

		for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
			super::getProperty(kwlist[ix],properties[ix]);
		}

		lock_guard<recursive_mutex> lock(Python::guard);

		debug("Initializing ",Py_TYPE(self)->tp_name);

		if(!PyObject_IsInstance(self, (PyObject *)&agent_type)) {
			Py_DecRef(self);
			self = NULL;
			throw logic_error(_("The object returned from factory method is not an agent"));
		}

	}

	Python::Agent::~Agent() {
	}

	std::shared_ptr<Abstract::State> Python::Agent::computeState() {
		for(auto state : states) {
			lock_guard<recursive_mutex> lock(Python::guard);
			if(state->equal(value)) {
				return state;
			}
		}
		return super::computeState();
	}

	Udjat::Value & Python::Agent::get(Udjat::Value &value) const {
		return Python::get(value,this->value);
	}

	void Python::Agent::set_value(PyObject *value) {

		lock_guard<recursive_mutex> lock(Python::guard);

		if(Python::compare(this->value,value)) {
			// Objects are equal
			updated(false);
			return;
		}

		// Objects are NOT equal

		Py_DecRef(this->value);

		this->value = value;
		if(this->value) {
			Py_IncRef(this->value);
		}

		Logger::Message{_("Agent value changed to {}"),to_string().c_str()}.info(name());

		updated(true);

	}

	bool Python::Agent::assign(const char *value) {
		return false;
	}

	std::shared_ptr<Abstract::State> Python::Agent::StateFactory(const XML::Node &node) {
		auto state = make_shared<Python::State>(node);
		states.push_back(state);
		return state;
	}

	bool Python::Agent::setup(const XML::Node &node) {
		return object_setup(self,name(),node);
	}
	
	void Python::Agent::start() {
		debug(__FUNCTION__);
		Python::call(self,"start");
		super::start();
	}

	void Python::Agent::stop() {
		debug(__FUNCTION__);
		lock_guard<recursive_mutex> lock(Python::guard);
		Python::call(self,"stop");
		super::stop();
	}

	bool Python::Agent::refresh(bool ondemand) {
	
		debug(__FUNCTION__);

		if(super::refresh(ondemand)) {
			return true;
		}

		lock_guard<recursive_mutex> lock(Python::guard);

		try {

			return Python::call(
					self,
					"refresh",
					make_handle(PyBool_FromLong(ondemand)).get(),
					NULL
			);

		} catch(const std::exception &e) {

			super::failed(_("Failed to refresh agent"),e);

		} catch(...) {

			super::failed(_("Failed to refresh agent"),_("Unexpected error"));

		}

		return true;

	}

	std::string Python::Agent::to_string() const noexcept {
		return Python::to_string(value);
	}

	bool Python::Agent::setProperty(const char *key, const char *value) {

		for(size_t ix = 0; ix < Python::State::PropertyCount; ix++) {
			if(!strcasecmp(key,kwlist[ix])) {

				// Store value.
				properties[ix] = value;

				// Update object.
				switch(ix) {
					case Python::Agent::Name:
						rename(properties[ix].c_str());
						break;

					case Python::Agent::Label:
						Udjat::Object::properties.label = properties[ix].c_str();
						break;

					case Python::Agent::Summary:
						Udjat::Object::properties.summary = properties[ix].c_str();
						break;

					case Python::Agent::Icon:
						Udjat::Object::properties.icon = properties[ix].c_str();
						break;

					case Python::Agent::Url:
						Udjat::Object::properties.url = properties[ix].c_str();
						break;

				}

				return true;

			}
		}

		return super::setProperty(key,value);
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE int agent_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	return 0;

 }

 UDJAT_PRIVATE void agent_finalize(PyObject *self) {

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Python::Agent &agent)> &callback) {

	try {

		return callback(*object_get_private<Udjat::Python::Agent>(self));

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



		return Py_None;
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

 UDJAT_PRIVATE int agent_setattr(PyObject *self, PyObject *attr, PyObject *value) {
	
	if(object_has_private(self)) {

		auto agent = object_get_private<Python::Agent>(self);

		/*

		// Unnecessary, object_setattr will call setProperty

		switch(String{PyUnicode_AsUTF8(attr)}.select("value","name","label","summary","url","icon",NULL)) {
		case 0:	// Value.
			agent->set_value(value);
			return 0;

		case 1: // Name.
			agent->rename(String{PyUnicode_AsUTF8(value)}.as_quark());
			return 0;

		case 2: // Label.
			agent->label(String{PyUnicode_AsUTF8(value)}.as_quark());
			return 0;

		case 3: // Summary.
			agent->summary(String{PyUnicode_AsUTF8(value)}.as_quark());
			return 0;

		case 4: // URL.
			agent->url(String{PyUnicode_AsUTF8(value)}.as_quark());
			return 0;

		case 5: // Icon.
			agent->icon(String{PyUnicode_AsUTF8(value)}.as_quark());
			return 0;

		}
		*/

	}
	
	return object_setattr(self, attr, value);
 }

 UDJAT_PRIVATE PyObject * agent_getattr(PyObject *self, PyObject *attr) {

	const char *attrname = PyUnicode_AsUTF8(attr);
	debug(__FUNCTION__,"(",attrname,")");

	if(attrname && *attrname != '_' && object_has_private(self)) {

		try {

			auto agent = object_get_private<Python::Agent>(self);
			if(!strcmp(attrname,"value")) {
				return (PyObject *) *agent;
			}

			if(!strcmp(attrname,"state")) {
				return Python::State::factory(agent->state());
			}

			/*
			
			Unnecessary, object_getproperty will call

			string response;
			if(agent->getProperty(attrname,response)) {
				return PyUnicode_FromString(
					response.c_str()
				);
			}
			*/

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());
			return NULL;

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));
			return NULL;

		}
	
	}

	return object_getattr(self, attr);

 }

 UDJAT_PRIVATE PyObject * agent_setup(PyObject *self, PyObject *args) {

	debug(__FUNCTION__);
	
	return call(self,[args](Udjat::Python::Agent &agent) -> PyObject * {

		debug("Setting up agent ",agent.name());
		
		if(PyTuple_Size(args) != 1) {
			throw logic_error(_("Agent setup requires a properties object as argument"));
		}

		PyObject* settings = PyTuple_GetItem(args, 0);
		if(PyObject_TypeCheck(settings, &xml_type)) {

			if(agent.Udjat::Abstract::Agent::setup(xml_get_native(settings))) {
				return Py_True;
			}

		} else {

			throw logic_error(_("Agent.setup() requires a valid properties object"));
		}

		return Py_False;
	});

 }
