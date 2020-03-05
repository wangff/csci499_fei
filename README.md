# RobustSoftware
CSCI499 Robust Software Design and Implementation

# Compile

Use cmake to do compiling.

```bash
# In the root directory of the project
mkdir build
cd build
cmake ..
make
```

# Run

###All exectuables will be in the bin directory.

The code will generated four execturables.

\- faas_unit_tests: for the purpose of unit tests

\- kvstore_server: to start the KeyValue Storage

\- func_server: to start the Func

\- warble: the warble application

### Run unit tests

The binary file fan_unit_tests includes unit tests suits for all of the modules: KeyValue Storage, Func and Warble.

```bash
# In the bin directory
./faas_unit_tests
```



### Execution Sequence

Open three terminal to start three excutables. 

1. Open 1st terminal to run the KeyValue Storage.

   ```bash
   # In the bin directory
   ./kvstore_server
   ```

2. Open 2nd terminal to run the Func.

   ```bash
   # In the bin directory
   ./func_server
   ```

3. Open 3rd terminal to run do the configuration and run warble.

   ```bash
   # In the bin directory
   ./configure_hooking
   ```

### Run Warble Application (use cases)

Now, we could start run warble application.

- Register User

  Exp: ./warble --registeruer --"user_name"

- Warble a Text

  Exp: ./warble --user "user_name" --warble "text"

  Exp: ./warble --user "user_name" --warble "text" --reply "warble_id"

- Follow

  Exp: ./warble --user "user_name_1" --follow "user_name_2"

- Read a Thread

  Exp: ./warble --read "warble_id"

- Read User's Profile

  Exp: ./warble --user "username" --profile