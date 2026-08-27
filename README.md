<img width="959" height="402" alt="image" src="https://github.com/user-attachments/assets/9c0f8845-a73f-475b-8341-10e9ab120f98" />
<img width="845" height="368" alt="image" src="https://github.com/user-attachments/assets/db899a27-4f73-47bc-ba13-fd58738ab728" />
The two major challenges that i faced
1. Removing Glitches (Outlier Rejection)
 Challenge: Real sensors often produce massive, unrealistic spikes (like dropping 50 meters in 1 second)
 Fix:  I wrote a loop that calculates the difference between consecutive seconds. If the depth changes by more than 15 meters
in one second, the code labels it a glitch and replaces it with the average of its neighbors.

2.Smoothing Noise (Moving Average)
 Challenge: Even without glitches, ocean waves cause minor, jagged ripples in the data.
 Fix:  We used a 3-second sliding average. For every second, the program takes the average of the current value, the second before it, 
and the second after it.This flattens out the ripples and reveals the true shape of the seafloor.
