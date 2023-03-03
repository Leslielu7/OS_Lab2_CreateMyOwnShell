//
//  test_fg.c
//  lab2nyush
//
//  Created by Leslie Lu on 3/2/23.
//

//#include <stdio.h>
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

int main() {
    int x;
    printf("Input number: \n");
    scanf("%d", &x);
    printf("Result %d * 5 = %d\n", x, x*5);
    return 0;
}
