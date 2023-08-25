#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<iostream>
//#include<io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1   //file
#define SPECIAL 2   //file

#define START  0   //OFFSET
#define CURRENT 1  //OFFSET
#define END 2       //OFFSET

typedef struct superblock
{
    int totalinodes;
    int  freeinode;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
char filename[50];
int inodenumber;
int filesize;
int fileactualsize;
int filetype;
char *Buffer;
int  linkcount;
int referencecount;
int permisssion;   //1  23
struct inode *next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
    int readoffset;
    int count;
    int writeoffset;
    int mode;   // 1 2 3
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTarr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;

void man(char *name)
{
    if(name==NULL)return;

    if(strcmp(name,"create")==0)
{
    printf("description:used to create new regular file\n");
    printf("usage:create file_name permission\n");
}
else if(strcmp(name,"read")==0)
{
    printf("description:used to read data from regular file\n");
    printf("usage:read file name no of bytes to read\n");
}
else if(strcmp(name,"write")==0)
{
    printf("description:used to write into regular file\n");
    printf("usage:writer  file_name\n after this enter the data that we want to write\n");
}
else if(strcmp(name,"ls")==0)
{
    printf("description:used  to list all information of files\n");
    printf("usage: ls\n");
}
else if(strcmp(name,"stat")==0)
{
    printf("description:used to display information of files\n");
    printf("usage: stat file_name\n");
}
else if(strcmp(name,"fstat")==0)
{
    printf("description:used to display information of files\n");
    printf("usage: stat file_descriminator\n");
}
else if(strcmp(name,"truncate")==0)
{
    printf("description:used  to remove the data from files\n");
    printf("usage: truncate file_name\n");
}
else if(strcmp(name,"open")==0)
{
    printf("description:used to open existing file\n");
    printf("usage: open file_name\n");
}
else if(strcmp(name,"close")==0)
{
    printf("description:used to close opened files\n");
    printf("usage: close file_name mode\n");
}
else if(strcmp(name,"closeall")==0)
{
    printf("description:used to close all opened file\n");
    printf("usage: closeall\n");
}
else if(strcmp(name,"lseek")==0)
{
    printf("description:used  to chnaged files offset\n");
    printf("usage: oseek file_name changedinoffset startpoint\n");
}
else if(strcmp(name,"rm")==0)
{
    printf("description:used delete the file\n");
    printf("usage: rm file_name\n");
}
else
{
    printf("ERROR:no manual entry available.\n");
}
}

void displayhelp()
{
    printf("is:to list out all files\n");
    printf("clear:to clear console\n");
    printf("open:to open the file\n");
    printf("close:to close the file\n");
    printf("closeall:to close all opend file\n");
    printf("read:to read the contents into file\n");
    printf("write:to write contents from file\n");
    printf("exit:to terminatate file system\n");
    printf("stat: to display information of file using name\n");
    printf("fstat:to display information of file using file descriminator\n");
    printf("truncate:to remove all data from file\n");
    printf("rm: to delete the file\n");
}

int getfdfromname(char *name)
{
    int i=0;

    while(i<50)
    {
        if(UFDTarr[i].ptrfiletable!=NULL)
        if(strcmp((UFDTarr[i].ptrfiletable->ptrinode->filename),name)==0)
        break;
        i++;
    }

    if(i==50)    return -1;
    else       return i;
}

PINODE get_inode(char * name)
{
    PINODE temp=head;
    int i=0;

    if(name==NULL)
    return NULL;

    while(temp!=NULL)
    {
        if(strcmp(name,temp->filename)==0)
        break;
        temp=temp->next;
    }
return temp;
}

void createDILB()
{
    int i=1;
    PINODE newn=NULL;
    PINODE temp=head;

    while(i<=MAXINODE)
    {
        newn=(PINODE)malloc(sizeof(INODE));

        newn->linkcount=0;
        newn->referencecount=0;
        newn->filetype=0;
        newn->filesize=0;

        newn->Buffer=NULL;
        newn->next=NULL;

        newn->inodenumber=i;

        if(temp==NULL)
        {
            head=newn;
            temp=head;
        }
        else
        {
            temp->next=newn;
            temp=temp->next;
        }
        i++;
        }
        printf("DILB created succesfully\n");
}

void initialisesuperblock()
{
    int i=0;
    while(i<MAXINODE)
    {
        UFDTarr[i].ptrfiletable=NULL;
        i++;
    }

    SUPERBLOCKobj.totalinodes=MAXINODE;
    SUPERBLOCKobj.freeinode=MAXINODE;
}

int createFile(char *name,int permission)
{
    int i=0;
    PINODE temp=head;

    if((name==NULL)||(permission==0)||(permission>3))
    return -1;

    if(SUPERBLOCKobj.freeinode==0)
    return -2;

    (SUPERBLOCKobj.freeinode)--;

    if(get_inode(name)!=NULL)
    return -3;

    while(temp!=NULL)
    {
        if(temp->filetype==0)
        break;
        temp=temp->next;
    }

    while(i<50)
    {
        if(UFDTarr[i].ptrfiletable==NULL)
        break;
        i++;
    }

    UFDTarr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTarr[i].ptrfiletable->count=1;
    UFDTarr[i].ptrfiletable->mode=permission;
    UFDTarr[i].ptrfiletable->readoffset=0;
    UFDTarr[i].ptrfiletable->writeoffset=0;

    UFDTarr[i].ptrfiletable->ptrinode=temp;

   strcpy(UFDTarr[i].ptrfiletable->ptrinode->filename,name);
   UFDTarr[i].ptrfiletable->ptrinode->filetype=REGULAR;
   UFDTarr[i].ptrfiletable->ptrinode->referencecount=1;
   UFDTarr[i].ptrfiletable->ptrinode->linkcount=1;
   UFDTarr[i].ptrfiletable->ptrinode->filesize=MAXFILESIZE;
   UFDTarr[i].ptrfiletable->ptrinode->fileactualsize=0;
   UFDTarr[i].ptrfiletable->ptrinode->permisssion=permission;
   UFDTarr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(MAXFILESIZE);

   return i;
}

//re_file("demo.txt")
int rm_file(char*name)
{
    int fd=0;

    fd=getfdfromname(name);
    if(fd==-1)
    return -1;

    (UFDTarr[fd].ptrfiletable->ptrinode->linkcount)--;

    if(UFDTarr[fd].ptrfiletable->ptrinode->linkcount==0)
    {
        UFDTarr[fd].ptrfiletable->ptrinode->filetype=0;
        //free(ufdtarr[fd]).ptrfile->ptrnode->buffer);
        free(UFDTarr[fd].ptrfiletable);
    }

    UFDTarr[fd].ptrfiletable=NULL;
    (SUPERBLOCKobj.freeinode)++;
}

int Readfile(int fd,char *arr,int isize)
{
    int read_size=0;

    if(UFDTarr[fd].ptrfiletable==NULL)     return -1;

    if(UFDTarr[fd].ptrfiletable->mode!=READ&&UFDTarr[fd].ptrfiletable->mode!=READ+WRITE)  return -2;

    if(UFDTarr[fd].ptrfiletable->ptrinode->permisssion!=READ&&UFDTarr[fd].ptrfiletable->ptrinode->permisssion!=READ+WRITE)  return -2;

if(UFDTarr[fd].ptrfiletable->readoffset==UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)   return-3;

if(UFDTarr[fd].ptrfiletable->ptrinode->filetype!=REGULAR)  return -4;

read_size=(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)-(UFDTarr[fd].ptrfiletable->readoffset);
if(read_size<isize)
{
    strncpy(arr,(UFDTarr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTarr[fd].ptrfiletable->readoffset),read_size);

    UFDTarr[fd].ptrfiletable->readoffset=UFDTarr[fd].ptrfiletable->readoffset+read_size;
}
else
{
    strncpy(arr,(UFDTarr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTarr[fd].ptrfiletable->readoffset),isize);

(UFDTarr[fd].ptrfiletable->readoffset)=(UFDTarr[fd].ptrfiletable->readoffset)+isize;
}

return isize;
}

int writefile(int fd,char *arr,int isize)
{
    if(((UFDTarr[fd].ptrfiletable->mode)!=WRITE)&&((UFDTarr[fd].ptrfiletable->mode)!=READ+WRITE)) return -1;

if(((UFDTarr[fd].ptrfiletable->ptrinode->permisssion)!=WRITE)&& ((UFDTarr[fd].ptrfiletable->ptrinode->permisssion)!=READ+WRITE))  return -1;

if((UFDTarr[fd].ptrfiletable->writeoffset)==MAXFILESIZE)  return -2;

if((UFDTarr[fd].ptrfiletable->ptrinode->filetype)!=REGULAR ) return -3;

strncpy((UFDTarr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTarr[fd].ptrfiletable->writeoffset),arr,isize);

(UFDTarr[fd].ptrfiletable->writeoffset)=(UFDTarr[fd].ptrfiletable->writeoffset)+isize;

(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)=(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+isize;

return isize;
}

int openfile(char *name,int mode)
{
    int i=0;
    PINODE temp=NULL;

    if(name==NULL||mode<=0)
    return -1;

    temp=get_inode(name);
    if((temp)==NULL)
    return -2;

    if(temp->permisssion<mode)
    return -3;

    while(i<50)
    {
        if(UFDTarr[i].ptrfiletable==NULL)
        break;
        i++;
    }
    
    UFDTarr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(filetable));
    if(UFDTarr[i].ptrfiletable==NULL)  return -1;
    UFDTarr[i].ptrfiletable->count=1;
    UFDTarr[i].ptrfiletable->mode=mode;
    if(mode==READ+WRITE)
    {
        UFDTarr[i].ptrfiletable->readoffset=0;
        UFDTarr[i].ptrfiletable->writeoffset=0;
    }
    else if(mode==READ)
    {
        UFDTarr[i].ptrfiletable->readoffset=0;
    }
    else if(mode==WRITE)
    {
        UFDTarr[i].ptrfiletable->writeoffset=0;
    }
UFDTarr[i].ptrfiletable->ptrinode=temp;
(UFDTarr[i].ptrfiletable->ptrinode->referencecount)++;

return i;
}

void closefilebyname(int fd)
{
    UFDTarr[fd].ptrfiletable->readoffset=0;
    UFDTarr[fd].ptrfiletable->writeoffset=0;
    (UFDTarr[fd].ptrfiletable->ptrinode->referencecount)--;
}

int closefilebyname(char *name)
{
    int i=0;
    i=getfdfromname(name);
    if(i==-1)
    return -1;

    UFDTarr[i].ptrfiletable->readoffset=0;
    UFDTarr[i].ptrfiletable->writeoffset=0;
    (UFDTarr[i].ptrfiletable->ptrinode->referencecount)--;

    return 0;
}

void closeallfile()
{
    int i=0;
    while(i<50)
    {
        if(UFDTarr[i].ptrfiletable!=NULL)
        {
            UFDTarr[i].ptrfiletable->readoffset=0; 
    UFDTarr[i].ptrfiletable->writeoffset=0;
    (UFDTarr[i].ptrfiletable->ptrinode->referencecount)--;
  break;
        }
        i++;
    }
}

int lseekfile(int fd,int size,int from)
{
    if((fd<0)||(from>2))   return -1;
    if(UFDTarr[fd].ptrfiletable==NULL)  return -1;

    if((UFDTarr[fd].ptrfiletable->mode==READ)||(UFDTarr[fd].ptrfiletable->mode==READ+WRITE))
    {
        if(from==START)
        {
            if(((UFDTarr[fd].ptrfiletable->readoffset)+size)>UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)  return -1;
            if(((UFDTarr[fd].ptrfiletable->readoffset)+size)<0)return -1;
            (UFDTarr[fd].ptrfiletable->readoffset)=(UFDTarr[fd].ptrfiletable->readoffset)+size;
        }
        else if(from==START)
        {
             if(size>(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)) return -1;
            if(size<0)    return -1;
            (UFDTarr[fd].ptrfiletable->readoffset)=size;
        }
        else if(from==END)
        {
            if((UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+size>MAXFILESIZE)     return -1;
            if((UFDTarr[fd].ptrfiletable->readoffset)=(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+size);
        (UFDTarr[fd].ptrfiletable->readoffset)=(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+size;
        }
        }
        else if (UFDTarr[fd].ptrfiletable->mode==WRITE)
        {
            if(from==CURRENT)
            {
                if(((UFDTarr[fd].ptrfiletable->writeoffset)+size)>MAXFILESIZE)  return -1;
                if(((UFDTarr[fd].ptrfiletable->writeoffset)+size)<0)  return -1;
                if(((UFDTarr[fd].ptrfiletable->writeoffset)+size)>(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)) 
                (UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)=(UFDTarr[fd].ptrfiletable->writeoffset)+size;
                (UFDTarr[fd].ptrfiletable->writeoffset)=(UFDTarr[fd].ptrfiletable->writeoffset)+size;
            }
            else if(from==START)
            {
                if(size > MAXFILESIZE) return -1;
                if(size<0) return -1;
                if(size> (UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize))
                (UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)=size;
                (UFDTarr[fd].ptrfiletable->writeoffset)=size;
            }
            else if(from==END)
            {
                if((UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+size>MAXFILESIZE)  return -1;
                if(((UFDTarr[fd].ptrfiletable->writeoffset)+size<0))   return -1;
                (UFDTarr[fd].ptrfiletable->writeoffset)=(UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize)+size;
            }
        }
    }

void ls_file()
{
    int i=0;
    PINODE temp=head;

    if(SUPERBLOCKobj.freeinode==MAXINODE)
    { 
        printf("ERROR: ther are no files\n");
        return;
    }

    printf("\n file name\t inode number\t file size\tlink count\n");
    printf("---------------------------------------\n");
    while(temp!=NULL)
    {
        if(temp->filetype!=0)
{
    printf("%s\t\t%d\t\t%d\n",temp->filename,temp->inodenumber,temp->fileactualsize,temp->linkcount);
}
    temp=temp->next;
}
   printf("---------------------------------------\n");
   }

   int fstat_file(int fd)
   {
    PINODE temp=head;
    int i=0;

    if(fd<0) return -1;

    if(UFDTarr[fd].ptrfiletable->ptrinode==NULL)  return-2;

    temp=UFDTarr[fd].ptrfiletable->ptrinode;

    printf("\n--------statistical information about file------\n");
    printf("file name: %s\n",temp->filename);
    printf("inode number: %d\n",temp->inodenumber);
    printf("file size: %d\n",temp->filesize);
    printf("actual file size: %d\n",temp->fileactualsize);
    printf("link count: %d\n",temp->linkcount);
    printf("refernce count:%d\n",temp->referencecount);

    if(temp->permisssion==1)
    printf("file permission: read only\n");
    else if(temp->permisssion==2)
    printf("file permission: write only\n");
    else if(temp->permisssion==3)
    printf("file permission:write&read only\n");
    printf("--------------------------------\n\n");
    
    return 0;
 }

 int stat_file(char *name)
 {
   PINODE temp=head;
   int i=0;

   if(name==NULL)    return -1;

   while(temp!=NULL)
   {
    if(strcmp(name,temp->filename)==0)
    break;
    temp=temp->next;
   }

   if(temp==NULL)   return -2;

   printf("\n--------statistical information about file------\n");
    printf("file name: %s\n",temp->filename);
    printf("inode number: %d\n",temp->inodenumber);
    printf("file size: %d\n",temp->filesize);
    printf("actual file size: %d\n",temp->fileactualsize);
    printf("link count: %d\n",temp->linkcount);
    printf("refernce count:%d\n",temp->referencecount);

    if(temp->permisssion==1)
    printf("file permission: read only\n");
    else if(temp->permisssion==2)
    printf("file permission: write only\n");
    else if(temp->permisssion==3)
    printf("file permission:write&read only\n");
    printf("--------------------------------\n\n");
    
    return 0;
 }

 int truncate_file(char *name)
 {
    int fd=getfdfromname(name);
    if(fd==-1)
    return -1;

    memset(UFDTarr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTarr[fd].ptrfiletable->readoffset=0;
    UFDTarr[fd].ptrfiletable->writeoffset=0;
    UFDTarr[fd].ptrfiletable->ptrinode->fileactualsize=0;
 }

 int main()
 {
    char *ptr=NULL;
    int ret=0,fd=0,count=0;
    char command[4][80],str[80],arr[1024];

    initialisesuperblock();
    createDILB();

    while(1)
    {
        fflush(stdin);
        strcpy(str,"");

        printf("\nmarvellous VFS: >");

        fgets(str,80,stdin);  

        count=sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

        if(count==1)
        {
            if(strcmp(command[0],"ls")==0)
            {
                ls_file();
            }
             else if(strcmp(command[0],"closeall")==0)
            {
                closeallfile();
                printf("all files closed succesfully\n");
                continue;
            }
              else if(strcmp(command[0],"clear")==0)
            {
                system("cls");
                continue;
            }
              else if(strcmp(command[0],"help")==0)
            {
               displayhelp();
                continue;
            }
              else if(strcmp(command[0],"exit")==0)
            {
                printf("terminating the marvellous virtual file system\n");
                break;
            }
            else
            {
                printf("\nERROR:command not found!!!\n");
                continue;
            }  
        } 
        else if(count==2)
        {
            if(strcmp(command[0],"stat")==0)
            {
                ret=stat_file(command[1]);
                if(ret==-1)
                printf("ERROR:incorrect parameters\n");
                if(ret==-2)
                printf("ERROR:there is no such file\n");
                continue;
            }
            else if(strcmp(command[0],"fstat")==0)
            {
                ret=fstat_file(atoi(command[1]));
                if(ret==-1)
                printf("ERROR:incorrect parameters\n");
                if(ret==-2)
                printf("ERROR:there is no such file\n");
                continue;
            }
            else if(strcmp(command[0],"close")==0)
            {
                ret=closefilebyname(command[1]);
                if(ret==-1)
                printf("ERROR:there is no such file\n");
                continue;
            }
            if(strcmp(command[0],"rm")==0)
            {
                ret=rm_file(command[1]);
                if(ret==-1)
                printf("ERROR:there is no such file\n");
                continue;
            }
           else if(strcmp(command[0],"man")==0)
           {
            fd=getfdfromname(command[1]);
           }
           else if(strcmp(command[0],"write")==0)
            {
                fd=getfdfromname(command[1]);
                if(fd==-1)
                {
                    printf("ERROR: incorrect parameter\n");
                    continue;
                }
                printf("enter the data:\n");
                scanf("%[^\n]",arr);

                ret=strlen(arr);
                if(ret==0)
                {
                    printf("ERROR: incorrect parameter\n");
                    continue;
                }
                  ret=writefile(fd,arr,ret);
                    if(ret==-1)
                    printf("ERROR: permission denied\n");
                    if(ret==-2)
                    printf("ERROR: there is no sufficient memory to write\n");
                    if(ret==-1)
                    printf("ERROR: it is no regular file\n");
            }
            else if(strcmp(command[0],"truncate")==0)
            {
                ret=truncate_file(command[1]);
                if(ret==-1)
                printf("ERROR:incorrect parametr\n");
            }
            else
            {
              printf("\nERROR:command not found!!!");
              continue;;
            }       
            }
            else if(count==3)
            {
               if(strcmp(command[0],"create")==0)
              {
                ret=createFile(command[1],atoi(command[2]));
                if(ret>=0)
                printf("file is succesfully created with file descriptor:%d\n",ret);
                if(ret==-1)
                printf("ERROR:incorrect parameters\n");
                if(ret==-2)
                printf("ERROR:there is no inodes\n");
                if(ret==-3)
                printf("ERROR:file already exist\n");
                if(ret==-4)
                printf("ERROR:memory alloacation failure\n");
                continue;
              }
              else if(strcmp(command[0],"open")==0)
              {
                ret=openfile(command[1],atoi(command[2]));
                if (ret>=0)
                printf("file is succesfully created with file descriptor:%d\n",ret);
                if(ret==-1)
                printf("ERROR:incorrect parameters\n");
                if(ret==-2)
                printf("ERROR:file not present\n");
                if(ret==-3)
                printf("ERROR:permission denied\n");
                continue;
              }
                else if(strcmp(command[0],"read")==0)
              {
                 fd=getfdfromname(command[1]);
                if(fd==-1)
                {
                    printf("ERROR: memory allocation failure\n");
                    continue;
                }
                ret=Readfile(fd,ptr,atoi(command[2]));
                if(ptr==NULL)
                {
                    printf("ERROR:memory alloaction failure\n");
                    continue;
                }
                ret=Readfile(fd,ptr,atoi(command[2]));
                if(ret==-1)
                printf("ERROR: file not existing\n");
                if(ret==-2)
                printf("ERROR: permission denied\n");
                if(ret==-3)
                printf("ERROR: reached at end of file\n");
                if(ret==-4)
                printf("ERROR: it is not a regular file");
                if(ret==0)
                printf("ERROR:file empty\n");
                if(ret>0)
                {
                    write(2,ptr,ret);
                }
                continue;
                 }
                 else
                 {
                    printf("\nERROR: commnd not found!!!\n");
                    continue;
                 }
              }
              else if(count==4)
              {
                if(strcmp(command[0],"lseek")==0)
                {
                    fd=getfdfromname(command[1]);
                    if(fd==-1)
                    {
                        printf("ERROR:incorrect parameter\n");
                        continue;
                    }
                    ret=lseekfile(fd,atoi(command[2]),atoi(command[3]));
                    if(ret==-1)
                    {
                        printf("ERROR:unable to perform lseek\n");
                    }
                }
                else
                {
                    printf("\nERROR:command not found!!\n");
                    continue;
                }
              }
              else
              {
                printf("\n ERROR:command not found!!\n");
                continue;
              }
            }
            return 0;
           } 
    
    

 






































