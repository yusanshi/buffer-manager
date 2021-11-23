# Buffer Manager

```bash
./run.sh
```


```bash
tail -n 4 ./log/*.log
```


```
==> ./log/replacement_CLOCK_threads_1.log <==
Hit count: 151484       Miss count: 348516
Hit rate: 0.302968
Read count: 348516      Write count: 187725
Elapsed time: 0.907046s

==> ./log/replacement_CLOCK_threads_2.log <==
Hit count: 151618       Miss count: 348382
Hit rate: 0.303236
Read count: 348382      Write count: 187605
Elapsed time: 1.19089s

==> ./log/replacement_CLOCK_threads_4.log <==
Hit count: 151889       Miss count: 348111
Hit rate: 0.303778
Read count: 348111      Write count: 187554
Elapsed time: 2.75932s

==> ./log/replacement_LRU_2_threads_1.log <==
Hit count: 217859       Miss count: 282141
Hit rate: 0.435718
Read count: 282141      Write count: 138715
Elapsed time: 0.791046s

==> ./log/replacement_LRU_2_threads_2.log <==
Hit count: 218541       Miss count: 281459
Hit rate: 0.437082
Read count: 281459      Write count: 138377
Elapsed time: 1.25402s

==> ./log/replacement_LRU_2_threads_4.log <==
Hit count: 218609       Miss count: 281391
Hit rate: 0.437218
Read count: 281391      Write count: 138267
Elapsed time: 2.9556s

==> ./log/replacement_LRU_threads_1.log <==
Hit count: 169565       Miss count: 330435
Hit rate: 0.33913
Read count: 330435      Write count: 172955
Elapsed time: 0.982602s

==> ./log/replacement_LRU_threads_2.log <==
Hit count: 169471       Miss count: 330529
Hit rate: 0.338942
Read count: 330529      Write count: 173042
Elapsed time: 1.40901s

==> ./log/replacement_LRU_threads_4.log <==
Hit count: 169775       Miss count: 330225
Hit rate: 0.33955
Read count: 330225      Write count: 173053
Elapsed time: 2.88096s
```