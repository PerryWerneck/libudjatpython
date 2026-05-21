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
 #include <udjat/action.h>
 #include <private/interpreter.h>
 #include <private/object.h>
 #include <private/action.h>
 #include <private/tools.h>
 #include <private/value.h>
 #include <private/xml.h>
 #include <private/request.h>
 #include <private/response.h>
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

	static const char * const kwlist[Python::Action::PropertyCount+1] = {

		// Object properties
		"name",

		NULL
	};

	Python::Action::Factory::Factory(const char *name) : Udjat::Action::Factory{name} {
	}

	Python::Action::Factory::~Factory() {
	}

	std::shared_ptr<Udjat::Action> Python::Action::Factory::ActionFactory(const char *pysource, const XML::Node &node) const {

		debug(__FUNCTION__);

		Python::Guard guard;

		PyObject *self = Python::factory(pysource,"ActionFactory",node);

		debug("Initializing ",Py_TYPE(self)->tp_name);

		if(!PyObject_IsInstance(self, (PyObject *)&action_type)) {
			Py_DecRef(self);
			throw logic_error(_("The object returned from action factory is not an action"));
		}

		if(object_is_empty(self)) {
			Py_DecRef(self);
			throw logic_error(_("The object returned from action factory is empty"));
		}

		return object_get_private<Udjat::Action>(self);

	}

	std::shared_ptr<Udjat::Action> Python::Action::Factory::ActionFactory(const XML::Node &node) const {
		return ActionFactory(XML::AttributeFactory(node,"src").as_string(),node);
	}

	Python::Action::Action(PyObject *s, const XML::Node &node) : Udjat::Action{node}, self{s} {

		for(size_t ix = 0; ix < Python::Action::PropertyCount; ix++) {
			auto attr = XML::AttributeFactory(node,kwlist[ix]);
			properties[ix] = attr.as_string(ix == Name ? "python" : "");
		}

	}

	Python::Action::~Action() {
		if(self) {
			Py_DecRef(self);
			self = NULL;
		}
	}

	const char * Python::Action::name() const noexcept {
		return properties[Name].c_str();
	}

	bool Python::Action::getProperty(const char *key, std::string &value) const {
		for(size_t ix = 0; ix < Python::Action::PropertyCount; ix++) {
			if(!strcasecmp(key,kwlist[ix])) {
				value = properties[ix];
				return true;
			}
		}
		return super::getProperty(key,value);
	}

	bool Python::Action::setProperty(const char *key, const char *value) {
		for(size_t ix = 0; ix < Python::Action::PropertyCount; ix++) {
			if(!strcasecmp(key,kwlist[ix])) {
				properties[ix] = value;
				return true;
			}
		}
		return super::setProperty(key,value);
	}

	int Python::Action::call(Udjat::Request &request, Udjat::Response &response, bool except) {
		debug(__FUNCTION__)
		return Python::call(self,request,response,except);
	}

 }

 // ---------------------------------------------------------------------------------------
 // Python bindings
 // ---------------------------------------------------------------------------------------

 UDJAT_PRIVATE int action_init(PyObject *self, PyObject *args, PyObject *kwds) {

	debug(__FUNCTION__," ",Py_TYPE(self)->tp_name, " with ",PyTuple_Size(args)," argument(s)");

	try {

		std::shared_ptr<Python::Action> action;

		switch(PyTuple_Size(args)) {
			case 0:	// No arguments, build an empty agent.
				debug("Building an empty agent");
				action = make_shared<Python::Action>(self, XML::Node());
				break;

			case 1:	// Single argument, should be the XML definition;
				{
					PyObject* settings = PyTuple_GetItem(args, 0);
					if(PyObject_TypeCheck(settings, &xml_type)) {

						auto &node = xml_get_native(settings);
						debug("Building action '",node.attribute("name").as_string(),"' from node <",node.name(),">");

						action = make_shared<Python::Action>(self, node);

					} else {

						throw logic_error(_("Action requires a XML properties object as argument"));

					}

				}
				break;

			default:
				throw logic_error(_("Too many arguments"));
		}

		debug("Action '",action->name(),"' was built");
		((pyAbstractObject *) self)->pvt->object = action;

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

 UDJAT_PRIVATE void action_finalize(PyObject *self) {

 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(Udjat::Python::Action &action)> &callback) {

	try {

		return callback(*object_get_private<Udjat::Python::Action>(self));

	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());

	} catch(...) {

		PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));

	}

	return NULL;
	
 }

 UDJAT_PRIVATE int action_setattr(PyObject *self, PyObject *attr, PyObject *value) {
	
	/*
	try {


	} catch(const std::exception &e) {

		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;

	}
	*/

	return object_setattr(self, attr, value);
 }

 UDJAT_PRIVATE PyObject * action_getattr(PyObject *self, PyObject *attr) {

	/*
	const char *attrname = PyUnicode_AsUTF8(attr);
	debug(__FUNCTION__,"(",attrname,")");

	if(attrname && *attrname != '_' && object_has_private(self)) {

		try {

			auto action = object_get_private<Python::Agent>(self);

		} catch(const std::exception &e) {

			PyErr_SetString(PyExc_RuntimeError, e.what());
			return NULL;

		} catch(...) {

			PyErr_SetString(PyExc_RuntimeError, _("Unexpected error in python callback."));
			return NULL;

		}
	
	}
	*/

	return object_getattr(self, attr);

 }

 UDJAT_PRIVATE PyObject * action_call(PyObject *self, PyObject *args) {

	return call(self,[args](Udjat::Python::Action &action) -> PyObject * {

		if(PyTuple_Size(args) != 2) {
			throw logic_error(_("Method requires 2 arguments"));
		}

		int rc = action.call(
			Python::value_get_private<Udjat::Request>(PyTuple_GetItem(args, 0)),
			Python::value_get_private<Udjat::Response>(PyTuple_GetItem(args, 1))
		);

		return PyLong_FromLong(rc);
	});

 }
