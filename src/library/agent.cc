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
 #include <udjat/tools/request.h>
 #include <udjat/tools/response.h>
 #include <private/object.h>
 #include <private/agent.h>
 #include <private/tools.h>
 #include <private/xml.h>
 #include <private/state.h>
 #include <private/value.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/value.h>
 #include <udjat/tools/memory.h>
 #include <string>
 #include <cstdarg>

 using namespace Udjat;
 using namespace std;

 // ---------------------------------------------------------------------------------------
 // C++ Object
 // ---------------------------------------------------------------------------------------

 namespace Udjat {

	static const char * const kwlist[Python::Agent::PropertyCount+3] = {

		// Object properties
		"name",
		"label",
		"summary",
		"icon",
		"url",

		// Extra properties.
		"value",
		"state",
		NULL
	};

	Python::Agent::Factory::Factory(const char *name) : Abstract::Agent::Factory{name} {
	}

	Python::Agent::Factory::~Factory() {
	}

	bool Python::Agent::operator==(PyObject *obj) const {
		return Python::compare(this->value,obj);
	}

	bool Python::Agent::operator!=(PyObject *obj) const {
		return !Python::compare(this->value,obj);
	}

	bool Python::Agent::set_state(std::shared_ptr<Python::State> state) {
		return super::set(state);
	}
	
	std::shared_ptr<Abstract::Agent> Python::Agent::Factory::AgentFactory(const char *pysource, const XML::Node &node) const {

		debug(__FUNCTION__);

		Python::Guard guard;

		PyObject *self = Python::factory(pysource,"AgentFactory",node);

		debug("Initializing ",Py_TYPE(self)->tp_name);

		if(!PyObject_IsInstance(self, (PyObject *)&agent_type)) {
			Py_DecRef(self);
			throw logic_error(_("The object returned from agent factory is not an agent"));
		}

		if(object_is_empty(self)) {
			Py_DecRef(self);
			throw logic_error(_("The object returned from agent factory is empty"));
		}

		return object_get_private<Abstract::Agent>(self);

	}

	std::shared_ptr<Abstract::Agent> Python::Agent::Factory::AgentFactory(const XML::Node &node) const {
		return AgentFactory(XML::AttributeFactory(node,"src").as_string(),node);
	}

	std::shared_ptr<Abstract::Agent> Python::Agent::Factory::AgentFactory(const char *pysource) const {
		return AgentFactory(pysource,XML::Node());
	}

	Python::Agent::Agent(PyObject *s, const XML::Node &node) : Abstract::Agent{node}, self{s} {
	}

	Python::Agent::~Agent() {
		if(self) {
			Py_DecRef(self);
			self = NULL;
		}
	}

	std::shared_ptr<Abstract::State> Python::Agent::computeState() {

		Python::Guard guard;

		auto current = dynamic_pointer_cast<Python::State>(this->state()); 
		if(current && current->equal(value)) {
			debug("The current state matches the value, keeping it");
			return current;
		}

		debug("Computing state for value '",std::to_string(this->value).c_str(),"'");
		for(auto state : states) {
			if(state->equal(value)) {
				debug("Accepting state '",state->name(),"'");
				return state;
			}
#ifdef DEBUG
			else {
				debug("Rejecting state '",state->name(),"'");
			}
#endif
		}

		debug("Cant determine state, let the parent decide");
		return super::computeState();
	}

	std::shared_ptr<Abstract::State> Python::Agent::StateFactory(const XML::Node &node) {

		Python::Guard guard;

		if(!value) {
			throw logic_error(_("Cant factory state for empty agent"));
		}

		std::shared_ptr<Python::State> state;
		
		if (PyLong_Check(value)) {
			// It is a Python int (or a subclass of int)
			state = make_shared<Python::State>(node,Python::State::Type::Numeric);
		} else {
			// It's an object or a string
			state = make_shared<Python::State>(node,Python::State::Type::String);
		}

		states.push_back(state);
		return state;

	}


	Udjat::Value & Python::Agent::get(Udjat::Value &value) const {
		return Python::get(value,this->value);
	}

	void Python::Agent::set_value(PyObject *value) {

		Python::Guard guard;

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

	void Python::Agent::start() {
		debug(__FUNCTION__);
		Python::call(self,"start");
		super::start();
	}

	void Python::Agent::stop() {
		debug(__FUNCTION__);
		Python::call(self,"stop");
		super::stop();
	}

	bool Python::Agent::refresh(bool ondemand) {
	
		debug(__FUNCTION__," ----> Updating agent '",name(),"'");

		if(super::refresh(ondemand)) {
			return true;
		}

		Python::Guard guard;

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
		if(this->value) {
			return std::to_string(value);
		}
		return "";
	}

	PyObject * Python::Agent::get_value() const noexcept {
		if(this->value) {
			return this->value;
		}
		return Py_None;
	}

	bool Python::Agent::getProperty(const char *key, std::string &value) const {
		for(size_t ix = 0; ix < Python::Agent::PropertyCount; ix++) {
			if(!strcasecmp(key,kwlist[ix])) {
				value = properties[ix];
				return true;
			}
		}
		return super::getProperty(key,value);
	}

	bool Python::Agent::setProperty(const char *key, const char *value) {

		for(size_t ix = 0; ix < Python::Agent::PropertyCount; ix++) {
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

	int Python::Agent::call(const Udjat::Request &request, Udjat::Response &response) {
		debug(__FUNCTION__)
		return Python::call(self,request,response);
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE int agent_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	try {

		std::shared_ptr<Python::Agent> agent;

		switch(PyTuple_Size(args)) {
			case 0:	// No arguments, build an empty agent.
				debug("Building an empty agent");
				agent = make_shared<Python::Agent>(self, XML::Node());
				break;

			case 1:	// Single argument, should be the XML definition;
				{
					PyObject* settings = PyTuple_GetItem(args, 0);
					if(PyObject_TypeCheck(settings, &xml_type)) {

						auto &node = xml_get_native(settings);
						debug("Building agent from node <",node.name(),">");

						agent = make_shared<Python::Agent>(self, node);

					} else {

						throw logic_error(_("Agent requires a XML properties object as argument"));

					}

				}
				break;

			default:
				throw logic_error(_("Too many arguments"));
		}

		((pyAbstractObject *) self)->pvt->object = agent;

		if(kwds) {

			
			/*
			// Has arguments, load then.
			std::string props[Python::Agent::PropertyCount];

			// Load current properties.
			for(size_t ix = 0; ix < Python::Agent::PropertyCount; ix++) {
				agent->getProperty(kwlist[ix],props[ix]);
			}

			// Parse arguments
			load(kwds,props);

			// Set parsed values
			for(size_t ix = 0; ix < Python::Agent::PropertyCount; ix++) {
				agent->setProperty(kwlist[ix],props[ix].c_str());
			}
			*/

			throw runtime_error("Incomplete");

		}

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;

	}

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
		
	try {

		if(!strcmp(std::to_string(attr).c_str(),"value")) {
			
			debug("Updating internal value");

				auto agent = object_get_private<Udjat::Python::Agent>(self);
				if(!agent) {
					PyErr_SetString(PyExc_RuntimeError, _("Cant set value on empty agent"));
					return -1;
				}

				agent->set_value(value);
				return 0;

		} else if(!strcmp(std::to_string(attr).c_str(),"state")) {

			auto agent = object_get_private<Udjat::Python::Agent>(self);
			if(!agent) {
				PyErr_SetString(PyExc_RuntimeError, _("Cant set state on empty agent"));
				return -1;
			}

			if(PyObject_IsInstance(value, (PyObject *) &state_type)) {

				debug("Updating agent state from state object");
				auto state = object_get_private<Udjat::Python::State>(value);
				state->set(agent->get_value());
				agent->set_state(state);
				
			} else {

				debug("Updating agent with new state");
				auto state = Python::State::factory(value);
				state->set(agent->get_value());
				agent->set_state(state);

			}

			return 0;
		}

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;

	}

	return object_setattr(self, attr, value);
 }

 UDJAT_PRIVATE PyObject * agent_getattr(PyObject *self, PyObject *attr) {

	/*
	if (PyErr_Occurred()) {
		debug(__FUNCTION__,": PyErr_Occurred()")
		return NULL;
	}
	*/

	const char *attrname = PyUnicode_AsUTF8(attr);
	debug(__FUNCTION__,"(",attrname,")");

	if(attrname && *attrname != '_' && object_has_private(self)) {

		try {

			auto agent = object_get_private<Python::Agent>(self);
			if(!strcmp(attrname,"value")) {
				return agent->get_value();
			}

			if(!strcmp(attrname,"state")) {
				debug("Getting selected state");
				return Python::State::factory(agent->state());
			}

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

 UDJAT_PRIVATE PyObject * agent_call(PyObject *self, PyObject *args) {

 	return call(self,[args](Udjat::Python::Agent &agent) -> PyObject *{

		if(PyTuple_Size(args) != 2) {
			throw logic_error(_("Method requires 2 arguments"));
		}

		int rc = agent.Abstract::Agent::call(
			Python::value_get_private<Udjat::Request>(PyTuple_GetItem(args, 0)),
			Python::value_get_private<Udjat::Response>(PyTuple_GetItem(args, 1))
		);

		return PyLong_FromLong(rc);
	});

 }

 /*
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
 */
