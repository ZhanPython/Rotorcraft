// ImageProcess.cpp: implementation of the ImageProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int BACKGROUNDTHICK=30;
int TARGETGATESCALE=160;
int COMPRESSEDIMAGEGATESCALE=1;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageProcess::CImageProcess(unsigned char *pimagadata,
							 unsigned char *pup,
							 unsigned char *pdown,
							 unsigned char *pleft,
							 unsigned char *pright,
							 int iHeight,
							 int iWidth,
							 int imagethick,
							 int imagelength)
{
	PutImage(pimagadata,iHeight,iWidth);
	
	PutUpBackGround(pup,imagethick,imagelength);
	PutDownBackGround(pdown,imagethick,imagelength);
	PutLeftBackGround(pleft,imagelength,imagethick);
	PutRightBackGround(pright,imagelength,imagethick);
	/////////////////////////////////////////////////////////////////////
	for(int i=0;i<256;i++)
	{
		m_iImageZFT[i]=0;
		m_iBackgroundZFT[i]=0;
	}
}

CImageProcess::~CImageProcess()
{
	delete m_pchrGrayImageData1;
}

void CImageProcess::ImageProcessing()
{
	GetImageZFT();//获取前景直方图
	GetBackgroundZFT();//获取背景直方图
	//zzlb(m_pchrGrayImageData,m_iHeight,m_iWidth);

	//zzlb(m_pchrUpBackGround,m_iHeight_Up,m_iWidth_Up);
	//zzlb(m_pchrDownBackGround,m_iHeight_Down,m_iWidth_Down);
	//zzlb(m_pchrLeftBackGround,m_iHeight_Left,m_iWidth_Left);
	//zzlb(m_pchrRightBackGround,m_iHeight_Right,m_iWidth_Right);

	ImageEZH();//前景图像二值化
	//BlackWhiteImageDilationOnePixel();
	m_iObjectNum=ImageProcessmbfg(m_pchrGrayImageData1,m_iHeight,m_iWidth);//连通域分割完毕
}
void CImageProcess::PutImage(unsigned char *pch,int ih,int iw)
{	
	m_pchrGrayImageData=pch;
	m_iHeight=ih;
	m_iWidth=iw;
	m_pchrGrayImageData1= new unsigned char [m_iHeight*m_iWidth];
}
void CImageProcess::PutUpBackGround(unsigned char* pch,int iHeight ,int iWidth)
{
	m_pchrUpBackGround=pch;
	m_iHeight_Up=iHeight;
	m_iWidth_Up=iWidth;

}
void CImageProcess::PutDownBackGround(unsigned char *pch,int iHeight ,int iWidth)
{
	m_pchrDownBackGround=pch;
	m_iHeight_Down=iHeight;
	m_iWidth_Down=iWidth;

}
void CImageProcess::PutLeftBackGround(unsigned char* pch,int iHeight ,int iWidth)
{
	m_pchrLeftBackGround=pch;
	m_iHeight_Left=iHeight;
	m_iWidth_Left=iWidth;
}
void CImageProcess::PutRightBackGround(unsigned char* pch,int iHeight ,int iWidth)
{
	m_pchrRightBackGround=pch;
	m_iHeight_Right=iHeight;
	m_iWidth_Right=iWidth;
}
void CImageProcess::GetImageZFT()
{
	int x,y;
	for(x=0;x<m_iHeight;x++)
	{
		for(y=0;y<m_iWidth;y++)
		{
			m_iImageZFT[m_pchrGrayImageData[x*m_iWidth+y]]++;
		}
	}
}
void CImageProcess::GetBackgroundZFT()//背景直方图
{
	int x,y;
	if(m_pchrUpBackGround!=NULL)
	{
		for(x=0;x<m_iHeight_Up;x++)
		{
			for(y=0;y<m_iWidth_Up;y++)
			{
				m_iBackgroundZFT[m_pchrUpBackGround[x*m_iWidth_Up+y]]++;
			}
		}
	}
	if(m_pchrDownBackGround!=NULL)
	{
		for(x=0;x<m_iHeight_Down;x++)
		{
			for(y=0;y<m_iWidth_Down;y++)
			{
				m_iBackgroundZFT[m_pchrDownBackGround[x*m_iWidth_Down+y]]++;
			}
		}
	}
	if(m_pchrLeftBackGround!=NULL)
	{
		for(x=0;x<m_iHeight_Left;x++)
		{
			for(y=0;y<m_iWidth_Left;y++)
			{
				m_iBackgroundZFT[m_pchrLeftBackGround[x*m_iWidth_Left+y]]++;
			}
		}
	}
	if(m_pchrRightBackGround!=NULL)
	{
		for(x=0;x<m_iHeight_Right;x++)
		{
			for(y=0;y<m_iWidth_Right;y++)
			{
				m_iBackgroundZFT[m_pchrRightBackGround[x*m_iWidth_Right+y]]++;
			}
		}
	}
}
void CImageProcess::ImageEZH()//图像二值化
{
	int x,y;
	for(x=0;x<m_iHeight;x++)
	{
		for(y=0;y<m_iWidth;y++)
		{
			if(m_iBackgroundZFT[m_pchrGrayImageData[x*m_iWidth+y]]<=5)
				m_pchrGrayImageData1[x*m_iWidth+y]=1;
			else
				m_pchrGrayImageData1[x*m_iWidth+y]=0;
		}
	}
}

