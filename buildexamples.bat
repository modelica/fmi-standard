set REPODIR=%~pd0

docker run -v "%REPODIR%:/documents/" fm12/build_examples /documents/.circleci/build_examples.sh 
