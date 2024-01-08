#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "ext2_func.h"
#include "init.h"

int main() {
    struct ext2_inode current;
    printf("Authorized users only. All activities may be monitored and reported.\n");
    initialize_ext2(&current);
    if(login()){
        printf("Wrong password!\n");
        quit_display();
        return 1;
    }
    else printf("\n\n\n\n                  Welcome to Ext2 File System!                  \n\n\n\n");
    shell(current);
    return 0;
}

//get ESC to terminate write;
int getch(){
    int ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO,&oldt);//获取与终端相关的参数
    newt=oldt;
    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    ch=getchar();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    return ch;
}
//to modify password
int Password(){
    char password[12];
    char ch;
    printf("Please input the old password\n");
    scanf("%s",password);
    getchar();
    if(strcmp(password,groupDesc.bg_password)!=0){
        printf("False Password!\n");
        return 1;
    }
    while(1){
        printf("Please input the new password(12 words max):\n");
        scanf("%s",password);
        getchar();
        while(1){
            printf("Modify the password?[Y/N]");
            scanf("%c",&ch);
            getchar();
            if(ch=='N'||ch=='n'){
                printf("The modification has canceled!\n");
                return 1;
            }
            else if(ch=='Y'||ch=='y'){
                strcpy(groupDesc.bg_password,password);
                FILE *fp=NULL;
                while(fp==NULL) fp= fopen(PATH,"r+");
                fseek(fp,0,SEEK_SET);
                fwrite(&groupDesc,sizeof(struct ext2_group_desc),1,fp);//更新组描述符
                fclose(fp);
                return 0;
            }
            else{
                printf("error input!\n");
            }
        }
    }
}
//login
int login(){
    char password[12];
    printf("Please input your password:");
    scanf("%s",password);
    getchar();
    if(strcmp(groupDesc.bg_password,password)==0) return 0;
    else return 1;
}
//exit
void quit_display(){
    printf("This File System is closed successfully!\n");
}
//initialize current, move to root dir
int ret_root(struct ext2_inode *begin){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    fseek(fp,inode_begin_block*blocksize,SEEK_SET);//移动指针到索引结点开始处
    fread(begin,inodesize,1,fp);//读取一个索引到begin指针处
    fclose(fp);
    return 0;
}
//initialize FS
int initialize_ext2(struct ext2_inode *begin){
    FILE *fp=NULL;
    fp= fopen(PATH,"r+");
    while(fp==NULL){//未找到此文件
        char ch;
        printf("File system couldn't be found.Create one?[Y/N]\n");
        scanf("%c",&ch);
        getchar();//吃掉回车
        if(ch=='N'||ch=='n'){
            quit_display();
            return 1;
        }
        else if(ch=='Y'||ch=='y'){
            format();
            fp= fopen(PATH,"r+");
        }
        else{
            printf("error!\n");
        }
    }
    fseek(fp,0,SEEK_SET);
    fread(&groupDesc,sizeof(struct ext2_group_desc),1,fp);
    fseek(fp,inode_begin_block*blocksize,SEEK_SET);//索引结点开始处
    fread(&inode,inodesize,1,fp);
    fclose(fp);
    ret_root(begin);
    return 0;
}
//format FS
int format(){
    FILE *fp=NULL;
    int i;
    u_int32_t zero[blocksize/4]; //零数组，用来初始化块为0,一个元素占用四个字节
    memset(zero,0, sizeof(zero)); //初始化为0
    time_t now;
    time(&now);
    while(fp==NULL) fp= fopen(PATH,"w+");
    for(i=0;i<blocks;i++) {//初始化所有的4611块为0,所有内容都置为0
        fseek(fp,i*blocksize,SEEK_SET);//移动指针的位置
        fwrite(&zero, sizeof(zero),1,fp);//一次写入512个字节，全部置为0
    }

    //初始化组描述符
    strcpy(groupDesc.bg_volume_name,"my_ext2"); //初始化卷名
    groupDesc.bg_block_bitmap=1;//保存数据块位图的块号为1
    groupDesc.bg_inode_bitmap=2;//保存索引结点位图的块号为2
    groupDesc.bg_inode_table=3;//第一个索引结点表的块号
    groupDesc.bg_free_blocks_count=4095; //除去一个初始化目录
    groupDesc.bg_free_inodes_count=4095; //空闲索引结点减一
    groupDesc.bg_used_dirs_count=1; //一个初始化目录
    strcpy(groupDesc.bg_password,"123456");//将密码值初始化为123456
    fseek(fp,0*blocksize,SEEK_SET);//移回文件指针至最前面
    fwrite(&groupDesc, sizeof(struct ext2_group_desc),1,fp);//将组描述符放入第一个块号

    //初始化数据块位图和索引结点位图
    zero[0]=0x80000000; //十六进制一共4个字节，最高位为1，表示第一个数据块已经用了
    fseek(fp,1*blocksize,SEEK_SET); //第二块为数据块位图
    fwrite(&zero,sizeof(zero),1,fp);
    fseek(fp,2*blocksize,SEEK_SET); //第三块为索引结点位图
    fwrite(&zero,sizeof(zero),1,fp);

    //初始化索引结点表
    inode.i_mode=DIR_RW_MODE;//表示这是一个目录type为2
    inode.i_blocks=1;//刚开始所有的目录只占用一个数据块
    inode.i_size=dirsize*2; //所有目录文件加起来的文件大小
    inode.i_ctime=now;
    inode.i_atime=now;
    inode.i_mtime=now;
    inode.i_dtime=0;
    memset(inode.i_block,-1, sizeof(inode.i_block));//初始化索引指针为-1
    inode.i_block[0]=0;//第一个直接索引指向第一个数据块
    fseek(fp,3*blocksize,SEEK_SET);//移动文件指针到第四块
    fwrite(&inode,sizeof(struct ext2_inode),1,fp);//在第四块(索引结点表第一块)中写入一个索引结点

    //在第一个数据块中写入两个目录体
    //初始化当前目录 "."
    dir.inode=0; //表示是第一个索引结点号
    dir.rec_len= dirsize;//为了简单，所有的目录项长度都是固定的，为32个字节
    dir.name_len=1;//当前目录名为".",长度为1
    dir.file_type=2;//目录文件type为2
    strcpy(dir.name,".");
    fseek(fp,data_begin_block*blocksize,SEEK_SET);//移动文件指针到数据块开始处
    fwrite(&dir,dirsize,1,fp);//将当前目录写入数据块
    //初始化上一个目录 ".."
    dir.inode=0; //表示是第一个索引结点号
    dir.rec_len= dirsize;
    dir.name_len=2;
    dir.file_type=2;
    strcpy(dir.name,"..");
    fseek(fp,data_begin_block*blocksize+dirsize,SEEK_SET);//移动文件指针到上面刚写完的后面
    fwrite(&dir,dirsize,1,fp);//写入上一级目录
    fclose(fp);
    return 0;
}
//实现shell接口
void shell(struct ext2_inode current){
    char cmd[10];//用于存储用户输入的命令
    char msg_1;//用于读取用户输入的一位字符
    char msg_2[128];//用于读取用户输入的文件名等信息
    char path[10];//存储路径信息
    struct ext2_inode node;
    char current_dir[20];
    char hash_t[14][10]={"create","delete","cd","close","read","write","password","format","quit","ls","help","mkdir","rmdir","chmod"};

    //开始无限循环
    while(1){
        //根据当前指针所在获取当前目录
        get_dir_name(current_dir,current);

        //生成与命令匹配的序号值
        printf("\n[wzy@ext2fs %s]# ",current_dir);//显示命令接受字段，并显示当前所在目录
        scanf("%s",cmd);
        getchar();
        int i;
        for(i=0;i<14;i++){
            if(strcmp(cmd,hash_t[i])==0) break;//返回与hash_t匹配命令的序号值
        }

        //根据序号值选择相应函数执行
        if(i==0||i==1){//create/delete file

            scanf("%s",msg_2);
            getchar();

            //对创建和删除分别处理
            if(i==0){
                //反馈创建是否成功
                if(Create(1,&current,msg_2)==1) printf("%s can't be successfully created!\n",msg_2);
                else printf("%s is created successfully\n",msg_2);
            }
            else{
                //反馈删除是否成功
                if(Delete(1,&current,msg_2)==1) printf("%s can't be successfully deleted\n",msg_2);
                else printf("%s is deleted successfully\n",msg_2);
            }
        }
        else if(i==2){
            scanf("%s",msg_2);//读取用户输入的路径
            getchar();
            int p=0;
            int q=0;
            node=current;//保存当前的目录信息，以便发生错误可以返回原目录
            while(q<=strlen(msg_2)){
                path[p]=msg_2[q];//将读取到的信息按位存入路径path中
                if(path[p]=='/'){//当出现分隔符时
                    //第一个为/，表示要跳到根目录去
                    if(q==0) ret_root(&current);
                    else if(p==0){
                        printf("path input error!\n");
                        current=node;//回到原来的目录
                        break;
                    }
                    else{//已经读取完毕
                        path[p]='\0';
                        if(Open(&current,path)==1){
                            printf("path input error!\n");
                            current=node;//回到原来的目录
                            break;
                        }
                    }
                    p=0;//重置
                }
                else if(path[p]=='\0'){//读到最后一位
                    if(p==0) break;
                    if(Open(&current,path)==1){
                        printf("path input error!\n");
                        current=node;//回到原来的目录
                        break;
                    }
                }
                else p++;

                q++;
            }
        }
        else if(i==3){//close
            if(Close(&current)==1){
                printf("Something went wrong!\n");
                break;
            }           
        }
        else if(i==4){//read
            scanf("%s",msg_2);
            getchar();
            if(check_auth(&current,msg_2)==1||check_auth(&current,msg_2)==4){
                if(Read(&current,msg_2)==1) printf("Sorry! Failed read\n");
            }
            else {
                printf("Sorry! Insufficient permissions.\n");
            }
        }
        else if(i==5){//write
            scanf("%s",msg_2);
            getchar();
            if(check_auth(&current,msg_2)==0||check_auth(&current,msg_2)==1){
                if(Write(&current,msg_2)==1) printf("Sorry! Failed Written\n");
            }
            else{
                printf("Sorry! Insufficient permissions.\n");
            }
        }
        else if(i==6){//password
            Password();
        }
        else if(i==7){//format
            int flag=0;
            while(1){
                printf("FORMAT?\n");
                printf("[Y/N]: ");
                scanf("%c",&msg_1);//读取用户输入的单字符
                getchar();
                if(msg_1=='N'||msg_1=='n') break;
                else if(msg_1=='Y'||msg_1=='y'){
                    if(login()==1) {
                        flag=1;
                        break;
                    }
                    format();//调用format函数
                    ret_root(&current);//初始化索引指针到根目录
                    printf("Format success!\n");
                    break;
                } 
                else{
                    printf("Please input [Y/N]: \n");//输入了其他invalid内容
                }
            }
            if(flag==1) printf("Wrong password! Format Failed.");
        }
        else if(i==8){//quit
            quit_display();
            return;
        }
        else if(i==9){//ls
            Ls(&current);
        }
        else if(i==10){//help
            help();
        }
        else if(i==11||i==12){//mkdir/rmdir

            scanf("%s",msg_2);
            getchar();

            //对创建和删除分别处理
            if(i==11){
                //反馈创建是否成功
                if(Create(2,&current,msg_2)==1) printf("%s can't be successfully created!\n",msg_2);
                else printf("%s is created successfully\n",msg_2);
            }
            else{
                //反馈删除是否成功
                if(Delete(2,&current,msg_2)==1) printf("%s can't be successfully deleted\n",msg_2);
                else printf("%s is deleted successfully\n",msg_2);
            }
        }
        else if(i==13){
            scanf("%s",msg_2);
            getchar();
            char auth[10]={0};
            scanf("%s",auth);
            getchar();
            if(chmod(&current,msg_2,auth)==1) printf("Sorry! Modification is Failed or Not exist.\n");
        }
        else printf("invalid Command! \n");
    }
}
//传入type类型当前索引指针和文件名创建文件或目录
int Create(int type,struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    u_int32_t block_location;//块位置
    u_int32_t node_location;//索引结点位置
    u_int32_t dir_entry_location;//目录体位置

    struct ext2_inode anode;
    struct ext2_dir_entry aentry,bentry;
    time_t now;
    time(&now);
    for(int i=0;i<current->i_size/dirsize;i++){//搜索当前目录索引结点，查看是否存在相同的文件
        fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
        fread(&aentry,dirsize,1,fp);//读取目录体
        if((aentry.file_type==type)&&(strcmp(aentry.name,name)==0)) return 1;//有相同文件，退出
    }

    node_location=ext2_new_inode();//为新创建的文件或者目录寻找一个空的索引结点
    fseek(fp, dir_entry_position(0,current->i_block),SEEK_SET);
    fread(&bentry,dirsize,1,fp);//取出当前目录索引结点中指向的数据块中的"."的目录体
    if(type==1){//文件
        anode.i_mode=FILE_RWX_MODE;
        anode.i_size=0;//暂时没有内容
        anode.i_blocks=0;//没有为它分配数据块
        anode.i_atime=now;
        anode.i_ctime=now;
        anode.i_mtime=now;
        anode.i_dtime=0;
        memset(anode.i_block,-1,sizeof(anode.i_block));//全部初始化为-1，表示没有数据块指针
        for(int i=0;i<8;i++) anode.i_pad[i]=(char)0xff;//没有任何含义，只是为了填充
    }
    else{//创建目录
        anode.i_mode=DIR_RW_MODE;
        anode.i_blocks=1;//需要一个数据块来存放创建的目录，包括当前目录体和上一个目录体
        anode.i_size=dirsize*2;
        anode.i_atime=now;
        anode.i_ctime=now;
        anode.i_mtime=now;
        anode.i_dtime=0;
        memset(anode.i_block,-1,sizeof(anode.i_block));
        block_location=ext2_alloc_block();//申请一个空的数据块
        anode.i_block[0]=block_location;
        for(int i=0;i<8;i++) anode.i_pad[i]=(char)0xff;//没有任何含义，只是为了填充
        //表示当前目录，指向新创建的索引结点
        aentry.inode=node_location;
        aentry.rec_len= dirsize;
        aentry.name_len=1;
        aentry.file_type=2;
        strcpy(aentry.name,".");
        fseek(fp,data_begin_block*blocksize+block_location*blocksize,SEEK_SET);
        fwrite(&aentry,dirsize,1,fp);
        //表示上一级目录
        aentry.inode=bentry.inode;//指向当前目录索引结点(current)的"."目录体所在的索引结点
        aentry.rec_len=dirsize;
        aentry.name_len=2;
        aentry.file_type=2;
        strcpy(aentry.name,"..");
        fwrite(&aentry,dirsize,1,fp);
        //一个空条目,用于对剩下的空间进行格式化
        aentry.inode=0;//表示为空
        aentry.rec_len=dirsize;
        aentry.name_len=0;
        aentry.file_type=0;//不知道是目录还是文件
        strcpy(aentry.name,"");
        fwrite(&aentry,dirsize,14,fp);//一个数据块512个字节，一个目录体32个字节，一共有16个目录体，除去当前目录和上一级目录，还可以写14个目录体
    }
    //往索引结点表写入新的索引结点
    fseek(fp,inode_begin_block*blocksize+node_location*inodesize,SEEK_SET);
    fwrite(&anode,inodesize,1,fp);
    //往current索引结点指向的数据块写入目录体，表示是文件还是目录
    aentry.inode=node_location;
    aentry.rec_len=dirsize;
    aentry.name_len= strlen(name);
    if(type==1) aentry.file_type=1;
    else aentry.file_type=2;
    strcpy(aentry.name,name);
    fseek(fp, ext2_empty_dir(current),SEEK_SET);
    fwrite(&aentry,dirsize,1,fp);
    //更新current索引结点
    fseek(fp,inode_begin_block*blocksize+bentry.inode*inodesize,SEEK_SET);
    fwrite(current,inodesize,1,fp);
    fclose(fp);
    return 0;
}
//删除文件或者目录
int Delete(int type,struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    int dir_entry_location;//表示寻找到的目录体在当前目录中的相对位置
    struct ext2_inode delete_inode;
    struct ext2_dir_entry temp_entry,delete_entry,empty_entry;
    //一个空条目
    empty_entry.inode=0;
    empty_entry.rec_len=dirsize;
    empty_entry.name_len=0;
    strcpy(empty_entry.name, "");
    empty_entry.file_type=0;
    int i;
    for (i = 0; i < current->i_size/dirsize;i++) {
        fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
        fread(&delete_entry, dirsize, 1, fp);
        if((strcmp(delete_entry.name, name) == 0) && (delete_entry.file_type == type)){
            dir_entry_location=i;
            break;
        }
    }
    if(i<current->i_size/dirsize){
        fseek(fp,inode_begin_block*blocksize+delete_entry.inode*inodesize,SEEK_SET);
        fread(&delete_inode,inodesize,1,fp);
        //directory
        if(type==2){
            if(delete_inode.i_size>2*dirsize){
                //
                for (i = 0; i < delete_inode.i_size/dirsize;i++) {
                    fseek(fp, dir_entry_position(i*dirsize,delete_inode.i_block),SEEK_SET);
                    fread(&delete_entry, dirsize, 1, fp);
                    //递归删除
                    if(Delete(delete_entry.file_type,&delete_inode,delete_entry.name)==1){
                        printf("Failed!");
                        return 1;
                    }

                }
            }
            else{
                ext2_free_blocks(delete_inode.i_block[0]);
                ext2_free_inode(delete_entry.inode);
                current->i_size=current->i_size-dirsize;//从目录的数据块中删去一个目录体
                int last_entry_location= dir_entry_position(current->i_size,current->i_block);
                fseek(fp,last_entry_location,SEEK_SET);//找到当前目录的最后一个条目
                fread(&temp_entry,dirsize,1,fp);//读取最后一个条目
                fseek(fp,last_entry_location,SEEK_SET);//找到当前目录的最后一个条目
                fwrite(&empty_entry,dirsize,1,fp);//清空最后一个条目
                last_entry_location-=data_begin_block*blocksize;//在数据块中的绝对位置
                if(last_entry_location%blocksize==0){//如果最后一个条目刚好是一个数据块的起始位置，删除这个数据块
                    ext2_free_blocks(last_entry_location/blocksize);
                    current->i_blocks--;
                    if(current->i_blocks==6)//正好是一级子索引的第一个指针
                        ext2_free_blocks(current->i_block[6]);
                    else if(current->i_blocks==128+6){//正好是二级子索引
                        fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize,SEEK_SET);
                        int block_location;
                        fread(&block_location,sizeof(u_int32_t),1,fp);
                        ext2_free_blocks(block_location);
                        ext2_free_blocks(current->i_block[7]);
                    }
                    else if((current->i_blocks-128-6)%128==0){
                        fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize+((current->i_blocks-128-6)/128)*sizeof(u_int32_t),SEEK_SET);
                        int block_location;
                        fread(&block_location,sizeof(u_int32_t),1,fp);
                        ext2_free_blocks(block_location);
                    }
                }
                if(dir_entry_location*dirsize<current->i_size){//如果删除的条目不是最后一条，直接用最后一个条目覆盖,如果删除的是最后一个，也不用覆盖
                    fseek(fp, dir_entry_position(dir_entry_location*dirsize,current->i_block),SEEK_SET);
                    fwrite(&temp_entry,dirsize,1,fp);
                }
                printf("The %s is deleted!\n",name);
            }
        }
        //删除文件
        else{
            //删除直接指向的块
            for(int j=0;j<6;j++){
                if(delete_inode.i_blocks==0) break;
                ext2_free_blocks(delete_inode.i_block[j]);
                delete_inode.i_blocks--;
            }
            //删除一级索引中的数据块
            if(delete_inode.i_blocks>0){
                int block_location;
                fseek(fp,data_begin_block*blocksize+delete_inode.i_block[6]*blocksize,SEEK_SET);
                for(int j=0;j<128;j++){
                    if(delete_inode.i_blocks==0) break;
                    fread(&block_location,sizeof(u_int32_t),1,fp);
                    ext2_free_blocks(block_location);
                    delete_inode.i_blocks--;
                }
                ext2_free_blocks(delete_inode.i_block[6]);//删除一级索引
            }
            //二级索引
            if(delete_inode.i_blocks>0){
                int block_location_first;
                for(int j=0;j<128;j++){
                    fseek(fp,data_begin_block*blocksize+delete_inode.i_block[7]*blocksize+j*sizeof(u_int32_t),SEEK_SET);
                    fread(&block_location_first, sizeof(u_int32_t), 1, fp);
                    fseek(fp,data_begin_block*blocksize + block_location_first * blocksize, SEEK_SET);
                    int block_location_second;
                    for(int k=0;k<128;k++){
                        if(delete_inode.i_blocks==0) break;
                        fread(&block_location_second,sizeof(u_int32_t),1,fp);
                        ext2_free_blocks(block_location_second);
                        delete_inode.i_blocks--;
                    }
                    ext2_free_blocks(block_location_first);
                    if(delete_inode.i_blocks==0)
                        break;
                }
                ext2_free_blocks(delete_inode.i_block[7]);
            }
            ext2_free_inode(delete_entry.inode);
            int last_entry_location;
            current->i_size=current->i_size-dirsize;
            last_entry_location= dir_entry_position(current->i_size,current->i_block);//找到最后的条目
            fseek(fp,last_entry_location,SEEK_SET);
            fread(&temp_entry,dirsize,1,fp);
            fseek(fp,last_entry_location,SEEK_SET);
            fwrite(&empty_entry,dirsize,1,fp);
            last_entry_location-=data_begin_block*blocksize;
            if(last_entry_location%blocksize==0){//如果最后一个条目刚好是一个数据块的起始位置，删除这个数据块
                ext2_free_blocks(last_entry_location/blocksize);
                current->i_blocks--;
                if(current->i_blocks==6)//正好是一级子索引的第一个指针
                    ext2_free_blocks(current->i_block[6]);
                else if(current->i_blocks==128+6){//正好是二级子索引的第一个指针
                    fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize,SEEK_SET);
                    int block_location;
                    fread(&block_location,sizeof(u_int32_t),1,fp);
                    ext2_free_blocks(block_location);
                    ext2_free_blocks(current->i_block[7]);
                }
                else if((current->i_blocks-128-6)%128==0){
                    fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize+((current->i_blocks-128-6)/128)*sizeof(u_int32_t),SEEK_SET);
                    int block_location;
                    fread(&block_location,sizeof(u_int32_t),1,fp);
                    ext2_free_blocks(block_location);
                }
            }
            if(dir_entry_location*dirsize<current->i_size){//如果删除的条目不是最后一条，直接用最后一个条目覆盖,如果删除的是最后一个，也不用覆盖了
                fseek(fp, dir_entry_position(dir_entry_location*dirsize,current->i_block),SEEK_SET);
                fwrite(&temp_entry,dirsize,1,fp);
            }
        }
        fseek(fp,data_begin_block*blocksize+current->i_block[0]*blocksize,SEEK_SET);
        fread(&temp_entry,dirsize,1,fp);//表示当前目录
        fseek(fp,inode_begin_block*blocksize+temp_entry.inode*inodesize,SEEK_SET);
        fwrite(current,inodesize,1,fp);//更新current信息
    }
    else{
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}
//计算目录项在文件系统中的绝对位置
int dir_entry_position(int dir_entry_begin,u_int16_t i_block[8]){
    int dir_blocks=dir_entry_begin/512; //求在第几个数据块
    int block_offset=dir_entry_begin%512; //求在数据块内的偏移量
    FILE *fp=NULL;
    int dir_entry_position;
    //前六个直接索引块，第七个一级索引，第八个二级索引
    if(dir_blocks<=5) {
        dir_entry_position=data_begin_block*blocksize+i_block[dir_blocks]*blocksize+block_offset;//表示在数据块开始位置向后移动直接索引的数据块所在位置，在向后移动数据块内的偏移量
    }
    else{
        while(fp==NULL) fp= fopen(PATH,"r+");
        dir_blocks=dir_blocks-6;//超出了直接索引的范围，需要去一级子索引寻找，因此需要减去直接索引的范围
        if(dir_blocks<128){ //一级子索引最多能指向(512/4)=128个数据块，因为指针是int(4字节),一级子索引只用一个数据块(512字节),整个数据块中存放了128个指向其他数据块的指针
            int position;
            fseek(fp,data_begin_block*blocksize+i_block[6]*blocksize+dir_blocks*4,SEEK_SET);//寻找一级子索引中指向所需数据块的指针的位置
            fread(&position,sizeof(int),1,fp);
            dir_entry_position=data_begin_block*blocksize+position*blocksize+block_offset;
        }
        else {//超出一级子索引的范围，需要减去一级子索引的范围，二级子索引最多可寻找128*128个数据块
            int position_first;
            int position_second;
            dir_blocks=dir_blocks-128;
            fseek(fp,data_begin_block*blocksize+i_block[7]*blocksize+(dir_blocks/128)*4,SEEK_SET);
            fread(&position_first, sizeof(int), 1, fp);
            fseek(fp,data_begin_block*blocksize + position_first * blocksize + (dir_blocks % 128) * 4, SEEK_SET);
            fread(&position_second, sizeof(int), 1, fp);
            dir_entry_position=data_begin_block*blocksize + position_second * blocksize + block_offset;
        }
        fclose(fp);
    }
    return dir_entry_position;
}
//传入当前的索引结点指针和文件名，寻找文件是否存在
int Open(struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL){
        fp= fopen(PATH,"r+");
    }
    for(int i=0;i<(current->i_size/32);i++) {//一个目录数据结构的大小为32个字节，计算一共有多少个目录体
        fseek(fp, dir_entry_position(i*32,current->i_block),SEEK_SET);//定位当前目录索引结点指向的数据块中第i个目录体
        fread(&dir,sizeof(struct ext2_dir_entry),1,fp);
        if(strcmp(dir.name,name)==0){
            if(dir.file_type==2){//查找类型为目录的文件
                fseek(fp,inode_begin_block*blocksize+dir.inode*sizeof(struct ext2_inode),SEEK_SET);//从第一个索引结点出发，定位该目录所在的索引结点
                fread(current,sizeof(struct ext2_inode),1,fp);
                fclose(fp);
                return 0;//表示成功找到
            }
        }
    }
    fclose(fp);
    return 1;//表示查找失败
}
//打开上一级目录并实现了关闭当前目录
int Close(struct ext2_inode *current){
    time_t now;
    struct ext2_dir_entry bentry;
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    time(&now);
    current->i_atime=now;
    fseek(fp, dir_entry_position(0,current->i_block),SEEK_SET);//寻找当前目录索引结点指向的第一个数据块中的第一个目录体
    fread(&bentry,sizeof(struct ext2_dir_entry),1,fp);//第一个目录体就是当前目录"."，里面存放了当前目录索引结点的位置
    fseek(fp,inode_begin_block*blocksize+bentry.inode*sizeof(struct ext2_inode),SEEK_SET);//寻找当前目录所在的索引结点在硬盘的位置
    fwrite(current,sizeof(struct ext2_inode),1,fp);
    fclose(fp);
    return Open(current,"..");//打开上一级目录
}
//在给定目录下读取文件内容
int Read(struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    for(int i=0;i<(current->i_size/32);i++){//其实和Open函数很像，只是要的是文件
        fseek(fp, dir_entry_position(i*32,current->i_block),SEEK_SET);
        fread(&dir,sizeof(struct ext2_dir_entry),1,fp);
        if(strcmp(dir.name,name)==0){
            if(dir.file_type==1){
                time_t now;
                struct ext2_inode node;
                char content;
                fseek(fp,inode_begin_block*blocksize+dir.inode*inodesize,SEEK_SET);
                fread(&node,sizeof(struct ext2_inode),1,fp);
                for(int j=0;j<node.i_size;j++){
                    fseek(fp, dir_entry_position(j,node.i_block),SEEK_SET);//每次都找一个字节，我感觉这样的效率不高，可以优化一下
                    fread(&content,sizeof(char),1,fp);
                    if(content==0x0D)//0x0D（asc码是13） 指的是“回车”   \r是把光标置于本行行首
                        printf("\n");
                    else
                        printf("%c",content);
                }
                printf("\n");
                time(&now);
                node.i_atime=now;
                fseek(fp,inode_begin_block*blocksize+dir.inode*inodesize,SEEK_SET);
                fwrite(&node, inodesize,1,fp);//更新原来的索引结点
                fclose(fp);
                return 0;
            }
        }
    }
    fclose(fp);
    return 1;
}
//搜索 inode 位图找到第一个未分配的索引结点，更新相关的位图和描述符，并返回该索引结点的位置
u_int32_t ext2_new_inode(){
    FILE *fp=NULL;
    u_int32_t zero[blocksize/4];
    while(fp==NULL) fp= fopen(PATH,"r+");
    fseek(fp,2*blocksize,SEEK_SET); //索引结点位图
    fread(zero,blocksize,1,fp); //zero中存储索引结点位图
    for(u_int32_t i=last_allco_inode;i<(last_allco_inode+blocksize/4);i++) {//从上一个分配的结点开始搜索
        if(zero[i%(blocksize/4)]!=0xffffffff){
            u_int32_t j=0x80000000,k=zero[i%(blocksize/4)];
            //找到第一个空的索引结点
            for(u_int32_t p=0;p<32;p++){
                if((j&k)==0x00000000){//如果为0，表示偏移量为p的索引结点是空的
                    zero[i%(blocksize/4)]=zero[i%blocksize/4]|j;//更新inode位图，添加一个1

                    groupDesc.bg_free_inodes_count--;//可分配的索引结点减一
                    fseek(fp,0,SEEK_SET);
                    fwrite(&groupDesc, sizeof(struct ext2_group_desc),1,fp);//更新组描述符

                    fseek(fp,2*blocksize,SEEK_SET);
                    fwrite(zero,blocksize,1,fp);//更新inode位图
                    last_allco_inode=i%(blocksize/4);
                    fclose(fp);
                    return (i%(blocksize/4))*32+p;//返回索引结点的位置
                }
                else
                    j=j/2;
            }
        }
    }
    fclose(fp);
    return -1;//没有找到空的索引结点
}
//用于查找并返回一个未分配的数据块的位置
u_int32_t ext2_alloc_block(){
    FILE *fp=NULL;
    u_int32_t zero[blocksize/4];
    while(fp==NULL) fp= fopen(PATH,"r+");
    fseek(fp,1*blocksize,SEEK_SET);
    fread(zero, blocksize,1,fp);
    for(u_int32_t i=last_allco_block;i<(last_allco_block+blocksize/4);i++) {//从上一个分配的结点开始搜索，并且会循环搜索，最后一个找不到会重新回到第一个元素开始搜索
        if(zero[i%(blocksize/4)]!=0xffffffff){//最多有4096个数据块,zero数组每一个元素能存储32个数据块的信息(有32位),表示当前的32个有空的数据块
            u_int32_t j=0x80000000,k=zero[i%(blocksize/4)];
            //找到第一个空的数据块
            for(u_int32_t p=0;p<32;p++){
                if((j&k)==0x00000000){//如果为0，表示偏移量为p的数据块是空的
                    zero[i%(blocksize/4)]=zero[i%blocksize/4]|j;//更新block位图，添加一个1
                    groupDesc.bg_free_blocks_count--;//可分配的数据块减一
                    fseek(fp,0,SEEK_SET);
                    fwrite(&groupDesc, sizeof(struct ext2_group_desc),1,fp);//更新组描述符

                    fseek(fp,1*blocksize,SEEK_SET);
                    fwrite(zero,blocksize,1,fp);//更新block位图
                    last_allco_block=i%(blocksize/4);
                    fclose(fp);
                    return (i%(blocksize/4))*32+p;//返回数据块的位置
                }
                else j=j/2;//算术向右移位，唯一的1向右移位一次
            }
        }
    }
    fclose(fp);
    return -1;//没有找到空的数据块
}
//用于删除指定位置的索引结点
void ext2_free_inode(int position){
    u_int32_t zero[blocksize/4];
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    fseek(fp,2*blocksize,SEEK_SET);//将文件指针先指向索引结点位图
    fread(zero,blocksize,1,fp);
    u_int32_t j=0x80000000;
    //定位到偏移量为position%32的位置
    for(int i=0;i<position%32;i++) j=j/2;
    zero[position/32]=zero[position/32]^j;//异或操作，会将对应的1置0，其他位不受影响
    fseek(fp,2*blocksize,SEEK_SET);
    fwrite(zero,blocksize,1,fp);
    fclose(fp);
}
//用于删除指定位置的数据块
void ext2_free_blocks(int position){
    u_int32_t zero[blocksize/4];
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    fseek(fp,1*blocksize,SEEK_SET);
    fread(zero,blocksize,1,fp);
    u_int32_t j=0x80000000;
    //定位到偏移量为position%32的位置
    for(int i=0;i<position%32;i++) j=j/2;
    zero[position/32]=zero[position/32]^j;//异或操作，会将对应的1置0，其他位不受影响
    fseek(fp,1*blocksize,SEEK_SET);
    fwrite(zero,blocksize,1,fp);
    fclose(fp);
}
//用于为给定的索引结点分配一个数据块
void Add_block(struct ext2_inode *current,int block_number){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    //直接索引
    if(block_number<6) current->i_block[block_number]=ext2_alloc_block();
    else{
        block_number=block_number-6;
        if(block_number==0){
            current->i_block[6]=ext2_alloc_block();//为一级子索引找一个空的数据块来存放指针，最多可以存放512/4=128个
            fseek(fp,data_begin_block*blocksize+current->i_block[6]*blocksize,SEEK_SET);
            u_int32_t empty_block=ext2_alloc_block();
            fwrite(&empty_block, sizeof(u_int32_t),1,fp);//往数据块里写入第一个指针
        }
        else if(block_number<128){//在一级子索引指向的第一个数据块写指针
            u_int32_t empty_block=ext2_alloc_block();
            fseek(fp,data_begin_block*blocksize+current->i_block[6]*blocksize+block_number* sizeof(u_int32_t),SEEK_SET);
            fwrite(&empty_block, sizeof(u_int32_t),1,fp);
        }
        else{//二级索引
            block_number=block_number-128;
            if(block_number==0){//写入二级索引的第一个数据块指针
                current->i_block[7]=ext2_alloc_block();
                fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize,SEEK_SET);
                u_int32_t empty_block=ext2_alloc_block();
                fwrite(&empty_block,sizeof(u_int32_t),1,fp);//往数据块写入第一个第二级指针，一共可以写入128个第二级指针
                fseek(fp,data_begin_block*blocksize+empty_block*blocksize,SEEK_SET);
                empty_block=ext2_alloc_block();
                fwrite(&empty_block,sizeof(u_int32_t),1,fp);//第一级索引的第一个指针->第一个二级索引->第一个数据块指针
            }
            else if(block_number%128==0){//需要更新一个二级索引，顺便放一个数据块指针
                fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize+(block_number/128)*4,SEEK_SET);
                u_int32_t empty_block=ext2_alloc_block();
                fwrite(&empty_block,sizeof(u_int32_t),1,fp);//写入一个二级索引指针
                fseek(fp,data_begin_block*blocksize+empty_block*blocksize,SEEK_SET);
                empty_block=ext2_alloc_block();
                fwrite(&empty_block,sizeof(u_int32_t),1,fp);//往数据块写入一个数据块指针
            }
            else{//往二级索引的地方写入新的数据块指针
                fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize+(block_number/128)*4,SEEK_SET);
                u_int32_t block_read;
                fread(&block_read,sizeof(u_int32_t),1,fp);
                fseek(fp,data_begin_block*blocksize+block_read*blocksize+(block_number%128)*4,SEEK_SET);
                u_int32_t empty_block=ext2_alloc_block();
                fwrite(&empty_block,sizeof(u_int32_t),1,fp);
            }
        }
    }
}
//查找给定索引结点中的下一个空目录项的位置
int ext2_empty_dir(struct ext2_inode *current){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    int location;//目录体的绝对地址
    u_int32_t block_location;
    int temp=blocksize/sizeof(u_int32_t);//每个block可以存放的指针数量
    if(current->i_size%blocksize==0){//当前的文件大小正好占用了所有可用的数据块，因为目录体占用32个字节，一个数据块最多存放512/32=16个目录体，超出就要申请新的数据块
        Add_block(current,current->i_blocks);//申请一个新的数据块
        current->i_blocks++;
    }
    if((current->i_blocks-1)<6){//前六个直接索引
        location=data_begin_block*blocksize+current->i_block[current->i_blocks-1]*blocksize+current->i_size%blocksize;
    }
    else if((current->i_blocks-1-6)<128){//一级索引
        fseek(fp,data_begin_block*blocksize+current->i_block[6]*blocksize+(current->i_blocks-1-6)*sizeof(u_int32_t),SEEK_SET);
        fread(&block_location,sizeof(u_int32_t),1,fp);
        location=data_begin_block*blocksize+block_location*blocksize+current->i_size%blocksize;
    }
    else{//二级索引 
        int remain_block;//剩余块数
        remain_block=current->i_blocks-1-6-128;
        fseek(fp,data_begin_block*blocksize+current->i_block[7]*blocksize+(remain_block/128)*sizeof(u_int32_t),SEEK_SET);//首先寻找第一级索引指针的位置
        fread(&block_location,sizeof(u_int32_t),1,fp);
        remain_block=remain_block%128;//求偏移量
        fseek(fp,data_begin_block*blocksize+block_location*blocksize+remain_block*sizeof(u_int32_t),SEEK_SET);
        fread(&block_location,sizeof(u_int32_t),1,fp);
        location=data_begin_block*blocksize+block_location*blocksize+current->i_size%blocksize;
    }
    current->i_size=current->i_size+dirsize;//文件大小加一个目录体的大小
    fclose(fp);
    return location;
}
//向给定文件写入内容
int Write(struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    struct ext2_dir_entry dirEntry;
    struct ext2_inode node;
    time_t now;
    char str;
    while(1){
        int i;
        for(i=0;i<(current->i_size/32);i++){//遍历所有的目录体
            fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
            fread(&dirEntry,dirsize,1,fp);
            if(strcmp(dirEntry.name,name)==0){
                if(dirEntry.file_type==1){//是文件类型
                    fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
                    fread(&node,inodesize,1,fp);
                    break;
                }
            }
        }
        if(i<(current->i_size/32))//有文件存在
            break;
        printf("There isn't this file,please create it first\n");
        return 1;
    }
    str=getch();
    while(str!=27){
        printf("%c",str);
        if((node.i_size%blocksize==0)||(node.i_size==0)){//写入的数据占满一个数据块了，需要申请新的数据块;或者刚开始需要一个数据块
            Add_block(&node,(node.i_size)/blocksize);
            node.i_blocks++;
        }
        fseek(fp, dir_entry_position(node.i_size,node.i_block),SEEK_SET);
        fwrite(&str,sizeof(char),1,fp);
        node.i_size=node.i_size+sizeof(char);
        if(str==0x0d) printf("%c",0x0a);
        str=getch();
        if(str==27) break;
    }
    time(&now);
    node.i_mtime=now;
    node.i_atime=now;
    fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);//更新索引结点信息
    fwrite(&node,inodesize,1,fp);
    fclose(fp);
    printf("\n");
    return 0;
}
//列出当前目录内容
void Ls(struct ext2_inode *current){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    struct ext2_dir_entry dirEntry;
    struct ext2_inode node;
    char time[150];
    printf("Type\t\tAuthorization\t\tFileName\tCreateTime\t\t\tLastAccessTime\t\t\tModifyTime\n");
    for(int i=0;i<(current->i_size/dirsize);i++){
        fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
        fread(&dirEntry,dirsize,1,fp);
        fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
        fread(&node,inodesize,1,fp);
        strcpy(time,"");
        strcat(time, asctime(localtime((const time_t *) &node.i_ctime)));
        strcat(time, asctime(localtime((const time_t *) &node.i_atime)));
        strcat(time, asctime(localtime((const time_t *) &node.i_mtime)));
        for(int j=0;j<strlen(time)-1;j++){
            if(time[j]=='\n')
                time[j]='\t';
        }
        //uint16_t j=0x04;
        char ch[]={'d','r','w','x','-','-','-','-','-','-','\0'};
        if(node.i_mode==262||node.i_mode==518) {
            ch[3]='-';
        }
        else if(node.i_mode==261||node.i_mode==517){
            ch[2]='-';
        }
        else if(node.i_mode==260||node.i_mode==516){
            ch[3]='-';
            ch[2]='-';
        }
        else if(node.i_mode==259||node.i_mode==515){
            ch[1]='-';
        }
        else if(node.i_mode==258||node.i_mode==514){
            ch[1]='-';
            ch[3]='-';
        }
        else if(node.i_mode==257||node.i_mode==513){
            ch[1]='-';
            ch[2]='-';
        }
        //printf("%d\n",node.i_mode);
        if(dirEntry.file_type==1) {
            ch[0]='-';
            printf("File\t\t%s\t\t%s\t\t%s",ch,dirEntry.name,time);
        }
        else printf("Directory\t%s\t\t%s\t\t%s",ch,dirEntry.name,time);
    }
    fclose(fp);
}
//修改文件权限
int chmod(struct ext2_inode *current,char *name,char *auth){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    struct ext2_dir_entry dirEntry;
    struct ext2_inode node;
    time_t now;
    int i;
    for(i=0;i<(current->i_size/dirsize);i++){
        fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
        fread(&dirEntry,dirsize,1,fp);
        // fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
        // fread(&node,inodesize,1,fp);
        if(strcmp(dirEntry.name,name)==0){
            //if(dirEntry.file_type==1){//是文件类型
            fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
            fread(&node,inodesize,1,fp);
            break;
        }
    }
    if(i>=(current->i_size/32)) return 1;
    uint16_t j=0x00;

    if(auth[1]=='r') j=j|0b0000000000000100;
    if(auth[2]=='w') j=j|0b0000000000000010;
    if(auth[3]=='x') j=j|0b0000000000000001;

    if(dirEntry.file_type==1){
        j=j|0b0000000100000000;
        node.i_mode=j;
    }
    else{
        j=j|0b0000001000000000;
        node.i_mode=j;
    }
    //printf("%d\n",node.i_mode);
    time(&now);
    node.i_mtime=now;
    node.i_atime=now;
    fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);//更新索引结点信息
    fwrite(&node,inodesize,1,fp);
    printf("The authorization has been modified!\n");
    fclose(fp);
    return 0;
}
//获取该目录在上一级目录中的名称
void get_dir_name(char *name,struct ext2_inode node){
    struct ext2_dir_entry entry;
    struct ext2_inode current=node;
    u_int16_t i_node;
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    //current指向上一个目录
    Open(&current,"..");
    //在当前目录中搜索 "."目录体，记录所在的索引号
    for(int i=0;i<node.i_size/32;i++){
        fseek(fp, dir_entry_position(i*dirsize,node.i_block),SEEK_SET);
        fread(&entry,dirsize,1,fp);
        if(strcmp(entry.name,".")==0){
            i_node=entry.inode;
            break;
        }
    }
    //在上一级目录搜索索引号为i_node的目录体，获得当前目录的名字
    for(int i=0;i<current.i_size/32;i++){
        fseek(fp, dir_entry_position(i*dirsize,current.i_block),SEEK_SET);
        fread(&entry,dirsize,1,fp);
        if(entry.inode==i_node){
            strcpy(name,entry.name);
            return;
        }
    }
}
//检查文件的读写权限
int check_auth(struct ext2_inode *current,char *name){
    FILE *fp=NULL;
    while(fp==NULL) fp= fopen(PATH,"r+");
    struct ext2_dir_entry dirEntry;
    struct ext2_inode node;
    time_t now;
    int i;
    for(i=0;i<(current->i_size/dirsize);i++){
        fseek(fp, dir_entry_position(i*dirsize,current->i_block),SEEK_SET);
        fread(&dirEntry,dirsize,1,fp);
        // fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
        // fread(&node,inodesize,1,fp);
        if(strcmp(dirEntry.name,name)==0){
            //if(dirEntry.file_type==1){//是文件类型
            fseek(fp,inode_begin_block*blocksize+dirEntry.inode*inodesize,SEEK_SET);
            fread(&node,inodesize,1,fp);
            break;
        }
    }
    //read-1
    //write-0
    //read+write-4
    //not found-3
    //none-2
    if(i>=(current->i_size/32)) return 3;//not found
    if(node.i_mode==263||node.i_mode==519){//rwx
        return 4;
    }
    else if(node.i_mode==262||node.i_mode==518){//rw-
        return 4;
    }
    else if(node.i_mode==261||node.i_mode==517){//r-x
        return 1;
    }
    else if(node.i_mode==260||node.i_mode==516){//r--
        return 1;
    }
    else if(node.i_mode==259||node.i_mode==515){//-wx
        return 0;
    }
    else if(node.i_mode==258||node.i_mode==514){//-w-
        return 0;
    }
    else if(node.i_mode==257||node.i_mode==513){//--x
        return 2;
    }
    fclose(fp);
}
//提供help
void help(){
    printf("------------------------commands--------------------------\n\n");
    printf("There are 14 commands you can choose as follows:\n");
    printf("create: create [File name]\n");
    printf("Create a file or directory whose name need to be provided.\n\n");
    printf("delete: delete [File name]\n");
    printf("Delete a file or directory whose name need to be provided.\n\n");
    printf("cd: cd [Path]\n");
    printf("Enter the directory of the given path.\n\n");
    printf("close: close [none]\n");
    printf("Close the current directory\n\n");
    printf("read: read [File name]\n");
    printf("Read the file content of the file name given.\n\n");
    printf("write: write [File name]\n");
    printf("Write content to the file with the given file name.\n\n");
    printf("password: password [none]\n");
    printf("Modify your password.\n\n");
    printf("format: format [none]\n");
    printf("Format this File System.\n\n");
    printf("quit: quit [none]\n");
    printf("Quit this File System.\n\n");
    printf("ls: ls [none]\n");
    printf("List all the contents under the current directory.\n\n");
    printf("help: help [none]\n");
    printf("Provide help.\n");
    printf("mkdir: mkdir [File name]\n");
    printf("Create a directory whose name need to be provided.\n\n");
    printf("rmdir: rmdir [File name]\n");
    printf("Delete a directory whose name need to be provided.\n\n");
    printf("chmod: chmod [File name] [drwxdrwxdrwx]\n");
    printf("Change the rights of a File or directory\n\n");
    printf("------------------------commands--------------------------\n");
}