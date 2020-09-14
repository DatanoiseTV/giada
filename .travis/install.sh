#!/usr/bin/env bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then

  brew update
  brew install rtmidi
  brew install libsamplerate
  brew install fltk
  brew install libsndfile
  brew install upx

  # Remove dynamic libraries to force static linking.

  rm -rf /usr/local/lib/librtmidi.dylib
  rm -rf /usr/local/lib/librtmidi.4.dylib
  rm -rf /usr/local/lib/libsamplerate.dylib
  rm -rf /usr/local/lib/libsamplerate.0.dylib
  rm -rf /usr/local/lib/libfltk.1.3.dylib
  rm -rf /usr/local/lib/libfltk.dylib
  rm -rf /usr/local/lib/libfltk_forms.1.3.dylib
  rm -rf /usr/local/lib/libfltk_forms.dylib
  rm -rf /usr/local/lib/libfltk_forms.dylib
  rm -rf /usr/local/lib/libfltk_gl.1.3.dylib
  rm -rf /usr/local/lib/libfltk_gl.dylib 
  rm -rf /usr/local/lib/libfltk_images.1.3.dylib
  rm -rf /usr/local/lib/libfltk_images.dylib 
  rm -rf /usr/local/lib/libsndfile.1.dylib 
  rm -rf /usr/local/lib/libsndfile.dylib 
  rm -rf /usr/local/lib/libFLAC++.6.dylib
  rm -rf /usr/local/lib/libFLAC++.dylib
  rm -rf /usr/local/lib/libFLAC.8.dylib
  rm -rf /usr/local/lib/libFLAC.dylib
  rm -rf /usr/local/lib/libogg.0.dylib
  rm -rf /usr/local/lib/libogg.dylib
  rm -rf /usr/local/lib/libvorbis.0.dylib
  rm -rf /usr/local/lib/libvorbis.dylib
  rm -rf /usr/local/lib/libvorbisenc.2.dylib
  rm -rf /usr/local/lib/libvorbisenc.dylib

elif [[ $TRAVIS_OS_NAME == 'linux' ]]; then

  sudo apt-get install -y g++-8 libsndfile1-dev libsamplerate0-dev \
  	libasound2-dev libxpm-dev libpulse-dev libjack-dev \
  	libxft-dev libxrandr-dev libx11-dev libxinerama-dev libxcursor-dev \
    libfontconfig1-dev libfltk1.3-dev librtmidi-dev

  # Symlink gcc in order to use the latest version

  sudo ln -f -s /usr/bin/g++-8 /usr/bin/g++

  # Download linuxdeployqt for building AppImages.

  wget https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage
  chmod a+x linuxdeployqt-6-x86_64.AppImage

  # Update the shared libraries cache

  sudo ldconfig

fi