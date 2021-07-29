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
#include<iostream>

#define MAXLINE 4096
#define SERV_PORT 8006

using namespace std;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_RES *resfri;
MYSQL_RES *resgr;
MYSQL_RES *us1c;
MYSQL_RES *us2c;
MYSQL_ROW row;
 char nowid[30];
 char send_buff[MAXLINE];
  char recv_buff[MAXLINE];
 char frilist[MAXLINE];
 char grlist[MAXLINE];
  char admlist[MAXLINE];
  char myfilelist[MAXLINE];

struct s_info 
{
	struct sockaddr_in cliaddr;
	int connfd;
};

int regissql(char a[],char b[])//a是用户，b是密码
{
	char sel[100]="insert ignore into user_info(name,password)values('";
	strcat(sel,a);
	strcat(sel,"','");
	//strcat(sel,b);
	//strcat(sel,"','");
	strcat(sel,b);
	strcat(sel,"')");
	int l1=strlen(sel);
	sel[l1]=' ';
	
	if(mysql_query(conn,sel))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	if(mysql_affected_rows(conn)) 
	{
		return 1;}
	else 
	{
		return 0;}
	
}

int loginsql(char a[],char b[])//a是用户，b是密码
{
	char tmp[100]="select password from user_info where name='";
	strcat(tmp,a);
	memset(nowid,0,sizeof(nowid));
	strcpy(nowid,a);
	int ll=strlen(tmp);
	tmp[ll]='\'';
	if(mysql_query(conn,tmp))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	res=mysql_use_result(conn);
	if((row=mysql_fetch_row(res))!=NULL)
	{
		if(strcmp(row[0],b)==0) 
		{if(res!=NULL) mysql_free_result(res);
				return 1;}
		else 
		{if(res!=NULL) mysql_free_result(res);
				return 0;}
	}
	else 
	{if(res!=NULL) mysql_free_result(res);	
	return 0;}
}

