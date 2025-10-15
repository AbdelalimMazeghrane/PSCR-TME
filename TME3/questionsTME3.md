# Questions - TME 3 : Threads

Instructions : copiez vos réponses dans ce fichier (sous la question correspondante). A la fin de la séance, commitez vos réponses.

## Question 1.

```
cd build-release && ./TME3 ../WarAndPeace.txt freqstd && ./TME3 ../WarAndPeace.txt freqstdf && ./TME3 ../WarAndPeace.txt freq && check.sh *.freq


traces pour les 3 modes, invocation a check qui ne rapporte pas d'erreur
```
Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 1790 ms

./TME3 ../WarAndPeace.txt freqstdf
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 1822 ms


Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 1795 ms


All files are identical
## Question 2.

start vaut 0

end vaut file_size

Code des lambdas :
```
code
```
[&](const std::string& word) {
                        total_words++;
                        um[word]++;
                }

[&](const std::string& word) {
                        total_words++;
                        hm.incrementFrequency(word);
                }

Accès identifiés :

accede par refernce a total words et les clés de la unordered_map

## Question 3.

Continuez de compléter ce fichier avec vos traces et réponses.
./TME3 ../WarAndPeace.txt partition
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 1825 ms
...

## Question 6.
ca que le probleme de total_waords mais pas ceux de la map um
## Question 8.
mt_mutex est beaucoup plus performant
## Question 9.
le temps ils ont presque pareil
## Question12.
atomic ca ne sert a rien car Chaque bucket est protégé par son mutex.

Donc une seule thread accède aux données du bucket à la fois.



