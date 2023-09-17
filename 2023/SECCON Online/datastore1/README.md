# DataStore1

## Estimated Difficulty

medium

## Deployment Notes

- Need a server: Yes
- Need a constant healthcheck: Yes

### How to run challange

```
$ docker compose up
```

### How to run solver

```
$ docker build -t solver-datastore1 -f solver/Dockerfile .
$ docker run --rm -e SECCON_HOST=***.***.***.*** -e SECCON_PORT=4585 solver-datastore1
```

