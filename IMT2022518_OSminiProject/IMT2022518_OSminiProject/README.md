## How to run the code

- Install make if you do not have make installed.
```bash
sudo apt install make
```
- Create necessary datafiles and compile the src files.
```bash
cd osminiproject
make all
```
- Change directory to /bin.
```bash
cd bin
```
- Execute the `./create_admin` in the bin directory and enter the admin username and admin password.
```bash
./create_admin
```
- To run the server-side program, run the following:
```bash
./server
```
- To run the client-side program, run the following:
```bash
./client
```
