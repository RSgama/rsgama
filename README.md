# RSgama

----
# What is RSgama?

> RSgama is a protocol that works at the data link level. Originally, [Charles 
Daniel](https://github.com/chaws) and 
[Rodrigo Siqueira](https://github.com/rodrigosiqueira) were challenged by 
professor Fernando to create a data protocol as a homework  in his class 
(Computer Network). The final result was the modeling and creation of the data 
link protocol, it was implemented many rules for handling many different 
situations. As an overview, look at the state machine of the protocol below:

![alt text](https://github.com/rodrigosiqueira/rsgama/blob/master/others/stateMachine.jpeg "State machine")

For more information, take a look at wiki.

----
## What is the the meaning of "RSgama"?

> RS is a reference to RS232, which is the interface that we have used 
originally to create the protocol. Finally, "gama" is a little tribute for 
"Universidade de Brasília - **Campus GAMA**".

----
# Development

* System Dependencies (Kubuntu/Ubuntu/Debian)
 * build-essential
 * cmake
* GCC 4.8
* Doxygen
* Google test

----
# Install

> What do you need for test and run RSgama:

* Follow the steps below to compile the code:
 * Go to rsgama folder.
```
  cd /local/of/origin/rsgama
```
 * Compile protocol
```
  make clean
  make
```
 * Go to bin/ for see the binary file.
* We recommend you use two cables with RS232 to USB interface. 

![alt text](https://github.com/rodrigosiqueira/rsgama/blob/master/others/cable.jpg "Cable")


