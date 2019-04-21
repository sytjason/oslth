#!/usr/bin/env python
# coding: utf-8
import numpy as np
f = open('test.out')
lines = f.readlines()
tmp = np.double(0)
for line in lines[:-1]:
    line = line[:-1]
    line = line.split(' ')
    tmp += (np.double(line[4]) - np.double(line[3]))

res = np.double(tmp/5000)
print("Unit time = %.9f" % res)




