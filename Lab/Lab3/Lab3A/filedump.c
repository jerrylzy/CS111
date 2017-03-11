//
//  filedump.c
//  Lab 3A: File System Dump
//
//  Created by Jerry Liu on 2/26/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

//  Disk image file descriptor
int dfd = -1;

//  Output file descriptors
#define NUM_OUTPUT  6
FILE *ofds[NUM_OUTPUT];
const char *const OutputFiles[] = { //  Output file names
    "super.csv",        //  for super block
    "group.csv",        //  for group descriptors
    "bitmap.csv",       //  for free inodes and blocks
    "inode.csv",        //  for inodes
    "directory.csv",    //  for directory entries
    "indirect.csv"      //  for indirect block entries
};

//  Index of specific output file descriptors
#define FD_SB 0
#define FD_GD 1
#define FD_BM 2
#define FD_IN 3
#define FD_DR 4
#define FD_ID 5

//  File types
#define REGULAR_FILE    0x8000
#define DIRECTORY       0x4000
#define SYMBOLIC_LINK   0xA000

//  The following are adopted from ext2_fs.h
//  Unused parameters after the last used parameter are deleted

//  Structure of the super block
typedef struct ext2_super_block {
    uint32_t s_inodes_count;  /* Inodes count */
    uint32_t s_blocks_count;  /* Blocks count */
    uint32_t s_r_blocks_count; /* Reserved blocks count, won't use */
    uint32_t s_free_blocks_count; /* Free blocks count, won't use */
    uint32_t s_free_inodes_count; /* Free inodes count, won't use */
    uint32_t s_first_data_block; /* First Data Block */
    uint32_t s_log_block_size; /* Block size */
    int32_t  s_log_frag_size; /* Fragment size */
    uint32_t s_blocks_per_group; /* # Blocks per group */
    uint32_t s_frags_per_group; /* # Fragments per group */
    uint32_t s_inodes_per_group; /* # Inodes per group */
    uint32_t s_mtime;  /* Mount time, won't use */
    uint32_t s_wtime;  /* Write time, won't use */
    uint16_t s_mnt_count;  /* Mount count, won't use */
    int16_t  s_max_mnt_count; /* Maximal mount count, won't use */
    uint16_t s_magic;  /* Magic signature */
} SuperBlock_t;

#define EXT2_SUPER_MAGIC 0xEF53
#define SB_SIZE (sizeof(SuperBlock_t))  // 58

//  Processed meta-information from superblock
typedef struct {
    uint32_t inodes_count;  /* Inodes count */
    uint32_t blocks_count;  /* Blocks count */
    uint32_t first_data_block; /* First Data Block */
    uint32_t block_size; /* Block size */
    int32_t  frag_size; /* Fragment size */
    uint32_t blocks_per_group; /* # Blocks per group */
    uint32_t frags_per_group; /* # Fragments per group */
    uint32_t inodes_per_group; /* # Inodes per group */
    uint32_t magic;  /* Magic signature */
    off_t    file_size;
} Metainfo_t;
Metainfo_t mi;

//  Structure of a blocks group descriptor
typedef struct ext2_group_desc {
    uint32_t bg_block_bitmap;  /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;  /* Inodes bitmap block */
    uint32_t bg_inode_table;  /* Inodes table block */
    uint16_t bg_free_blocks_count; /* Free blocks count */
    uint16_t bg_free_inodes_count; /* Free inodes count */
    uint16_t bg_used_dirs_count; /* Directories count */
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
} GD_t;

#define GD_SIZE                 (sizeof(GD_t))  //  32
#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)

//  Structure of an inode on the disk
typedef struct ext2_inode {
    
    uint16_t i_mode;  /* File mode */
    uint16_t i_uid;  /* Owner Uid */
    uint32_t i_size;  /* Size in bytes */
    uint32_t i_atime; /* Access time */
    uint32_t i_ctime; /* Creation time */
    uint32_t i_mtime; /* Modification time */
    uint32_t i_dtime; /* Deletion Time, won't use */
    uint16_t i_gid;  /* Group Id */
    uint16_t i_links_count; /* Links count */
    uint32_t i_blocks; /* Blocks count, number of 512B blocks */
    uint32_t i_flags; /* File flags, won't use */
    union {
        struct {
            uint32_t  l_i_reserved1;
        } linux1;
        struct {
            uint32_t  h_i_translator;
        } hurd1;
        struct {
            uint32_t  m_i_reserved1;
        } masix1;
    } osd1; /* Won't use */
    uint32_t i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
} Inode_t;

