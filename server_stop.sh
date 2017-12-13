#!/bin/bash

line=$(head -n 1 pid)
kill $line SIGINT
