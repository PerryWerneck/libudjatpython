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
 #include <config.h>
 #include <udjat/defs.h>
 #include <Python.h>

 #ifdef __cplusplus
	extern "C" {
		void *native;	///< @brief Native object.
	} pyAgent;
 #endif // __cplusplus

 typedef struct {
	PyObject_HEAD
 } pyAgent;

 UDJAT_PRIVATE PyObject	* agent_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  agent_dealloc(PyObject * self);
 UDJAT_PRIVATE int		  agent_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  agent_finalize(PyObject *self);

 UDJAT_PRIVATE PyObject * agent_str(PyObject *self);
 UDJAT_PRIVATE int 		  agent_setattr(PyObject *self, char *attr, PyObject *value);
 UDJAT_PRIVATE int 		  agent_getattr(PyObject *self, char *attr, PyObject **value);
 
 UDJAT_PRIVATE PyObject * agent_failed(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_get_by_path(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_invalidate(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_start(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_stop(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_refresh(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
