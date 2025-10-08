#include <iostream>
using namespace std;
int main() {
    int tab[10];
    for(int i=0;i<10;i++){
        tab[i]=i;
    }
     for(int i=0;i<10;i++){
        std::cout << tab[i];
    }
std::cout << std::endl;
for (int i=9; i >= 0 ; i--) {

if (i>0 && tab[i] - tab[i-1] != 1) {
cout << "probleme !";
}
}
return 0;
}