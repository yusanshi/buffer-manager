# 实验报告

[TOC]

## 实验内容

实现 Storage and Buffer Manager。

完成了以下选做内容：

- 实现了 CLOCK、LRU-2 缓存置换算法；
- 支持并发缓存管理，通过锁机制解决冲突。



## 实验设计

### 整体框架

![](https://img.yusanshi.com/upload/20211230173231128657.svg)

核心组件是 `BufferManager` 和 `StorageManager`，前者负责核心的缓存管理功能，后者负责和数据库数据文件的交互。在 `BufferManager` 之外，还包了一层 `ParallelBufferManager` 以满足并行访问缓存的需求。

当有对某 page 的请求时， `ParallelBufferManager` 根据其请求的 page 的 `page_id` 将该请求映射到某个 `BufferManager` ，由其完成具体的缓存管理功能。

具体请求的过程是：首先调用 `BufferManager` 的 `FixPage` 方法将该 page pin 到 buffer 中，然后调用 `BufferManager` 的 `ReadPage` 或 `WritePage` 方法完成读/写过程，最后再调用`UnfixPage` 取消 pin 状态。



### `FixPage` 方法

下面重点介绍一下 `FixPage` 方法的实现。

它的输入是要 pin 的 page 的 id，输出是 pin 到 buffer pool 里的 frame 的 id。

`BufferManager` 类维护了一个成员变量 `page_table_` 记录了当前在缓存中的 page 的从 page id 到 frame id 的映射关系。

首先在 `page_table_` 中搜索该 page id，如果找到，先调用缓存置换器的 `HookFound` 方法（下面会介绍），然后返回 frame id。

如果没找到，而且 buffer pool 没有满，在 buffer pool 中新建 entry 添加到 `page_table_` 中，然后调用缓存置换器的 `HookNotFoundNotFull` 方法。

如果没找到而且 buffer pool 也已经满了，则需要调用缓存置换器的 `GetVictim` 方法以获得要被替换掉的 page （即 victim page）的 id，注意此时如果该 victim page 的 dirty 为真，需要调用 `StorageManager` 的 `WritePage` 方法以写回。接着执行置换过程：将 page 的内容读到 buffer pool 里面、修改 `page_table_` 。

其核心代码如下。

```c++
// If found in cache
if (this->page_table_.find(page_id) != this->page_table_.end()) {
    this->hit_count_ += 1;
    if (VERBOSE) {
        std::cout << "Page " << page_id << " found in cache" << std::endl;
    }
    int frame_id = this->page_table_[page_id].frame_id;
    this->replacer_->HookFound(page_id, frame_id);
    return frame_id;
}
this->miss_count_ += 1;

// If not found in cache and buffer is not full, allocate a new frame
if (this->page_table_.size() < this->buffer_size_) {
    if (VERBOSE) {
        std::cout << "Page " << page_id
            << " not found in cache, allocate a new frame in buffer"
            << std::endl;
    }

    for (auto i = page_id % this->buffer_size_;
         i < page_id % this->buffer_size_ + this->buffer_size_; i++) {
        auto j = i % this->buffer_size_;
        if (this->page_table_.find(j) == this->page_table_.end()) {
            int frame_id = j;
            this->buffer_[frame_id] = this->storage_manager_->ReadPage(page_id);
            this->page_table_[page_id] = PageTable{frame_id, false};
            this->replacer_->HookNotFoundNotFull(page_id, frame_id);
            return frame_id;
        }
    }
}

// If not found and buffer is full, try to get a victim frame ...
int victim_page_id = this->replacer_->GetVictim(page_id);
if (VERBOSE) {
    std::cout << "Page " << page_id
        << " not found in cache and buffer is full, found a victim frame "
        "(page id: "
        << victim_page_id << ")" << std::endl;
}
auto [victim_frame_id, victim_dirty] = this->page_table_[victim_page_id];
if (victim_dirty) {
    this->storage_manager_->WritePage(victim_page_id,
                                      this->buffer_[victim_frame_id]);
}
this->buffer_[victim_frame_id] = this->storage_manager_->ReadPage(page_id);
this->page_table_.erase(victim_page_id);
this->page_table_[page_id] = PageTable{victim_frame_id, false};
return victim_frame_id;
```



### 缓存置换算法

本实验中我完成了三种置换算法：`LRUReplacer`、`LRU2Replacer` 和 `ClockReplacer`。

它们都需要实现三个成员方法：

- `void HookFound(int page_id, int frame_id);`

  当被请求的 page 在 buffer pool 中找到时，调用此成员方法。

- `void HookNotFoundNotFull(int page_id, int frame_id);`

  如果被请求的 page 在 buffer pool 中没找到，而且 buffer pool 没有满，调用此成员方法。

- `int GetVictim(int page_id);`

  如果没找到而且 buffer pool 也已经满了，则需要调用此方法以获得要被替换掉的 page（即 victim page）。

每个置换器需要根据自己的置换策略实现以上这些方法。



#### `LRUReplacer`

使用一个链表，表头表示新近被访问的 page，表尾表示较早被访问的 page。

##### `HookFound`
将该 page 移动到链表表头。


##### `HookNotFoundNotFull`
新建该 page 结点并插入到链表头部。


##### `GetVictim`
从链表尾部向前遍历，找到第一个 `pin_count` 为 0 的 page 作为 victim page，并把该页移到链表头部（因为它将被替换成新的 page）。



#### `LRU2Replacer`

和 `LRUReplacer` 类似但是使用两个链表，分别是 cold cache 和 hot cache。

##### `HookFound`
如果该页在 cold cache 中，访问数增加 1，如果访问数不小于 2 则移到 hot cache 的表头，否则移到 cold cache 表头。
如果该页在 hot cache 中，移动到 hot cache 表头。


##### `HookNotFoundNotFull`
新建该 page 结点并插入到 cold cache 头部。


##### `GetVictim`
首先从 cold cache 中从尾到头找第一个 `pin_count` 为 0 的 page 作为 victim page，并把该页移到 cold cache 头部（因为它将被替换成新的 page）。

如果没找到，从 hot cache 中从尾到头找第一个 `pin_count` 为 0 的 page 作为 victim page，并把该页移到 cold cache 头部（注意这里仍然是移到 cold cache 头部）。



#### `ClockReplacer`

维护一个环形链表（当然具体实现起来很简单：使用普通链表，当访问到尾部时跳到表头即可）。
##### `HookFound`
置该页 `referenced` 位为 `true`。


##### `HookNotFoundNotFull`
在链表中添加该 entry。


##### `GetVictim`
后移指针，如果当前页 `pin_count` 为 0，若 `referenced` 位为 `true`，置为 `false` 并跳到下一页，否则选择该页作为 victim page，注意此时需要设 `referenced` 位为 `true`。



### 并发设计

我一开始实现的并发设计是对整个 BufferManager 设定一个 mutex 变量，然后在 BufferManager 的每个方法的代码开始处加上 `std::lock_guard<std::mutex> lock(this->mutex_);`，但是使得在并行访问的时候，多个线程中只有一个线程可以访问 buffer manager，这样显然多线程不可能比单线程跑得快。

后来在调研的时候，我发现 [CMU 的数据库实验](https://15445.courses.cs.cmu.edu/fall2021/project1/)里用了多个 buffer manger instance 的方法，只要保证每个请求的 page 都能唯一映射到一个固定的 buffer manger instance 即可。

于是，我也这么做。当有对某 page 的请求时， `ParallelBufferManager` 将请求映射到 index 为 `page_id % num_threads` 的 `BufferManager` 上，由其完成具体的缓存管理功能。多个 `BufferManager` 共同使用同一个 `StorageManager`，因此只需要在它们调用  `StorageManager`  的 `ReadPage`  和 `WritePage` 方法时申请下 mutex 锁即可。



## 实现结果

### 环境要求

- Linux-based OS
- CMake 3.12+
- C++17



### 编译和运行

**编译**

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

**运行**

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



### 运行结果

**命中率和 IO**

| Metric | Hit rate | Hit count | Miss count | Read count | Write count |
| --- | --- | --- | --- | --- | --- |
| LRU | 0.33905 | 169526.0 | 330474.0 | 330474.0 | 172998.0 |
| LRU_2 | 0.44027 | 220135.9 | 279864.1 | 279864.1 | 137691.4 |
| CLOCK | 0.32821 | 164104.2 | 335895.8 | 335895.8 | 177092.6 |

**运行时间 (s)**

| Num threads | 1 | 2 | 4 | 8 | 16 | 32 | 64 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LRU | 0.13006 | 0.11770 | 0.059671 | 0.042988 | 0.041902 | 0.054878 | 0.068988 |
| LRU_2 | 0.089972 | 0.080328 | 0.048345 | 0.039020 | 0.038143 | 0.049674 | 0.065099 |
| CLOCK | 0.12202 | 0.11196 | 0.058688 | 0.040626 | 0.039294 | 0.051695 | 0.065018 |

**可视化图表**

![](https://img.yusanshi.com/upload/20211207002326543178.svg)



### 结果分析

- 在缓存命中率方面，LRU_2 取得了远超其他方法的效果；CLOCK 方法作为 LRU 的粗略近似，命中率差了一点点。

- 在运行时间上看，LRU_2 依然是效果最好的，但事实上 LRU_2 是操作最复杂的，能取得最小的运行时间主要是因为它的缓存命中率高得多，所以减少了费时的缓存替换的次数。CLOCK 由于比 LRU 少了在链表中移动元素的操作所以时间上要好一点点。

- 在并发上，可以看到随着线程数增大，三个方法的运行时间都经历了先减小后增大的过程。整体上看，多线程带来的收益是非常可观的。

  运行时间的最低点发生在并发量为 16 的时候，可能是因为我电脑的 CPU R7-4800H 是 8 核 16 线程的。

  另外，当线程数越来越多时，它们运行时间的差异越来越小，这可能是因为线程数多时 overhead 的大头主要体现在进程调度上，每个缓存替换策略本身的差异相比之下就不那么明显了。
