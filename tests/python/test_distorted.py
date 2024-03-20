# Move the current directory from the beginning to the end of sys.path
import sys
if sys.path[0] == '': sys.path.append(sys.path.pop(0))

import ioh

print('  DistortedOnemax:')

f = ioh.problem.DistortedOnemax(-1, 5, 1000, 0.9)
x = [0, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)
y = f(x)
print(x, "->", y)

f = ioh.problem.DistortedOnemax(
  instance = 1,
  n_variables = 5,
  distortion = 1000,
  distortion_probability = 0.9,
)
x = [0, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)
y = f(x)
print(x, "->", y)

f = ioh.problem.DistortedOnemax(
  instance = 1,
  n_variables = 5,
)
x = [1, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)
