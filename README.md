# Buffer Manager

```bash
./run_lru.sh
./run_lru_2.sh
./run_clock.sh
```


```bash
tail -n 5 *.log
```

```
==> clock.log <==
Buffer manager:
Hit count: 151484       Miss count: 348516
Hit rate: 0.302968
Storage manager:
Read count: 348516      Write count: 187725

==> lru_2.log <==
Buffer manager:
Hit count: 217859       Miss count: 282141
Hit rate: 0.435718
Storage manager:
Read count: 282141      Write count: 138715

==> lru.log <==
Buffer manager:
Hit count: 169565       Miss count: 330435
Hit rate: 0.33913
Storage manager:
Read count: 330435      Write count: 172955
```