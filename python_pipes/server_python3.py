#!/usr/bin/env python3

import json
import pickle as pkl

def times_two(request):
    return [2*x for x in request]

def main():
    with open("server_python3_request", "rb") as request_file:
        with open("server_python3_response", "wb") as response_file:
            while True:
                # request = json.loads(request_file.readline())
                # json.dump(times_two(request), response_file)
                # response_file.write("\n")
                # response_file.flush()
                request = pkl.load(request_file)
                pkl.dump(times_two(request), response_file, protocol=2)
                response_file.flush()

if __name__ == "__main__":
    main()

