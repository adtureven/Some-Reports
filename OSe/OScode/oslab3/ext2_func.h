#ifndef _EXT2_FUNC_H
#define _EXT2_FUNC_H

#include "init.h"
#include <stdint.h>


struct ext2_group_desc groupDesc; //组描述符
struct ext2_inode inode;   //索引结点
struct ext2_dir_entry dir; //目录体
FILE *file; //文件指针
u_int32_t last_allco_inode=0; //上次分配的索引结点号所在的元素位置
u_int32_t last_allco_block=0; //上次分配的数据块号

int check_auth(struct ext2_inode *current,char *name);
int chmod(struct ext2_inode *current,char *name,char *auth);
int getch();
int Password();
int login();
void quit_display();
int ret_root(struct ext2_inode *begin);
int initialize_ext2(struct ext2_inode *begin);
int format();
void shell(struct ext2_inode current);
int Create(int type,struct ext2_inode *current,char *name);
int Delete(int type,struct ext2_inode *current,char *name);
int dir_entry_position(int dir_entry_begin,u_int16_t i_block[8]);
int Open(struct ext2_inode *current,char *name);
int Close(struct ext2_inode *current);
int Read(struct ext2_inode *current,char *name);
u_int32_t ext2_new_inode();
u_int32_t ext2_alloc_block();
void ext2_free_inode(int position);
void ext2_free_blocks(int position);
void Add_block(struct ext2_inode *current,int block_number);
int ext2_empty_dir(struct ext2_inode *current);
int Write(struct ext2_inode *current,char *name);
void Ls(struct ext2_inode *current);
void get_dir_name(char *name,struct ext2_inode node);
void help();


#endif