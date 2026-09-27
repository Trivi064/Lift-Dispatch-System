Multi-Lift Dispatch System

The idea
--------
I was literally just standing next to the lift in my apartment building (there's two lifts per floor - one passenger, one stretcher/service) and started wondering how it actually decides which one to send when you press the button. So I started mentally mapping out cases - both lifts idle, one moving, both moving, opposite directions, etc. Ended up with something like 30+ cases before I'd even written a line of code.

Why I didn't just hardcode all 30 cases
----------------------------------------
Once I actually sat down to turn all of that into if-else logic, it was obviously going to turn into a mess, and every new rule I thought of (like not letting a lift stop too abruptly if it's already moving) meant even more branches on top. So instead I built it as a scoring system - every lift gets a "cost" number based on a few factors, and whichever lift has the lowest cost gets picked. Adding a new rule just means adding one more scoring factor, not untangling more nested conditions. Bonus: this works for any number of lifts, not just two - the scoring loop just runs once per lift regardless of how many exist.

How the scoring works
----------------------
- Distance cost - how many floors away the lift is from the caller.
- Direction cost - if the lift's already moving toward the caller in the direction they want, it gets a bonus (free pickup on the way). If it's moving away or the wrong direction, it gets a penalty since it'll have to finish its route and come back.
- Interruption cost - if the lift's already committed to another stop, diverting it costs extra, since it's disrupting someone already being served.
- Abrupt stop penalty - if a lift's moving and within 2 floors of the caller, it gets a heavy penalty instead of being picked, since stopping that close while moving isn't safe.
- Tie-break - if two lifts score exactly equal, the passenger lift wins over the stretcher lift.

One decision I was pretty deliberate about: none of these are hard "you can't pick this lift" rules - they're all just penalties. If I'd made bad-direction or too-close a hard exclusion, I could end up with zero valid lifts in some situations (like the other lift being out of service), and the system would have no answer at all. This way it always picks something - worst case, the least-bad option.

Sample run
----------
```
Lift 1 (Passenger):
  Distance cost:     5
  Direction cost:    -2  (Already moving towards caller, same direction - Bonus)
  Interruption cost:  0  (Has no stop - No Penalty)
  Abrupt stop cost:   0  (No abrupt stop scenario - No Penalty)
  TOTAL:              3

Lift 2 (Stretcher):
  Distance cost:     3
  Direction cost:    4  (Moving away from the caller or opposite direction - Penalty)
  Interruption cost:  0  (Has no stop - No Penalty)
  Abrupt stop cost:   0  (No abrupt stop scenario - No Penalty)
  TOTAL:             7

>>> Lift 1 is dispatched.
```

How to run it
--------------
```
g++ Multiliftsys.cpp -o Multiliftsys
./Multiliftsys
```
It'll ask how many lifts there are, then their type/floor/motion state one by one, then the caller's floor and direction - and prints the full score breakdown per lift plus which one won and why.

Stuff I haven't figured out / want to add later
-------------------------------------------------
- Right now the "wrong direction" and "too close" penalties are just flat numbers I picked (4 and 8). Ideally they'd be calculated from how far the lift actually has to travel before it can turn around, but that needs me to track each lift's actual committed destination first, not just its direction.
- Nothing yet for a lift being out of service, or someone holding the door open too long.
- The passenger-vs-stretcher tie-break assumes exactly one lift of each type. Haven't worked out what "priority" even means if there were, say, 2 passenger lifts and 1 stretcher lift.

Note
----
Designed the logic and all the scoring decisions myself - this came out of my own thinking about the lift in my building, not a tutorial.