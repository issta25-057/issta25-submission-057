#!/bin/bash

BASENAME=u18-fuzzing-crypto
CANARY=.container_run
sudo docker container stop  ${BASENAME}-container
sudo docker container rm    ${BASENAME}-container
sudo docker rmi             ${BASENAME}-image

if [ -f "${BASENAME}-image.tar" ]; then
	sudo docker load < ${BASENAME}-image.tar
else
	sudo docker build -t        ${BASENAME}-image .
fi
rm -f $CANARY