#define IN_SIZE         (sizeof(Inode_t))  //  100
#define EXT2_INODE_SIZE 128
#define INDIR_BLK_SIZE  256

//  Structure of a directory entry
typedef struct ext2_dir_entry {
    uint32_t inode;   /* Inode number */
    uint16_t rec_len;  /* Directory entry length */
    uint16_t name_len;  /* Name length */
    char name[256]; /* File name */
} Dir_t;
#define DIR_LENGTH  (sizeof(Dir_t))

off_t filesize(int fd) {
    struct stat st;
    fstat(fd, &st);
    return st.st_size;
}

void process_superblock(SuperBlock_t *sb) {
    ssize_t ret = pread(dfd, sb, SB_SIZE, 1024);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading superblock.\n");
        exit(EXIT_FAILURE);
    }
    
    mi.magic = sb->s_magic;
    if (mi.magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "*** Error: Superblock - invalid magic: %#x\n", mi.magic);
        exit(EXIT_FAILURE);
    }

    mi.inodes_count = sb->s_inodes_count;

    mi.blocks_count = sb->s_blocks_count;
    mi.block_size = 1024 << sb->s_log_block_size;
    if (sb->s_log_block_size > 64) {
        fprintf(stderr, "*** Error: Superblock - invalid block size: %u\n", mi.block_size);
        exit(EXIT_FAILURE);
    }
    mi.file_size = filesize(dfd);
    if (mi.blocks_count * mi.block_size > mi.file_size) {
        fprintf(stderr, "*** Error: Superblock - invalid block count %u > image size %ld\n",
                mi.blocks_count, mi.file_size);
        exit(EXIT_FAILURE);
    }

    mi.frag_size = (sb->s_log_frag_size > 0) ? 1024 << sb->s_log_frag_size :
                                               1024 >> -sb->s_log_frag_size;
    
    mi.blocks_per_group = sb->s_blocks_per_group;
    if (mi.blocks_count % mi.blocks_per_group) {
        fprintf(stderr, "*** Error: Superblock - %u blocks, %u blocks / group\n",
                mi.blocks_count, mi.blocks_per_group);
        exit(EXIT_FAILURE);
    }

    mi.inodes_per_group = sb->s_inodes_per_group;
    if (mi.inodes_count % mi.inodes_per_group) {
        fprintf(stderr, "*** Error: Superblock - %u inodes, %u inodes / group\n",
                mi.inodes_count, mi.inodes_per_group);
        exit(EXIT_FAILURE);
    }

    mi.frags_per_group = sb->s_frags_per_group;
    mi.first_data_block = sb->s_first_data_block;
    //  If s_log_block_size != 0, we have block size > 1K, which means first block is 0
    //  If s_log_block_size == 0, we have block size 1K, which means first block is 1
    //  Thus we just need to check if first_data_block == !! sb->s_log_block_size
    if (mi.first_data_block == !! sb->s_log_block_size) {
        fprintf(stderr, "*** Error: Superblock - invalid first data block with block size %uKiB: %u\n",
                sb->s_log_block_size + 1, mi.first_data_block);
        exit(EXIT_FAILURE);
    }

    FILE *ofd = ofds[FD_SB];
    fprintf(ofd, "%x,%u,%u,%u,%d,%u,%u,%u,%u\n",  //  fprintf is faster than dprintf
           mi.magic, mi.inodes_count, mi.blocks_count, mi.block_size, mi.frag_size,
           mi.blocks_per_group, mi.inodes_per_group, mi.frags_per_group, mi.first_data_block);
}

//  returns true if it is valid; false otherwise
bool check_valid_blocks(size_t start, size_t end, size_t block_num) {
    return start <= block_num && block_num <= end;
}

char get_file_type(uint16_t c) {
    char type;
    switch (c) {
        case REGULAR_FILE:  type = 'f'; break;
        case DIRECTORY:     type = 'd'; break;
        case SYMBOLIC_LINK: type = 's'; break;
        default:            type = '?'; break;
    }
    return type;
}

typedef struct {
    size_t inum;
    int pos;
    uint32_t block_entry;
    size_t *blocks_addr;
    size_t block_length;
    uint32_t nblocks;
    void (*process) (void *);
} Args_t;  //  7 arguments are too many

