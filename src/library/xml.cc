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
 #include <private/xml.h>
 #include <private/tools.h>
 #include <udjat/tools/xml.h>
 #include <udjat/tools/abstract/object.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/memory.h>

 #include <Python.h>

 using namespace Udjat;
 using namespace std;

 PyObject * Udjat::Python::factory(const char *pysource, const char *method, const XML::Node &node) {

	lock_guard<recursive_mutex> lock(guard);

	debug("Creating object from ",pysource," using ",method,"(settings)");
	auto module = Python::make_handle(PyImport_ImportModule(pysource));
	if(!module) {
		exception(true);
		throw runtime_error(Logger::String{"Unable to load '",pysource,"'"});
	}
	
	debug("Searching for '",method,"'");
	auto func = make_handle(PyObject_GetAttrString(module.get(), method));
	if(!func) {
		throw logic_error(Logger::Message{_("The method {}(settings) is required on '{}"),method,pysource});
	}

	if(!PyCallable_Check(func.get())) {
		throw logic_error(Logger::Message{_("The method {}(settings) is not callable on '{}"),method,pysource});
	}

	// Build python object for XML::Node
	auto settings = factory(node);
	auto args = make_handle(PyTuple_Pack(1, settings.get()));

	debug("Calling object...");
	PyObject *response = PyObject_CallObject(func.get(), args.get());

	if(!response) {
		throw runtime_error(exception());
	}

	if(response == Py_None) {
		throw runtime_error(_("Unexpected response from factory method"));
	}

	debug(__FUNCTION__,"Response was ",Py_TYPE(response)->tp_name);

	return response;
 }

 std::shared_ptr<PyObject> Udjat::Python::factory(const Udjat::XML::Node &node) {

	lock_guard<recursive_mutex> lock(guard);

	debug("Building settings...");
	auto settings = make_handle(PyObject_CallFunction((PyObject*)&xml_type, "O", Py_None));

	if(!settings) {
		debug("Failed building settings");
		throw runtime_error(exception());
	}

	// Store XML::Node in the object.
	{
		pyXML *native = ((pyXML *) settings.get());
		native->handler = &node;
	}
	
	return settings;
 }

 UDJAT_PRIVATE PyObject * Udjat::Python::call(PyObject *self, const char *method, const Udjat::XML::Node &node) {

	lock_guard<recursive_mutex> lock(guard);

	debug("Searching for '",method,"'");

	auto func = make_handle(PyObject_GetAttrString(self, method));
	if(!func) {
		throw logic_error(Logger::Message{_("The method {} is required"),method});
	}

	if(!PyCallable_Check(func.get())) {
		throw logic_error(Logger::Message{_("The method {} is not callable"),method});
	}

	auto settings = factory(node);
	auto args = make_handle(PyTuple_Pack(1, settings.get()));

	debug("Calling object...");
	PyObject *response = PyObject_CallObject(func.get(), args.get());

	if(!response) {
		throw runtime_error(exception());
	}

	return response;
 }

 UDJAT_PRIVATE PyObject	* xml_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
	if (PyErr_Occurred()) {
        return NULL; // Exit early if something else already failed
    }
 	return type->tp_alloc(type,0);
 }

 UDJAT_PRIVATE void xml_dealloc(PyObject * self) {
	debug(__FUNCTION__);
	Py_TYPE(self)->tp_free(self);
 } 
 
 UDJAT_PRIVATE int xml_init(PyObject *self, PyObject *args, PyObject *kwds) {
	debug(__FUNCTION__);
	return 0;

 }

 UDJAT_PRIVATE void xml_finalize(PyObject *self) {

 }

 UDJAT_PRIVATE const XML::Node & xml_get_native(const PyObject *self) {
	pyXML *settings = ((pyXML *) self);
	if(!settings->handler) {
		throw logic_error(_("The properties object is empty"));
	}
	return *(settings->handler);
 }

 static PyObject * call(PyObject *self,const std::function<PyObject *(const XML::Node &node)> &callback) {

	pyXML *settings = ((pyXML *) self);
	
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

 UDJAT_PRIVATE PyObject * xml_str(PyObject *self) {

	debug(__FUNCTION__);
	return call(self,[](const XML::Node &node) -> PyObject * {

		return PyUnicode_FromString(
			node.name()
		);

	});

 }

 UDJAT_PRIVATE PyObject * xml_getattr(PyObject *self, PyObject *attr) {

	debug(__FUNCTION__);
	return call(self,[self,attr](const XML::Node &node) -> PyObject * {

		const char *attrname = PyUnicode_AsUTF8(attr);

		debug("setting.getattr(",attrname,")");

		// 1. Handle your custom dynamic properties
		if(attrname[0] != '_') {
			auto attribute = XML::AttributeFactory(node,attrname);
			if(attribute) {
				return PyUnicode_FromString(attribute.as_string());
			} 
		}

		// 2. Delegate EVERYTHING ELSE to Python's standard logic
		// This handles __dict__, __class__, methods, and members automatically.
		return PyObject_GenericGetAttr(self, attr);

	});

 }
 
 UDJAT_PRIVATE PyObject * xml_get(PyObject *self, PyObject *args) {

	debug(__FUNCTION__);
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

