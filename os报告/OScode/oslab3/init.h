#ifndef _INIT_H_
#define _INIT_H_

#include <stdint.h>

#define blocks 4611 //1+1+1+512+4096
#define blocksize 512 //每块字节数
#define inodesize 64 //索引长度
#define data_begin_block 515 //数据开始块，1+1+1+512
#define inode_begin_block 3 //索引结点开始块，1+1+1
#define dirsize sizeof(struct ext2_dir_entry)
#define PATH "FS.txt" 

//不考虑组和其他用户，只考虑所有者的权限
#define DIR_R_MODE 0b0000001000000100
#define DIR_W_MODE 0b0000001000000010
#define DIR_RW_MODE 0b0000001000000110

#define FILE_R_MODE 0b0000000100000100
#define FILE_W_MODE 0b0000000100000010
#define FILE_X_MODE 0b0000000100000001
#define FILE_RW_MODE 0b0000000100000110
#define FILE_RWX_MODE 0b0000000100000111


struct ext2_group_desc{
    char bg_volume_name[16]; //卷名
    uint16_t bg_block_bitmap; //保存块位图的块号
    uint16_t bg_inode_bitmap; //保存索引结点位图的块号
    uint16_t bg_inode_table; //索引结点表的起始块号
    uint16_t bg_free_blocks_count; //本组空闲块的个数
    uint16_t bg_free_inodes_count; //本组空闲索引结点的个数
    uint16_t bg_used_dirs_count; //本组目录的个数
    char bg_password[12];//填充密码
};


struct ext2_inode{
    

    uint16_t i_mode; //文件类型及访问权限,
    uint16_t i_blocks; //文件的数据块个数
    
    uint32_t i_size; //文件大小(字节)
    uint64_t i_atime; //访问时间
    uint64_t i_ctime; //创建时间
    uint64_t i_mtime; //修改时间
    uint64_t i_dtime; //删除时间
    uint16_t i_block[8]; //指向数据块的指针
    char i_pad[8]; //填充1(0xff)
};
#define MAX_NAME_LEN 25

struct ext2_dir_entry{
    uint16_t inode; //索引结点号
    uint16_t rec_len; //目录项长度
    uint8_t name_len; //文件名长度
    uint8_t file_type; //文件类型
    char name[MAX_NAME_LEN]; //文件名
};
#endif