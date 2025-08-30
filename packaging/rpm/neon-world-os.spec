Name: neon-world-os
Version: 130.0.0
Release: 1%{?dist}
Summary: Host-embedded OS simulator (C++)
License: MIT
URL: https://example.invalid/neon-world-os
BuildRequires: cmake, gcc-c++, make
%description
NEON-WORLD-OS is a host-embedded OS simulator written in C++.

%prep

%build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

%install
cmake --install build --prefix %{buildroot}/usr

%files
/usr/bin/neon
/usr/include/*
/usr/lib*/libneon*
%license LICENSE
%doc README.md
