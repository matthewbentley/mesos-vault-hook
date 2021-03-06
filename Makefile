itest_trusty: docker_trusty
	mkdir -p dist/trusty
	docker run --rm -v $(CURDIR):/src -v $(CURDIR)/dist:/dist mesos-hooks-build-trusty /src/build.sh

docker_trusty:
	docker build -t mesos-hooks-build-trusty .

clean:
	rm -rf dist/*
