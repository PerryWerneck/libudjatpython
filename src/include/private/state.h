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

 #ifdef __cplusplus

 #include <udjat/agent/abstract.h>
 #include <private/tools.h>
 #include <cstdint>

 namespace Udjat::Python {

	class UDJAT_API State : public Udjat::Abstract::State {
	private:
		PyObject * current_value = nullptr;

	public:

		enum Type {
			Numeric,
			String
		};

		State(const XML::Node &node, const Type type = Type::String);
		~State() override;

		std::string value() const override;

		inline bool operator==(PyObject *obj) const {
			return Python::compare(this->current_value,obj);
		}

		inline bool operator!=(PyObject *obj) const {
			return !Python::compare(this->current_value,obj);
		}

		inline void rename(const char *value) {
			Udjat::Abstract::State::rename(value);
		}


	};

 };

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject state_type;

 UDJAT_PRIVATE PyObject	* state_alloc(PyTypeObject *type, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  state_dealloc(PyObject * self);

 UDJAT_PRIVATE int		  state_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  state_finalize(PyObject *self);
 UDJAT_PRIVATE int 		  state_setattr(PyObject *self, PyObject *attr, PyObject *value);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
