//
//  Partition.c
//  360_Lab1
//
//  Created by Travis Cripe on 8/27/19.
//  Copyright © 2019 Travis Cripe. All rights reserved.
//

#include <stdio.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
    u8 drive;             /* drive number FD=0, HD=0x80, etc. */
    
    u8  head;             /* starting head */
    u8  sector;           /* starting sector */
    u8  cylinder;         /* starting cylinder */
    
    u8  sys_type;         /* partition type: NTFS, LINUX, etc. */
    
    u8  end_head;         /* end head */
    u8  end_sector;       /* end sector */
    u8  end_cylinder;     /* end cylinder */
    
    u32 start_sector;     /* starting sector counting from 0 */
    u32 nr_sectors;       /* number of of sectors in partition */
};