//  reads indirect blocks, returns the number of blocks read
size_t read_idbs(size_t *blocks_addr, size_t addr, size_t block_length,
                 uint32_t nblocks, size_t inum,
                 void (*process) (void *)) {
    uint32_t pblocks[INDIR_BLK_SIZE];
    ssize_t ret = pread(dfd, &pblocks, INDIR_BLK_SIZE * sizeof(uint32_t), addr);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading indirect blocks.\n");
        exit(EXIT_FAILURE);
    }
    Args_t args;
    args.inum = inum;
    args.blocks_addr = blocks_addr;
    args.block_length = block_length;
    args.nblocks = nblocks;
    for (int i = 0; i < INDIR_BLK_SIZE && args.block_length < nblocks; i++) {
        args.pos = i;
        args.block_entry = pblocks[i];
        process(&args);
    }
    return args.block_length;
}

//  under the hood function for indirect blocks
void read_idb(void *arg) {
    Args_t *args = (Args_t *) arg;
    if (args->block_entry > mi.blocks_count)
        fprintf(stderr, "*** Error: Inode %lu - invalid block pointer[%d] = %x\n",
                args->inum, args->pos, args->block_entry);
    //  address of the actual block
    args->blocks_addr[args->block_length++] = args->block_entry * mi.block_size;
}

//  reads multi-indirect blocks
void read_midb(Args_t *args){
    if (args->block_entry > mi.blocks_count)  //  don't dump its contents
        fprintf(stderr, "*** Error: Inode %lu - invalid block pointer[%d] = %x\n",
                args->inum, args->pos, args->block_entry);
    args->block_length = read_idbs(args->blocks_addr, args->block_entry * mi.block_size,
                                  args->block_length, args->nblocks, args->inum, read_idb);
}

//  read double indirect block
void read_didb(void *arg) {
    ((Args_t *) arg)->process = read_idb;
    read_midb(arg);
}

//  read triple indirect block
void read_tidb(void *arg) {
    ((Args_t *) arg)->process = read_didb;
    read_midb(arg);
}

//  returns the size of the directory record
size_t read_files(size_t blocks_addr, size_t inum, size_t entry_number, size_t total_len) {
    Dir_t dr;
    //  Read the record length
    ssize_t ret = pread(dfd, &dr.rec_len, sizeof(dr.rec_len), blocks_addr + sizeof(dr.inode));
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading directory entry length for inode %lu.\n", inum);
        exit(EXIT_FAILURE);
    }
    uint16_t rec_len = dr.rec_len;
    if (rec_len < 8 || rec_len > 1024 || rec_len > mi.file_size) {
        fprintf(stderr, "*** Error: Inode %lu, block %x - bad dirent: len = %u\n",
                inum, (uint32_t) (blocks_addr / mi.block_size), rec_len);
        return rec_len;  //  stop interpreting this entry
    }
    if (rec_len + total_len > mi.block_size)
        return mi.block_size + 1;  // indicate that it won't read

    //  Read the whole directory block
    ret = pread(dfd, &dr, MIN(rec_len, DIR_LENGTH), blocks_addr);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading directory entry for inode %lu.\n", inum);
        exit(EXIT_FAILURE);
    }

    uint32_t inode = dr.inode;
    if (inode > mi.inodes_count)
        fprintf(stderr, "*** Error: Inode %lu, block %x - bad dirent: Inode = %u\n",
                inum, (uint32_t) (blocks_addr / mi.block_size), inode);
    if (inode == 0)  //  unused inode
        return rec_len;
    uint8_t name_len = dr.name_len & 0xFFu;  //  name_len is located in the lower byte
    if (rec_len <= name_len) {
        fprintf(stderr, "*** Error: Inode %lu, block %x - bad dirent: len = %u, namelen = %u\n",
                inum, (uint32_t) (blocks_addr / mi.block_size), rec_len, name_len);
        return rec_len;  //  stop interpreting this entry
    }

    char *name = dr.name;
    //  per EXT2 manual: The name must be no longer than 255 bytes after encoding.
    //  Thus it is safe to add a null byte. Our size is 256
    name[name_len] = '\0';

    FILE *ofd = ofds[FD_DR];
    fprintf(ofd, "%lu,%lu,%u,%u,%u,\"%s\"\n",
            inum, entry_number, rec_len, name_len, inode, name);
    return rec_len;
}

