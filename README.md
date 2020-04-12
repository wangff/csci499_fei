# CSCI499 Robust Software Design and Implementation

# Description

This is a course project, which includes three major parts: Faas platform, Warble application, KeyValue backend storage.

- Faas platform aka. func in the context of this project: a basic Function-as-a-Service platform.
- Warble application: an application which runs on the Func and have the similiar basic functionality of Twitter.
- KeyValue backend storage: a backend data storage which stores data in the format of Key-Value pair.

# Table of Content

- [Setup Environmnt](#setup-environmnt)
  - [Setup Vagrant box](#setup-vagrant-box)
  - [Setup the compiling tool - Cmake](#setup-the-compiling-tool---cmake)
    - [Setup gflags](#setup-gflags)
    - [Setup glog](#setup-glog)
    - [Setup GTest](#setup-gtest)
    - [Setup GMock](#setup-gmock)
    - [Setup gRPC and Protobuf](#setup-grpc-and-protobuf)
- [Compile the project](#compile-the-project)
  - [Run the project](#run-the-project)
  - [Run unit tests](#run-unit-tests)
  - [Execution Sequence](#execution-sequence)
- [Usage: Run Warble Application](#usage-run-warble-application)
- [Refactor in Phase 2](#refactor-in-phase-2)
  - [Ephemeral warble code](#ephemeral-warble-code)
  - [Key-value store disk persistence](#key-value-store-disk-persistence)
    - [Strategy](#strategy)
    - [Usage](#usage)

# Setup Environment

## Setup Vagrant box

The Vagrantfile in the respository.

After clone the respository to the local, move out Vagrantfile from the repository root directory to make it in the same level directory with the project.

```bash
# In the parent directory of the project
$ vagrant up
$ vagrant ssh
```

## Setup the compiling tool - Cmake

```bash
# Go to the user's root directory
$ cd ~

# Install PPA
$ sudo apt-get install software-properties-common
$ sudo apt-get update

# Install g++
$ sudo apt-get install g++

$ sudo apt-get install build-essential

# Install OPENSSL
$ sudo apt install libssl-dev

# Download and install cmake
$ wget https://github.com/Kitware/CMake/releases/download/v3.17.0-rc2/cmake-3.17.0-rc2.tar.gz
$ tar -zxvf cmake-3.17.0-rc2.tar.gz
$ cd cmake-3.17.0-rc2/
$ ./bootstrap
$ make
$ sudo make install
```

## Dependency Package

All my dependencies packages should be install globally.

### Setup gflags

```bash
sudo apt-get install libgflags-dev
```

### Setup glog

```bash
# Sine I need version 0.4.0, so will build from source code of new release
$ cd ~
$ git clone https://github.com/google/glog.git
$ cd glog
$ mkdir cmake/build
$ cd cmake/build
$ cmake ../..
$ make
$ sudo make install
```

### Setup GTest

```bash
$ sudo apt-get install libgtest-dev

# compile gtest library
$ cd /usr/src/gtest
$ sudo cmake CMakeLists.txt
$ sudo make

# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
$ sudo cp *.a /usr/lib
```

### Setup GMock

```bash
$ sudo apt-get install google-mock
$ cd /usr/src/gmock
$ sudo cmake CMakeLists.txt
$ sudo make
$ sudo cp *.a /usr/lib
```

### Setup gRPC and Protobuf

#### Install pre-requisites

```bash
 $ cd ~
 $ sudo apt-get install autoconf libtool pkg-config
 $ sudo apt-get install clang-5.0 libc++-dev
```

#### Clone the repository (including submodules)

```bash
 $ git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
 $ cd grpc
 $ git submodule update --init
```

#### Compile and install gRPC and protobuf

```bash
# This will compile and install both grpc and protobuf
$ cd ~
$ sudo apt install golang-go
$ cd grpc
$ mkdir cmake/build
$ cd cmake/build
$ cmake ../..
$ make
$ sudo make install
```

# Compile the project

Use cmake to do compiling.

```bash
# Go to the root director of the project
$ cd /vagrant/csci499_fei-master

# In the root directory of the project
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Run the project

All exectuables will be in the bin directory.

The code will generated four execturables.

\- faas_unit_tests: for the purpose of unit tests

\- kvstore_server: to start the KeyValue Storage

\- func_server: to start the Func

\- warble: the warble application

## Run unit tests

The binary file fan_unit_tests includes unit tests suits for all of the modules: KeyValue Storage, Func and Warble.

```bash
# In the bin directory
$ ./faas_unit_tests
```

## Execution Sequence

Open three terminal to start three excutables.

1. Open 1st terminal to run the KeyValue Storage.

   ```bash
   # In the bin directory
   $ ./kvstore_server
   ```

2. Open 2nd terminal to run the Func.

   ```bash
   # In the bin directory
   $ ./func_server
   ```

3. Open 3rd terminal to run do the configuration and run warble.

   ```bash
   # In the bin directory
   $ ./configure_hooking
   ```

# Usage: Run Warble Application

Now, we could start run warble application.

- Register User

  e.g.: ./warble --registeruer "user_name"

- Warble a Text

  e.g.: ./warble --user "user_name" --warble "text"

  e.g.: ./warble --user "user_name" --warble "text" --reply "warble_id"

- Follow

  e.g.: ./warble --user "user_name_1" --follow "user_name_2"

- Read a Thread

  e.g.: ./warble --read "warble_id"

- Read User's Profile

  e.g.: ./warble --user "user_name" --profile

# Refactor in Phase 2

## Ephemeral warble code

Two changes are done to make my warble code ephemeral.

\- Directly pass storage pointer in to each warble function instead of storing it as a member variable of warble class.

\- Substitute the incremental index for new warbles' index in warble class with a random generate id for each new warble.

## Key-value store disk persistence

### Strategy

I adopted the persistence strategy that store the in-memory data in one time upon terminateion signal is detected by the kvstore_server.

### Usage

```bash
# in bin directory
# start kvstore_server in the in-memory model
$ ./kvstore_server

# start kvstore_server in the persistence model
$ ./kvstore_server <file_name>
```
