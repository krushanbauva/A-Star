# A-star
## Overview
This repository demonstrates the use of A-star (A*) algorithm using Diagonal shortcut distance heuristics on top of Dijkstra's algorithm in path planning problems
## What does this do?
This program will read an image named 'a-star-image.jpg' which is in the current directory itself. The image contains a green region which is the start region, a red region which is the destination region, and black regions which are non-accessible regions(obstructed). This program will find an efficient path from the start region to the destination region through the white patches avoiding the black region(obstructed region)
## Requirements
This project depends on OpenCV
## How to compile?
Run the following command using gcc compiler : 
```bash
$ g++ a-star.cpp -o a-star `pkg-config opencv --cflags --libs`
```
## Run
The compilation process will create a binary file with the name 'a-star' which can be easily run by `./a-star`
## Things to be careful about :
Make sure your image is named as 'a-star-image.jpg' and is saved in the current directory itself.
