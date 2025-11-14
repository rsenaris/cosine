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

run-no-install *ARGS:
  #!/usr/bin/env sh
  ./install/cosine/bin/cosine "$@"
  exit $?

clean:
  rm build install -rf

parallelize n_jobs n_evt first pattern seed *ARGS: install
  #!/usr/bin/env sh
  njobs=$(({{n_jobs}} - 1))
  for i in `seq 0 $njobs`; do
      i=$((i + {{first}}))
      seed=$(({{seed}} + $i))
      filename="{{pattern}}_$i.h5"
      logfile="log/$i.log"
      start="$((startid + i * {{n_evt}}))"
      stdbuf -oL                      \
      ./install/cosine/bin/cosine -e  \
          "/sim/seed       $seed"     \
          "/sim/outputfile $filename" \
          "/sim/start_id   $start"    \
          -n {{n_evt}}                \
          "${@:6}" 2>&1 > $logfile &
      echo "JOB $i has PID $!"
      if [ $((i % 12)) -eq 11 ]; then
          echo "Waiting to schedule more jobs ($((njobs-i)) remaining)"
          wait
      fi
  done
  echo "Waiting for last few jobs to finish"
  wait
