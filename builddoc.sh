#!/bin/sh

REPODIR="$(cd "$(dirname "$0")"; pwd)"

docker run -v "$REPODIR":/documents/ asciidoctor/docker-asciidoctor asciidoctor -B /documents/ -b html5 -v docs/index.adoc && \
if [ "_$1" = "_checkLinks" ]; then
  docker run -v "$REPODIR/docs/":/documents/ 18fgsa/html-proofer /documents --only-4xx
else
  docker run -v "$REPODIR/docs/":/documents/ 18fgsa/html-proofer /documents --only-4xx --checks-to-ignore LinkCheck
fi
