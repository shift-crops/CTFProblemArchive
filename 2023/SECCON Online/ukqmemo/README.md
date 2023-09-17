# ukqmemo

## Estimated Difficulty

- user   : easy
- kernel : medium
- qemu   : medium

## Deployment Notes

- Need a server: Yes
- Need a constant healthcheck: Yes

### How to run challange

```
$ docker compose up
```

### How to run solver

IP: aaa.aaa.aaa.aaa, PORT: bbbb で solver を動かす場合  
環境変数 `KMEMO_EXPLOIT_HOST` と `KMEMO_EXPLOIT_PORT` にそれぞれアドレスとポートを指定  
`KMEMO_EXPLOIT_PORT` のポートで http サーバが立ち上がるので、ポート転送も同じ値を指定  

```
$ docker build -t solver-kmemo -f solver/Dockerfile .
$ docker run --rm -e SECCON_HOST=***.***.***.*** -e SECCON_PORT=6318 -e KMEMO_EXPLOIT_HOST=aaa.aaa.aaa.aaa -e KMEMO_EXPLOIT_PORT=bbbb -p bbbb:bbbb solver-kmemo
```

