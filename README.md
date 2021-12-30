# Buffer Manager

A simple buffer pool manager with LRU, LRU-2 and CLOCK replacement policy, and multithreading support.

![](https://img.yusanshi.com/upload/20211230173231128657.svg)

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
| CLOCK | 0.32821 | 164104.2 | 335895.8 | 335895.8 | 177092.6 |


**Elapsed time (s)**

| Num threads | 1 | 2 | 4 | 8 | 16 | 32 | 64 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LRU | 0.13006 | 0.11770 | 0.059671 | 0.042988 | 0.041902 | 0.054878 | 0.068988 |
| LRU_2 | 0.089972 | 0.080328 | 0.048345 | 0.039020 | 0.038143 | 0.049674 | 0.065099 |
| CLOCK | 0.12202 | 0.11196 | 0.058688 | 0.040626 | 0.039294 | 0.051695 | 0.065018 |

And `./log/stat.svg`:

![](https://img.yusanshi.com/upload/20211207002326543178.svg)
