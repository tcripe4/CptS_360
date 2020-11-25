//
//  main.c
//  t
//
//  Created by Travis Cripe on 1/20/20.
//  Copyright © 2020 Travis Cripe. All rights reserved.
//

#include <stdio.h>
//int g;
//int g = 3;
//int g[10000];
int g[10000] = {4};

main()
{
   //int a,b,c;
   //int a,b,c, d[10000];
   static int a,b,c, d[10000];
   a = 1; b = 2;
   c = a + b;
   printf("c=%d\n", c);
}
