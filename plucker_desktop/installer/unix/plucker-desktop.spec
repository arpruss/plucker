#------------------------------------------------------------------------------
# Name:        plucker-desktop.spec
# Purpose:     File to specify a binary-only installation of the application using 
#              Redhat's Package Manager ( a "spec" file ).
#              To use, copy this file to the plucker-desktop root directory and 
#              type: 
# Author:      Robert O'Connor
# Modified by:
# Created:     2002/07/13
# Copyright:   (c) Robert O'Connor ( rob@medicalmnemonics.com )
# Licence:     GPL
# RCS-ID:      $Id: plucker-desktop.spec,v 1.5 2003/10/21 04:12:50 robertoconnor Exp $
#------------------------------------------------------------------------------

%define	name	plucker-desktop
%define	version	1.6.0.0
%define	release	1


Summary: Visual GUI management for the command-line tools of Plucker (the Free offline web browser for handhelds)
Name: %{name}
Version: %{version}
Release: %{release}
Group: Applications/Internet
License: GNU General Public License (GPL)
URL: http://desktop.plkr.org
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: plucker


%description
Plucker Desktop is a visual content management GUI for Plucker, the popular 
suite of tools that provides an offline webbrowser for handheld devices. 
Plucker Desktop allows you to configure and use Plucker within 
a visual desktop interface. You can use your mouse to quickly add and 
configure channels and send them to the handheld. 

Plucker Desktop provides:
visual configuration of all the latest parameters for Plucker;
interactive wizards to setup Plucker, install software and add new channels;
autoupdate capability to update individual channels at specified intervals;
drag-and-drop and clipboard support;
a point-and-click showcase of preconfigured Plucker channels which can be instantly added;
installing channel output to selected handhelds or directories;
easy visual editing of exclusion lists;
an integrated editor/previewer for all Plucker tags;
visual progress feedback of channel updating;
use of the graphical interface in multiple translated languages;
extensive context-sensitive help throughout.


%prep
%setup -n %{name}-%{version}

%build
./configure \
	--disable-palmosbuild \
	--disable-docbuild
cd ./plucker_desktop
make


%install
rm -rf %{buildroot}
mkdir %{buildroot}
cd ./plucker_desktop
%makeinstall DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root)
%doc /usr/share/doc/packages/plucker-desktop
%{_bindir}/plucker-desktop
%{_datadir}/plucker-desktop
%{_datadir}/plucker-desktop/resource/graphics_dialog/*.*
%{_datadir}/plucker-desktop/resource/graphics_misc/*.*
%{_datadir}/plucker-desktop/resource/graphics_wizard/*.*
%{_datadir}/plucker-desktop/resource/icons_application/*.*
%{_datadir}/plucker-desktop/resource/icons_dialog/*.*
%{_datadir}/plucker-desktop/resource/icons_image_list/*.*
%{_datadir}/plucker-desktop/resource/icons_art_provider/*.*
%{_datadir}/plucker-desktop/resource/plkrdata_installation/*.plkrdata
%{_datadir}/plucker-desktop/resource/plkrdata_setup_wizard/*.*
%{_datadir}/plucker-desktop/resource/plkrdata_showcase/*.*
%{_datadir}/plucker-desktop/resource/strings_tips/*.txt
%{_datadir}/plucker-desktop/resource/xrc/*.xrc
%{_datadir}/plucker-desktop/resource/html_about/*.htm


%post


%postun


%changelog -n %{name}
* Mon Oct 20 2003 - rob@medicalmnemonics.com
- Multi-image support, progress dialog works well (tested RH9), sample document icons for launcher

* Thu Jun 5 2003 - rob@medicalmnemonics.com
- RPM can now be built directly from the commandline

* Sun Jul 14 2002 - rob@medicalmnemonics.com
- Initial RPM created
