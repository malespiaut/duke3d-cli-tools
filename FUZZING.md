# Fuzzing mapinfo

## Add Sanitizers for Better Bug Detection

```
set --export --global AFL_USE_ASAN 1
set --export --global AFL_USE_UBSAN 1
afl-clang-fast -O0 -ggdb3 -D _POSIX_C_SOURCE=200809L -o mapinfo mapinfo.c
```

## Verify Instrumentation

```
strings mapinfo | grep __afl
afl-showmap -o /dev/null -- ./mapinfo fuzz_inputs/e1l1.map
```

## Making the input corpus unique

Note: `fuzz_inputs` is a directory that contains as many `.map` files as possible.

```afl-cmin.bash -i fuzz_inputs -o fuzz_inputs_unique -- ./mapinfo @@```

## Minimizing all corpus files

```
mkdir fuzz_inputs_final

cd fuzz_inputs_unique/

for i in (ls *.map)
afl-tmin -i "$i" -o "../fuzz_inputs_final/$i" -- ../mapinfo @@
end
```

## Better performance

```sudo afl-system-config```

## Fuzzing the target

```
cd ..
afl-fuzz -i fuzz_inputs_final/ -o fuzz_outputs/ -- ./mapinfo @@
```

## Links

https://aflplus.plus/docs/fuzzing_in_depth/
https://github.com/mykter/afl-training
https://afl-1.readthedocs.io/en/latest/fuzzing.html
