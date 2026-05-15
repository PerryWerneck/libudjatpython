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
 #include <udjat/agent/state.h>
 #include <private/tools.h>
 #include <udjat/tools/abstract/object.h>
 #include <cstdint>
 #include <string>

 namespace Udjat::Python {

	class UDJAT_API State : public Udjat::Abstract::State {
	protected:
		State(const char *name = "Python") : Udjat::Abstract::State{"python"}  {	
			rename(name);	
		}

		State(const char *name, const char *level)
			: Udjat::Abstract::State{"python",level} {
			rename(name);
		}

	public:

		enum Property : uint8_t {
			Name,
			Label,
			Summary,
			Body,
			Icon,
			Url,

			PropertyCount
		};

		enum Type {
			Numeric,
			String
		};

		State(const XML::Node &node, const Type type = Type::String);
		~State() override;

		/// @brief Build Python state object reflecting a shared_ptr
		/// @param state The state to convert into a python object.
		/// @return The python object for the state.
		static PyObject * factory(std::shared_ptr<Abstract::State> state);

		std::string value() const override;

		inline bool equal(PyObject *obj) const {
			return Python::compare(this->current_value,obj);
		}

		inline bool operator==(PyObject *obj) const {
			return Python::compare(this->current_value,obj);
		}

		inline bool operator!=(PyObject *obj) const {
			return !Python::compare(this->current_value,obj);
		}

		inline void rename(const char *value) {
			properties[Property::Name] = value;
			Udjat::Abstract::State::rename(properties[Property::Name].c_str());
		}

		bool setProperty(const char *key, const char *value) override;
		bool getProperty(const char *key, std::string &value) const override;

	private:

		PyObject * current_value = nullptr;

		std::string properties[PropertyCount];

	};

 };

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject state_type;

 UDJAT_PRIVATE int		  state_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  state_finalize(PyObject *self);
 UDJAT_PRIVATE int 		  state_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * state_getattr(PyObject *self, PyObject *attr);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
