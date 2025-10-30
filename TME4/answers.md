# TME4 Answers

Tracer vos expériences et conclusions dans ce fichier.

Le contenu est indicatif, c'est simplement la copie rabotée d'une IA, utilisée pour tester une version de l'énoncé.
On a coupé ses réponses complètes (et souvent imprécises voire carrément fausses, deadlocks etc... en Oct 2025 les LLM ont encore beaucoup de mal sur ces questions, qui demandent du factuel et des mesures, et ont de fortes tendances à inventer).
Cependant on a laissé des indications en particulier des invocations de l'outil possibles, à adapter à votre code.

## Question 1: Baseline sequential
 ./TME4 --input ../input_images -m pipe
Image resizer starting with input folder '../input_images', output folder 'output_images/', mode 'pipe', nthreads 4
Thread 136916076852800 (treatImage): 4164 ms CPU
Thread 136916131551488 (main): 3 ms CPU
Total runtime (wall clock): 4355 ms
Memory usage: Resident: 58.4 MB, Peak: 138 MB
Total CPU time across all threads: 4167 ms

./TME4 --input ../input_images -m resize
Image resizer starting with input folder '../input_images', output folder 'output_images/', mode 'resize', nthreads 4
Thread 138470337161472 (main): 4150 ms CPU
Total runtime (wall clock): 4330 ms
Memory usage: Resident: 67 MB, Peak: 138 MB
Total CPU time across all threads: 4150 ms


le temps cpu c'est le temps ou le processeur a vraiment travaillé
le wallclock c'est le temps total ecoulé du début a la fin meme quand le programme est en pause
le pic de ram correspant au moment ou toutes les images sont chargées


## Question 2:
Recherche des fichiers (1) : principalement I/O (accès au système de fichiers).
Chargement (2) : I/O car on lit les fichiers image depuis le disque, mais aussi un peu CPU selon la décompression.
Redimensionnement (3) : CPU intensif, manipulation en mémoire, calcul graphique.
Sauvegarde (4) : I/O uniquement, écrire sur disque.

oui on peut executer en parallele


## Question 3:
La constante FILE_POISON sert de "poison pill", un marqueur spécial pour signaler la fin des données dans la file bloquante. Elle est définie dans le namespace pr dans le fichier tasks.h



Dans le thread principal (producteur) :
Après avoir inséré tous les fichiers à traiter dans la file (fileQueue), le producer pousse la valeur FILE_POISON dans la queue pour indiquer au consommateur que plus aucun fichier ne sera produit.

Dans le thread worker (consommateur) :
Lorsqu’il pop la valeur FILE_POISON de la queue, il sait qu’il doit terminer sa boucle de traitement et donc s’arrêter proprement.


Usage dans le mode pipe

    Étape 1 : création de la file bornée.

    Étape 2 : instanciation du thread worker qui va consommer la queue.

    Étape 3 : dans le thread main, découverte et push des fichiers.

    Étape 4 : push de la poison pill pour signaler la fin.

    Étape 5 : join du thread worker (attendre fin traitement).

L’ordre des étapes, est-il imposé ?

    Peut-on inverser 2 et 3 ?
    Non recommandé mais possible. Il est préférable de lancer le thread worker avant de remplir la queue, pour qu’il soit prêt à consommer dès les premiers fichiers produits. Sinon, si tu commences à remplir avant, le worker pourrait ne pas commencer immédiatement, retardant ainsi le traitement.

    Peut-on inverser 3 et 4 ?
    Non. L’ordre est strict :
    Tu dois d’abord pousser tous les fichiers (3), puis la poison pill (4).
    Pousser la poison pill avant tous les fichiers entraînerait la fin prématurée du traitement car le worker verrait le poison pill et s’arrêterait avant de consommer les fichiers.

    Peut-on inverser 4 et 5 ?
    Non.
    Le join (5) doit venir après la poison pill (4) afin de garantir que le worker ait reçu le signal de fin et ait terminé avant que le thread principal continue. Sinon, tu risques de bloquer indéfiniment si le poison pill n’est pas encore dans la queue


