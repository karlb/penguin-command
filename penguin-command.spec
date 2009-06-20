Name: 		penguin-command
Version: 	1.6.10
Release: 	1

Group:          Amusements/Games       
Group(cs): 	Zábava/Hry
Summary: 	Penguin Command is a clone of the classic Missile Command game.

Vendor:		karlb@gmx.net
Packager:       Michal Ambroz (o_o) (rebus@seznam.cz)
License: 	GPL
URL: 		http://www.linux-games.com
Source0: 	%{name}-%{version}.tar.gz
BuildRoot: 	%{_tmppath}/%{name}-%{version}-root
Prefix: /usr
Prefix: /etc

%description
Penguin Command is a clone of the classic Missile Command game, but has better
graphics and sound. The gameplay has been slightly modified.

%prep
#Unpack package
%setup


%build
./configure --prefix=%{_prefix}
make 


%install
mkdir -p %{buildroot}%{_prefix}/bin
make DESTDIR=%{buildroot} install

#Install application link for X-Windows
install -d %{buildroot}/etc/X11/applnk/Games
echo -e "[Desktop Entry]
Name=Penguin-Command
Comment=Missile Attack game
Exec=penguin-command
Icon=penguin-command.xpm
Terminal=0
Type=Application" > %{buildroot}/etc/X11/applnk/Games/"Penguin-Command".desktop




%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc AUTHORS COPYING INSTALL NEWS README ChangeLog 
/etc/X11/applnk/*
%{_bindir}/penguin-command
%{_prefix}/share/penguin-command/*



%changelog
* Thu May 16 2002 Michal Ambroz (O_O) <rebus@seznam.cz>
- initial specfile
