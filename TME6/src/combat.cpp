#include <iostream>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "util/mtrand.h"
#include "util/rsleep.h"
#include <errno.h>


using namespace std;
using namespace pr;
volatile sig_atomic_t PV=3;
void handler(int p){
    PV=PV-1;
}

void parry_handler(int) {
    printf("PID %d : coup paré !\n", getpid());
}

void attaque (pid_t adversaire){
    struct sigaction sa;
    sa.sa_handler=handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sigaction(SIGUSR1,&sa,NULL);

    if(kill(adversaire, SIGUSR1) == -1) {
        if(errno == ESRCH) {
            std::cout << "Adversaire déjà mort ! Victoire !" << std::endl;
            exit(0);
        } 
        
    }
    randsleep();
}

void defense(){
   signal(SIGUSR1,SIG_IGN);
   randsleep();

}

void defenseLuke() {
    sigset_t mask, oldmask;

    // 1) Installer un handler qui affiche "coup paré"
    struct sigaction sa;
    sa.sa_handler = parry_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // 2) Masquer SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // 3) Dormir normalement
    randsleep();

    // 4) Attendre une éventuelle attaque → affiche "coup paré" si reçue
    sigset_t suspend_mask;
    sigemptyset(&suspend_mask); // masque vide => on autorise réception
    sigsuspend(&suspend_mask);

    // 5) Restaurer le masque
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void combat_vador(pid_t adversaire) {
    while (PV > 0) {

        // Vérifier si Luke (enfant) est mort → éviter zombie
        pid_t r = waitpid(adversaire, NULL, WNOHANG);
        if (r == adversaire) {
            printf("Vador %d : Luke est déjà mort ! Victoire.\n", getpid());
            exit(0);
        }

        printf("Vador %d : PV = %d (défense)\n", getpid(), PV);
        defense();
        printf("Vador %d : PV = %d (attaque)\n", getpid(), PV);
        attaque(adversaire);
    }

    printf("Vador %d : Je suis vaincu...\n", getpid());
    exit(1);
}


void combat_luke(pid_t adversaire) {
    while (PV > 0) {
        printf("Luke %d : PV = %d (défense)\n", getpid(), PV);
        defenseLuke();
        printf("Luke %d : PV = %d (attaque)\n", getpid(), PV);
        attaque(adversaire);
    }
    printf("Luke %d : Je suis vaincu...\n", getpid());
    exit(1);
}
int main() {
    pid_t pid=fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    if(pid==0){
        printf("Luke ready (pid=%d)\n", getpid());
        combat_luke(getppid());
    }else{
        printf("Vador ready (pid=%d), Luke est %d\n", getpid(), pid);
        combat_vador(pid);
    }
    std::cout << "Placeholder for combat" << std::endl;
    return 0;
}

