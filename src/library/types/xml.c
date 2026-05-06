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

 #include <private/xml.h>
 #include <Python.h>

 static PyMethodDef xml_methods[] = {
    {
		.ml_name = "get",
		.ml_meth = (PyCFunction) xml_get,
		.ml_flags = METH_VARARGS,
		.ml_doc =	"Get attribute from XML definition\n\n"
					"get(name,default): Get attribute 'name' from XML definition, use default value if not found\n"
    },

    {
    	NULL
	}
 };

 PyTypeObject xml_type = {

	PyVarObject_HEAD_INIT(NULL, 0)

	.tp_name = "udjat.xml",
	.tp_doc = "UDJAT Parsed XML Object",
	.tp_basicsize = sizeof(pyXML),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_HAVE_FINALIZE|Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,

	.tp_new = xml_alloc,
	.tp_dealloc = xml_dealloc,

	.tp_init = xml_init,
	.tp_finalize = xml_finalize,

	.tp_str = xml_str,

	.tp_getattro = xml_getattr,

	.tp_methods = xml_methods,

 };

