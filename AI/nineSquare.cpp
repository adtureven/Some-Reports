#include <iostream>
#include <cstring>
#include <chrono>

using namespace std;

typedef struct nineSquare{
    int nine[3][3];
    int dp;
    struct nineSquare *n;
}nS;

nS open[1000000];
nS close[1000000];
int otop=1;
int oinr=1;
int obot=0;
int ctop=0;
//check overflow
int over(){
    if(otop==999999) {
        cout<<"overflow"<<endl;
        return 1;
    }
    return 0;
}
//print the result
void disp(int a[3][3]){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(a[i][j]!=0) cout<<a[i][j]<<" ";
            else cout<<"  ";
        }
        cout<<endl;
    }
}
//check the array
int check(int a[3][3]){
    if(a[0][0]!=1||a[0][1]!=2||a[0][2]!=3) return 1;
    if(a[1][0]!=8||a[1][1]!=0||a[1][2]!=4) return 1;
    if(a[2][0]!=7||a[2][1]!=6||a[2][2]!=5) return 1;
    return 0;
}
//find the location of 0
int find_zero(int a[3][3]){
    int lo=0;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(a[i][j]==0) return lo;
            lo++;
        }
    }
    return lo;
}
//swap
void exchange(int i,int j,int a[3][3]){
    int t=a[i/3][i%3];
    a[i/3][i%3]=a[j/3][j%3];
    a[j/3][j%3]=t;
}
//BFS
void bfs(nS a){
    int zero=find_zero(a.nine);
    if((zero%3)!=0){//left ext
        exchange(zero-1,zero,a.nine);
        open[otop++]=a;
        if(over()) return ;
        exchange(zero-1,zero,a.nine);
    }
    if(zero>=3){//up ext
        exchange(zero-3,zero,a.nine);
        open[otop++]=a;
        if(over()) return ;
        exchange(zero-3,zero,a.nine);
    }
    if(zero<=5){//down ext
        exchange(zero+3,zero,a.nine);
        open[otop++]=a;
        if(over()) return ;
        exchange(zero+3,zero,a.nine);
    }
    if((zero+1)%3 != 0){//right ext
        exchange(zero+1,zero,a.nine);
        open[otop++]=a;
        if(over()) return ;
        exchange(zero+1,zero,a.nine);
    }
}
//DFS
void dfs(nS a){
    int zero=find_zero(a.nine);
    if((zero%3)!=0){//left ext
        exchange(zero-1,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero-1,zero,a.nine);
    }
    if(zero>=3){//up ext
        exchange(zero-3,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero-3,zero,a.nine);
    }
    if(zero<=5){//down ext
        exchange(zero+3,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero+3,zero,a.nine);
    }
    if((zero+1)%3 != 0){//right ext
        exchange(zero+1,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero+1,zero,a.nine);
    }
}
//Astar
void astar(nS a){
    int zero=find_zero(a.nine);
    if((zero%3)!=0){//left ext
        exchange(zero-1,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero-1,zero,a.nine);
    }
    if(zero>=3){//up ext
        exchange(zero-3,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero-3,zero,a.nine);
    }
    if(zero<=5){//down ext
        exchange(zero+3,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero+3,zero,a.nine);
    }
    if((zero+1)%3 != 0){//right ext
        exchange(zero+1,zero,a.nine);
        a.dp++;
        open[otop++]=a;
        a.dp--;
        if(over()) return ;
        exchange(zero+1,zero,a.nine);
    }
}
//same
int cmp(int a[3][3],int b[3][3]){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(a[i][j]!=b[i][j]) return 1;
        }
    }
    return 0;
}
//cost
int cost(nS a){
    int num=0;
    if(a.nine[0][0]!=1&&a.nine[0][0]!=0) num++;
    if(a.nine[0][1]!=2&&a.nine[0][1]!=0) num++;
    if(a.nine[0][2]!=3&&a.nine[0][2]!=0) num++;
    if(a.nine[1][0]!=8&&a.nine[1][0]!=0) num++;
    if(a.nine[1][1]!=0) num++;
    if(a.nine[1][2]!=4&&a.nine[1][2]!=0) num++;
    if(a.nine[2][0]!=7&&a.nine[2][0]!=0) num++;
    if(a.nine[2][1]!=6&&a.nine[2][1]!=0) num++;
    if(a.nine[2][2]!=5&&a.nine[2][2]!=0) num++;

    return num+a.dp;
}
//show path
void path(){
    nS *b=&open[obot];
    nS *c[1000];
    int i=0;
    while(1){
        c[i++]=b;
        if(b->n==NULL) break;
        b=b->n;
    }
    for(int j=i-1;j>0;j--){
        disp(c[j]->nine);
        cout<<"  ↓   "<<endl;
    }
    disp(c[0]->nine);
}
//astar find the minist 
int index(nS a[],int at,nS b[],int bt){
    int flag[10000]={0};
    for(int i=0;i<at;i++){
        for(int j=0;j<bt;j++){
            if(cmp(a[i].nine,b[j].nine)==0){
                flag[i]=1;
                break;
            }
        }
    }
    int min=100;
    int index=0;
    for(int i=0;i<at;i++){
        if(flag[i]==0){
            if(cost(a[i])<min){
                min=cost(a[i]);
                index=i;
            }
        }
    }
    return index;
}

