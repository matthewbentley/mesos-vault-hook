itest_trusty: docker_trusty
	mkdir -p dist/trusty
	docker run --rm -v $(CURDIR):/build -v $(CURDIR)/dist:/dist mesos-hooks-build-trusty /build/build.sh

docker_trusty:
	docker build -t mesos-hooks-build-trusty .

clean:
	rm -rf dist/* modules