void process_dir(Inode_t in, size_t inum) {
    uint32_t *pblocks = in.i_block;  //  block pointers
    uint32_t nblocks = in.i_blocks / (2 << mi.block_size);  //  # blocks
    
    size_t *blocks_addr = malloc(sizeof(size_t) * EXT2_NDIR_BLOCKS);
    if (blocks_addr == NULL) {
        perror("*** Error: malloc blocks_addr failed.\n");
        exit(EXIT_FAILURE);
    }
    
    size_t block_length = 0;
    size_t ub = MIN(EXT2_NDIR_BLOCKS, nblocks);
    //  If pblocks[i] is 0, we finished reading all entries
    for (size_t i = 0; i < ub && pblocks[i]; i++)
        blocks_addr[block_length++] = pblocks[i] * mi.block_size;

    //  Has blocks left to read in indirect blocks
    if (block_length < nblocks && pblocks[EXT2_IND_BLOCK]) {
        size_t *baddr2 = realloc(blocks_addr, sizeof(size_t) *
                                (EXT2_NDIR_BLOCKS +  // 12 direct blocks
                                 INDIR_BLK_SIZE));   // 256 indirect blocks
        if (baddr2 == NULL) {
            perror("*** Error: realloc blocks_addr failed.\n");
            exit(EXIT_FAILURE);
        }
        blocks_addr = baddr2;
        block_length = read_idbs(blocks_addr,
                                pblocks[EXT2_IND_BLOCK] * mi.block_size,
                                block_length, nblocks, inum, read_idb);
    }
    
    //  Has blocks left to read in double indirect blocks
    if (block_length < nblocks && pblocks[EXT2_DIND_BLOCK]) {
        size_t *baddr2 = realloc(blocks_addr, sizeof(size_t) *
                                 (EXT2_NDIR_BLOCKS +  // 12 direct blocks
                                  INDIR_BLK_SIZE +   // 256 indirect blocks
                                  INDIR_BLK_SIZE * INDIR_BLK_SIZE)); //  256^2 double indirect blocks
        if (baddr2 == NULL) {
            perror("*** Error: realloc blocks_addr failed.\n");
            exit(EXIT_FAILURE);
        }
        blocks_addr = baddr2;
        block_length = read_idbs(blocks_addr,
                                 pblocks[EXT2_DIND_BLOCK] * mi.block_size,
                                 block_length, nblocks, inum, read_didb);
    }

    //  Has blocks left to read in triple indirect blocks
    if (block_length < nblocks && pblocks[EXT2_TIND_BLOCK]) {
        size_t *baddr2 = realloc(blocks_addr, sizeof(size_t) *
                                 (EXT2_NDIR_BLOCKS +  // 12 direct blocks
                                  INDIR_BLK_SIZE +   // 256 indirect blocks
                                  INDIR_BLK_SIZE * INDIR_BLK_SIZE + //  256^2 double indirect blocks
                                  INDIR_BLK_SIZE * INDIR_BLK_SIZE * INDIR_BLK_SIZE)); //  256^3 triple
        if (baddr2 == NULL) {
            perror("*** Error: realloc blocks_addr failed.\n");
            exit(EXIT_FAILURE);
        }
        blocks_addr = baddr2;
        block_length = read_idbs(blocks_addr,
                                pblocks[EXT2_TIND_BLOCK] * mi.block_size,
                                block_length, nblocks, inum, read_tidb);
    }

    //  total_len is the total number of bytes read
    //  rec_len is the size of a single directory record
    size_t total_len = 0, rec_len = 0;
    for (size_t i = 0, entry_number = 0; i < block_length; i++, total_len = 0) {
        if ( ! blocks_addr[i])  //  invalid block pointer: 0
            continue;
        rec_len = read_files(blocks_addr[i], inum, entry_number, total_len);
        if (rec_len == mi.block_size + 1) {
            continue;
        } else if (rec_len == 0) {
            entry_number++;
            continue;
        }
        total_len += rec_len;
        entry_number++;
        while (total_len < mi.block_size) {  //  See if there are files left to read
            rec_len = read_files(blocks_addr[i] + total_len, inum, entry_number, total_len);
            if (rec_len == mi.block_size + 1) {
                //  Current record exceeds the size of the total block
                break;
            } else if (rec_len == 0) {  //  Nothing more exists
                entry_number++;
                break;
            }
            total_len += rec_len;
            entry_number++;
        }
    }
}

//  read and process indirect blocks, returns # blocks read
size_t process_idbs(uint32_t block, size_t len, uint32_t nblocks,
                   size_t (*process) (size_t, uint32_t, int, uint32_t, uint32_t)) {
    uint32_t pblocks[INDIR_BLK_SIZE];
    ssize_t ret = pread(dfd, &pblocks, INDIR_BLK_SIZE * sizeof(uint32_t), block * mi.block_size);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading indirect blocks.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < INDIR_BLK_SIZE && len < nblocks; i++, len++)
        len = process(len, block, i, pblocks[i], nblocks);
    return len;
}

