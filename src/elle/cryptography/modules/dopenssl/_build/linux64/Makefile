CC=gcc
AR=ar
RANLIB=ranlib

OPENSSL_INCLUDE_DIRECTORY=/usr/local/opt/openssl/include
OPENSSL_LIBRARY_DIRECTORY=/usr/local/opt/openssl/lib

all: build tests check tests/dopenssl/bn.status tests/dopenssl/rand.status tests/dopenssl/rsa.status

build: lib/libdopenssl.a lib/libdopenssl.so

lib/libdopenssl.so: src/dopenssl/rand.o src/dopenssl/bn.o src/dopenssl/rsa.o
	@mkdir -p lib
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} '-Wl,-rpath,$$ORIGIN' src/dopenssl/bn.o src/dopenssl/rand.o src/dopenssl/rsa.o -shared -o lib/libdopenssl.so -lcrypto -lssl

lib/libdopenssl.a: src/dopenssl/rand.o src/dopenssl/bn.o src/dopenssl/rsa.o
	@mkdir -p lib
	${AR} crs lib/libdopenssl.a src/dopenssl/bn.o src/dopenssl/rand.o src/dopenssl/rsa.o
	${RANLIB} lib/libdopenssl.a

src/dopenssl/bn.o: ../../src/dopenssl/bn.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -fPIC -c ../../src/dopenssl/bn.c -o src/dopenssl/bn.o

src/dopenssl/rand.o: ../../src/dopenssl/rand.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -fPIC -c ../../src/dopenssl/rand.c -o src/dopenssl/rand.o

src/dopenssl/rsa.o: ../../src/dopenssl/rsa.c
	@mkdir -p src/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -fPIC -c ../../src/dopenssl/rsa.c -o src/dopenssl/rsa.o

tests: tests/dopenssl/bn tests/dopenssl/rand tests/dopenssl/rsa

check: tests/dopenssl/bn.status tests/dopenssl/rand.status tests/dopenssl/rsa.status

tests/dopenssl/bn: tests/dopenssl/bn.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' -fprofile-arcs -lgcov --coverage tests/dopenssl/bn.o -o tests/dopenssl/bn -lcrypto -ldopenssl -lssl

tests/dopenssl/rand: tests/dopenssl/rand.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' -fprofile-arcs -lgcov --coverage tests/dopenssl/rand.o -o tests/dopenssl/rand -lcrypto -ldopenssl -lssl

tests/dopenssl/rsa: tests/dopenssl/rsa.o lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../../lib' -fprofile-arcs -lgcov --coverage tests/dopenssl/rsa.o -o tests/dopenssl/rsa -lcrypto -ldopenssl -lssl

tests/dopenssl/bn.o: ../../tests/dopenssl/bn.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -c ../../tests/dopenssl/bn.c -o tests/dopenssl/bn.o

tests/dopenssl/rand.o: ../../tests/dopenssl/rand.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -c ../../tests/dopenssl/rand.c -o tests/dopenssl/rand.o

tests/dopenssl/rsa.o: ../../tests/dopenssl/rsa.c
	@mkdir -p tests/dopenssl
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -c ../../tests/dopenssl/rsa.c -o tests/dopenssl/rsa.o

tests/dopenssl/rsa.status: tests/dopenssl/rsa lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rsa

tests/dopenssl/rand.status: tests/dopenssl/rand lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/rand

tests/dopenssl/bn.status: tests/dopenssl/bn lib/libdopenssl.so
	@mkdir -p tests/dopenssl
	tests/dopenssl/bn

sample:
	${CC} -Wall -fprofile-arcs -ftest-coverage -fPIC -isystem . -isystem ${OPENSSL_INCLUDE_DIRECTORY} -I ../../src -I src -O2 -c ../../src/sample.c -o src/sample.o
	${CC} -L lib -L ${OPENSSL_LIBRARY_DIRECTORY} -Wl,-rpath-link,lib '-Wl,-rpath,$$ORIGIN' '-Wl,-rpath,$$ORIGIN/../lib' -fprofile-arcs -lgcov --coverage src/sample.o -o src/sample -lcrypto -ldopenssl -lssl