void chk(char a[],int leixing,int daxiao)
{
	int len = strlen(a);
	if(a[0]=='0'&&a[1]=='0')//登录检查
	{	printf("00");
		char username1[30];
		char password1[30];
		int i;
		int puser=0;
		for(i=2;i<len;i++)
		{
			if(a[i]=='#') break;
			else
			{
				username1[puser]=a[i];
				puser++;
			}
		}
		username1[puser]='\0';//看看是不是需要删除
		i++;
		int ppass=0;
		for(;i<len;i++)
		{
			password1[ppass]=a[i];
			ppass++;
		}
		password1[ppass]='\0';//看看是不是需要删除
		if(loginsql(username1,password1))  
		{
		//printf("1、年\n");
		write(leixing,"1",MAXLINE);
		}
		else write(leixing,"0",MAXLINE);	
	}
	
	else if(a[0]=='0'&&a[1]=='1')//注册
	{
		printf("01");
		int i;
		int puser=0;
		char username2[20];
		char password2[20];
		//char tel2[20];
		for(i=2;i<len;i++)
		{
			if(a[i]=='#') break;
			else
			{
				username2[puser]=a[i];
				puser++;
			}
		}
		username2[puser]='\0';//看看是不是需要删除
		i++;
		int ppass=0;
		for(;i<len;i++)
		{
			if(a[i]=='#') break;
			else
			{
			password2[ppass]=a[i];
			ppass++;
			}
		}
		password2[ppass]='\0';//看看是不是需要删除
		/*i++;
		for(;i<len;i++)
		{
			if(a[i]=='#') break;
		}
		i++;
		int ptel=0;
		for(;i<len;i++)
		{
			tel2[ptel]=a[i];
			ptel++;
		}
		tel2[ptel]='\0';//看看是不是需要删除*/
		if(regissql(username2,password2)) write(leixing,"1",MAXLINE);
		else  write(leixing,"0",MAXLINE);	
	}
	else if(a[0]=='0'&&a[1]=='2')//查询好友信息
	{
		printf("the needp is:%s\n",a);
		char needfri[20];
		int needp;
		int i;
		for( i=2,needp=0;i<len;i++,needp++)
		{	
			needfri[needp]=a[i];
		}
		needfri[needp]='\0';
		printf("the needp is:%s\n",needfri);
		char fritmp[200]="select name from user_info where uid in ( select fuid from friend where uid in (select uid from user_info where name = '";
		strcat(fritmp,needfri);
		strcat(fritmp,"')");
		int lenfp=strlen(fritmp);
		fritmp[lenfp]=')';
		
		memset(frilist,0,sizeof(frilist));
		if(mysql_query(conn,fritmp))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	res=mysql_use_result(conn);
	while((row=mysql_fetch_row(res))!=NULL)
	{
		strcat(frilist,row[0]);
		strcat(frilist,"#");
	}
	write(leixing,frilist,MAXLINE);
	printf("frrrrrrrrrrrr:%s\n",frilist);	
	}
	
	else if(a[0]=='0'&&a[1]=='3')//查询群组信息
	{
		printf("the needp is:%s\n",a);
		char needgr[20];
		int needp;
		int i;
		for( i=2,needp=0;i<len;i++,needp++)
		{	
			needgr[needp]=a[i];
		}
		needgr[needp]='\0';
		printf("the needp is:%s\n",needgr);
		char grtmp[200]="select gname from grp where gid in ( select gid from group_member where uid in (select uid from user_info where name = '";
		strcat(grtmp,needgr);
		strcat(grtmp,"')");
		int lenfp=strlen(grtmp);
		grtmp[lenfp]=')';
		
		memset(grlist,0,sizeof(grlist));
		
		if(mysql_query(conn,grtmp))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	
	resgr=mysql_use_result(conn);
	while((row=mysql_fetch_row(resgr))!=NULL)
	{
		strcat(grlist,row[0]);
		strcat(grlist,"#");
	}
	write(leixing,grlist,MAXLINE);
	printf("grrrrrrrrrrrr:%s\n",grlist);	
	}
	
	else if(a[0]=='0'&&a[1]=='4')//查询历史信息，这里默认传来了对话两方的名字
	{
		printf("the needp is:%s\n",a);
		if(a[2]=='1')
		{
			char sdr[20];
			char rcv[20];
			int sdp;
			int i;
			for( i=3,sdp=0;i<len;i++,sdp++)
			{	
				if(a[i]!='#')
				{
					sdr[sdp]=a[i];
				}
				else break;
			}
			i++;
			sdr[sdp]='\0';
			for(sdp=0;i<len;i++,sdp++)
			{
				rcv[sdp]=a[i];
			}
			rcv[sdp]='\0';
			
			char u1id[10];//求得第一个用户的id
			char us1[50]="SELECT uid from user_info WHERE name = '";
			strcat(us1,sdr);
			int lenu1=strlen(us1);			
			us1[lenu1]='\'';
			if(mysql_query(conn,us1))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			us1c=mysql_use_result(conn);
			if((row=mysql_fetch_row(us1c))!=NULL)
			{
				//u1id=row[0];
				strcpy(u1id,row[0]);
			}
			
			//printf("this is point 1\n");
			if(us1c!=NULL) mysql_free_result(us1c);
			
			char u2id[10];//求得第二个用户的id
			char us2[50]="SELECT uid from user_info WHERE name = '";
			strcat(us2,rcv);
			int lenu2=strlen(us2);			
			us2[lenu2]='\'';
			if(mysql_query(conn,us2))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			us2c=mysql_use_result(conn);
			if((row=mysql_fetch_row(us2c))!=NULL)
			{
				//u2id=row[0];
				strcpy(u2id,row[0]);
			}
			//printf("this is point 2\n");
			if(us2c!=NULL) mysql_free_result(us2c);
			
			char his1[170];
			sprintf(his1,"SELECT from_uid,msg from private_recv where (from_uid = %s and to_uid = %s) or (from_uid = %s and to_uid = %s) order by mid desc limit 0,10",u1id,u2id,u2id,u1id);//这里有\n，不一定能查到
			if(mysql_query(conn,his1))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			//printf("this is point 3\n");
			res=mysql_use_result(conn);//这里用了res,没准出错
			char his1w[MAXLINE];
			memset(his1w,0,sizeof(his1w));
			
			//printf("this is point 4\n");
			char tmpid[10];
			while((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(tmpid,row[0]);
				if(!strcmp(tmpid,u1id))
				{
					strcat(his1w,sdr);
					strcat(his1w,":");
					strcat(his1w,row[1]);
					strcat(his1w,"#");
				}
				else if(!strcmp(tmpid,u2id))
				{
					strcat(his1w,rcv);
					strcat(his1w,":");
					strcat(his1w,row[1]);
					strcat(his1w,"#");
				}
			}
			write(leixing,his1w,MAXLINE);
			printf("grrrrrrrrrrrr:%s\n",his1w);	
	
			
		}
		
		else if(a[2]=='2')
		{
			char sdr[20];
			char rcv[20];
			int sdp;
			int i;
			for( i=3,sdp=0;i<len;i++,sdp++)
			{	
				if(a[i]!='#')
				{
					sdr[sdp]=a[i];
				}
				else break;
			}
			i++;
			sdr[sdp]='\0';
			for(sdp=0;i<len;i++,sdp++)
			{
				rcv[sdp]=a[i];
			}
			rcv[sdp]='\0';
						
			char u2id[10];//求得群组的id
			char us2[50]="SELECT gid from grp WHERE gname = '";
			strcat(us2,rcv);
			int lenu2=strlen(us2);			
			us2[lenu2]='\'';
			
			if(mysql_query(conn,us2))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			us2c=mysql_use_result(conn);
			if((row=mysql_fetch_row(us2c))!=NULL)
			{
				//u2id=row[0];
				strcpy(u2id,row[0]);
			}
			//printf("this is point 2\n");
			if(us2c!=NULL) mysql_free_result(us2c);
			
			char his2[170];
			sprintf(his2,"SELECT uname,msg from group_recv where gid = %s order by mid desc limit 0,10",u2id);//这里有\n，不一定能查到
			if(mysql_query(conn,his2))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			//printf("this is point 3\n");
			res=mysql_use_result(conn);//这里用了res,没准出错
			
			char his2w[MAXLINE];
			memset(his2w,0,sizeof(his2w));
			
			//printf("this is point 4\n");
			//char tmpmsg[200];
			while((row=mysql_fetch_row(res))!=NULL)
			{
				//strcpy(tmpid,row[0]);
				
					strcat(his2w,row[0]);
					strcat(his2w,":");
					strcat(his2w,row[1]);
					strcat(his2w,"#");
				
				/*else if(!strcmp(tmpid,u2id))
				{
					strcat(his1w,rcv);
					strcat(his1w,":");
					strcat(his1w,row[1]);
					strcat(his1w,"#");
				}*/
			}
			write(leixing,his2w,MAXLINE);
			printf("grrrrrrrrrrrr:%s\n",his2w);	
		}
		
		
		
	}
	
	else if(a[0]=='0'&&a[1]=='5')//发消息
	{
		printf("the needp is:%s\n",a);
		if(a[2]=='1')//格式：发信人#收信人#msg
		{
			int msg1p;
			char msgsd[30];
			char msgrcv[30];
			char realmsg[256];
			int i;
			for( i=3,msg1p=0;i<len;i++,msg1p++)
			{	
				//needfri[msg1p]=a[i];
				if(a[i]!='#')
				{
					msgsd[msg1p]=a[i];
				}
				else
				{
					i++;
					msgsd[msg1p]='\0';
					break;
				}
				
			}
			
			for(msg1p=0;i<len;i++,msg1p++)
			{
				if(a[i]!='#')
				{
					msgrcv[msg1p]=a[i];
				}
				else
				{
					i++;
					msgrcv[msg1p]='\0';
					break;
				}
			}
			
			for(msg1p=0;i<len;i++,msg1p++)
			{
				realmsg[msg1p]=a[i];
			}
			realmsg[msg1p]='\0';
			
			char lfuid[50]="SELECT uid from user_info where name = '";
			strcat(lfuid,msgsd);
			strcat(lfuid,"'");
			
			if(mysql_query(conn,lfuid))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			res=mysql_use_result(conn);//这里用了res,没准出错
			while((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(msgsd,row[0]);
			}
			if(res!=NULL) mysql_free_result(res);
			
			strcpy(lfuid,"SELECT uid from user_info where name = '");
			strcat(lfuid,msgrcv);
			strcat(lfuid,"'");
			
			if(mysql_query(conn,lfuid))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			res=mysql_use_result(conn);//这里用了res,没准出错
			while((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(msgrcv,row[0]);
			}
			if(res!=NULL) mysql_free_result(res);
			
			char sndqry[400];
			sprintf(sndqry,"insert ignore into private_recv(from_uid,to_uid,msg)values(%s,%s,'%s')",msgsd,msgrcv,realmsg);
			if(mysql_query(conn,sndqry))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			
			if(mysql_affected_rows(conn)) write(leixing,"1",MAXLINE);
			else write(leixing,"0",MAXLINE);
						
		}
		
		if(a[2]=='2')//格式：发信人#群组#msg uname+gid+msg
		{
			int msg1p;
			char msgsd[30];
			char msgrcv[30];
			char realmsg[256];
			int i;
			for( i=3,msg1p=0;i<len;i++,msg1p++)
			{	
				//needfri[msg1p]=a[i];
				if(a[i]!='#')
				{
					msgsd[msg1p]=a[i];
				}
				else
				{
					i++;
					msgsd[msg1p]='\0';
					break;
				}
				
			}
			
			for(msg1p=0;i<len;i++,msg1p++)
			{
				if(a[i]!='#')
				{
					msgrcv[msg1p]=a[i];
				}
				else
				{
					i++;
					msgrcv[msg1p]='\0';
					break;
				}
			}
			
			for(msg1p=0;i<len;i++,msg1p++)
			{
				realmsg[msg1p]=a[i];
			}
			realmsg[msg1p]='\0';

			char schgid[50]="SELECT gid from grp where gname = '";
			strcat(schgid,msgrcv);
			strcat(schgid,"'");

			if(mysql_query(conn,schgid))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}

			res=mysql_use_result(conn);//这里用了res,没准出错
			while((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(msgrcv,row[0]);
			}
			if(res!=NULL) mysql_free_result(res);
			
			char sndqry[400];
			sprintf(sndqry,"insert ignore into group_recv(uname,gid,msg)values('%s',%s,'%s')",msgsd,msgrcv,realmsg);
			if(mysql_query(conn,sndqry))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			
			if(mysql_affected_rows(conn)) write(leixing,"1",MAXLINE);
			else write(leixing,"0",MAXLINE);
		}
	}
	
	else if(a[0]=='0'&&a[1]=='6')//发文件 //06+发送人+#+对象+#+文件名+#+大小
	{
		printf("the needp is:%s\n",a);
		char fsdr[30];
		char frcvr[30];
		char serfname[256];
		char cap[50];
		int fi;
		int charhd;
		for(fi=2,charhd=0;fi<len;fi++,charhd++)
		{
			if(a[fi]!='#')
			{
				fsdr[charhd]=a[fi];
			}
			else
			{
				fi++;
				fsdr[charhd]='\0';
				break;
			}
			
		}

		for(charhd=0;fi<len;fi++,charhd++)
		{
			if(a[fi]!='#')
			{
				frcvr[charhd]=a[fi];
			}
			else
			{
				fi++;
				frcvr[charhd]='\0';
				break;
			}
			
		}

		for(charhd=0;fi<len;fi++,charhd++)
		{
			if(a[fi]!='#')
			{
				serfname[charhd]=a[fi];
			}
			else
			{
				fi++;
				serfname[charhd]='\0';
				break;
			}
			
		}

		for(charhd=0;fi<len;fi++,charhd++)
		{
			cap[charhd]=a[fi];
		}
		cap[charhd]='\0';

		char fqry[300];
		sprintf(fqry,"insert ignore into filemng(fsdr,frcv,fname,size)values('%s','%s','%s',%s)",fsdr,frcvr,serfname,cap);
		if(mysql_query(conn,fqry))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
		}
			
		if(mysql_affected_rows(conn)) write(leixing,"1",MAXLINE);
		else write(leixing,"0",MAXLINE);
		//char num[20];
		//atoi(cap,num,10);//这可能有问题
		std::fstream fs;
		fs.open(serfname,std::fstream::out|std::fstream::binary|std::fstream::trunc);
			if(!fs) printf("文件没找到啊\n");
			int fen;
			int fxs=atoi(cap);
			while(fxs)//文件大小
			{
				printf("this is shengyu :%d\n",fxs);
				fen=recv(leixing,recv_buff,MAXLINE,0);
				if(fen>0)//读入的大小
				{
					fs.write(recv_buff,fen);
					fxs-=fen;
				}
				printf("收到一个包\n");
				
			
			}
			fs.close();
		
	}
	
	else if(a[0]=='1'&&a[1]=='1')
	{
		int yi;
		char sdt[256];
		int pp;
		for(yi=2,pp=0;yi<len;yi++,pp++)
		{
			sdt[pp]=a[yi];
		}
		sdt[pp]='\0';
		
		std::fstream fs1;
		fs1.open(sdt,std::fstream::in|std::fstream::binary);
		fs1.seekg(0,std::fstream::beg);
		
		while(!fs1.eof())
		{
			usleep(80000);
			//printf("%s\n",send_buff);
			fs1.read(send_buff,MAXLINE);
			int ll=fs1.gcount();
			send(leixing,send_buff,ll,0);
			printf("发去一个包\n");
		}
		fs1.close();
	}
	
	else if(a[0]=='0'&&a[1]=='7')
	{
		int skfi;
		int frt;
		char mein[30];
		char frin[30];
		for(skfi=2,frt=0;skfi<len;skfi++,frt++)
		{
			if(a[skfi]!='#')
			{
				mein[frt]=a[skfi];
			}
			else
			{
				skfi++;
				mein[frt]='\0';
				break;
			}
			
		}
		
		for(frt=0;skfi<len;skfi++,frt++)
		{
			frin[frt]=a[skfi];
		}
		frin[frt]='\0';
		
		char skfme[50]="select uid from user_info where name = '";
		strcat(skfme,mein);
		strcat(skfme,"'");
		if(mysql_query(conn,skfme))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
		}
		
		char meid[10];
		res=mysql_use_result(conn);//这里用了res,没准出错
		while((row=mysql_fetch_row(res))!=NULL)
		{
				strcpy(meid,row[0]);
		}
		if(res!=NULL) mysql_free_result(res);
		
		char skfhim[50]="select uid from user_info where name = '";
		strcat(skfhim,frin);
		strcat(skfhim,"'");
		if(mysql_query(conn,skfhim))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
		}
		
		char frid[10];
		res=mysql_use_result(conn);//这里用了res,没准出错
		while((row=mysql_fetch_row(res))!=NULL)
		{
				strcpy(frid,row[0]);
		}
		if(res!=NULL) mysql_free_result(res);
		
		char isrt[100];
		memset(isrt,0,sizeof(isrt));
		
		sprintf(isrt,"insert ignore into friend(uid,fuid)values(%s,%s),(%s,%s)",meid,frid,frid,meid);
		if(mysql_query(conn,isrt))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
		}
			
		if(mysql_affected_rows(conn)) write(leixing,"1",MAXLINE);
		else write(leixing,"0",MAXLINE);
		
		
	}
	
	else if(a[0]=='0'&&a[1]=='8')//查询管理群组信息
	{
		printf("the needp is:%s\n",a);
		char needgr[25];
		int needp;
		int i;
		for( i=2,needp=0;i<len;i++,needp++)
		{	
			needgr[needp]=a[i];
		}
		needgr[needp]='\0';
		printf("the needp is:%s\n",needgr);
		
		
		
		char grtmp[200];
		memset(grtmp,0,sizeof(grtmp));
		sprintf(grtmp,"select gname from grp where lordid in (SELECT uid from user_info WHERE name = '%s') ",needgr);
		
		memset(admlist,0,sizeof(admlist));
		
		if(mysql_query(conn,grtmp))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	
	resgr=mysql_use_result(conn);
	while((row=mysql_fetch_row(resgr))!=NULL)
	{
		strcat(admlist,row[0]);
		strcat(admlist,"#");
	}
	write(leixing,admlist,MAXLINE);
	//printf("grrrrrrrrrrrr:%s\n",grlist);	
	}
	
	else if(a[0]=='0'&&a[1]=='9')
	{
		char laucher[30];
		char grname[30];
		char grmname[10][30];
		int p;
		int hdr;
		for(p=2,hdr=0;p<len;p++,hdr++)
		{
			if(a[p]!='#') laucher[hdr]=a[p];
			else 
			{
				p++;
				laucher[hdr]='\0';
				break;
			}
		}
		
		for(hdr=0;p<len;p++,hdr++)
		{
			if(a[p]!='#')
			{
				grname[hdr]=a[p];
			}
			else
			{
				p++;
				grname[hdr]='\0';
				break;
			}
		}
		
		int innerp;
		for(hdr=0,innerp=0;p<len;)
		{
			if(a[p]!='#')
			{
				grmname[hdr][innerp]=a[p];
				p++;
				innerp++;
			}
			else
			{
				grmname[hdr][innerp]='\0';
				p++;
				printf("hdrrrrrr:%s\n",grmname[hdr]);
				hdr++;
				innerp=0;
			}
		}
		//hdr是多少就有几个好友
		//insert ignore into grp(lordid,gname)values((select uid from user_info where name ='haha'),'hasking')
		char grmkr[110];
		sprintf(grmkr,"insert ignore into grp(lordid,gname)values((select uid from user_info where name ='%s'),'%s')",laucher,grname);
		if(mysql_query(conn,grmkr))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
			mysql_close(conn);
			exit(1);
		}
		if(mysql_affected_rows(conn)) 
		{
			//取得群id和每个人的id
			char getpid[50];
			sprintf(getpid,"select gid from grp where gname='%s'",grname);
			if(mysql_query(conn,getpid))
			{
				fprintf(stderr,"%s\n",mysql_error(conn));
				mysql_close(conn);
				exit(1);
			}
			char pid[20];
			memset(pid,0,sizeof(pid));
			resgr=mysql_use_result(conn);
			
			while((row=mysql_fetch_row(resgr))!=NULL)
			{
				strcat(pid,row[0]);
			}//获得了群pid
			if(resgr!=NULL) mysql_free_result(resgr);
			
			int quni;
			char addmb[150];
			//INSERT IGNORE INTO group_member(gid,uid) VALUES(1,(SELECT uid from user_info where name='idiot'))
			for(quni=0;quni<hdr;quni++)
			{
				memset(addmb,0,sizeof(addmb));
				sprintf(addmb,"INSERT IGNORE INTO group_member(gid,uid) VALUES(%s,(SELECT uid from user_info where name='%s'))",pid,grmname[quni]);
				if(mysql_query(conn,addmb))
				{
					fprintf(stderr,"%s\n",mysql_error(conn));
					mysql_close(conn);
					exit(1);
				}
			}
			
			write(leixing,"1",MAXLINE);
		}
		else write(leixing,"0",MAXLINE);
		
	}
	
	else if(a[0]=='1'&&a[1]=='0')//查询管理群组信息
	{
		printf("the needp is:%s\n",a);
		char needgr[25];
		int needp;
		int i;
		for( i=2,needp=0;i<len;i++,needp++)
		{	
			needgr[needp]=a[i];
		}
		needgr[needp]='\0';
		printf("the needp is:%s\n",needgr);
		
		
		
		char grtmp[100];
		memset(grtmp,0,sizeof(grtmp));
		sprintf(grtmp,"SELECT fname from filemng where frcv = '%s' ",needgr);
		
		memset(myfilelist,0,sizeof(myfilelist));
		
		if(mysql_query(conn,grtmp))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	
	resgr=mysql_use_result(conn);
	while((row=mysql_fetch_row(resgr))!=NULL)
	{
		strcat(myfilelist,row[0]);
		strcat(myfilelist,"#");
	}
	write(leixing,myfilelist,MAXLINE);
	//printf("grrrrrrrrrrrr:%s\n",grlist);	
	}
	
}
void *do_work(void *arg)
{
	int n,i;
	char reply[MAXLINE];
	struct s_info *ts = (struct s_info*)arg;
	char buf[100000];//在线程自己的用户空间栈开辟的,该线程运行结束的时候,主控线程就不能操作这块内存了
	char rebuf[MAXLINE];
	char str[INET_ADDRSTRLEN];//INET_ADDRSTRLEN 是宏16个字节
 	
	//在创建线程前设置线程创建属性,设为分离态,效率高
	pthread_detach(pthread_self());
 
	while (1)// would accept null strings 
	{
	n = read(ts->connfd, buf, MAXLINE);
	if (n == 0) 
	{
		printf("the other side has been closed.\n");
		break;
	}
	printf("received from %s at PORT %d\n",inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr, str, sizeof(str)),ntohs((*ts).cliaddr.sin_port));
 	printf("buf:%s\n",buf);
	chk(buf,ts->connfd,n);	
	memset(buf,0,sizeof(buf));
	}
	
	close(ts->connfd);
}
int main()
{
	char server[]="localhost";
	char user[]="root";
	char password[]="123456";
	char database[]="mychatroom";
	conn = mysql_init(NULL);
	
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		exit(1);
	}
	//*********************no problem******************
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	int i = 0;
	pthread_t tid;
	struct s_info ts[3497];
 
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
 
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//set ip as 0.0.0.0
	servaddr.sin_port = htons(SERV_PORT);//SET IT AS 5000
	
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, 20);
 
	printf("Accepting connections ...\n");
/***********************************上面一小段应该没问题*************************************/
	while (1) 
	{
		cliaddr_len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
		ts[i].cliaddr = cliaddr;
		ts[i].connfd = connfd;
/* 达到线程最大数时，pthread_create出错处理, 增加服务器稳定性 */
		pthread_create(&tid, NULL, do_work, (void*)&ts[i]);//把accept得到的客户端信息传给线程，让线程去和客户端进行数据的收发
		i++;
	}
	//else printf("ok\n");
return 0;
}
