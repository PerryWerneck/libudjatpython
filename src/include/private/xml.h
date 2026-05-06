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
 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>
 #include <Python.h>

 #ifdef __cplusplus
	#include <udjat/tools/xml.h>
	#include <memory>
 #endif // __cplusplus

 typedef struct {
 	PyObject_HEAD;
#ifdef __cplusplus
		const Udjat::XML::Node *handler;
 #else
		const void *handler;
 #endif // __cplusplus
 } pyXML;

 #ifdef __cplusplus

 UDJAT_PRIVATE const Udjat::XML::Node & xml_get_native(const PyObject *object);
 UDJAT_PRIVATE std::shared_ptr<PyObject> xml_get_pyObject(const Udjat::XML::Node &node);

 namespace Udjat::Python {

	 UDJAT_PRIVATE std::shared_ptr<PyObject> factory(const Udjat::XML::Node &node);
	 UDJAT_PRIVATE PyObject * factory(const char *pysource, const char *method, const Udjat::XML::Node &node);
	 UDJAT_PRIVATE PyObject * call(PyObject *self, const char *method, const Udjat::XML::Node &node);

 }

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject xml_type;

 UDJAT_PRIVATE PyObject	* xml_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  xml_dealloc(PyObject * self);
 UDJAT_PRIVATE int		  xml_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  xml_finalize(PyObject *self);

 UDJAT_PRIVATE PyObject * xml_str(PyObject *self);
 UDJAT_PRIVATE PyObject * xml_getattr(PyObject *self, PyObject *attr);
 
 UDJAT_PRIVATE PyObject * xml_get(PyObject *self, PyObject *args);

 #ifdef __cplusplus
 }
 #endif // __cplusplus
