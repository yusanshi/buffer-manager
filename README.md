# Buffer Manager

A simple buffer pool manager with LRU, LRU-2 and CLOCK replacement policy, and multithreading support.

## Requirements

- Linux-based OS
- CMake 3.12+
- C++17

## Get started

### Build
```bash
git clone https://github.com/yusanshi/buffer-manager && cd buffer-manager
mkdir log
mkdir data
wget -P ./data https://storage.yusanshi.com/data-5w-50w-zipf.txt

mkdir build
cd build
cmake ..
make
cd ..
mv build/manager manager.out
```

### Run
```bash
for round in {1..10}
do
    for replacement in LRU LRU_2 CLOCK
    do
        for threads in 1 2 4 8 16 32 64
        do
            echo "Running with ${replacement} replacement policy, ${threads} thread(s), round ${round}"
            ./manager.out -r ${replacement} -t ${threads} > ./log/replacement_${replacement}_threads_${threads}_round_${round}.log
        done
    done
done
```

Also check `./manager.out -h` and [./doc/report.md](./doc/report.md) for more details.

## Check the performance

```bash
python3 stat.py > ./log/stat.md
cat ./log/stat.md
```

The following is taken from `./log/stat.md`:

**Hit rate and IO**

| Metric | Hit rate | Hit count | Miss count | Read count | Write count |
| --- | --- | --- | --- | --- | --- |
| LRU | 0.33905 | 169526.0 | 330474.0 | 330474.0 | 172998.0 |
| LRU_2 | 0.44027 | 220135.9 | 279864.1 | 279864.1 | 137691.4 |
| CLOCK | 0.30343 | 151716.9 | 348283.1 | 348283.1 | 187500.0 |


**Elapsed time (s)**

| Num threads | 1 | 2 | 4 | 8 | 16 | 32 | 64 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LRU | 0.11872 | 0.11321 | 0.054520 | 0.039077 | 0.037684 | 0.047050 | 0.065261 |
| LRU_2 | 0.084691 | 0.077624 | 0.046481 | 0.037257 | 0.036909 | 0.047953 | 0.061070 |
| CLOCK | 0.11842 | 0.10729 | 0.055193 | 0.040958 | 0.036869 | 0.049037 | 0.063484 |

And `./log/stat.svg`:

![](https://img.yusanshi.com/upload/20211125124207537569.svg)

Note the `PROCESS_IO` in `src/common.h` defaults to `false`, which means reading/writing from/to storage is skipped. If we set it to `true`, it will be (note the difference with above figure in `Elapsed time`):

![](https://img.yusanshi.com/upload/20211125130532184674.svg)