int main(){

    nS a;
    a.n=NULL;
    a.dp=0;

    a.nine[0][0]=2;
    a.nine[0][1]=8;
    a.nine[0][2]=3;
    a.nine[1][0]=1;
    a.nine[1][1]=0;
    a.nine[1][2]=4;
    a.nine[2][0]=7;
    a.nine[2][1]=6;
    a.nine[2][2]=5;

    if(check(a.nine)==0){
        cout<<"your input need no operation"<<endl;
        return 0;
    }

    //cout<<cost(a)<<endl;;

    cout<<"select a algorithm: "<<endl;
    char al[3][10]={"bfs","dfs","astar"};
    char p[10];
    cin>>p;
    int i=0;
    for(i=0;i<3;i++){
        if(strcmp(p,al[i])==0) break;
    }


    open[0]=a;
    open[0].dp=0;
    open[0].n=NULL;

    auto start_time = chrono::high_resolution_clock::now();

    if(i==0){//bfs
        while(check(open[obot].nine)==1){
            int flag=0;
            for(int i=0;i<ctop;i++){
                if(cmp(open[obot].nine,close[i].nine)==0){
                    obot++;
                    flag=1;
                    break;
                }      
            }

            if(flag==1) continue;

            close[ctop]=open[obot];
            obot++;      
            bfs(close[ctop]);
            if(over()) return 1;
            for(int i=oinr;i<otop;i++){
                open[i].n=&close[ctop];
            }
            ctop++;
            oinr=otop;
        }
    }
    else if(i==1){//dfs
        while(check(open[obot].nine)==1&&otop!=ctop){
            int flag=0;
            for(int i=0;i<ctop;i++){
                if(cmp(open[obot].nine,close[i].nine)==0){
                    obot--;
                    flag=1;
                    break;
                }      
            }
            if(flag==1) continue;

            if(open[obot].dp==5){
                close[ctop++]=open[obot--];
                flag=1;
            }
            if(flag==1) continue;

            close[ctop]=open[obot];
            obot--;      
            dfs(close[ctop]);
            if(over()) return 1;
            for(int i=oinr;i<otop;i++){
                open[i].n=&close[ctop];
            }
            ctop++;
            oinr=otop;
            obot=otop-1;
        }
    }
    else{//astar
        while(check(open[obot].nine)==1&&otop!=ctop){

            close[ctop]=open[obot];   

            astar(close[ctop]);

            if(over()) return 1;
            for(int i=oinr;i<otop;i++){
                open[i].n=&close[ctop];
            }

            ctop++;
            oinr=otop;

            obot=index(open,otop,close,ctop);

        }
    }
    cout<<ctop<<" extended"<<endl;
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "Algorithm executed in " << duration.count() << " milliseconds." << endl;
    path();

    return 0;
}