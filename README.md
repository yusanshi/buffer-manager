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
for replacement in LRU LRU_2 CLOCK
do
    for threads in 1 2 4
    do
        for round in {1..5}
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
| LRU | 169595.7 | 330404.3 | 0.33919 | 330404.3 | 173009.0 |
| LRU_2 | 218257.1 | 281742.9 | 0.43651 | 281742.9 | 138518.7 |
| CLOCK | 151661.0 | 348339.0 | 0.30332 | 348339.0 | 187638.2 |


**Elapsed time (s)**

| Num threads | 1 | 2 | 4 |
| --- | --- | --- | --- |
| LRU | 0.79798 | 1.1963 | 2.7819 |
| LRU_2 | 0.80892 | 1.0908 | 2.2246 |
| CLOCK | 0.88087 | 1.1679 | 2.5819 |
