common:
	sudo apt-get install -y --no-install-recommends python3-tqdm libssl-dev xsltproc unzip

libkeccak:
	git clone https://github.com/maandree/libkeccak
	cd libkeccak; make -j; make install

cur_deps:
	sudo apt-get install -y --no-install-recommends astyle cmake gcc ninja-build python3-pytest python3-pytest-xdist doxygen graphviz python3-yaml valgrind

cur_liboqs: common cur_deps
	git clone https://github.com/open-quantum-safe/liboqs.git cur_liboqs
	mkdir -p cur_liboqs/build
	cd cur_liboqs/build; git checkout 0.8.0; cmake -GNinja .. ; ninja

OLDCC=gcc-7

old_deps:
	sudo apt-get install -y --no-install-recommends $(OLDCC) wget

old_liboqs: common old_deps
	git clone https://github.com/open-quantum-safe/liboqs.git old_liboqs
	cd old_liboqs; git checkout nist-branch-snapshot-2018-11
	mkdir -p old_liboqs/vendor
	wget -O old_liboqs/vendor/XKCP-master.zip https://github.com/XKCP/XKCP/archive/c98cd37ccb95c20412f1d57b58c7619a72115075.zip
	cd old_liboqs/vendor; unzip XKCP-master.zip; mv XKCP-c98cd37ccb95c20412f1d57b58c7619a72115075 XKCP-master
	find old_liboqs -name Makefile -print -exec sed -i.bak 's/gcc/$(OLDCC)/g' {} \;
	sed -i 's/#ifdef USE_AVX512F_INSTRUCTIONS/#if defined(USE_AVX512F_INSTRUCTIONS)||defined(AVX512)/' old_liboqs/src/kem/bike/upstream/additional_implementation/decode.c
	cd old_liboqs; CC=$(OLDCC) make -j5
	mkdir -p old_liboqs/build/lib
	cp old_liboqs/liboqs.a old_liboqs/build/lib/liboqs.a
	cp -r old_liboqs/include old_liboqs/build/include

mid_deps:
	sudo apt-get install -y --no-install-recommends cmake gcc ninja-build python3-pytest python3-pytest-xdist doxygen graphviz

mid_liboqs: common mid_deps
	git clone https://github.com/open-quantum-safe/liboqs.git mid_liboqs
	cd mid_liboqs; git checkout 0.4.0
	mkdir -p mid_liboqs/build
	cd mid_liboqs/build; cmake -GNinja .. ; ninja

aflpp_deps:
	sudo apt-get install -y --no-install-recommends curl python3-sympy python3-bitarray python3-cffi python3-dev gcc-7-plugin-dev
	if [ ! -d "/root/.cargo" ]; then curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y; fi
	# info: latest update on 2023-12-28, rust version 1.75.0 (82e1608df 2023-12-21) works

aflpp: aflpp_deps
	git clone https://github.com/AFLplusplus/AFLplusplus aflpp
	cd aflpp; git checkout 4.02c;
	# sed -i 's/if (likely(!new_bits)) {/if (0) {/' aflpp/src/afl-fuzz-bitmap.c
	sed -i 's/#define KEEP_UNIQUE_CRASH 10000U/#define KEEP_UNIQUE_CRASH 10U/' aflpp/include/config.h
	sed -i 's/#define MAX_FILE (1 \* 1024 \* 1024L)/#define MAX_FILE (10 * 1024 * 1024L)/' aflpp/include/config.h
	# sed -i 's/if (afl->saved_crashes >= KEEP_UNIQUE_CRASH) { return keeping; }/\/\/ if (afl->saved_crashes >= KEEP_UNIQUE_CRASH) { return keeping; }/' aflpp/src/afl-fuzz-bitmap.c
	sed -i 's/if (!has_new_bits(afl, afl->virgin_crash)) { return keeping; }/\/\/ if (!has_new_bits(afl, afl->virgin_crash)) { return keeping; }/' aflpp/src/afl-fuzz-bitmap.c
	cd aflpp; bash -c "make all -j"
	#
	#
	# Finished compiling AFL++
	# Remember to run the following command on the host machine
	# sudo bash -c "echo core >/proc/sys/kernel/core_pattern"

supercop_deps:
	sudo apt-get install -y --no-install-recommends libssl-dev libgmp-dev libcrypto++-dev

supercop: supercop_deps
	wget https://bench.cr.yp.to/supercop/supercop-20240107.tar.xz
	unxz < supercop-20240107.tar.xz | tar -xf -
	cp -r supercop-20240107 supercop-20240107.backup
	cp -r /fuzzing/tech/paper_fuzzing/supercop/crypto_hash /fuzzing/tech/paper_fuzzing/crypto_hash.backup
	cp /fuzzing/tech/paper_fuzzing/supercop/crypto_hash/*  supercop-20240107/crypto_hash/
	rm -rf /fuzzing/tech/paper_fuzzing/supercop
	mv supercop-20240107 /fuzzing/tech/paper_fuzzing/supercop
	echo "Running supercop's data-init: please wait"
	cd /fuzzing/tech/paper_fuzzing/supercop; ./data-init
	echo "data-init done"
	cp -r /fuzzing/tech/paper_fuzzing/supercop-data /fuzzing/tech/paper_fuzzing/supercop-data.backup
	cd /fuzzing/tech/paper_fuzzing/supercop/crypto_hash/; make clean libs
	cd /fuzzing/tech/paper_fuzzing/supercop/crypto_hash/; SUPERDIR=/fuzzing/tech/paper_fuzzing/supercop ./supercop.sh -l 1 -u 257

all_deps: common libkeccak cur_deps mid_deps old_deps aflpp_deps supercop_deps

make clean:
	-rm -rf cur_liboqs
	-rm -rf mid_liboqs
	-rm -rf old_liboqs
	-rm -rf aflpp
	-rm -rf libkeccak
	-rm -rf supercop-20240107 supercop-20240107.tar.xz
