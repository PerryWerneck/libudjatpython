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

 /// @brief Declare python interpreter class.

 #pragma once
 #include <Python.h>

 #ifdef HAVE_CONFIG_H
	 #include <config.h>
 #endif // HAVE_CONFIG_H

 #include <udjat/defs.h>

 #ifdef __cplusplus
	#include <functional>
	#include <udjat/tools/value.h>
	#include <udjat/tools/intl.h>
	#include <stdexcept>
 #endif

 typedef struct {
 	PyObject_HEAD;
#ifdef __cplusplus
		Udjat::Value *handler;
#else
		void *handler;
#endif // __cplusplus
 } pyValue;

 #ifdef __cplusplus
  namespace Udjat::Python {

	/// @brief Create python object from Udjat::Value
	/// @return Python Object.
	// PyObject * factory(const Udjat::Value &value) noexcept;
	
	UDJAT_PRIVATE std::shared_ptr<PyObject> factory(Udjat::Value &value);
	UDJAT_PRIVATE Udjat::Value & get(Udjat::Value &value, PyObject *obj) noexcept;

	inline bool value_has_private(PyObject *self) {
		return (bool) ((pyValue *) self)->handler;
	}

	template <class T>
	void value_set_private(PyObject *self, T &value) {
		pyValue *object = ((pyValue *) self);
		if(object->handler) {
			throw std::logic_error(_("The object is not empty"));
		}
		object->handler = dynamic_cast<Udjat::Value *>(&value);
		if(object->handler) {
			throw std::logic_error(_("The value to set is invalid"));
		}
	}

	template <class T>
	T & value_get_private(PyObject *self) {
		pyValue *object = ((pyValue *) self);
		if(object->handler) {
			T *ptr = dynamic_cast<T *>(object->handler);
			if(!ptr) {
				throw std::logic_error(_("The object is invalid"));
			}
			return *ptr;
		}
		throw std::logic_error(_("The object is empty"));
	}

 }

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject value_type;

 UDJAT_PRIVATE PyObject	* value_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  value_dealloc(PyObject * self);
 UDJAT_PRIVATE int		  value_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  value_finalize(PyObject *self);

 UDJAT_PRIVATE int 		  value_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * value_getattr(PyObject *self, PyObject *attr);

 UDJAT_PRIVATE PyObject * value_str(PyObject *self);
 UDJAT_PRIVATE PyObject * value_serialize(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus

 