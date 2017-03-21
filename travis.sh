#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "linux" ];
then
	sudo apt-get -qq update
	sudo apt-get install -y -qq cmake
fi

if [ "$TRAVIS_OS_NAME" == "osx" ];
then
	brew update
	brew install openssl
fi
