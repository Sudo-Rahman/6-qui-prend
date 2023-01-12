#!/bin/bash

clear

gcc Bot.c -o bot

gcc Client.c -lpthread -o Client

./Client
