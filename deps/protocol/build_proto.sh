#!/bin/bash
mkdir cc java python
protoc -I=. --cpp_out=./cc comet.proto
protoc -I=. --java_out=./java comet.proto
protoc -I=. --python_out=./python comet.proto
