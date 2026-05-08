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

 #pragma once
 #include <Python.h>

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <udjat/tools/intl.h>

 #ifdef __cplusplus
	#include <udjat/tools/abstract/object.h>
	#include <udjat/tools/xml.h>
	#include <udjat/tools/logger.h>
	#include <stdexcept>
 #endif // __cplusplus

 typedef struct {	
	PyObject_HEAD;
 #ifdef __cplusplus
	Udjat::Abstract::Object *handler;
 #else
	 void *handler;
 #endif // __cplusplus

 } pyAbstractObject;

 #ifdef __cplusplus

	UDJAT_PRIVATE bool object_setup(PyObject *self, const char *name, const Udjat::XML::Node &node);
	
	extern "C" {
 #endif // __cplusplus

 UDJAT_PRIVATE PyObject * object_str(PyObject *self);
 UDJAT_PRIVATE PyObject * object_getattr(PyObject *self, PyObject *attr);
 UDJAT_PRIVATE int 		  object_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * object_trace(PyObject *self, PyObject *msg);
 UDJAT_PRIVATE PyObject * object_error(PyObject *self, PyObject *msg);
 UDJAT_PRIVATE PyObject * object_warning(PyObject *self, PyObject *msg);
 UDJAT_PRIVATE PyObject * object_info(PyObject *self, PyObject *msg);
 
 #ifdef __cplusplus
 }

	template <class T>
	int set_property(PyObject *self, PyObject *attr, PyObject *value) {
		const char *attrname = PyUnicode_AsUTF8(attr);
		if(attrname && *attrname != '_') {
			pyAbstractObject *object = ((pyAbstractObject *) self);
			auto *handler = dynamic_cast<T *>(object->handler);
			if(handler) {
				const char *val = PyUnicode_AsUTF8(value);
				try {
					if(handler->setProperty(attrname,val)) {
						return 0;
					}
				} catch(const std::exception &e) {
					Udjat::Logger::String{e.what()}.warning(handler->name());
				}
			}
		}
		return PyObject_GenericSetAttr(self, attr, value);
	}

	template <class T>
	T & get_private(PyObject *self) {
		pyAbstractObject *object = ((pyAbstractObject *) self);
		auto *result = dynamic_cast<T *>(object->handler);
		if(!result) {
			throw std::logic_error(_("The object is invalid at this context"));
		}
		return *result;
	}

 #endif // __cplusplus
