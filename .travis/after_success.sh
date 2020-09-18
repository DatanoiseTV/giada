#!/usr/bin/env bash

mkdir build

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then

	cp giada_osx ./build
	upx --best ./build/giada_osx

elif [[ $TRAVIS_OS_NAME == 'linux' ]]; then

	# Strip binary.

	strip ./giada_lin

	# Create temporary dir. Binary and desktop files must go in there. This is 
	# necessary for linuxdeployqt to work correctly.

	mkdir ./temp/
	mv ./giada_lin ./temp/

	# Create .desktop file.

cat << EOF > ./temp/giada_lin.desktop
[Desktop Entry]
Name=Giada
Name[es]=Giada
GenericName=Drum machine and loop sequencer
GenericName[es]=Caja de ritmos y secuenciador de loops
Icon=giada_lin
Type=Application
Exec=/usr/bin/giada_lin.AppImage
Terminal=false
Categories=AudioVideo;Audio;X-Digital_Processing;X-Jack;X-MIDI;Midi;
Keywords=Giada;
EOF

	# Prepare logo. 
	
	mv extras/giada-logo.svg ./temp/giada_lin.svg

	# Run linuxdeploy to make the AppImage, then move it to ./build dir. 
	# For some reasons linuxdeploy uses the commit hash in the filename, so
	# rename it first.

	./linuxdeploy-x86_64.AppImage -e ./temp/giada_lin -d ./temp/giada_lin.desktop -i ./temp/giada_lin.svg --output appimage --appdir .
	mv Giada-*-x86_64.AppImage Giada-x86_64.AppImage 
	cp Giada-x86_64.AppImage ./build

fi