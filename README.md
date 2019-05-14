# Morpion

*Imperative programming project.*<br /><br />   
Morpion is a solitaire game. Rules and goals are explained in ['instructions.pdf'](doc/instructions.pdf).

## Getting Started

>These instructions will get you a copy of the project up and running on your local machine.

```console
foo@bar$ git clone https://github.com/GeoffreyDlvl/ENSIIE-morpion.git
```

### Prerequisites

>The project runs preferably on a linux machine. The only required tools are GCC, the GNU Compiler Collection, and make, the GNU make utility. They should be installed by default. If they are not, the following linux command will install gcc

```console
foo@bar$ sudo apt install gcc
```

>And this command will install make

```console
foo@bar$ sudo apt install build-essential
```

### Building

>To build the project, simple type the following command on the root of the project

```console
foo@bar$ make
```
### Testing

> Installing CUnit

```console
foo@bar$ sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
```

> Including it in c file

```c
#include <CUnit/CUnit.h>
```
> Including it in build


```console
foo@bar$ gcc  -o test test.c  -lcunit
```


## Running the game

Go to the sources directory and execute the program

```console
foo@bar$ ./morpion
```

## Built With

* [GCC](http://gcc.gnu.org) - The GNU Compiler Collection

## Versioning

We use [Git](http://git-scm.com) for versioning. 

## Authors

* **Geoffrey Delval** - [GeoffreyDlvl](https://github.com/GeoffreyDlvl)
* **Rémi Guijarro Espinosa** - [Remi-Guijarro](https://github.com/Remi-Guijarro)
* **Gabriel Moran** - [ggabimoran](https://github.com/ggabimoran)
* **Clément Peresse** - [LTRobot](https://github.com/LTRobot)

## License

This project is licensed under the MIT License - see the LICENSE.md file for details

