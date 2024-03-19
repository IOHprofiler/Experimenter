from ioh import get_problem, ProblemClass

print('DistortedOnemax ===============================================================')

print('instance 1 ===============================================================')

f1 = get_problem("DistortedOnemax", 1, 5, 0.5, 1_000, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)
