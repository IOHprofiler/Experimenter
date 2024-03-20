# Move the current directory from the beginning to the end of sys.path
import sys
if sys.path[0] == '': sys.path.append(sys.path.pop(0))

import ioh

print('DistortedOnemax ===============================================================')

f = ioh.get_problem("DistortedOnemax", 1, 5, ioh.ProblemClass.INTEGER)
print(ioh.problem.IntegerSingleObjective.problems)
f.set_distortion(1000)
f.set_distortion_probability(0.9)
x = [0, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)

x = [0, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)

f = ioh.get_problem("DistortedOnemax", 1, 5, ioh.ProblemClass.INTEGER)
x = [1, 1, 1, 1, 0]
y = f(x)
print(x, "->", y)
