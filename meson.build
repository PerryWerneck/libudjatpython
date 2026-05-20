# Maintainer: Perry Werneck <perry.werneck@gmail.com>

# SPDX-License-Identifier: LGPL-3.0-or-later 

# Copyright (C) 2026 Perry Werneck <perry.werneck@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(
	'udjatpython', 
	['cpp','c'],
	version: '1.0.0',
	default_options : ['c_std=c11', 'cpp_std=c++17', 'buildtype=release'],
	license: 'GPL-3.0-or-later',
)

project_description = 'Python bindings for udjat'

#
# Versioning
#
pkg_version = meson.project_version()
version_array = pkg_version.split('.')
pkg_major_version = version_array[0].to_int()
pkg_minor_version = version_array[1].to_int()
pkg_micro_version = version_array[2].to_int()

libversion = '@0@.@1@'.format(pkg_major_version,pkg_minor_version)

#
# i18n
# https://mesonbuild.com/i18n-module.html
#
subdir('po')

#
# Required libraries
#
libudjat = dependency(
  'libudjat',
  version: '>=2.5.0',
  fallback: ['libudjat', 'static_library'],
)

# Import the python module
# zypper in python314-meson-python
py_mod = import('python')

# Find the specific python installation (e.g., 'python3')
py_inst = py_mod.find_installation('python3')
py_dep = py_inst.dependency(embed: true)

#
# Compiler flags
#
cxx = meson.get_compiler('cpp')
c = meson.get_compiler('cpp')
enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

time_epoch = run_command('sh', '-c', 'echo "$SOURCE_DATE_EPOCH"', check : true).stdout().strip()
if time_epoch == ''
  time_epoch = run_command('date','+%Y%m%d', check: true).stdout().strip()
endif

compiler_flags = [
  '-ggdb3',
  '-fvisibility=hidden',
  '-fPIC',
  '-DHAVE_CONFIG_H=1',
  '-DBUILD_DATE='+time_epoch,
]

if enable_debug
  compiler_flags += [
	  '-DDEBUG=1'
  ]
else
  compiler_flags += [
	'-DNDEBUG=1'
  ]
endif

add_project_arguments(cxx.get_supported_arguments(compiler_flags + ['-Wno-overloaded-virtual']), language: 'cpp')
add_project_arguments(c.get_supported_arguments(compiler_flags), language: 'c')

#
# Configuration
#
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_REVISION','@0@'.format(time_epoch))
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)
app_conf.set('PACKAGE_VERSION_MAJOR', pkg_major_version)
app_conf.set('PACKAGE_VERSION_MINOR', pkg_minor_version)
app_conf.set('PACKAGE_VERSION_MICRO', pkg_micro_version)

app_conf.set_quoted('LOG_DOMAIN', 'python')
app_conf.set('PY_SSIZE_T_CLEAN', 1)

app_conf.set('PRODUCT_NAME', libudjat.get_variable('product_name'))
app_conf.set('PRODUCT_VERSION', libudjat.version())

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

includes_dir = include_directories('src/include')

#
# Sources
#
lib_src = [
  'src/library/interpreter.cc',
  'src/library/module.cc',
  'src/library/handler.cc',
  'src/library/testprogram.cc',
  'src/library/tools/misc.cc',
  'src/library/tools/call.cc',
  'src/library/tools/value.cc',
]

# Python modules
py_modules = [
  'src/library/logger.cc',
  'src/library/config.cc',
  'src/library/object.cc',
  'src/library/xml.cc',
  'src/library/agent.cc',
  'src/library/action.cc',
  'src/library/state.cc',
  'src/library/value.cc',
  'src/library/request.cc',
  'src/library/response.cc',
  'src/library/types/object.c',
  'src/library/types/agent.c',
  'src/library/types/xml.c',
  'src/library/types/module.c',
  'src/library/types/value.c',
  'src/library/types/state.c',
  'src/library/types/action.c',
  'src/library/types/request.c',
  'src/library/types/response.c',
]

module_src = [
  'src/module/init.cc',
]

test_src = [
  'src/testprogram/testprogram.cc'
]

#
# SDK
#
# https://mesonbuild.com/Pkgconfig-module.html
#
pkg = import('pkgconfig')

#
# OS dependencies
#
if host_machine.system() == 'windows'

  pkg.generate(
    name: 'lib' + meson.project_name(),
    description: project_description,
    requires: [ libudjat ],
    libraries: [ '-l' + meson.project_name() + '.dll' ]
  )

  pkg.generate(
    name: 'lib' + meson.project_name() + '-static',
    description: project_description,
    requires: [ libudjat ],
    libraries: [ 
      '-l' + meson.project_name(),
      '-lws2_32',
    ]
  )

