# -*-Makefile-*-

# Needed to make `"$@"` usable in recipes
set positional-arguments := true

default:
  just run --beam-on 10

setup BUILD SRC:
  #!/usr/bin/env sh
  if [[ ! -d {{BUILD}} ]]; then
    meson setup {{BUILD}} {{SRC}}
  fi

install-tests:
  just install-lib-quiet
  just setup build/cosine-tests test
  meson compile -C build/cosine-tests
  meson install -C build/cosine-tests

test *REST: install-tests
  sh install/cosine-tests/run-each-test-in-separate-process.sh "$@"

catch2-demo *REST:
  echo "$@"
  meson setup      build/cosine-tests test
  meson compile -C build/cosine-tests
  meson install -C build/cosine-tests
  install/cosine-tests/bin/catch2-demo-test "$@"

build:
  just setup build/cosine src
  meson compile -C build/cosine

install: build
  meson install -C build/cosine


install-lib-quiet:
  #!/usr/bin/env sh
  if [[ -f install/cosine/lib/libcosine.so    ||
        -f install/cosine/lib/libcosine.dylib ]]; then
    install_output=$(just install)
    install_ok=$?
    if ! test install_ok; then
      echo $install_output
      exit $install_ok
    fi
  else
    just install
  fi

run *ARGS: install
  #!/usr/bin/env sh
  sh execute-with-nixgl-if-needed.sh ./install/cosine/bin/cosine "$@"
  exit $?

clean:
  rm build install -rf
