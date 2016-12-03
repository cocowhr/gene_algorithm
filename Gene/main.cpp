#include <WinSock2.h>  
#include "mysql.h"  
#pragma comment(lib,"wsock32.lib")  
#pragma comment(lib,"libmysql.lib")  
#include <atlstr.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
#define NS 10
#define ST 2000000//T中的序列个数 T为参考模式库
#define LEN 7
#define CNUM 1000//群体规模
#define NUM 5//迭代次数
typedef int* Seq ;
class Chrom
{
public:
	Chrom() 
	{
		seq=new int[LEN];
		M=0;//记得要清0
		fit=0;
	}
	Chrom(const Chrom& a)
	{
		seq=new int[LEN];
		for(int i=0;i<LEN;i++)
		{
			seq[i]=a.seq[i];
		}
		M=a.M;
		fit=a.fit;
		MM.assign(a.MM.begin(),a.MM.end());
	}
	~Chrom() {}
	Seq seq ;
	vector<int>MM;
	double M;
	double fit;//适应值
};
bool Comp(Chrom& first,Chrom& second)
{
	return first.fit > second.fit;
}
typedef struct Code                           // 结构体类型，为单个染色体的结构；
{
	int id;//0号预留给*
	double count;
}code;                                       
//定义将会用到的几个函数；
void evpop (vector<Chrom>& popcurrent,vector<code>&codes,vector<Seq>ref);//进行种群的初始
void pickchroms (vector<Chrom>& popcurrent,vector<Chrom>& popnext);//选择操作
void crossover (vector<Chrom>& popcurrent,vector<Chrom>& popnext,vector<code>&codes,vector<Seq>ref);//交叉操作
void mutation(vector<Chrom>& popnext,vector<code>&codes,vector<Seq>&ref);//突变

