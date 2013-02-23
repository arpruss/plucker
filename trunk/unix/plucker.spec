#
# $Id: plucker.spec,v 1.12 2004/11/27 11:23:38 nordstrom Exp $
#
# Spec file for Plucker
#
# Copyright (c) 2001 Michael Nordstrom
#
# This file is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#

%{expand: %%define pyver %(python -c 'import sys;print(sys.version[0:3])')}

Name: plucker
Version: 1.9
Release: 2
Group: None
Summary: Offline HTML reading for Palm devices
License: GPL
URL: http://www.plkr.org/
Source0: plucker_viewer-%{version}.tar.bz2
Source1: plucker_viewer_extras-%{version}.tar.bz2
Source2: plucker_viewer_translations-%{version}.tar.bz2
Source3: plucker_parser-%{version}.tar.bz2
Source4: plucker_docs-%{version}.tar.bz2
Source5: setup.py.in
Requires: python, pilot-link
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Plucker increases the utility of your handheld device by letting you view
web pages and any document that can be converted to HTML or text. Plucker
has many advanced features including the ability to read web pages with
embedded images, an advanced find function, the ability to open an e-mail
form when tapping on mail-links in web documents, an impressive compression
ratio for the documents and an open, documented format. It can also be
customized for your specific needs.

%prep

%setup -q -T -b0 -b1 -b2 -b3 -b4
cp %{_sourcedir}/setup.py.in %{_builddir}/%{name}-%{version}

%build
# nothing at the moment...

%install
%define PyPluckerDir %{_libdir}/python%{pyver}/site-packages/PyPlucker
%define DataDir %{_datadir}/plucker
%define DocDir %{_datadir}/doc/packages/plucker

rm -rf %{buildroot}

# Viewer
mkdir -p %{buildroot}/%{DataDir}/palm
install -m 755 *.prc %{buildroot}/%{DataDir}/palm
install -m 755 *.pdb %{buildroot}/%{DataDir}/palm
install -m 755 ppi/*.prc %{buildroot}/%{DataDir}/palm
install -m 755 xlit/*.pdb %{buildroot}/%{DataDir}/palm
install -m 755 imode/*.pdb %{buildroot}/%{DataDir}/palm

# Python Parser
mkdir -p %{buildroot}/%{PyPluckerDir}/helper
install -m 755 PyPlucker/*.py %{buildroot}/%{PyPluckerDir}
install -m 755 PyPlucker/helper/*.py %{buildroot}/%{PyPluckerDir}/helper

# Config files
mkdir -p %{buildroot}/%{DataDir}/config
install -m 644 exclusionlist.txt %{buildroot}/%{DataDir}/config
install -m 644 home.html %{buildroot}/%{DataDir}/config
install -m 644 pluckerrc.sample %{buildroot}/%{DataDir}/config

mkdir -p %{buildroot}/%{_bindir}
sed -e "s:@VERSION@:%{version}:" \
    -e "s:@PLUCKERDIR@:%{DataDir}:" setup.py.in > setup.py
install -m 755 setup.py %{buildroot}/%{_bindir}/plucker-setup

#Documentation
mkdir -p %{buildroot}/%{DocDir}/manual
install -m 644 manual/* %{buildroot}/%{DocDir}/manual

install -m 644 AUTHORS BUGREPORT COPYING CREDITS ChangeLog %{buildroot}/%{DocDir}
install -m 644 FAQ NEWS README REQUIREMENTS %{buildroot}/%{DocDir}

mkdir -p %{buildroot}/%{_mandir}/man1/
install -m 644 plucker-build.1 %{buildroot}/%{_mandir}/man1/
install -m 644 plucker-decode.1 %{buildroot}/%{_mandir}/man1/
install -m 644 plucker-dump.1 %{buildroot}/%{_mandir}/man1/

%clean
rm -rf %{buildroot}

%post
%define PyPluckerDir %{_libdir}/python%{pyver}/site-packages/PyPlucker

python %{_libdir}/python%{pyver}/compileall.py %{PyPluckerDir}
python -O %{_libdir}/python%{pyver}/compileall.py %{PyPluckerDir}

# make sure we don't have some old cruft in the binary dir
rm -f %{_bindir}/plucker-build
rm -f %{_bindir}/plucker-decode
rm -f %{_bindir}/plucker-dump

# add links to parser tools
ln -s %{PyPluckerDir}/Spider.py  %{_bindir}/plucker-build
ln -s %{PyPluckerDir}/PluckerDocs.py %{_bindir}/plucker-decode
ln -s %{PyPluckerDir}/Decode.py %{_bindir}/plucker-dump

%preun
rm -f %{PyPluckerDir}/*.pyc
rm -f %{PyPluckerDir}/*.pyo
rm -f %{PyPluckerDir}/helper/*.pyc
rm -f %{PyPluckerDir}/helper/*.pyo

%postun
rm -f %{_bindir}/plucker-build
rm -f %{_bindir}/plucker-decode
rm -f %{_bindir}/plucker-dump

%files
%{_bindir}/plucker-setup
%doc %{_datadir}/doc/packages/plucker
%{_mandir}/man1/plucker-build*
%{_mandir}/man1/plucker-decode*
%{_mandir}/man1/plucker-dump*
%{_libdir}/python%{pyver}/site-packages/PyPlucker
%{_datadir}/plucker

%changelog -n plucker
* Tue Oct 26 2004 Bill Barnard <bill@barnard-engineering.com>
- added -h flag to setup test for pilot-xfer to prevent hangs

* Wed May 19 2004 - micke@sslug.dk
- removed TODO file

* Sun Mar 21 2004 - micke@sslug.dk
- updated for new packaging format

* Sun Aug  3 2003 - micke@sslug.dk
- fix to remove links to parser tools

* Fri May 23 2003 - micke@sslug.dk
- updated to use new packages

* Sun Jul 22 2001 - micke@sslug.dk
- added version tag that will be set when the package is built

* Sat Jul 21 2001 - micke@sslug.dk
- moved the pluck-comics files inside the Plucker data dir

* Sat Jul 14 2001 - micke@sslug.dk
- initial version

