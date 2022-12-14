#!/usr/bin/env python2

import subprocess
import json
import time
import random
import os
import cPickle as pkl

class Service:
    def __init__(self, *args):
        self._args = args
        self._process = None
        self._service_name = os.path.splitext(os.path.basename(args[0]))[0]
        self._request_file_name = self._service_name + "_request"
        self._response_file_name = self._service_name + "_response"
        self._request_file = None
        self._response_file = None

    def __call__(self, request):
        # json.dump(request, self._request_file)
        # self._request_file.write("\n")
        # self._request_file.flush()
        # return json.loads(self._response_file.readline())
        pkl.dump(request, self._request_file)
        self._request_file.flush()
        return pkl.load(self._response_file)

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.end()

    def start(self):
        if not os.path.exists(self._request_file_name):
            os.mkfifo(self._request_file_name)
        if not os.path.exists(self._response_file_name):
            os.mkfifo(self._response_file_name)
        self._process = subprocess.Popen(self._args)
        self._request_file = open(self._request_file_name, "wb")
        self._response_file = open(self._response_file_name, "rb")

    def end(self):
        if self._process is not None:
            self._process.terminate()
            self._process.wait()
            self._process = None
        if self._request_file is not None:
            self._request_file.close()
        if self._response_file is not None:
            self._response_file.close()
        if os.path.exists(self._request_file_name):
            os.remove(self._request_file_name)
        if os.path.exists(self._response_file_name):
            os.remove(self._response_file_name)


def times_two_native(request):
    return [2*x for x in request]


def timeit(service, request, N=1000):
    start = time.time()
    for _ in range(N):
        service(request)
    elapsed = time.time() - start
    return elapsed*1000/N


def main():
    with Service("./server_python3.py") as times_two:
        # calling a service is as easy as this
        print times_two([1,2,3])
        # do several calls with large vectors to test speed
        request = [1 for _ in range(1000)]
        print "Avg. elapsed per call (service): ", timeit(times_two, request), "ms"
    print "Avg. elapsed per call (native): ", timeit(times_two_native, request), "ms"

if __name__ == "__main__":
    main()