//  process an indirect block, returns the # blocks read
size_t process_idb(size_t len, uint32_t block, int pos,
                   uint32_t block_entry, uint32_t nblocks) {
    if (block_entry > mi.blocks_count)
        fprintf(stderr, "*** Error: Indirect block %x - invalid entry[%d] = %u\n",
                block, pos, block_entry);
    FILE *ofd = ofds[FD_ID];
    if (block_entry && len < nblocks)
        fprintf(ofd, "%x,%u,%x\n", block, pos, block_entry);
    return len;
}

//  process a multi-indirect block, returns the # blocks read
size_t process_midb(size_t len, uint32_t block, int pos,
                    uint32_t block_entry, uint32_t nblocks,
                    size_t (*process) (size_t, uint32_t, int, uint32_t, uint32_t)) {
    if (block_entry > mi.blocks_count) {
        fprintf(stderr, "*** Error: Indirect block %x - invalid entry[%d] = %u\n",
                block, pos, block_entry);
        return len;   //  don't dump its contents
    }
    if ( ! block_entry)  //  block pointer 0, corrupted
        return len;
    len = process_idbs(block_entry, len, nblocks, process);
    FILE *ofd = ofds[FD_ID];
    fprintf(ofd, "%x,%u,%x\n", block, pos, block_entry);
    return len;
}

//  wrapper for double indirect blocks
size_t process_didb(size_t len, uint32_t block, int pos,
                    uint32_t block_entry, uint32_t nblocks) {
    return process_midb(len, block, pos, block_entry, nblocks, process_idb);
}

//  wrapper for triple indirect blocks
size_t process_tidb(size_t len, uint32_t block, int pos,
                    uint32_t block_entry, uint32_t nblocks) {
    return process_midb(len, block, pos, block_entry, nblocks, process_didb);
}

//  inum: inode number
void process_inode(size_t inum, long inode_table) {
    Inode_t in;
    size_t local_inode = (inum - 1) % mi.inodes_per_group;  //  suggested by ext2 manual
    //  inode_table: the block that contains the inode table
    //  local_inode: the desired index of the inode table
    ssize_t ret = pread(dfd, &in, IN_SIZE, inode_table * mi.block_size + EXT2_INODE_SIZE * local_inode);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading inode %lu.\n", inum);
        exit(EXIT_FAILURE);
    }

    uint16_t type_mask = 0xF000;
    uint16_t mode = in.i_mode;
    char type = get_file_type(mode & type_mask);
    uint16_t uid = in.i_uid;
    uint16_t gid = in.i_gid;
    uint16_t links_count = in.i_links_count;
    uint32_t ctime = in.i_ctime;
    uint32_t mtime = in.i_mtime;
    uint32_t atime = in.i_atime;
    uint32_t size = in.i_size;

    //  i_blocks refer to number of 0.5K blocks.
    //  According to EXT2 manual, below is a fast way to calculate:
    uint32_t nblocks = in.i_blocks / (2 << mi.block_size);
    uint32_t *pblocks = in.i_block;
    //  Sanity check
    size_t ub = MIN(nblocks, EXT2_NDIR_BLOCKS);
    for (size_t i = 0; i < ub; i++)
        if ( type != 'd' && ! check_valid_blocks(0, mi.blocks_count, pblocks[i]))
            fprintf(stderr, "*** Error: Inode %lu - invalid block pointer[%lu]: %x\n",
                    inum, i, pblocks[i]);

    //  Current inode represents a directory, process it
    if (type == 'd')
        process_dir(in, inum);
    
    size_t len = EXT2_NDIR_BLOCKS;
    if (len < nblocks)  //  If there are blocks left, process those indirect blocks
        len = process_idbs(pblocks[EXT2_IND_BLOCK], len, nblocks, process_idb);
    if (len < nblocks)
        len = process_idbs(pblocks[EXT2_DIND_BLOCK], len, nblocks, process_didb);
    if (len < nblocks)
        len = process_idbs(pblocks[EXT2_TIND_BLOCK], len, nblocks, process_tidb);

    FILE *ofd = ofds[FD_IN];
    fprintf(ofd, "%lu,%c,%o,%u,%u,%u,%x,%x,%x,%u,%u",
            inum, type, mode, uid, gid, links_count,
            ctime, mtime, atime, size, nblocks);

    //  print out block pointers
    for (int i = 0; i < EXT2_N_BLOCKS; i++)
        fprintf(ofd, ",%x", pblocks[i]);
    fprintf(ofd, "\n");
}

