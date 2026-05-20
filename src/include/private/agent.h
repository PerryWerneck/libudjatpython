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
 #include <private/state.h>
 #include <udjat/tools/value.h>

 namespace Udjat::Python {

	class UDJAT_API Agent : public Udjat::Abstract::Agent {
	protected:

		std::shared_ptr<Abstract::State> computeState() override;
		Udjat::Value & get(Udjat::Value &value) const override;
		std::shared_ptr<Abstract::State> StateFactory(const XML::Node &node) override;

	public:

		class UDJAT_API Factory : public Abstract::Agent::Factory {
		public:
			Factory(const char *name = "python");
			~Factory() override;

			std::shared_ptr<Abstract::Agent> AgentFactory(const XML::Node &node) const override;
			std::shared_ptr<Abstract::Agent> AgentFactory(const char *pysource, const XML::Node &node) const;
			std::shared_ptr<Abstract::Agent> AgentFactory(const char *pysource) const;

		};

		enum Property : uint8_t {
			Name,
			Label,
			Summary,
			Icon,
			Url,

			PropertyCount
		};

		Agent(PyObject *self,const XML::Node &node);
		~Agent() override;

		bool setProperty(const char *key, const char *value) override;
		bool getProperty(const char *key, std::string &value) const override;
		int call(const Udjat::Request &request, Udjat::Response &response) override;

		PyObject * get_value() const noexcept;

		inline operator PyObject *() const noexcept {
			return value;
		}

		void start() override;
		void stop() override;

		bool refresh(bool ondemand) override;

		std::string to_string() const noexcept override;

		bool assign(const char *value) override;

		void set_value(PyObject *value);

		inline bool operator==(PyObject *obj) const {
			return Python::compare(this->value,obj);
		}

		inline bool operator!=(PyObject *obj) const {
			return !Python::compare(this->value,obj);
		}

		inline void failed(const char *summary, const char *body) noexcept {
			Udjat::Abstract::Agent::failed(summary,body);
		}

		inline void rename(const char *value) {
			Udjat::Abstract::Agent::rename(value);
		}

		inline bool set_state(std::shared_ptr<Python::State> state) {
			return super::set(state);
		}

	private:
		PyObject * self = nullptr;
		PyObject * value = nullptr;

		/// @brief Storage for agent properties.
		std::string properties[PropertyCount];

		/// @brief Agent states.
		std::vector<std::shared_ptr<Python::State>> states;

	};

 };

 extern "C" {
 #endif // __cplusplus

 extern UDJAT_PRIVATE PyTypeObject agent_type;

 UDJAT_PRIVATE int		  agent_init(PyObject *self, PyObject *args, PyObject *kwds);
 UDJAT_PRIVATE void		  agent_finalize(PyObject *self);
 UDJAT_PRIVATE int 		  agent_setattr(PyObject *self, PyObject *attr, PyObject *value);
 UDJAT_PRIVATE PyObject * agent_getattr(PyObject *self, PyObject *attr);

 UDJAT_PRIVATE PyObject * agent_failed(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_get_by_path(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_invalidate(PyObject *self, PyObject *args);
 UDJAT_PRIVATE PyObject * agent_call(PyObject *self, PyObject *args);

 #ifdef __cplusplus
	}
 #endif // __cplusplus
