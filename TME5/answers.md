# TME5 Correction: Parallelization of a Ray Tracer

## Baseline Sequential

### Question 1
Extracted TME5.zip from Moodle, added to repo, committed and pushed.

Configured project with CMake as previous TMEs. No dependencies, all handmade.

### Question 2

Ran `build/TME5` and generated `spheres.bmp`.
./TME5 -W 2000 -H 2000 -m sequential -s 250
Ray tracer starting with output 'spheres.bmp', resolution 2000x2000, spheres 250, mode sequential
Total time 54255ms.
Platform: 
Nom de modèle :                           AMD Ryzen 5 5500U with Radeon Graphi
                                            cs
    Famille de processeur :                 23
    Modèle :                                104
    Thread(s) par cœur :                    2
    Cœur(s) par socket :                    6
    Socket(s) :                             1
    Révision :                              1
    Vitesse maximale du processeur en MHz : 4056,0000
    Vitesse minimale du processeur en MHz : 400,0000

    RAM : 7,1Go
    OS:Linux abdelalim-HP-Laptop-15s-eq2xxx 6.8.0-84-generic #84~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Tue Sep  9 14:29:36 UTC 2 x86_64 x86_64 x86_64 GNU/Linux


./TME5 -W 400 -H 400 -m sequential -s 250
Ray tracer starting with output 'spheres.bmp', resolution 400x400, spheres 250, mode sequential
Temps/baseline choisi :
moyenne 3.38
## With Manual Threads

### Question 3
Implemented `void renderThreadPerPixel(const Scene& scene, Image& img)` in Renderer.

mesures
./TME5 -W 150 -H 150 -m ThreadPerPixel -s 250
Ray tracer starting with output 'spheres.bmp', resolution 150x150, spheres 250, mode ThreadPerPixel
Total time 1453ms.

### Question 4
Implemented `void renderThreadPerRow(const Scene& scene, Image& img)` in Renderer.

mesures
./TME5 -W 150 -H 150 -m ThreadPerRow -s 250
Ray tracer starting with output 'spheres.bmp', resolution 150x150, spheres 250, mode ThreadPerRow
Total time 61ms.

### Question 5
Implemented `void renderThreadManual(const Scene& scene, Image& img, int nbthread)` in Renderer.

mesures

dans le meilleur cas c'est environ 60s
./TME5 -W 150 -H 150 -m ThreadManual -s 250 -n 500
Ray tracer starting with output 'spheres.bmp', resolution 150x150, spheres 250, mode ThreadManual, threads 500
Total time 61ms.
## With Thread Pool

### Question 6
Queue class: blocking by default, can switch to non-blocking.

### Question 7
Pool class: constructor with queue size, start, stop.
Job abstract class with virtual run().

### Question 8
PixelJob: derives from Job, captures ?TODO?

renderPoolPixel: 

Mode "-m PoolPixel" with -n.

mesures
./TME5 -W 150 -H 150 -m PoolPixel -s 250 -n 10
Ray tracer starting with output 'spheres.bmp', resolution 150x150, spheres 250, mode PoolPixel, threads 10
Total time 68ms.
### Question 9
LineJob: derives from Job, captures TODO

renderPoolRow: 

Mode "-m PoolRow -n nbthread".

mesures
./TME5 -W 150 -H 150 -m PoolRow -s 250 -n 15
Ray tracer starting with output 'spheres.bmp', resolution 150x150, spheres 250, mode PoolRow, threads 15
Total time 61ms.
### Question 10
Best:
poolrow avec le bon nbthread
## Bonus

### Question 11

pool supportant soumission de lambda.