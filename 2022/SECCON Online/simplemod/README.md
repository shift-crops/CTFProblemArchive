# SimpleMod

## Author

ShiftCrops

## Estimated Difficulty

medium

## Steps to Review

### How to Run

1. run `./run.sh`

### Notes for Reviewers

N/A

## Steps to Run a Solver

You can run a solver for this challenge as follows:

```sh
docker run -it \
    -e SECCON_HOST=<host name e.g. hoge.untara.seccon.jp> \
    -e SECCON_PORT=<port number e.g. 8080> \
	$(docker build . -f solver/Dockerfile -q)
```

## Deployment Notes

- Need a server: Yes
- Need a constant healthcheck: Yes

## Additional Notes

