from math import *

for x in xrange(256):
    y = (2 ** 15 - 1) * sin(2 * pi * x / 256.0) + 2**15 -1
    print "\t\t8'h"+str(format(x, 'x'))+": sine = 16'h"+str(format(int(y), 'x'))+';'


l = [110.0, 123.47, 130.81, 146.83, 164.81, 174.61, 196.0, 220.0, 246.94, 261.63,
    293.66, 329.63, 349.23, 392.00, 440.0, 493.88, 523.25, 587.33, 659.26, 698.46,
    783.99, 880.0, 987.77, 1046.5, 1174.7, 1318.5, 1396.9, 1568.0, 1760.0]

freq = 1e8/2048

l.reverse()
ctword = []
lut = ""

for i in xrange(len(l)):
    lut = lut + str(long(2 ** 32 * l[i]/freq)) + ", "
    ctword.append(long(2 ** 32 * l[i]/freq))
print lut

distance = [300+i for i in xrange(5000-300)]

t = []

for i in xrange(len(distance)):
    tmp = distance[i]- 300 >>4;
    if tmp > 28:
        t.append(28)
    else:
        t.append(tmp)

volume = []
for i in xrange(len(distance)):
    if distance[i] - 300 > 255:
        volume.append(0)
    else:
        volume.append(255 - (distance[i] - 300))


print len(l)
print len(ctword)
print t
print volume
