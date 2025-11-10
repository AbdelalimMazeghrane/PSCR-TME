#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>

volatile sig_atomic_t child_pid = -1;
volatile sig_atomic_t child_done = 0;
volatile sig_atomic_t timed_out = 0;

void sigchld_handler(int) {
    child_done = 1;
}

// Handler pour SIGALRM
void sigalrm_handler(int) {
    timed_out = 1;
}
int wait_till_pid(pid_t pid,int sec);
void sigint_handler(int) {
    const char msg[] = "SIGINT reçu par le shell, interruption du fils en cours !\n";
    write(STDOUT_FILENO, msg, strlen(msg)); 
    if (child_pid > 0) {
        // Envoyer SIGINT à tout le groupe du fils
        kill(-child_pid, SIGINT);
    }
}

char* mystrdup(const char* src) {
    size_t len = strlen(src) + 1;
    char* dest = new char[len];
    memcpy(dest, src, len);
    return dest;
}

int main() {
    signal(SIGINT, sigint_handler);

    std::string line;
    while (true) {
        std::cout << "mini-shell> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::vector<std::string> args;
        std::string token;
        while (iss >> token) args.push_back(token);
        if (args.empty()) continue;

        if (args[0] == "exit") {
            std::cout << "Bye !\n";
            break;
        }

        char** argv = new char*[args.size() + 1];
        for (size_t i = 0; i < args.size(); ++i)
            argv[i] = mystrdup(args[i].c_str());
        argv[args.size()] = nullptr;

        child_pid = fork();
        if (child_pid == 0) {
            // Créer un nouveau groupe pour le fils
            setpgid(0, 0);
            // Rétablir comportement par défaut pour SIGINT
            signal(SIGINT, SIG_DFL);
            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        } else if (child_pid > 0) {
            // S'assurer que le fils est dans son propre groupe
            setpgid(child_pid, child_pid);
            int status;
            while (waitpid(child_pid, &status, 0) == -1) {
                if (errno != EINTR) { // autre erreur
                perror("waitpid");
                break;
                }
            }

            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                std::cout << "Fils terminé normalement avec code de sortie : " << exit_code << std::endl;
            } else if (WIFSIGNALED(status)) {
                int sig = WTERMSIG(status);
                std::cout << "Fils interrompu par le signal : " << sig << std::endl;
            }
            child_pid = -1;
        } else {
            perror("fork");
        }

        for (size_t i = 0; i < args.size(); ++i) delete[] argv[i];
        delete[] argv;
    }

    std::cout << "\nExiting mini-shell." << std::endl;
    return 0;
}


int wait_till_pid(pid_t pid, int sec) {
    struct sigaction sa_chld, sa_alrm;
    sigset_t mask, oldmask;

    // Installer handler SIGCHLD
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = 0;
    sigaction(SIGCHLD, &sa_chld, NULL);

    // Installer handler SIGALRM
    sa_alrm.sa_handler = sigalrm_handler;
    sigemptyset(&sa_alrm.sa_mask);
    sa_alrm.sa_flags = 0;
    sigaction(SIGALRM, &sa_alrm, NULL);

    // Bloquer les signaux SIGCHLD et SIGALRM temporairement
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Démarrer l’alarme
    alarm(sec);

    while (!child_done && !timed_out) {
        // Attendre SIGCHLD ou SIGALRM
        sigsuspend(&oldmask); 
    }

    // Désactiver l’alarme si le fils est terminé
    alarm(0);

    int status;
    pid_t wpid = -1;
    if (child_done) {
        // Vérifier si le fils demandé est bien terminé
        wpid = wait(&status); // récupère un fils terminé
        if (wpid == pid) {
            return pid;
        } else {
            // Terminaison d'un autre fils perdu
            return 0;
        }
    }

    // Timeout atteint
    return 0;
}