else

  pkg.generate(
    name: 'lib' + meson.project_name(),
    description: project_description,
    requires: [ 'libudjat' ],
    libraries: [ '-l' + meson.project_name() ]
  )

  pkg.generate(
    name: 'lib' + meson.project_name() + '-static',
    description: project_description,
    requires: [ 'libudjat' ],
    libraries: [ '-l:lib' + meson.project_name() + '.a' ]
  )

endif

#
# Targets
#
config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

if host_machine.system() == 'windows'

  # https://mesonbuild.com/Windows-module.html
  windows = import('windows')
  resources = windows.compile_resources(
    configure_file(
      input : 'src/library/os/windows/resources.rc.in',
      output : 'resources.rc',
      configuration : app_conf
    )
  )

  dynamic = shared_library(
    meson.project_name(), 
    config_src + lib_src + py_modules + resources,
    install: true,
    version : libversion,
    soversion : libversion,
    gnu_symbol_visibility: 'hidden',
    dependencies: [ libudjat, py_dep ],
    link_args: [ 
      '-lws2_32',
      '-lwinhttp',
    ],
    include_directories: includes_dir
  )

  static = static_library(
    meson.project_name(), 
    config_src + lib_src + py_modules,
    install: true,
    dependencies: [ libudjat, py_dep ],
    cpp_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
    c_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
    include_directories: includes_dir
  )

  # https://mesonbuild.com/Reference-manual_functions.html#shared_library
  module = shared_library(
    'python', 
    config_src + module_src + resources,
    name_prefix: 'udjat-module-',
    install: true,
    install_tag: 'module',
    link_with: dynamic,
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    dependencies: [ libudjat, py_dep ],
    include_directories: includes_dir
  )

else

  dynamic = shared_library(
    meson.project_name(), 
    config_src + lib_src + py_modules,
    install: true,
    version : libversion,
    soversion : libversion,
    cpp_args: [ '-ffat-lto-objects' ],    
    gnu_symbol_visibility: 'hidden',
    dependencies: [ libudjat, py_dep ],
    include_directories: includes_dir
  )

  # https://mesonbuild.com/Reference-manual_functions.html#shared_library
  module = shared_library(
    'python', 
    config_src + module_src,
    name_prefix: 'udjat-module-',
    install: true,
    link_with: dynamic,
    install_dir: libudjat.get_variable('module_path'),
    gnu_symbol_visibility: 'hidden',
    dependencies: [ libudjat, py_dep ],
    include_directories: includes_dir
  )

  static = static_library(
    meson.project_name(), 
    config_src + lib_src + py_modules,
    install: true,
    dependencies: [ libudjat, py_dep ],
    cpp_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
    c_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
    include_directories: includes_dir
  )

endif

static_library = declare_dependency(
  link_with: static,
  include_directories: includes_dir,
  dependencies: [ libudjat, py_dep ],
  link_args: [ 
    '-lws2_32',
    '-lwinhttp',
  ],
)

dynamic_library = declare_dependency(
  link_with: dynamic,
  include_directories: includes_dir,
)

if not meson.is_cross_build()
  py_inst.extension_module(
    'udjat',
    [
      'src/extension/init.c',
    ],
    c_args: [ 
      '-DPACKAGE_VERSION="@0@.@1@.@2@"'.format(pkg_major_version,pkg_minor_version,pkg_micro_version),
      '-DPACKAGE_REVISION="@0@"'.format(time_epoch),
      '-fvisibility=hidden',
    ],
    cpp_args: [
      '-std=c++1z',
      '-Wno-overloaded-virtual',
      '-fvisibility=hidden',
      '-DPACKAGE_VERSION="@0@.@1@.@2@"'.format(pkg_major_version,pkg_minor_version,pkg_micro_version),
      '-DPACKAGE_REVISION="@0@"'.format(time_epoch),
    ],
    install: true,
    install_tag: 'python', 
    link_with: [ 
      dynamic
    ],
    dependencies: [
      py_inst.dependency(),
    ],
    include_directories: includes_dir
  )
endif

executable(
  meson.project_name(),
  config_src + test_src + module_src,
  install: false,
  link_with : [ dynamic, module ],
  dependencies: [ libudjat, py_dep ],
  include_directories: includes_dir
)

install_headers(
  'src/include/udjat/tools/actions/python.h',
  subdir: 'udjat/tools/actions'  
)

install_headers(
  'src/include/udjat/module/python.h',
  subdir: 'udjat/module'  
)

install_headers(
  'src/include/udjat/agent/python.h',
  subdir: 'udjat/agent'  
)

install_headers(
  'src/include/udjat/tools/url/handler/python.h',
  subdir: 'udjat/tools/url/handler'  
)
