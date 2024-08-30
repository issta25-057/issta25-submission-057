#!/bin/bash

BASENAME=u18-fuzzing-crypto
CANARY=.container_run

# enable afl debugging
sudo bash -c "echo core >/proc/sys/kernel/core_pattern"

# docker run -it --rm -v $(pwd):/fuzzing u18-fuzzing-crypto
# docker container start fuzz-container
# docker attach fuzz-container

if [ -f "$CANARY" ]; then
  # canary is here, need to rebuild
  bash build.sh
fi
touch $CANARY
# sudo docker container stop  ${BASENAME}-container
# sudo docker container rm    ${BASENAME}-container
# sudo docker rmi             ${BASENAME}-image
# sudo docker load < ${BASENAME}-image.tar
sudo docker run -it -v $(pwd)/reports:/fuzzing/reports --name ${BASENAME}-container ${BASENAME}-image
