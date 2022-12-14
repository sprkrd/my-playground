#!/usr/bin/env python3

from z3 import *


on_a_b, on_b_a = Bools("on_a_b on_b_a")

on_a_b_ = Bool("on_b_a")

print(on_a_b.decl().name())

print(on_a_b.get_id())
print(on_a_b_.get_id())


# ontable_a, ontable_b = Bools("ontable_a ontable_b")
# clear_a, clear_b = Bools("clear_a clear_b")
# holding_a, holding_b = Bools("holding_a holding_b")
# handempty = Bool("handempty")

# unstack = Bool("unstack")
# slot_1_a, slot_1_b, slot_2_a, slot_2_b = Bools("slot_1_a slot_1_b slot_2_a slot_2_b")

# s = Solver()

# s.add(on_a_b)
# s.add(Not(on_b_a))
# s.add(ontable_b)
# s.add(Not(ontable_a))
# s.add(clear_a)
# s.add(Not(clear_b))
# s.add(Not(holding_a))
# s.add(Not(holding_b))
# s.add(handempty)

# s.add(Or(Not(unstack), slot_1_a, slot_1_b))
# s.add(Or(Not(unstack), slot_2_a, slot_2_b))

# s.add(Or(Not(slot_1_a), Not(slot_1_b)))
# s.add(Or(Not(slot_2_a), Not(slot_2_b)))

# s.add(Or(Not(unstack), Not(slot_1_a), Not(slot_2_a)))
# s.add(Or(Not(unstack), Not(slot_1_b), Not(slot_2_b)))

# s.add(Or(Not(unstack), Not(slot_1_a), Not(slot_2_b), handempty))
# s.add(Or(Not(unstack), Not(slot_1_a), Not(slot_2_b), on_a_b))
# s.add(Or(Not(unstack), Not(slot_1_a), Not(slot_2_b), clear_a))

# s.add(Or(Not(unstack), Not(slot_1_b), Not(slot_2_a), handempty))
# s.add(Or(Not(unstack), Not(slot_1_b), Not(slot_2_a), on_b_a))
# s.add(Or(Not(unstack), Not(slot_1_b), Not(slot_2_a), clear_b))

# s.add(Or(Not(slot_1_a), unstack))
# s.add(Or(Not(slot_1_b), unstack))
# s.add(Or(Not(slot_2_a), unstack))
# s.add(Or(Not(slot_2_b), unstack))

# Tie, Shirt = Bools('Tie Shirt')
# s = Solver()
# s.add(Or(Tie, Shirt),
      # Or(Not(Tie), Shirt))
      # # Or(Not(Tie), Not(Shirt)))

# x, y, eq = Bools("x y eq")

# s = Solver()

# s.add(Implies(And(x,eq), y))
# s.add(Implies(And(Not(x),eq), Not(y)))
# s.add(Implies(And(x,y), eq))
# s.add(Implies(And(Not(x),Not(y)), eq))

# while s.check() == sat:
    # print(s.model())
    # args = []
    # for var in s.model():
        # args.append(var() != s.model()[var])
        # # if s.model()[var]:
            # # args.append(var())
        # # else:
            # # args.append(Not(var()))
    # x = Or(*args)
    # # print(x)
    # s.add(Or(*args))
    # input()
    # # model = s.model()
    # # print(model)
    # # s.add(Or(Tie != model[Tie], Shirt != model[Shirt]))

