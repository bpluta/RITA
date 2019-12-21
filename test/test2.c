#include <stdio.h>
#include <unistd.h>

int main() {
    for (int i=1; i<=2; i++) {
        printf("Hello world %d\n",i);
        sleep(1);
    }
    return 0;
}
