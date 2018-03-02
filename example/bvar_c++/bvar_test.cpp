//
// Created by ufo on 18-3-2.
//

#include <bvar/bvar.h>

int main() {
    bvar::Adder<int> count1;

    count1 << 10 << 20 << 30;   // values add up to 60.
    count1.expose("count1");  // expose the variable globally

    return 0;
}
