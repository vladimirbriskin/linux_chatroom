#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include<string.h>
#include<mysql/mysql.h>
#include<fstream>
#include<string>
#include <sys/stat.h> 
#include<iostream>
#define MAXLINE 4096

using namespace std;

/*
00 登陆 
01 注册
02 获取朋友列表
03 获取群组列表
04 查询历史消息
05 发消息
06 发文件操作
07 加好友操作
08 查询自己为管理员的群组信息
09 将好友拉入自己管理的群
10 显示可供下载的文件
11 请求文件下载
*/
int sockfd;
char * hostname = "127.0.0.1";
int myport = 8006;
char * str = "helloworld";


char history[256];
char sndmsg[256];
char send_buff[MAXLINE];
char ftob[256];
char transmsg[256];
char nowusr[256];
char recv_buff[MAXLINE];
char buf[MAXLINE];
int hisnum;
void showmyfile(char n[])
{
	char showmyf[100]="10";
	strcat(showmyf,n);
	memset(buf,0,sizeof(buf));
	if (send(sockfd, showmyf, strlen(showmyf), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
	if (read(sockfd, buf, 4096) == -1)//看看怎么把buf读出来
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
	//读文件列表
	int buflen=strlen(buf);
	char name[256];
	memset(name,0,sizeof(name));
	int np=0;
	for(int i=0;i<buflen;i++)
	{
		if(buf[i]!='#')
		{
			name[np]=buf[i];
			np++;
		}
		else
		{
			name[np]='\0';
			printf("%s\n",name);
			np=0;
			memset(name,0,sizeof(name));
		}
	}
	
}
void getadminis(char n[])
{
	char skadm[100]="08";
	strcat(skadm,n);
	memset(buf,0,sizeof(buf));
	if (send(sockfd, skadm, strlen(skadm), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
	if (read(sockfd, buf, 4096) == -1)//看看怎么把buf读出来
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
	int buflen=strlen(buf);
	char name[20];
	memset(name,0,sizeof(name));
	int np=0;
	for(int i=0;i<buflen;i++)
	{
		if(buf[i]!='#')
		{
			name[np]=buf[i];
			np++;
		}
		else
		{
			name[np]='\0';
			printf("%s\n",name);
			np=0;
			memset(name,0,sizeof(name));
		}
	}
		
}
void getfriendinfo(char n[])
{
	char search[100]="02";
	strcat(search,n);
	memset(buf,0,sizeof(buf));
	if (send(sockfd, search, strlen(search), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
	if (read(sockfd, buf, 4096) == -1)//看看怎么把buf读出来
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
	int buflen=strlen(buf);
	char name[20];
	memset(name,0,sizeof(name));
	int np=0;
	for(int i=0;i<buflen;i++)
	{
		if(buf[i]!='#')
		{
			name[np]=buf[i];
			np++;
		}
		else
		{
			name[np]='\0';
			printf("%s\n",name);
			np=0;
			memset(name,0,sizeof(name));
		}
	}
	//printf("%s",buf);
}
void getgroupinfo(char n[])
{
	char search[100]="03";
	strcat(search,n);
	memset(buf,0,sizeof(buf));
	if (send(sockfd, search, strlen(search), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
	if (read(sockfd, buf, 4096) == -1)//看看怎么把buf读出来
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
	int buflen=strlen(buf);
	char name[20];
	memset(name,0,sizeof(name));
	int np=0;
	for(int i=0;i<buflen;i++)
	{
		if(buf[i]!='#')
		{
			name[np]=buf[i];
			np++;
		}
		else
		{
			name[np]='\0';
			printf("%s\n",name);
			np=0;
			memset(name,0,sizeof(name));
		}
	}
	//printf("%s",buf);
}
int main(int argc, char *argv[])
{
   int logr;
   int command;//控制符
   //int uid;
   //int idatnow=-1;
   char psw[256];
   char nick[256];
   
   char message[256];
   
   struct sockaddr_in pin_addr;
   
   bzero(&pin_addr, sizeof(pin_addr));
   pin_addr.sin_family = AF_INET;
   pin_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   pin_addr.sin_port = htons(myport);
   
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      printf("can’t create socket\n");
      exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr *)&pin_addr,sizeof(pin_addr)) == -1)
{
   printf ("Error connecting to socket\n");
   exit(1);
}
while(1)
{
	printf("请先进行登陆或注册操作：\n");
	printf("1:登陆\n2:注册\n3:退出\n");
	scanf("%d",&logr);
	if(logr==1)
	{
		char  login[100]="00";
		memset(psw,0,sizeof(psw));
		memset(nowusr,0,sizeof(nowusr));
		printf("请输入id：\n");//存在问题：传送的过程中，id到底是字符还是int
		scanf("%s",&nowusr);
		printf("请输入密码：\n");
		scanf("%s",&psw);
		strcat(login,nowusr);
		strcat(login,"#");
		strcat(login,psw);
		//printf("%s\n",login);
		if (send(sockfd, login, strlen(login), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
		//这中间会有时间间隔吗？也就是说它是阻塞的吗？
	//memset(buf,0,sizeof(buf));
		//while(1)
		//{
		//printf("buf 1 %s",buf);
		if (read(sockfd, buf, 4096) == -1)//have problem,长度matters
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
		//printf("buf is: %s",buf);
		if(strcmp(buf,"1")==0)//登陆验证通过
		{
			printf("恭喜，验证通过\n");
			//nowusr保存当前用户是谁
			memset(buf,0,sizeof(buf));
			break;
		}
		//}
		else goto lagain;
	}
	else if(logr==2)
	{
		char regis[100]="01";
		memset(psw,0,sizeof(psw));
		memset(nick,0,sizeof(nick));
		printf("请输入注册昵称：\n");
		scanf("%s",&nick);
		printf("请输入注册密码：\n");
		scanf("%s",&psw);
		strcat(regis,nick);
		strcat(regis,"#");
		strcat(regis,psw);
		//printf("this is regis:%s\n",regis);
		if (send(sockfd, regis, strlen(regis), 0) == -1 ) 
		{
		   printf ("Error in send\n");
		   exit(1);
		}
		if (read(sockfd, buf, 4096) == -1)//have problem
		{
		   printf("Error in receiving response from server\n");
		   exit(1);
		}
		//printf("now,buf is:%s\n",buf);
		if(strcmp(buf,"1")==0)
		{
			printf("请牢记您刚才使用的用户名和密码\n");
		}
		else
		{
			printf("请换一个用户名再次尝试\n");
		}
		goto lagain;
		
	}
	else if(logr==3)
	{
		goto lend;
	}
	else
	{
	lagain:  printf("请继续进行操作\n");
	}
}
int a;
while(1)
{
	//打印好友和群组信息
  ref: printf("好友列表：\n");
	getfriendinfo(nowusr);
	printf("群组列表：\n");
	getgroupinfo(nowusr);
	printf("您为管理员的群组列表：\n");
	getadminis(nowusr);
	printf("请选择一项操作：\n");
	printf("1：查看消息\n");//子目录设置查看群消息还是个人消息
	printf("2：发送消息\n");//子目录设置发群消息还是个人消息
	printf("3：发送文件\n");//子目录设置发个人文件还是群文件
	printf("4：接收文件\n");//子目录设置接收什么文件
	printf("5：添加好友\n");
	printf("6：创建群组\n");
	printf("7：退出聊天程序\n");
	
	//break;
	scanf("%d",&command);
	//scanf("%c",&command);
	if(command==1)
	{
		memset(history,0,sizeof(history));
		int k;
		char hsrc[40]="04";
		printf("您要查看哪一类历史消息？\n1：个人\n2：群组\n");
		//scanf("%c",&k);
		scanf("%d",&k);
		if(k==1)
		{
			strcat(hsrc,"1");
			strcat(hsrc,nowusr);
			strcat(hsrc,"#");
			printf("输入你想查看历史消息的对象：\n");
			scanf("%s",&history);
			strcat(hsrc,history);
			if (send(sockfd, hsrc, strlen(hsrc), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}
			if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
			
			char perhis[10][100];
			memset(perhis,0,sizeof(perhis));
			int bbflen;
			int iti;
			int tiao;
			int m;
			bbflen=strlen(buf);
			tiao=0;
			iti=0;
			for(m=0;m<bbflen;m++)
			{
				if(buf[m]!='#')
				{
					perhis[tiao][iti]=buf[m];
					iti++;
				}
				else
				{
					perhis[tiao][iti]='\0';
					tiao++;
					iti=0;
				}
				
			}
			//tiao--;
			while(tiao--)
			{
				printf("%s\n",perhis[tiao]);
			}
			
			
		}
		else if(k==2)
		{
			strcat(hsrc,"2");
			strcat(hsrc,nowusr);
			strcat(hsrc,"#");
			printf("输入你想查看历史消息的对象：\n");
			scanf("%s",&history);
			strcat(hsrc,history);
			if (send(sockfd, hsrc, strlen(hsrc), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}
			if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
			
			char ghis[10][100];
			memset(ghis,0,sizeof(ghis));
			int bbflen;
			int iti;
			int tiao;
			int m;
			bbflen=strlen(buf);
			tiao=0;
			iti=0;
			for(m=0;m<bbflen;m++)
			{
				if(buf[m]!='#')
				{
					ghis[tiao][iti]=buf[m];
					iti++;
				}
				else
				{
					ghis[tiao][iti]='\0';
					tiao++;
					iti=0;
				}
				
			}
			//tiao--;
			while(tiao--)
			{
				printf("%s\n",ghis[tiao]);
			}
			
		}
		
		
		
	}
	else if(command==2)
	{
		memset(sndmsg,0,sizeof(sndmsg));
		memset(transmsg,0,sizeof(transmsg));
		int l;
		char ssrc[300]="05";
		printf("您要发送消息的对象是？\n1：个人\n2：群组\n");
		//scanf("%c",&k);
		scanf("%d",&l);
		if(l==1)
		{
			strcat(ssrc,"1");
			strcat(ssrc,nowusr);
			strcat(ssrc,"#");
			printf("您要向谁发送消息？\n");
			scanf("%s",&sndmsg);
			strcat(ssrc,sndmsg);
			strcat(ssrc,"#");
			printf("请输入您要发送的消息：\n");
			scanf("%s",&transmsg);
			strcat(ssrc,transmsg);
			if (send(sockfd, ssrc, strlen(ssrc), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}
			if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
			if(!strcmp(buf,"1"))
			{
				printf("消息已成功发送！\n");
			}
			else
			{
				printf("消息未能成功发送，请尝试重试\n");
			}
			
		}
		else if(l==2)
		{
			strcat(ssrc,"2");
			strcat(ssrc,nowusr);
			strcat(ssrc,"#");
			printf("您要向哪个群组发送消息？\n");
			scanf("%s",&sndmsg);
			strcat(ssrc,sndmsg);
			strcat(ssrc,"#");
			printf("请输入您要发送的消息：\n");
			scanf("%s",&transmsg);
			strcat(ssrc,transmsg);
			if (send(sockfd, ssrc, strlen(ssrc), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}
			if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
			if(!strcmp(buf,"1"))
			{
				printf("消息已成功发送！\n");
			}
			else
			{
				printf("消息未能成功发送，请尝试重试\n");
			}
		}
		
	}
	else if(command==3)
	{
		char fsskt[300];
		memset(fsskt,0,sizeof(fsskt));
		char fname[300];
		memset(fname,0,sizeof(fname));
		strcat(fsskt,"06");
		strcat(fsskt,nowusr);
		strcat(fsskt,"#");

		printf("你要发送文件的对象是？\n");
		scanf("%s",&ftob);
		
	
		strcat(fsskt,ftob);
		strcat(fsskt,"#");
		printf("请输入你要发送的文件名（发送前请先将其移动到与本客户端相同目录下）：\n");
		scanf("%s",&fname);
		strcat(fsskt,fname);
		strcat(fsskt,"#");
		//获取大小
		char lookf[300];
		memset(lookf,0,sizeof(lookf));
		strcat(lookf,"./");
		strcat(lookf,fname);
		//off_t 
		struct stat myfile; 
		int fhan;
		char Flen[40];
		fhan = stat (lookf, &myfile);   
		off_t myfilesz;
		myfilesz=myfile.st_size; 
		sprintf(Flen,"%ld",myfilesz);
		strcat(fsskt,Flen);

		if (send(sockfd, fsskt, strlen(fsskt), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}

		if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
		//发过去了标识符，等待那边创建好接收线程这边就开始发送
		//////////看看是否需要memset一下buf
		if(!strcmp(buf,"1"))
		{
				printf("成功创建发送任务，即将开始发送\n");
				//开始写发送过程
		std::fstream fs1;
		fs1.open(fname,std::fstream::in|std::fstream::binary);
		fs1.seekg(0,std::fstream::beg);
		
		while(!fs1.eof())
		{
			usleep(80000);
			printf("%s\n",send_buff);
			fs1.read(send_buff,MAXLINE);
			int ll=fs1.gcount();
			send(sockfd,send_buff,ll,0);
			printf("发去一个包\n");
		}
		fs1.close();
		}
		else
		{
				printf("未能成功创建文件发送任务，请尝试重试\n");
				goto ref;
		}
	}
	else if(command==4)
	{
		printf("可供您下载的文件有：\n");
		showmyfile(nowusr);
		printf("请输入一个文件名以下载:\n");
		char wtdf[256];
		scanf("%s",&wtdf);
		char rqsfle[300];
		memset(rqsfle,0,sizeof(rqsfle));
		strcat(rqsfle,"11");
		strcat(rqsfle,wtdf);
		if (send(sockfd, rqsfle, strlen(rqsfle), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}
			
		std::fstream fs;
		fs.open(wtdf,std::fstream::out|std::fstream::binary|std::fstream::trunc);
			if(!fs) printf("文件没找到啊\n");
			int fen;
			//int fxs=atoi(cap);
			while(1)//文件大小
			{
				//printf("this is shengyu :%d\n",fxs);
				fen=recv(sockfd,recv_buff,MAXLINE,0);
				if(fen>0)//读入的大小
				{
					fs.write(recv_buff,fen);
					//fxs-=fen;
					if(fen!=MAXLINE) break;
				}
				printf("收到一个包\n");
				
			
			}
			fs.close();
			printf("下载完成！\n");
		/*if (read(sockfd, buf, 4096) == -1)
			{
			   printf("Error in receiving response from server\n");
			   exit(1);
			}
		if(!strcmp(buf,"1"))
		{
			printf("即将开始下载\n");
			
		}
		else
		{
			printf("未能成功下载，请重新尝试\n");
		}*/
			
			
		
	}
	else if(command==5)//add friends
	{
		char addfr[100];
		memset(addfr,0,sizeof(addfr));
		strcat(addfr,"07");
		strcat(addfr,nowusr);
		strcat(addfr,"#");
		printf("选择你要添加的好友\n");
		char fri_n[30];
		scanf("%s",&fri_n);
		strcat(addfr,fri_n);
		
		/*char skfme[50]="select uid from user_info where name = '";
		strcat(skfme,nowusr);
		strcat(skfme,"'");
		if (send(sockfd, , strlen(fsskt), 0) == -1 ) 
			{
			   printf ("Error in send\n");
			   exit(1);
			}*/
			
		//sprintf(,"",);
		if (send(sockfd,addfr, strlen(addfr), 0) == -1 ) 
		{
			   printf ("Error in send\n");
			   exit(1);
		}
		
		if (read(sockfd, buf, 4096) == -1)
		{
			   printf("Error in receiving response from server\n");
			   exit(1);
		}
		if(!strcmp(buf,"1"))
		{
				printf("已成功添加好友！\n");
		}
		else
		{
				printf("未能成功添加好友，请尝试重试\n");
		}
		
		
	}
	else if(command==6)
	{
		printf("请输入你想创建的群组名：\n");
		char grpn[30];
		scanf("%s",&grpn);
		printf("请输入你想邀请的群成员名（最多10名成员，每个成员名后加一个‘#’符号以示区分）：\n");
		char grnams[200];
		scanf("%s",&grnams);
		char cmd6[300]="09";
		//memset(cmd6,0,sizeof(cmd6));
		strcat(cmd6,nowusr);
		strcat(cmd6,"#");
		strcat(cmd6,grpn);
		strcat(cmd6,"#");
		strcat(cmd6,grnams);
		
		if (send(sockfd, cmd6, strlen(cmd6), 0) == -1 ) 
		{
			   printf ("Error in send\n");
			   exit(1);
		}
		
		if (read(sockfd, buf, 4096) == -1)
		{
			   printf("Error in receiving response from server\n");
			   exit(1);
		}
		if(!strcmp(buf,"1"))
		{
				printf("已成功创建群聊！\n");
		}
		else
		{
				printf("未能成功创建群聊，请尝试重试\n");
		}
		
	}
	else if(command==7)
	{
		close(sockfd);
		break;
	}
}
/*
printf("Sending message %s to server…\n", str); 

if (send(sockfd, str, strlen(str)+1, 0) == -1 ) 
{
   printf ("Error in send\n");
   exit(1);
}

printf ("..sent message.. wiat for response..\n");

if (recv(sockfd, buf, 100, 0) == -1)//have problem
{
   printf("Error in receiving response from server\n");
   exit(1);
}

printf("\nResponse from server:\n\n%s\n", buf);*/

lend:;}


