#
# spec file for package python-udjat
#
# Copyright (c) <2026> Perry Werneck <perry.werneck@gmail.com>.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://github.com/PerryWerneck/libudjat/issues
#

%if 0%{?suse_version} < 1600
ExclusiveArch: do_not_build
%endif

%define skip_python2 1

Summary:		Python bindings for libudjat
Name:			python-udjat
Version:		1.0.0
Release:		0
License:		GPL-2.0
Source:			libudjat-%{version}.tar.xz
URL:			https://github.com/PerryWerneck/libudjat%{module_name}
Group:			Development/Libraries/Python

BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	gcc-c++
BuildRequires:	pkgconfig(libudjat) >= 2.5.0
BuildRequires:	meson >= 0.61.4

BuildRequires:	%{python_module devel}

# https://en.opensuse.org/openSUSE:Packaging_Python
BuildRequires:	python-rpm-macros
BuildRequires:	%{python_module packaging}
BuildRequires:	%{python_module pip}
BuildRequires:	%{python_module wheel}
BuildRequires:	%{python_module meson-python}
BuildRequires:	fdupes

%description
This is an extension allowing use of libudjat objects for python applications

%python_subpackages

%prep
%autosetup -n libudjat-%{version}

%build
%pyproject_wheel

%install
%pyproject_install
%python_expand %fdupes %{buildroot}%{python_sitearch}

%clean

%files %python_files
%{python_sitearch}/*

%changelog