void main ()                                    // 主函数；
{
	vector<Seq>ref;
	MYSQL mysql;  
	MYSQL_RES *result;  
	MYSQL_ROW row;  
	mysql_init(&mysql);  
	mysql_real_connect(&mysql, "localhost", "root", "root", "genet", 3306, NULL, 0); 
	CString strSQL;
	strSQL.Format("SELECT * FROM seq");
	mysql_query(&mysql, strSQL);
	result = mysql_store_result(&mysql);  
	while ((row = mysql_fetch_row(result))!=NULL)  
	{  
		Seq temp=new int[LEN];	
		for(int i=0;i<LEN;i++)
		{
			temp[i]=atoi(row[i+1]);
		}
		ref.push_back(temp);
	}  
	vector<code> codes;
	code a = {1,0.1};
	code b= {2,0.1};
	code c = {3,0.1};
	code d = {4,0.1};
	code e = {5,0.1};
	code f= {6,0.1};
	code g = {7,0.1};
	code h = {8,0.1};
	code i = {9,0.1};
	code j= {10,0.1};
	codes.push_back(a);
	codes.push_back(b);
	codes.push_back(c);
	codes.push_back(d);
	codes.push_back(e);
	codes.push_back(f);
	codes.push_back(g);
	codes.push_back(h);
	codes.push_back(i);
	codes.push_back(j);
	vector<Chrom>popcurrent;                        // 初始种群规模为；
	vector<Chrom>popnext(popcurrent);                           // 更新后种群规模仍为；
	srand(time(0));
	evpop(popcurrent,codes,ref);
	for(int i =0;i< NUM;i ++)                          // 开始迭代；
	{
		printf("\ni = %d\n" ,i);                 // 输出当前迭代次数；
		crossover(popcurrent,popnext,codes,ref);                     // 交叉得到新个体；
		/*	for(int j=0;j<CNUM;j++)
		{
		for(int k=0;k<LEN;k++)
		{
		cout<<popnext[j].seq[k]<<" ";
		}
		cout<<endl;
		cout<<popnext[j].fit<<endl;
		}*/
		mutation(popnext,codes,ref);                      // 变异得到新个体 
		/*for(int j=0;j<CNUM;j++)
		{
		for(int k=0;k<LEN;k++)
		{
		cout<<popnext[j].seq[k]<<" ";
		}
		cout<<endl;
		cout<<"M:  "<<popnext[j].M<<endl;
		for(int ii=0;ii<popnext[j].M;ii++)
		{
		cout<<"M"<<ii<<":  "<<popnext[j].MM[ii]<<endl;
		}
		cout<<"fit:  "<<popnext[j].fit<<endl;
		}*/
		pickchroms(popcurrent,popnext); // 挑选优秀个体； 
		/*cout<<"*****************************"<<endl<<endl;
		for(int j=0;j<CNUM;j++)
		{
		for(int k=0;k<LEN;k++)
		{
		cout<<popnext[j].seq[k]<<" ";
		}
		cout<<endl;
		cout<<"M:  "<<popnext[j].M<<endl;
		for(int ii=0;ii<popnext[j].M;ii++)
		{
		cout<<"M"<<ii<<":  "<<popnext[j].MM[ii]<<endl;
		}
		cout<<"fit:  "<<popnext[j].fit<<endl;
		}*/
		popcurrent.assign(popnext.begin(),popnext.end());
	}
	for(int j=0;j<CNUM;j++)
	{
		strSQL.Format("INSERT INTO  rules (`seq1`, `seq2`, `seq3`, `seq4`, `seq5`, `seq6`, `seq7`, `fit`)  VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%lf')"
			,popcurrent[j].seq[0],
			popcurrent[j].seq[1],
			popcurrent[j].seq[2],
			popcurrent[j].seq[3],
			popcurrent[j].seq[4],
			popcurrent[j].seq[5],
			popcurrent[j].seq[6],
			popcurrent[j].fit);
		mysql_query(&mysql, strSQL);	
	}
		
}                                             
void evpop (vector<Chrom>&popcurrent,vector<code>&codes,vector<Seq>ref)   // 函数：随机生成初始种群；
{
	int random ;
	for(int i=0;i<CNUM;i++)
	{
		Chrom chrom;
		for(int j=0;j<LEN;j++)
		{
			random=rand ()%NS;                     // 产生一个随机值
			chrom.seq[j]=codes[random].id;
		}
		popcurrent.push_back(chrom);
	}
	for(int i=0;i<CNUM;i++)
	{
		int random1=rand ()%3;
		for(int j=0;j<random1;j++)
		{
			random=rand ()%LEN; 
			popcurrent[i].seq[random]=0;
		}
	}
	for(int i=0;i<CNUM;i++)
	{
		for(int j=0;j<ST;j++)
		{
			bool eq=true;
			for(int k=0;k<LEN;k++)
			{
				int pop=popcurrent[i].seq[k];
				if(0!=pop&&ref[j][k]!=pop)
				{
					eq=false;
					break;
				}
			}
			if(eq)
			{
				popcurrent[i].MM.push_back(j);
				popcurrent[i].M++;
			}
		}
	}
	double sum=0;
	for(int i=0;i<CNUM;i++)
	{
		int E=LEN;
		double ci=0;
		for(int j=0;j<LEN;j++)
		{
			if(popcurrent[i].seq[j]!=0)
			{
				ci+=codes[popcurrent[i].seq[j]-1].count;
			}
			else
			{
				E--;
			}
		}
		popcurrent[i].fit=ci*popcurrent[i].M*pow(NS,E)/ST;
		sum+=popcurrent[i].fit;
		//cout<<"E: "<<E<<endl;
		//cout<<"Ns^E(Patterni)/S(T): "<<pow(NS,E)/ST<<endl;
		//cout<<"ci: "<<ci<<endl;
		//cout<<"M:"<<popcurrent[i].M<<endl;
		//cout<<"fit:    "<<popcurrent[i].fit<<endl;
	}             
}                                       
void pickchroms (vector<Chrom>& popcurrent,vector<Chrom>& popnext)          // 函数：选择个体；
{
	//ofstream cur("current.txt");
	//ofstream next("next.txt");
	//ofstream tempns("tempns.txt");
	//ofstream temps("temps.txt");
	//ofstream nnext("nnext.txt");
	//vector<Chrom> temp;
	//temp.insert(temp.end(), popcurrent.begin(), popcurrent.end());
	//for(int j=0;j<popcurrent.size();j++)
	//{
	//	for(int k=0;k<LEN;k++)
	//	{
	//		cur<<popcurrent[j].seq[k]<<" ";
	//	}
	//	cur<<endl;
	//	cur<<"M:  "<<popcurrent[j].M<<endl;
	//	for(int ii=0;ii<popcurrent[j].M;ii++)
	//	{
	//		cur<<"M"<<ii<<":  "<<popcurrent[j].MM[ii]<<endl;
	//	}
	//	cur<<"fit:  "<<popcurrent[j].fit<<endl;
	//}
	//for(int j=0;j<popnext.size();j++)
	//{
	//	for(int k=0;k<LEN;k++)
	//	{
	//		next<<popnext[j].seq[k]<<" ";
	//	}
	//	next<<endl;
	//	next<<"M:  "<<popnext[j].M<<endl;
	//	for(int ii=0;ii<popnext[j].M;ii++)
	//	{
	//		next<<"M"<<ii<<":  "<<popnext[j].MM[ii]<<endl;
	//	}
	//	next<<"fit:  "<<popnext[j].fit<<endl;
	//}
	//temp.insert(temp.end(), popnext.begin(), popnext.end());
	//for(int j=0;j<temp.size();j++)
	//{
	//	for(int k=0;k<LEN;k++)
	//	{
	//		tempns<<temp[j].seq[k]<<" ";
	//	}
	//	tempns<<endl;
	//	tempns<<"M:  "<<temp[j].M<<endl;
	//	for(int ii=0;ii<temp[j].M;ii++)
	//	{
	//		tempns<<"M"<<ii<<":  "<<temp[j].MM[ii]<<endl;
	//	}
	//	tempns<<"fit:  "<<temp[j].fit<<endl;
	//}
	//sort(temp.begin(),temp.end(),Comp);
	//for(int j=0;j<temp.size();j++)
	//{
	//	for(int k=0;k<LEN;k++)
	//	{
	//		temps<<temp[j].seq[k]<<" ";
	//	}
	//	temps<<endl;
	//	temps<<"M:  "<<temp[j].M<<endl;
	//	for(int ii=0;ii<temp[j].M;ii++)
	//	{
	//		temps<<"M"<<ii<<":  "<<temp[j].MM[ii]<<endl;
	//	}
	//	temps<<"fit:  "<<temp[j].fit<<endl;
	//}
	//popnext.assign(temp.begin(),temp.begin()+NUM-1);
	//for(int j=0;j<popnext.size();j++)
	//{
	//	for(int k=0;k<LEN;k++)
	//	{
	//		nnext<<popnext[j].seq[k]<<" ";
	//	}
	//	nnext<<endl;
	//	nnext<<"M:  "<<popnext[j].M<<endl;
	//	for(int ii=0;ii<popnext[j].M;ii++)
	//	{
	//		nnext<<"M"<<ii<<":  "<<popnext[j].MM[ii]<<endl;
	//	}
	//	nnext<<"fit:  "<<popnext[j].fit<<endl;
	//}
	sort(popcurrent.begin(),popcurrent.end(),Comp);
	sort(popnext.begin(),popnext.end(),Comp);
	vector<Chrom> temp;
	int i=0,j=0;
	int nextlen=popnext.size();
	for(int k=0;k<CNUM;k++)
	{
		if(j>=nextlen||popcurrent[i].fit>popnext[j].fit)
		{
			Chrom ctemp(popcurrent[i]);
			temp.push_back(ctemp);
			i++;
		}
		else
		{
			Chrom ctemp(popnext[j]);
			temp.push_back(ctemp);
			j++;
		}
	}
	popnext.assign(temp.begin(),temp.end()); 
}   
void crossover (vector<Chrom>& popcurrent,vector<Chrom>& popnext,vector<code>&codes,vector<Seq>ref)           // 函数：交叉操作；
{
	int random=rand()%100+1;

	for(int i=0;i<CNUM-1;i++)
	{
		for(int j=i+1;j<CNUM;j++)
		{
			Chrom c1;
			Chrom c2;
			if(1>random)//交叉概率0.8
			{
				int E1=LEN;
				double ci1=0;
				int E2=LEN;
				double ci2=0;
				random=rand ()%LEN;// 随机产生交叉点；
				for(int k=0;k< random;k++)                   
				{
					c1.seq[k]= popcurrent[i].seq [k];   // child 1 cross over
					c2.seq[k]= popcurrent[j].seq [k];   // child 2 cross over
					if(c1.seq[k]!=0)
					{
						ci1+=codes[c1.seq[k]-1].count;
					}
					else
					{
						E1--;
					}
					if(c2.seq[k]!=0)
					{
						ci2+=codes[c2.seq[k]-1].count;
					}
					else
					{
						E2--;
					}
				}
				for(int k=random;k<LEN;k++)                   
				{
					c1.seq[k]= popcurrent[j].seq [k];   // child 1 cross over
					c2.seq[k]= popcurrent[i].seq [k];   // child 2 cross over
					if(c1.seq[k]!=0)
					{
						ci1+=codes[c1.seq[k]-1].count;
					}
					else
					{
						E1--;
					}
					if(c2.seq[k]!=0)
					{
						ci2+=codes[c2.seq[k]-1].count;
					}
					else
					{
						E2--;
					}
				}
				for(int k=0;k<ST;k++)
				{
					bool eq1=true;
					bool eq2=true;
					for(int m=0;m<LEN;m++)
					{
						int refkm=ref[k][m];
						if(eq1)
						{
							if(0!=c1.seq[m]&&refkm!=c1.seq[m])
							{
								eq1=false;
							}
						}
						if(eq2)
						{
							if(0!=c2.seq[m]&&refkm!=c2.seq[m])
							{
								eq2=false;
							}
						}
						if(!eq1&&!eq2)
						{
							break;
						}
					}
					if(eq1)
					{
						c1.MM.push_back(j);
						c1.M++;
					}
					if(eq2)
					{
						c2.MM.push_back(j);
						c2.M++;
					}
				}
				//for(int k=0;k<LEN;k++)
				//{
				//	cout<<c1.seq[k]<<" ";
				//}
				//cout<<endl;
				//for(int k=0;k<LEN;k++)
				//{
				//	cout<<c2.seq[k]<<" ";
				//}
				//	cout<<endl;
				double sum1=0,sum2=0;
				for(int i=0;i<CNUM;i++)
				{
					c1.fit=ci1*c1.M*pow(NS,E1)/ST;
					sum1+=c1.fit;
					c2.fit=ci1*c2.M*pow(NS,E2)/ST;
					sum2+=c2.fit;
					/*cout<<"E: "<<E1<<endl;
					cout<<"Ns^E(Patterni)/S(T): "<<pow(NS,E1)/ST<<endl;
					cout<<"ci: "<<ci1<<endl;
					cout<<"M:"<<c1.M<<endl;
					cout<<"fit:    "<<c1.fit<<endl;*/
				}
				popnext.push_back(c1);
				popnext.push_back(c2);
			}
		}
	}
}                                          
void mutation(vector<Chrom>& popnext,vector<code>&codes,vector<Seq>&ref)             // 函数：变异操作；
{
	int random=0;
	double sum=0;
	int cnumnow=popnext.size();
	for(int i=0;i<cnumnow;i++)
	{
		int E=LEN;
		double ci=0;
		for(int j=0;j<LEN;j++)
		{
			random=rand ()%100;  // 随机产生到之间的数；
			//变异概率Pm取5%
			if(random<5)                              // random<5的概率为5%
			{
				random=rand()%NS;
				popnext[i].seq[j]=random+1;				
			}
			/*	cout<<popnext[i].seq[j]<<" ";*/
			if(popnext[i].seq[j]!=0)
			{
				ci+=codes[popnext[i].seq[j]-1].count;
			}
			else
			{
				E--;
			}
		}
		//cout<<endl;
		popnext[i].M=0;
		popnext[i].MM.clear();
		for(int j=0;j<ST;j++)
		{
			bool eq=true;
			for(int k=0;k<LEN;k++)
			{
				int pop=popnext[i].seq[k];
				if(0!=pop&&ref[j][k]!=pop)
				{
					eq=false;
					break;
				}
			}
			if(eq)
			{
				popnext[i].MM.push_back(j);
				popnext[i].M++;
			}
		}
		popnext[i].fit=ci*popnext[i].M*pow(NS,E)/ST;
		sum+=popnext[i].fit;
		/*	cout<<"E: "<<E<<endl;
		cout<<"Ns^E(Patterni)/S(T): "<<pow(NS,E)/ST<<endl;
		cout<<"ci: "<<ci<<endl;
		cout<<"M:"<<popnext[i].M<<endl;
		for(int ii=0;ii<popnext[i].M;ii++)
		{
		cout<<"M"<<ii<<":  "<<popnext[i].MM[ii]<<endl;
		}

		cout<<"fit:    "<<popnext[i].fit<<endl;*/
	}    
	/*for(int ii=0;ii<CNUM;ii++)
	{
	for(int j=0;j<LEN;j++)
	{
	cout<<popnext[ii].seq[j]<<" ";
	}
	cout<<endl;
	}
	cout<<endl<<endl;;*/

}   