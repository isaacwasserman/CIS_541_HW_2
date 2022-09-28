# Part 3
1. To compile either part, navigate to its corresponding directory and run `make`. To subsequently run the executable, run `./part_1` for part 1 or `./part_2` for part 2.
2.
    a. RM: 2, EDF: 3

    b. RM: 2, EDF: 5; The number of missed deadlines stays the same for rate monotonic scheduling because a missed deadline only affects the priority of later jobs on the same task. This is different from earliest deadline first scheduling, where a single missed deadline becomes the highest priority job, compounding the number of missed deadlines.

    c. If a system designer wants to allow for any missed deadlines, they should choose rate monotonic scheduling. This ensures that a single unexpectedly slow task cannot effect the realtimeness of a well-behaved task.

    d. The easiest example I can think of for why you might not want missed deadlines is video playback audio sychronization. In this case, you want to only render frames of video which are relevant to the current audio. However, if your task is accepting keyboard input, and the system is performing slower than expected, you don't want to ignore certain keystrokes just because the circumstances caused them to miss their deadline.

3. 
    a. Utilization for B: $1/4 + 2/6 + 3/8 = 0.9583$. Utilization for C: $1/4 + 2/6 + 4/8 = 1.083$

    b. I would expect no missed deadlines with EDF when scheduling set B, but would expect missed deadlines when scheduling set C. This is because set C has a utilization which is greater than 1.

    c. Yes. Tasks with utilization $U$ can be scheduled by RM without missed deadlines if $U\leq n(2^{1/n}-1)$. In the case of task sets B and C, this condition is $U\leq 0.78$. Both task sets have a utilization higher than the bound, so neither can be scheduled by RM without missed deadlines.

    d. The evidence below confirms my answers to (3b) and (3c).
    - Set B, EDF: 0 misses, utilization 0.96.
    - Set B, RM: 4 misses, utilization 0.96.
    - Set C, EDF: 4 misses utilization 1.0.
    - Set C, RM: 8 misses, utilization 1.0.