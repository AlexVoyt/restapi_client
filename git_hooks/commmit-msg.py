#!/usr/bin/python

import sys
import re

def main():
    with open(sys.argv[1], "r") as file:
        regex = "\[[0-9]+\] "
        message = file.read()

        if(re.match(regex, message) == None):
            print("Wrong commit message")
            sys.exit(1)
        else:
            print("Successfully committed")
            sys.exit(0)



if __name__ == "__main__":
    main()
