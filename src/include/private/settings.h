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
	extern "C" {
		void *native;	///< @brief Native object.
	} pySettings;
 #endif // __cplusplus

 UDJAT_PRIVATE PyObject	* settings_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  settings_dealloc(PyObject * self);
 UDJAT_PRIVATE int		  settings_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  settings_finalize(PyObject *self);

 UDJAT_PRIVATE PyObject * settings_str(PyObject *self);
 UDJAT_PRIVATE PyObject * settings_getattr(PyObject *self, PyObject *attr);
 
 UDJAT_PRIVATE PyObject * settings_get(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
