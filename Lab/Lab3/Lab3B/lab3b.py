import csv

IDX_NON_BLOCK = 11
NUM_NIND_BLOCKS = 12
IDX_IND_BLOCK = 12
IDX_DIND_BLOCK = 13
IDX_TIND_BLOCK = 14

class Inode:
    def __init__(self, inum, link_count):
        self.inum = inum # inode number
        self.link_count = link_count # hardlinks to this inode
        self.referred = set() # what dir entries points to this inode
        self.bp = set() # block pointers, 12 normal, 3 indirect


class Block:
    def __init__(self, bnum):
        self.bnum = bnum  # block number
        self.referred = set() # what inodes points to this block


class FSAnalyzer:
    def __init__(self, output_file):
        # Superblock
        self.ninodes = 0  # number of inodes
        self.nblocks = 0  # number of blocks
        self.inode_per_group = 0  # number of inodes per block group

        self.free_inodes = set()
        self.free_blocks = set()
        self.used_inodes = dict()
        self.error_inodes = dict()
        self.used_blocks = dict()
        self.ind_blocks = dict()
        self.dirs = dict()  # directory lists

        # Block group metainfo
        self.ibitmaps = set()  # inode bitmaps
        self.gnum_to_ibitmaps = dict()
        self.bbitmaps = set()  # block bitmaps

        # Output file
        self.f = output_file

    def read_input_file(self, filename):
        with open(filename, 'r') as csvfile:
            return csv.reader(csvfile, delimiter=',', quotechar='|')

    def read_superblock(self):
        with open('super.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for r in reader:
                self.ninodes = int(r[1])
                self.nblocks = int(r[2])
                self.inode_per_group = int(r[6])

    def read_group(self):
        with open('group.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            gnum = 0
            for r in reader:
                imap = int(r[4], 16) # base = 16
                bmap = int(r[5], 16)
                self.ibitmaps.add(imap)
                self.gnum_to_ibitmaps[gnum] = imap
                self.bbitmaps.add(bmap)
                self.used_blocks[imap] = Block(imap)
                self.used_blocks[bmap] = Block(bmap)
                gnum += 1

    def read_bitmap(self):
        with open('bitmap.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for r in reader:
                mnum = int(r[0], 16)  # map's block number
                bnum = int(r[1])
                if mnum in self.ibitmaps:
                    self.free_inodes.add(bnum)
                elif mnum in self.bbitmaps:
                    self.free_blocks.add(bnum)

    def read_ind(self):
        with open('indirect.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for r in reader:
                bind = int(r[0], 16)  # block number for the indirect block itself
                entry_num = int(r[1])
                bptr = int(r[2], 16)  # block pointer
                tup = (entry_num, bptr)
                if bind not in self.ind_blocks:
                    self.ind_blocks[bind] = list()
                self.ind_blocks[bind].append(tup)

    def invalid_block_error(self, bnum, inum, enum, ind):
        if ind == -1:
            self.f.write("INVALID BLOCK < {} > IN INODE < {} > ENTRY < {} >\n".format(bnum, inum, enum))
        else:
            self.f.write("INVALID BLOCK < {} > IN INODE < {} > INDIRECT BLOCK < {} > ENTRY < {} >\n".format(bnum, inum, ind, enum))

    def register_block(self, bnum, inum, enum, ind = -1):
        if bnum == 0 or bnum >= self.nblocks:
            self.invalid_block_error(bnum, inum, enum, ind)
        else:
            if bnum not in self.used_blocks:
                self.used_blocks[bnum] = Block(bnum)
            self.used_blocks[bnum].referred.add((inum, enum, ind))

    def process_idbs(self, bnum, inum, enum, nblocks, ind, func):
        self.register_block(bnum, inum, enum, ind)
        nblocks -= 1
        for ind_seq in self.ind_blocks[bnum]:
            if nblocks == 0:
                return nblocks
            nblocks = func(ind_seq[1], inum, enum, nblocks, ind_seq[0])  # seq[1]: enum, seq[0]: ind
        return nblocks

    def routine_idb(self, bnum, inum, enum, nblocks, ind):
        self.register_block(bnum, inum, enum, ind)
        return nblocks - 1

    def process_idb(self, bnum, inum, enum, nblocks, ind = -1):
        return self.process_idbs(bnum, inum, enum, nblocks, ind, self.routine_idb)

    def routine_didb(self, bnum, inum, enum, nblocks, ind):
        return self.process_idb(bnum, inum, enum, nblocks, ind)

    def process_didb(self, bnum, inum, enum, nblocks, ind = -1):
        return self.process_idbs(bnum, inum, enum, nblocks, ind, self.routine_didb)

    def routine_tidb(self, bnum, inum, enum, nblocks, ind):
        return self.process_didb(bnum, inum, enum, nblocks, ind)

    def process_tidb(self, bnum, inum, enum, nblocks, ind = -1):
        return self.process_idbs(bnum, inum, enum, nblocks, ind, self.routine_tidb)

    def read_inodes(self):
        with open('inode.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for r in reader:
                inum = int(r[0])
                self.used_inodes[inum] = Inode(inum, int(r[5]))  # r[5]: link count
                nblocks = int(r[10]) # r[10]: num of blocks
                ub = min(NUM_NIND_BLOCKS, nblocks)

                # non-indirect blocks
                for i in range(0, ub):
                    bnum = int(r[i + IDX_NON_BLOCK], 16)
                    self.register_block(bnum, inum, i)
                    nblocks -= 1

                # indirect block
                if nblocks > 0:
                    bnum = int(r[IDX_IND_BLOCK + IDX_NON_BLOCK], 16)
                    nblocks = self.process_idb(bnum, inum, nblocks, IDX_IND_BLOCK)

                # doubly indirect block
                if nblocks > 0:
                    bnum = int(r[IDX_DIND_BLOCK + IDX_NON_BLOCK], 16)
                    nblocks = self.process_didb(bnum, inum, nblocks, IDX_DIND_BLOCK)

                # triply indirect block
                if nblocks > 0:
                    bnum = int(r[IDX_TIND_BLOCK + IDX_NON_BLOCK], 16)
                    nblocks = self.process_tidb(bnum, inum, nblocks, IDX_TIND_BLOCK)

    def incorrect_entry_error(self, dinum, name, inum, cinum):  # cinum: correct inum
        name = name[1:-1]
        self.f.write("INCORRECT ENTRY IN < {} > NAME < {} > LINK TO < {} > SHOULD BE < {} >\n".format(dinum, name, inum, cinum))

    def read_dir(self):
        with open('directory.csv', 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quotechar='|')
            for r in reader:
                pinum = int(r[0])  # parent inode number
                enum = int(r[1])  # entry number
                inum = int(r[4])  # inode number of this file entry
                name = r[5]

                if (enum > 1 and inum != pinum) or pinum == 2: # root dir
                    self.dirs[inum] = pinum

                if inum in self.used_inodes:
                    self.used_inodes[inum].referred.add((pinum, enum))
                elif inum in self.error_inodes:
                    self.error_inodes[inum].add((pinum, enum))
                else:
                    self.error_inodes[inum] = [ (pinum, enum) ]

                ppinum = self.dirs[pinum]
                # check '.'
                if enum == 0 and pinum != inum:
                    self.incorrect_entry_error(pinum, name, inum, pinum)
                # parent inum of parent inum from directory map
                elif enum == 1 and inum != ppinum:
                    self.incorrect_entry_error(pinum, name, inum, ppinum)

    def read_all(self):
        self.read_superblock()
        self.read_group()
        self.read_bitmap()
        self.read_ind()
        self.read_inodes()
        self.read_dir()

    def missing_inode_error(self, inum, gnum):
        self.f.write("MISSING INODE < {} > SHOULD BE IN FREE LIST < {} >\n".format(inum, gnum))

    def incorrect_link_count_error(self, inum, link_count, clink_count):
        self.f.write("LINKCOUNT < {} > IS < {} > SHOULD BE < {} >\n".format(inum, link_count, clink_count))

    def multiple_referenced_block_error(self, inum, enum, ind):
        if ind == -1:
            return " INODE < {} > ENTRY < {} >".format(inum, enum)
        return " INODE < {} > INDIRECT BLOCK < {} > ENTRY < {} >".format(inum, ind, enum)

    def unallocated_block_error(self, inum, enum, ind):
        if ind == -1:
            return " INODE < {} > ENTRY < {} >".format(inum, enum)
        return " INODE < {} > INDIRECT BLOCK < {} > ENTRY < {} >".format(inum, ind, enum)

    def unallocated_inode_error(self, dinum, enum):
        return " DIRECTORY < {} > ENTRY < {} >".format(dinum, enum)

    def sanity_check(self):
        for bnum in self.used_blocks:
            block = self.used_blocks[bnum]
            link_count = len(block.referred)
            if link_count > 1:  # mult ref block error
                output = "MULTIPLY REFERENCED BLOCK < {} > BY".format(bnum)
                for node in sorted(block.referred):
                    output += self.multiple_referenced_block_error(node[0], node[1], node[2])
                self.f.write(output + '\n')

            if bnum in self.free_blocks:  # unallocated block error
                output = "UNALLOCATED BLOCK < {} > REFERENCED BY".format(bnum)
                for node in sorted(block.referred):
                    output += self.unallocated_block_error(node[0], node[1], node[2])
                self.f.write(output + '\n')

        for inum in self.used_inodes:
            node = self.used_inodes[inum]
            gnum = int(inum / self.inode_per_group)
            link_count = len(node.referred)
            if inum > 10 and len(node.referred) == 0:
                self.missing_inode_error(inum, self.gnum_to_ibitmaps[gnum])
            elif link_count != node.link_count:
                self.incorrect_link_count_error(inum, node.link_count, link_count)

        for inum in self.error_inodes:
            node = self.error_inodes[inum]
            output = "UNALLOCATED INODE < {} > REFERENCED BY".format(inum)
            for dir in sorted(node.referred):
                output += self.unallocated_inode_error(dir[0], dir[1])
            self.f.write(output + '\n')

        for i in range(11, self.ninodes):
            if i not in self.free_inodes and i not in self.used_inodes:
                gnum = int(i / self.inode_per_group)
                self.missing_inode_error(i, self.gnum_to_ibitmaps[gnum])

if __name__ == "__main__":
    f = open('lab3b_check.txt', 'w')
    f.truncate()
    fsa = FSAnalyzer(f)
    fsa.read_all()
    fsa.sanity_check()
    f.close()

