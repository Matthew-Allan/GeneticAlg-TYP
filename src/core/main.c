#include "tester.h"

int main(int argc, char const *argv[]) {
    Tester tester = create_tester("list", "arraylist", "linkedlist", 5000);

    if(tester == NULL) {
        return -1;
    }

    run_tester(tester);

    close_tester(tester);
}