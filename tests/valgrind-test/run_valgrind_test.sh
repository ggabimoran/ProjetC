#!/usr/bin/env bash

# INSTALL VALGRIND
ar=('valgrind')
../../assets/install_dependencies.sh  "${ar[@]}"
# RUN VALGRIND TEST
valgrind --leak-check=yes ./morpion