.DEFAULT_GOAL := all
TOKEN ?= submit
SHELL := /bin/bash

build:
	@if [[ ! -e build/Makefile ]]; then \
		mkdir -p build; \
		cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug; fi
	@make -C build

all:
	@if [[ ! -e build/Makefile ]]; then \
		mkdir -p build; \
		cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DADD_TRACE=true; fi
	@make -C build

clean:
	@if [[ -d build ]]; then \
		rm -r build; fi

map=maps/level1/level1-3.txt

run:
	@../QtOvercooked/QtOvercooked.exe -l $(map) -p build/main.exe

submit:
	$(eval TEMP := $(shell mktemp -d))
	$(eval BASE := $(shell basename $(CURDIR)))
	$(eval FILE := ${TEMP}/${TOKEN}.zip)
	@cd .. && zip -qr ${FILE} ${BASE}/.git
	@echo "Created submission archive ${FILE}"
	@curl -m 5 -w "\n" -X POST -F "TOKEN=${TOKEN}" -F "FILE=@${FILE}" \
		https://exam.problemsolving.top:8085/api/v2/submission/lab
	@rm -r ${TEMP}

