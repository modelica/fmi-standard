#!/bin/sh

REPODIR="$(cd "$(dirname "$0")"; pwd)"

docker run -v "$REPODIR":/documents/ fm12/build_examples /documents/.circleci/build_examples.sh 
