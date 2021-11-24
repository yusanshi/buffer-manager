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
for round in {1..5}
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

> The following is taken from `./log/stat.md`.

**Hit rate and IO**

| Metric | Hit count | Miss count | Hit rate | Read count | Write count |
| --- | --- | --- | --- | --- | --- |
| LRU | 169526.0 | 330474.0 | 0.33905 | 330474.0 | 172998.0 |
| LRU_2 | 220135.9 | 279864.1 | 0.44027 | 279864.1 | 137691.4 |
| CLOCK | 151716.9 | 348283.1 | 0.30343 | 348283.1 | 187500.0 |


**Elapsed time (s)**

| Num threads | 1 | 2 | 4 | 8 | 16 | 32 | 64 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LRU | 0.59896 | 0.46262 | 0.25943 | 0.17273 | 0.22395 | 0.31268 | 0.53427 |
| LRU_2 | 0.51739 | 0.43775 | 0.23887 | 0.16150 | 0.21834 | 0.31531 | 0.50202 |
| CLOCK | 0.52234 | 0.42577 | 0.24467 | 0.17332 | 0.19876 | 0.27798 | 0.44931 |