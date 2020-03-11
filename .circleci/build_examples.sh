#!/bin/sh

ERROR=0

echo
echo "Validate XML"
python3 /documents/.circleci/validate_xml.py || ERROR=1

echo
echo "Lint AsciiDoc files"
# this script prints non-ascii characters to the console
PYTHONIOENCODING=UTF-8 python3 /documents/.circleci/lint_docs.py || ERROR=1

echo
echo "Compile headers"
gcc /documents/headers/fmi3Functions.h || ERROR=1

echo
echo "Run CMake generator"
cmake /documents/docs/examples/c-code -Bbuild || ERROR=1

echo
echo "Build examples"
cmake --build build || ERROR=1

echo
echo "Run examples"
./build/jacobian || ERROR=1
./build/co_simulation || ERROR=1
./build/model_exchange || ERROR=1

if [ $ERROR != 0 ]; then
  echo
  echo "!!!!! At least one test failed, see above !!!!!"
else
  echo
  echo "All tests passed."
fi
exit $ERROR