## Question 5:
j'ai ajouter une poisonfile pour chaque thread
le meilleur reglage est 64 

## Question 6:
les champs est un QImage et le chemin vers le fichier
on stocke directement l'image

## Question 7:
BoundedBlockingQueue<TaskData*>

Avantages :

    Permet de stocker des pointeurs, donc pas de copie d’objet TaskData lors du passage dans la file : juste transfert d’adresse.

    Pratique si tu veux gérer explicitement la durée de vie (allocation/new/delete ou smart pointer) ; utile si TaskData est volumineux ou non copiable.

    La "poison pill" peut être simplement : un pointeur nul (nullptr) ou une adresse spéciale.

Inconvénients :

    Gestion mémoire à surveiller : il faut penser à détruire (delete/free) chaque tâche consommée, ou utiliser des smart pointers pour éviter les fuites.
Task_Poison:
un pointeur null
2. BoundedBlockingQueue<TaskData>

Avantages :

    Gestion mémoire automatique : copie des objets TaskData, pas besoin de gérer allocation ni suppression.

   

Inconvénients :

    Copie potentiellement coûteuse si TaskData ou QImage devient très volumineux.
Task_Poison:
on ajoute un champ de type bool

### Measurements (Release mode)

**Resize + pipe mode:**
```
./build/TME4 -m resize -i input_images -o output_images
... trace

./build/TME4 -m pipe -i input_images -o output_images
```



## Question 2: Steps identification

I/O-bound: 
CPU-bound: 

parallelisable a priori ?

## Question 3: BoundedBlockingQueue analysis



## Question 4: Pipe mode study

FILE_POISON ...

Order/invert :


## Question 5: Multi-thread pipe_mt

Implement pipe_mt mode with multiple worker threads.

For termination, ... poison pills...

Measurements:
- N=1: 
```
./build/TME4 -m pipe_mt -n 1 -i input_images -o output_images
...
```
- N=2: 
```
./build/TME4 -m pipe_mt -n 2 -i input_images -o output_images
...
```
- N=4: 
```
./build/TME4 -m pipe_mt -n 4 -i input_images -o output_images
...
```
- N=8: 
```
./build/TME4 -m pipe_mt -n 8 -i input_images -o output_images
...
```

Best: ??

## Question 6: TaskData struct

```cpp
struct TaskData {
... choix adopté
};
```

Fields: QImage ??? for the image data, ...

Use ??? for QImage, because ...

TASK_POISON: ...def...

## Question 7: ImageTaskQueue typing

pointers vs values

Choose BoundedBlockingQueue<TaskData???> as consequence

## Question 8: Pipeline functions

Implement reader, resizer, saver in Tasks.cpp.

mt_pipeline mode: Creates threads for each stage, with configurable numbers.

Termination: Main pushes the appropriate number of poisons after joining the previous stage.

Measurements: 
```
./build/TME4 -m mt_pipeline -i input_images -o output_images
...
```


## Question 9: Configurable parallelism

Added nbread, nbresize, nbwrite options.


Timings:
- 1/1/1 (default): 
```
./build/TME4 -m mt_pipeline -i input_images -o output_images
...
```
- 1/4/1: 
```
./build/TME4 -m mt_pipeline --nbread 1 --nbresize 4 --nbwrite 1 -i input_images -o output_images
```

- 4/1/1: 
```
./build/TME4 -m mt_pipeline --nbread 4 --nbresize 1 --nbwrite 1 -i input_images -o output_images
```
... autres configs

Best config: 
interprétation

## Question 10: Queue sizes impact


With size 1: 
```
./build/TME4 -m pipe_mt -n 2 --queue-size 1 -i input_images -o output_images
...
```

With size 100: 
```
./build/TME4 -m pipe_mt -n 2 --queue-size 100 -i input_images -o output_images
...
```

impact

Complexity: 


## Question 11: BoundedBlockingQueueBytes

Implemented with byte limit.

mesures

## Question 12: Why important

Always allow push if current_bytes == 0, ...

Fairness: ...

## Bonus