int CImageProcess::ImageProcessmbfg(unsigned char *b,int row,int col)//目标连通域分割
{
	int i,j,x,y,ii,jj;//循环变量
	int m,n;
	int k=1;//记录目标个数变量
	int win=1;//3*3窗口个数变量，即递归深度变量
	int flag=0;
	unsigned char *a=new unsigned char [row*col];
	struct WZH
	{
		unsigned char x;
		unsigned char y;
	};
	WZH *wzh=new WZH [row*col];
	//unsigned char *point[81*81];
	for(i=0;i<row;i++)
		for(j=0;j<col;j++)
		{
			a[i*col+j]=b[i*col+j];
			b[i*col+j]=0;
			//point[i*col+j]=NULL;
		}
	for(ii=0;ii<row;ii++)
	{
		for(jj=0;jj<col;jj++)
		{
			if(a[ii*col+jj]==0||b[ii*col+jj]!=0)
				continue;
			i=ii;
			j=jj;
next_win1:	
			for(x=i-1;x<=i+1;x++)
			{
				for(y=j-1;y<=j+1;y++)
				{
					
					if(x==i&&y==j) continue;
					else if(x<0||x>=row||y<0||y>=col) continue;
					else if(a[x*col+y]==1&&b[x*col+y]==0)
					{
						flag=1;
						if(k<256)
							b[x*col+y]=k;
						else
							b[x*col+y]=255;
						//记录该窗口中心所在a中的位置，即i,j
						//记录它所在此窗口中的位置
						wzh[x*col+y].x=i;
						wzh[x*col+y].y=j;
						//point[x*col+y]=&a[x*col+y];
						i=x;
						j=y;
						win++;
						goto next_win1;
					}
next_win:		x=x;
				}
			}
				if(win>1)
				{
					//一个3*3小窗口处理完
					//回到该窗口的进入位置，
					//即找到该中心(i,j)所在上一窗口的位置
					m=i;n=j;
					x=m;
					y=n;
					i=wzh[m*col+n].x;
					j=wzh[m*col+n].y;
					win--;//窗口个数减1
					goto next_win;
				}
				if(flag==1)
				{
					k++;
					flag=0;
				}
		}
	}
	free(a);
	free(wzh);
	return(k-1);
}
void CImageProcess::sort(unsigned char *buffer)
{
	int i,j;
	unsigned char mid;
	for(i=0;i<3-1;i++)
		for(j=i+1;j<3;j++)
		{
			if(buffer[i]>buffer[j])
			{
				mid=buffer[j];
				buffer[j]=buffer[i];
				buffer[i]=mid;
			}
		}
}
void CImageProcess::zzlb(unsigned char  *A,int row,int column)          //3*3
{
	//{
	//	printf("get in lb pro\n");
		//return;
	//}
	if(A==NULL)
		return;
	int	i,j;
	unsigned char  *B=new unsigned char [row*column];
	for(i=0;i<row;i++)
		for(j=0;j<column;j++)
		{
			B[i*column+j]=A[i*column+j];
		}
	unsigned char buffer[3][3];
	for(i=0;i<row;i++)
		for(j=0;j<column;j++)
		{
			//printf("the %d time\n",i*j);
			if(i==0&&j==0)          //northwest
			{
				buffer[0][0]=B[i*column+j];
				buffer[0][1]=B[i*column+j+1];
				buffer[0][2]=B[(i+1)*column+j];
				sort(buffer[0]);

				A[i*column+j]=buffer[0][1];
				continue;
			}
			else if(i==0&&j==column-1)         //northeast
			{
				buffer[0][0]=B[i*column+j];
				buffer[0][1]=B[i*column+j-1];
				buffer[0][2]=B[(i+1)*column+j];
				sort(buffer[0]);

				A[i*column+j]=buffer[0][1];
				continue;
			}
			else if(i==row-1&&j==0)                       //southwest
			{
				buffer[0][0]=B[i*column+j];
				buffer[0][1]=B[i*column+j+1];
				buffer[0][2]=B[(i-1)*column+j];
				sort(buffer[0]);

				A[i*column+j]=buffer[0][1];
				continue;
			}
			else if(i==row-1&&j==column-1)              //southeast
			{
				buffer[0][0]=B[i*column+j];
				buffer[0][1]=B[i*column+j-1];
				buffer[0][2]=B[(i-1)*column+j];
				sort(buffer[0]);

				A[i*column+j]=buffer[0][1];
				continue;
			}
			else
			{
				if(i-1<0)//当在第一行时，	    //up
				{
					buffer[0][0]=B[i*column+j-1];
					buffer[0][1]=B[i*column+j];
					buffer[0][2]=B[i*column+j+1];//由于不可能在右上角，所以不需判断
					sort(buffer[0]);
					A[i*column+j]=buffer[0][1];
					//if(i==row-1)//当在最后一行时
						//该种情况不可能
					//{
					//}
					//else
					//{
						//buffer[2][0]=B[(i+1)*column+j-1];
						//buffer[2][1]=B[(i+1)*column+j];
						//buffer[2][2]=B[(i+1)*column+j+1];
					//}
				}
				else	if(j-1<0)//当在第一列时    //left
				{
					buffer[0][0]=B[(i-1)*column+j];
					buffer[0][1]=B[i*column+j];
					buffer[0][2]=B[(i+1)*column+j];//由于不可能在左下角，所以不需判断
					sort(buffer[0]);
					A[i*column+j]=buffer[0][1];
					//if(i==column-1)//当在最后一列时，该种情况不可能
					//{
					//}
					//else
					//{
						//buffer[0][2]=B[(i-1)*column+j+1];
						//buffer[1][2]=B[i*column+j+1];
						//buffer[2][2]=B[(i+1)*column+j+1];
					//}
				}
				else	if(i==row-1)//当在最后一行时     //down
				{
					buffer[0][0]=B[i*column+j-1];
					buffer[0][1]=B[i*column+j];
					buffer[0][2]=B[i*column+j+1];
					sort(buffer[0]);
					A[i*column+j]=buffer[0][1];
					//buffer[1][0]=B[i*column+j-1];
					//buffer[1][1]=B[i*column+j];
					//buffer[1][2]=B[i*column+j+1];//由于不可能是在右下角，所以不需判断
				}
				else	if(j==column-1)//当在最后一列时  //right
				{
					//buffer[0][2]=0;
					//buffer[1][2]=0;
					//buffer[2][2]=0;
					buffer[0][0]=B[(i-1)*column+j];
					buffer[0][1]=B[i*column+j];
					buffer[0][2]=B[(i+1)*column+j];
					sort(buffer[0]);
					A[i*column+j]=buffer[0][1];
				}
				else
				{
					//buffer[0][0]=B[(i-1)*column+j-1];
					buffer[0][1]=B[(i-1)*column+j];
					//buffer[0][2]=B[(i-1)*column+j+1];
					buffer[1][0]=B[i*column+j-1];
					buffer[1][1]=B[i*column+j];
					buffer[1][2]=B[i*column+j+1];
					//buffer[2][0]=B[(i+1)*column+j-1];
					buffer[2][1]=B[(i+1)*column+j];
					//buffer[2][2]=B[(i+1)*column+j+1];
					////////////////////
					//sort(buffer[0]);
					sort(buffer[1]);
					//sort(buffer[2]);
					buffer[0][0]=buffer[0][1];
					buffer[0][1]=buffer[1][1];
					buffer[0][2]=buffer[2][1];
					sort(buffer[0]);
					A[i*column+j]=buffer[0][1];
				}

			}
		}
	free(B);
}
void CImageProcess::BlackWhiteImageDilationOnePixel()
//十字膨胀一个像素
{
	int x,y,w,z;
	for(x=1;x<m_iHeight-1;x++)
	{
		for(y=1;y<m_iWidth-1;y++)
		{
			if(m_pchrGrayImageData1[x*m_iWidth+y]==1)
			{
				w=x;
				z=y;
				if(m_pchrGrayImageData1[(w-1)*m_iWidth+z]==0)
					m_pchrGrayImageData1[(w-1)*m_iWidth+z]=2;
				if(m_pchrGrayImageData1[(w+1)*m_iWidth+z]==0)
					m_pchrGrayImageData1[(w+1)*m_iWidth+z]=2;
				
				if(m_pchrGrayImageData1[w*m_iWidth+z-1]==0)
					m_pchrGrayImageData1[w*m_iWidth+z-1]=2;
				if(m_pchrGrayImageData1[w*m_iWidth+z+1]==0)
					m_pchrGrayImageData1[w*m_iWidth+z+1]=2;
				
				
			}
		}
	}
	for(x=1;x<m_iHeight-1;x++)
	{
		for(y=1;y<m_iWidth-1;y++)
		{
			if(m_pchrGrayImageData1[x*m_iWidth+y]!=0)
				m_pchrGrayImageData1[x*m_iWidth+y]=1;
		}
	}
}