void process_bitmap(uint32_t block, uint32_t num_per_group, int block_group, long inode_table) {
    size_t start = num_per_group * block_group;

    //  inode table cannot live in block -1. This indicates block bitmap
    bool isinode = inode_table != -1;
    //  sizeof(uint8_t): 1 byte, so the size of the bitmap is the block size
    uint8_t *bitmap = malloc(mi.block_size);
    if (bitmap == NULL) {
        perror("*** Error: malloc bitmap failed.\n");
        exit(EXIT_FAILURE);
    }

    //  Read all bits from the bitmap
    ssize_t ret = pread(dfd, bitmap, mi.block_size, block * mi.block_size);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading bitmap.\n");
        exit(EXIT_FAILURE);
    }
    
    FILE *ofd = ofds[FD_BM];
    
    //  Below, the implmentation is not expected, but it matches sample solution's output.
    //  Does that mean sample solution's output is wrong? I don't know.
    //  Block number should start at 0 according to TA.
    for (size_t i = 0, nblock = 1; i < mi.block_size; i++) {
        uint8_t bit = bitmap[i];
        for (int j = 0; j < 8; j++, nblock++, bit >>= 1) {
            //  finished reading all blocks / inodes
            if (nblock > num_per_group) {
                free(bitmap);
                return;
            }

            if ( ! (bit & 0x1u)) {  //  block is free
                //  block group of the map, free block / inode number
                fprintf(ofd, "%x,%lu\n", block, nblock + start);
            } else if (isinode) {
                //  allocated inode, process it
                process_inode(nblock + start, inode_table);
            }
        }
    }
    free(bitmap);
}

//  bfd: bitmap fd
void process_group_descriptor() {
    int num_groups = mi.blocks_count / mi.blocks_per_group;

    size_t arr_size = GD_SIZE * num_groups;
    GD_t *gds = malloc(arr_size);
    if (gds == NULL) {
        perror("*** Error: malloc group descriptor failed.\n");
        exit(EXIT_FAILURE);
    }
    //  Group descriptor starts at the first block after the superblock
    //  Superblock is in the first data block
    ssize_t ret = pread(dfd, gds, arr_size, (mi.first_data_block + 1) * mi.block_size);
    if (ret < 0) {
        fprintf(stderr, "*** Error: reading group descriptor.\n");
        exit(EXIT_FAILURE);
    }
    
    FILE *ofd = ofds[FD_GD];

    for (int i = 0; i < num_groups; i++) {
        uint16_t free_blocks_count = gds[i].bg_free_blocks_count;
        uint16_t free_inodes_count = gds[i].bg_free_inodes_count;
        uint16_t dirs_count = gds[i].bg_used_dirs_count;
        uint32_t block_bitmap = gds[i].bg_block_bitmap;
        uint32_t inode_bitmap = gds[i].bg_inode_bitmap;
        uint32_t inode_table = gds[i].bg_inode_table;

        fprintf(ofd, "%u,%u,%u,%u,%x,%x,%x\n",
               mi.blocks_per_group, free_blocks_count, free_inodes_count,
               dirs_count, inode_bitmap, block_bitmap, inode_table);

        //  output corresponding bitmap information
        process_bitmap(block_bitmap, mi.blocks_per_group, i, -1);
        process_bitmap(inode_bitmap, mi.inodes_per_group, i, inode_table);
    }
    free(gds);
}

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "*** Error: wrong number of arguments: %d.\nUsage: %s diskname\n",
                argc, argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char *diskname = argv[1];
    if ((dfd = open(diskname, O_RDONLY)) < 0) {
        perror("*** Error: opening disk file.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_OUTPUT; i++) {
        //  Create output csv's
        if ((ofds[i] = fopen(OutputFiles[i], "w")) == NULL) {
            perror("*** Error: creating output file.\n");
            exit(EXIT_FAILURE);
        }
    }
    SuperBlock_t sb;
    process_superblock(&sb);
    process_group_descriptor();

    for (int i = 0; i < NUM_OUTPUT; i++) {
        //  Close output csv's
        if (fclose(ofds[i]) != 0) {
            perror("*** Error: closing output files.\n");
            exit(EXIT_FAILURE);
        }
    }
    
    if (close(dfd) < 0) {
        //  Close disk image
        perror("*** Error: closing disk file.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
