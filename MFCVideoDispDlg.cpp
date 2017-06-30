
// MFCVideoDispDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCVideoDisp.h"
#include "MFCVideoDispDlg.h"
#include "afxdialogex.h"
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMFCVideoDispDlg *pMainDlg;


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCVideoDispDlg �Ի���



CMFCVideoDispDlg::CMFCVideoDispDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCVideoDispDlg::IDD, pParent)
	, m_ImgFreq(0)
	, m_ObjectNum(0)
	, m_DeltaCenterRowForDisp(0)
	, m_DeltaCenterColForDisp(0)
	, m_pianyix(0)
	, m_pianyiy(0)
	, m_fVx(0)
	, m_KpOut(0)
	, m_KpIn(0)
	, m_KdOut(0)
	, m_KdIn(0)
	, m_Ki(0)
	, m_SightArea(0)
	, m_iTemplateSum(0)
	, m_iIsWhichShape(0)
	, m_fCurrentYaw(0)
	, m_iBreakingPeriodSetByUser(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	pMainDlg = this;
	m_pImageIniDIB = NULL;
	m_pIniCmpImgRChInSight = NULL;
	m_pIniCmpImgGChInSight = NULL;
	m_pIniCmpImgBChInSight = NULL;
	m_pIniCmpImgGreyInSight = NULL;
	m_pEZHImgEvryTimeInSight = NULL;
	m_hWndCap = NULL;
	m_ImgDispLoop = 0;
	m_ObjectXOffset=0;  //
    m_ObjectYOffset=0;
	mhCom=NULL; // ׼���򿪵Ĵ��ڵľ��
	m_fVx;
	m_KpOut=0.1;
	m_KpIn=0.01;
	m_KdOut=4;
	m_KdIn=6;
	m_Ki=0.2;
	m_SightArea=80;
	m_ctlParaIsChanged = 1;
	Action = KeepState;
	//m_pobjectPosLeftUp = NULL;
	//m_pobjectPosRightDown = NULL;
	m_iTemplateSum = 0;
	m_TargetProperty = TargetPropertyBasket;

	m_CurrentMission = CurrentMissionIsBlue;                  //Ĭ��Ϊ��ɫ
	m_LastTimeObjectXOffset = 0;
	m_LastTimeObjectYOffset = 0;
	
	StartGoRightStepsCount = 0;
	StartGoLeftStepsCount = 0;
	StartGoUpStepsCount = 0;
	StartGoDownStepsCount = 0;
	ConnerFrameCount1 = 0;
	ConnerFrameCount2 = 0;
	ConnerFrameCount3 = 0;
	ConnerFrameCount4 = 0;
	m_iBreakingPeriodSetByUser = 10;
}


CMFCVideoDispDlg::~CMFCVideoDispDlg()
{
	if (m_pImageIniDIB != NULL)
		free(m_pImageIniDIB);
	if (m_pIniCmpImgRChInSight != NULL)
		free(m_pIniCmpImgRChInSight);
	if (m_pIniCmpImgGChInSight != NULL)
		free(m_pIniCmpImgGChInSight);
	if (m_pIniCmpImgBChInSight != NULL)
		free(m_pIniCmpImgBChInSight);
	if (m_pIniCmpImgGreyInSight != NULL)
		free(m_pIniCmpImgGreyInSight);
	if (m_pEZHImgEvryTimeInSight != NULL)
		free(m_pEZHImgEvryTimeInSight);
	if (m_hWndCap != NULL)
		capDriverDisconnect(m_hWndCap);

	for (int i = 0; i < m_iTemplateSum; i++)
	{
		if (m_pTemplate[i] != NULL)
		{
			free(m_pTemplate[i]);
			m_iTemplateSum = 0;
		}
	}

}

void CMFCVideoDispDlg::DoDataExchange(CDataExchange* pDX)                     //(MFC�ؼ������������ݽ���)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, meraDeviceSelect_COMBO, m_ctlCameraDeviceSel);//   �ѱ����͵�ʲôʲô��
	DDX_Text(pDX, IDC_IMGFREQ_EDIT, m_ImgFreq);
	DDX_Text(pDX, IDC_OBJECTNum_EDIT, m_ObjectNum);
	//*******************************ʵ����ĺ���**********************************//
	DDX_Text(pDX, IDC_MouseX_EDIT, m_pianyix);
	DDX_Text(pDX, IDC_MouseY_EDIT, m_pianyiy);
	//DDX_Text(pDX, IDC_MouseX_EDIT, m_direction);               //    GoUp/GoDown/GoLeft/GoRight   

	//****************************************************************************//
	DDX_Text(pDX, IDC_VX_EDIT, m_fVx);
	DDX_Text(pDX, IDC_EDIT_KPOUT, m_KpOut);
	DDX_Text(pDX, IDC_EDIT_KPIN, m_KpIn);
	DDX_Text(pDX, IDC_EDIT_KDOUT, m_KdOut);
	DDX_Text(pDX, IDC_EDIT_KDIN, m_KdIn);
	DDX_Text(pDX, IDC_EDIT_KI, m_Ki);
	//  DDX_Control(pDX, ID_EDIT_SIGHTAREA, m_SightArea);
	DDX_Text(pDX, ID_EDIT_SIGHTAREA, m_SightArea);
	DDX_Control(pDX, IDC_COMBO_ControlChannals, m_ChannalSelectComboBoxCtrl);
	DDX_Text(pDX, IDC_EDIT_TemplateSum, m_iTemplateSum);
	DDX_Text(pDX, IDC_EDIT_IsAnyTemplate, m_iIsWhichShape);
	//  DDX_Text(pDX, IDC_EDIT_CurrentYaw, m_f);
	DDX_Text(pDX, IDC_EDIT_CurrentYaw, m_fCurrentYaw);
	DDX_Text(pDX, IDC_EDIT_BreakingPeriod, m_iBreakingPeriodSetByUser);
}

BEGIN_MESSAGE_MAP(CMFCVideoDispDlg, CDialogEx)                        //�ؼ���Ӧ����ҪΪ�������
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_OPENSERIAL_BUTTON, &CMFCVideoDispDlg::OnBnClickedOpenserialButton)
	ON_BN_CLICKED(IDC_BUTTON_ReadControlParaFromPX4, &CMFCVideoDispDlg::OnBnClickedButtonReadcontrolparafrompx4)
	ON_BN_CLICKED(IDC_BUTTON_WriteControlParaToPX4, &CMFCVideoDispDlg::OnBnClickedButtonWritecontrolparatopx4)
	ON_CBN_SELCHANGE(IDC_COMBO_ControlChannals, &CMFCVideoDispDlg::OnSelchangeComboControlchannals)
	ON_BN_CLICKED(IDC_CHECK_InputParaEnable, &CMFCVideoDispDlg::OnClickedCheckInputparaenable)
	ON_BN_CLICKED(IDC_BUTTON_SAVECurImg, &CMFCVideoDispDlg::OnBnClickedButtonSavecurimg)
	ON_BN_CLICKED(IDOK, &CMFCVideoDispDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMFCVideoDispDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_OPENVIDEO_BUTTON, &CMFCVideoDispDlg::OnBnClickedOpenvideoButton)
	ON_BN_CLICKED(IDC_BUTTON_ReadParasFromFile, &CMFCVideoDispDlg::OnBnClickedButtonReadparasfromfile)
	ON_BN_CLICKED(IDC_BUTTON_SaveCtrlParaToFile, &CMFCVideoDispDlg::OnBnClickedButtonSavectrlparatofile)
	ON_BN_CLICKED(IDC_BUTTON_AddTemplate, &CMFCVideoDispDlg::OnBnClickedButtonAddtemplate)
	ON_BN_CLICKED(IDC_BUTTON_ClearTemplate, &CMFCVideoDispDlg::OnBnClickedButtonCleartemplate)
	ON_BN_CLICKED(IDC_BUTTON_SaveTemplateToDisk, &CMFCVideoDispDlg::OnBnClickedButtonSavetemplatetodisk)
	ON_BN_CLICKED(IDC_BUTTON_AskingCurrentYaw, &CMFCVideoDispDlg::OnBnClickedButtonAskingcurrentyaw)
//	ON_BN_CLICKED(IDC_BUTTON_SendSartChangYawCMD, &CMFCVideoDispDlg::OnBnClickedButtonSendsartchangyawcmd)
ON_BN_CLICKED(IDC_BUTTON_SendStartChangYawCMD, &CMFCVideoDispDlg::OnBnClickedButtonSendstartchangyawcmd)
ON_UPDATE_COMMAND_UI(IDC_RADIO_MISSION1_BLUE, &CMFCVideoDispDlg::OnUpdateRadioMission1Blue)
ON_UPDATE_COMMAND_UI(IDC_RADIO_MISSION2_RED, &CMFCVideoDispDlg::OnUpdateRadioMission2Red)
ON_BN_CLICKED(IDC_RADIO_MISSION1_BLUE, &CMFCVideoDispDlg::OnBnClickedRadioMission1Blue)
ON_BN_CLICKED(IDC_RADIO_MISSION2_RED, &CMFCVideoDispDlg::OnBnClickedRadioMission2Red)
ON_BN_CLICKED(IDC_RADIO_MISSION3_YELLOW, &CMFCVideoDispDlg::OnBnClickedRadioMission3Yellow)
ON_EN_CHANGE(IDC_EDIT_CurrentYaw, &CMFCVideoDispDlg::OnEnChangeEditCurrentyaw)
ON_EN_CHANGE(IDC_EDIT_IsAnyTemplate, &CMFCVideoDispDlg::OnEnChangeEditIsanytemplate)
ON_EN_CHANGE(IDC_EDIT_TemplateSum, &CMFCVideoDispDlg::OnEnChangeEditTemplatesum)
ON_CBN_SELCHANGE(meraDeviceSelect_COMBO, &CMFCVideoDispDlg::OnCbnSelchangeCombo)
ON_EN_CHANGE(IDC_IMGFREQ_EDIT, &CMFCVideoDispDlg::OnEnChangeImgfreqEdit)
ON_BN_CLICKED(IDC_TextOfDsicribeVideoInput_STATIC, &CMFCVideoDispDlg::OnBnClickedTextofdsicribevideoinputStatic)
ON_STN_CLICKED(IDC_MouseX_STATIC, &CMFCVideoDispDlg::OnStnClickedMousexStatic)
ON_EN_CHANGE(IDC_MouseX_EDIT, &CMFCVideoDispDlg::OnEnChangeMousexEdit)
ON_EN_CHANGE(IDC_MouseY_EDIT, &CMFCVideoDispDlg::OnEnChangeMouseyEdit)
ON_EN_CHANGE(IDC_VX_EDIT, &CMFCVideoDispDlg::OnEnChangeVxEdit)
ON_BN_CLICKED(IDC_TextOfDsicribeVideoDisp_STATIC2, &CMFCVideoDispDlg::OnBnClickedTextofdsicribevideodispStatic2)
ON_BN_CLICKED(IDC_BUTTON_ClearCountingVariable, &CMFCVideoDispDlg::OnBnClickedButtonClearcountingvariable)
ON_STN_CLICKED(IDC_MouseY_STATIC, &CMFCVideoDispDlg::OnStnClickedMouseyStatic)
ON_BN_CLICKED(IDC_TextOfDsicribeVideoDisp_STATIC3, &CMFCVideoDispDlg::OnBnClickedTextofdsicribevideodispStatic3)
ON_STN_CLICKED(IDC_VideoProedDisp_STATIC, &CMFCVideoDispDlg::OnStnClickedVideoproeddispStatic)
END_MESSAGE_MAP()


// CMFCVideoDispDlg ��Ϣ�������

BOOL CMFCVideoDispDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	SearchVideoDevice();
	QueryPerformanceFrequency(&m_SysFreq);
	m_ChannalSelectComboBoxCtrl.AddString(_T("XY˫ͨ��"));
	m_ChannalSelectComboBoxCtrl.AddString(_T("��Xͨ��"));
	m_ChannalSelectComboBoxCtrl.SetCurSel(0);
	GetDlgItem(IDC_COMBO_ControlChannals)->EnableWindow(FALSE);
	((CButton    *)GetDlgItem(IDC_CHECK_InputParaEnable))->SetCheck(1);
	OnClickedCheckInputparaenable();
	ReadParaFromFile();
	ReadTemplateFromFile();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCVideoDispDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCVideoDispDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCVideoDispDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CALLBACK FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	if (!hWnd)
	{
		return NULL;
	}
	else
	{
		int x, y, cur, offset, i;
		unsigned char tempR, tempG, tempB;
		int iheight = pMainDlg->m_iCameraheight;
		int iwidth = pMainDlg->m_iCamerawidth;
		int Bitmap_bits = pMainDlg->m_BitmapInfo.bmiHeader.biBitCount;
		offset = Bitmap_bits / 8;
		unsigned long biAlign = iwidth*offset;
		double tempdata;

		memcpy(pMainDlg->m_pImageIniDIB, lpVHdr->lpData, pMainDlg->m_BitmapInfo.bmiHeader.biSizeImage);

		pMainDlg->m_ImgDispLoop++;

		/*QueryPerformanceCounter(&pMainDlg->m_CurrentImgTime);
		if (pMainDlg->m_ImgDispLoop > 1)
		{
			tempdata = (double)(pMainDlg->m_CurrentImgTime.QuadPart - pMainDlg->m_LastImgTime.QuadPart) / (double)pMainDlg->m_SysFreq.QuadPart * 1000;
			if (tempdata > 50)
				pMainDlg->m_ImgFreq = tempdata;
			pMainDlg->m_LastImgTime = pMainDlg->m_CurrentImgTime;
		}*/
		
	}

	return NULL;

}

//********************************���Ͼ�Ϊ�й�MFC�Ĳ���******************************//

//void CMFCVideoDispDlg::OnBnClickedOk3()
//{
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
//}


void CMFCVideoDispDlg::SearchVideoDevice()               //Ѱ�ҿ��õ�����ͷ�豸
{
	int  uIndex;

	int VideoDeviceCount = 0;//֧�ֵ��豸����������� 

	for (uIndex = 0; uIndex<5; uIndex++)
	{
		if (capGetDriverDescription(uIndex,
			(LPWSTR)m_chrAchVideoDeviceAndVersion[uIndex],
			sizeof(m_chrAchVideoDeviceAndVersion[uIndex]),
			(LPWSTR)m_chrAchVideoDeviceVersion[uIndex],
			sizeof(m_chrAchVideoDeviceVersion[uIndex])))
		{
			VideoDeviceCount++;
		}
		else
			break;
	}
	int temp = 0;
	if (VideoDeviceCount>0)
	{
		for (uIndex = 0; uIndex<VideoDeviceCount; uIndex++)
		{
			if (strlen(m_chrAchVideoDeviceAndVersion[uIndex]) != 0)
			{
				CString str;
				str.Format(_T("%s"), m_chrAchVideoDeviceAndVersion[uIndex]);
				m_ctlCameraDeviceSel.InsertString(temp, str);
				temp++;
			}
			if (VideoDeviceCount>1)
			{
				m_ctlCameraDeviceSel.SetCurSel(0);
				//m_iLastValOfCameraDeviceSel=1;
			}
			else
			{
				m_ctlCameraDeviceSel.SetCurSel(0);
				//m_iLastValOfCameraDeviceSel=0;
			}

		}
	}
}



void CMFCVideoDispDlg::OnTimer(UINT_PTR nIDEvent)                        
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	int i;
	static int rem_errx[20];
	static int rem_errx_count = 0;
	float tmpV[10], v2, v3, v4, v5, currentV;
	int errx_rem_num = 20;
	double tempdata;
	CDataExchange dx(this, FALSE);

	currentV = 0;
	

	int w, z;
	CRect temprect;
	CWnd* pwnd1;
	int checkval;
	POINT pt1, pt2, pt3, pt4;
	BITMAPINFO tempBitmapInfo = m_BitmapInfo;
	unsigned char tempR, tempG, tempB;
	unsigned char *ptempImageDIBForScreenShow;

	

	m_ObjectXOffset=0;
	m_ObjectYOffset=0;
	m_DeltaCenterColForDisp = 0;
	m_DeltaCenterRowForDisp = 0;
	//UpdateData(DipDataOnScreen);
	if (nIDEvent == DataUpdataTimer)
	{
		if (m_ctlParaIsChanged !=0)
		{
			m_ctlParaIsChanged = 0; 
			if (((CButton    *)GetDlgItem(IDC_CHECK_InputParaEnable))->GetCheck() == 1)
				UpdateData(DipDataOnScreen);
			PurgeComm(mhCom, PURGE_RXCLEAR);
			
			DoDataExchange(&dx);
			MessageBox(_T("�յ��ɿ�����!"));
			
		}
		
	}
	if (nIDEvent == DrawCrossOnVedioPictureTimer)
	{
		//UpdateData(ReadFromScreen);
		m_iBreakingPeriodSetByUser = GetDlgItemInt(IDC_EDIT_BreakingPeriod);
		
		tempBitmapInfo.bmiHeader.biBitCount = 24;
		tempBitmapInfo.bmiHeader.biHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;  // ѹ������
		tempBitmapInfo.bmiHeader.biWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
		tempBitmapInfo.bmiHeader.biSizeImage = tempBitmapInfo.bmiHeader.biHeight*
			tempBitmapInfo.bmiHeader.biWidth * 3;
		tempBitmapInfo.bmiHeader.biCompression = 0;
		ptempImageDIBForScreenShow = new unsigned char[tempBitmapInfo.bmiHeader.biSizeImage];
		/*	for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
			for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
			{
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + (tempBitmapInfo.bmiHeader.biWidth - 1 - w) * 3]
			= m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + (tempBitmapInfo.bmiHeader.biWidth - 1 - w) * 3 + 1]
			= m_pIniCmpImgGChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + (tempBitmapInfo.bmiHeader.biWidth - 1 - w) * 3 + 2]
			= m_pIniCmpImgBChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
			}
			*/


		//�����ĵ�ʮ����

		//pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2 - 20;
		//pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2 + 20;
		//DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);


		//pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2 - 20; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2;
		//pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2 + 20; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2;
		//DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);

		PyramidCompress();  //������ѹ��
		FindRedBlock();
		int CmpHeight, CmpWidth;
		CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
		CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;

		if (m_ObjectNum > 1)
		{
			
			Filter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth, m_ObjectNum);

			m_ObjectNum = 1;
		}
		///////��������������ߣ���ͼ���м䰴ֱ�߼����ҵ���ǰͼ���е�Ŀ���е�

		/*if (m_ObjectNum> 0 && m_iIsWhichShape == 1||m_iIsWhichShape == 2 ||
			m_iIsWhichShape == 3 || m_iIsWhichShape == 4 )
			GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);*/
		//������˳��û������

		///////����ǲ���������ķ�ʽ��������Ƶ����ǰ�Ϸ��Ƿ���Ŀ��
		//////���ǰ�Ϸ���Ŀ�꣬���������ǰ�Ϸ�һ��Ŀ��
		//////���ǰ�Ϸ�û��Ŀ�꣬�����Ƿ�Ĺյ�
		//////����ǹյ㣬���зɻ�ת����п���
		//////�ڷ�����ʼ�ռ���Ƿ�Ϊֹͣ�㣬����Ƿǵ�һ�η���ֹͣ�㣬��������У�������·���ֹͣ�㣬��ֹͣ���У�
		//////�����Ƿ��ֹ��յ�Ϊ��׼�ж��Ƿ���ֹͣ�㡣


		////////////////////////////////
		if (m_ObjectNum>=1)
		{
			

			////�ҵ���ӽ���Ƶ���ĵ������㣬

			////�ж�ǰ���Ƿ��������㣬����У�ֻ��������ǰ���ĵ���Ϊ�����㡣

			//FindTheNearestObject();

			GetObjectCenter();
			//�ж���ʲôĿ�꣬ʲô��������1
			if (m_ObjectNum>0)
			{ 
				////////////////////////////////////////////////
				if (m_iIsWhichShape == TypeOfNavyLine_Conner1
					||m_iIsWhichShape == TypeOfNavyLine_Conner2
					|| m_iIsWhichShape == TypeOfNavyLine_Conner3
					|| m_iIsWhichShape == TypeOfNavyLine_Conner4)
					GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);
				//��Ŀ��������Ϊ�ж�
				if (m_iIsWhichShape == TypeOfNavyLine_HorizonStraightLine)
				{
					if (ConnerFrameCount1 == 0)
					{
						///Ӧ�����Ҳ����
						StartGoRightStepsCount++;
						Action = GoRight;
						if (StartGoRightStepsCount %StopActionFrequent == 0)
							Action = StopRight;
					}
					else if (ConnerFrameCount2 >= FrameDownLimit&&ConnerFrameCount4<FrameDownLimit)
					{
						///Ӧ����������
						StartGoLeftStepsCount++;
						Action = GoLeft;
						if (StartGoLeftStepsCount %StopActionFrequent == 0)
							Action = StopLeft;
					}
					else if (ConnerFrameCount4 >= FrameDownLimit)
					{
						///Ӧ�����Ҳ����
						StartGoRightStepsCount++;
						Action = GoRight;
						if (StartGoRightStepsCount %StopActionFrequent == 0)
							Action = StopRight;
					}
				}
				if (m_iIsWhichShape == TypeOfNavyLine_VerticleStraightLine)
				{
					if (ConnerFrameCount1 >= FrameDownLimit&&ConnerFrameCount3 < FrameDownLimit)
					{
						///Ӧ�����Ҳ����        //��
						StartGoUpStepsCount++;
						Action = GoUp;
						if (StartGoUpStepsCount %StopActionFrequent == 0)
							Action = StopUp;
					}
					else if (ConnerFrameCount3 >= FrameDownLimit)
					{
						///Ӧ����������       //��
						StartGoDownStepsCount++;
						Action = GoDown;
						//StartGoUpStepsCount = 0;//�޸Ĺ�20170415
						if (StartGoDownStepsCount %StopActionFrequent == 0)
							Action = StopDown;
					}
				}
				else if (m_iIsWhichShape == TypeOfNavyLine_Conner1)
				{
					ConnerFrameCount1++; 
					if (ConnerFrameCount1 < FrameDownLimit)
					{						
						Action = KeepState;
					}
					else
						Action = GoUp;
						
				}
				else if (m_iIsWhichShape == TypeOfNavyLine_Conner2)
				{
					ConnerFrameCount2++; 
					if (ConnerFrameCount2 < FrameDownLimit)
					{
						Action = KeepState;
					}
					else
						Action = GoLeft;
				}
				else if (m_iIsWhichShape == TypeOfNavyLine_Conner3)
				{
					ConnerFrameCount3++;
					if (ConnerFrameCount3 < FrameDownLimit)
					{
						Action = KeepState;
					}
					else
						Action = GoDown;
				}
				else if (m_iIsWhichShape == TypeOfNavyLine_Conner4)
				{
					ConnerFrameCount4++;
					if (ConnerFrameCount4 < FrameDownLimit)
					{
						Action = KeepState;
					}
					else
						Action = GoRight;
				}
				else if (m_iIsWhichShape == TypeOfNavyLine_StartPoint)
				{
					if (ConnerFrameCount3 == 0)
						Action = GoRight;
					else
						Action = KeepState;
				}
			}
			else
			{
				Action = KeepState;
			}



			//Action = GoRight;

			//m_DeltaCenterColForDisp = 0;
			// = 0;

			/*if (m_ObjectNum > 0 && m_iIsWhichShape == TypeOfNavyLine_Conner1)  //3
			{
				GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);
				ConnerFrameCount1++;
				if (ConnerFrameCount1 > FrameDownLimit && ConnerFrameCount1 <FrameUpLimit
					&& (ConnerFrameCount1 - FrameDownLimit) % StopActionFrequent == 0)
					Action = StopLeft;     //����ɲ��ָ��  8
				else if (ConnerFrameCount1 == FrameUpLimit) //??>=??==
					Action = GoUp;                  //2
				else
					Action = KeepState;
			}
			else if (m_ObjectNum > 0 && m_iIsWhichShape == TypeOfNavyLine_Conner2)  //4
			{
				GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);
				ConnerFrameCount2++;
				if (ConnerFrameCount2 > FrameDownLimit && ConnerFrameCount2 <FrameUpLimit 
					&& (ConnerFrameCount2 - FrameDownLimit) % StopActionFrequent == 0)
					Action = StopDown;     //����ɲ��ָ��   7
				else if (ConnerFrameCount2 == FrameUpLimit)
					Action = GoLeft;              //4
				else
					Action = KeepState;
			}
			else if (m_ObjectNum > 0 && m_iIsWhichShape == TypeOfNavyLine_Conner3)     //5
			{
				GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);
				ConnerFrameCount3++;
				if (ConnerFrameCount3 > FrameDownLimit && ConnerFrameCount3 <FrameUpLimit 
					&& (ConnerFrameCount3 - FrameDownLimit) % StopActionFrequent == 0)
					Action = StopRight;     //����ɲ��ָ��   9
				if (ConnerFrameCount3 == FrameUpLimit)
					Action = GoDown;             //3
				else
					Action = KeepState;
			}
			else if (m_ObjectNum > 0 && m_iIsWhichShape == TypeOfNavyLine_Conner4)     //6
			{
				GetCornerCenter(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);
				ConnerFrameCount4++;
				if (ConnerFrameCount4 > FrameDownLimit && ConnerFrameCount4 < FrameUpLimit 
					&& (ConnerFrameCount4 - FrameDownLimit) % StopActionFrequent == 0)
					Action = StopUp;     //����ɲ��ָ��      //6
				if (ConnerFrameCount4 == FrameUpLimit)
					Action = GoRight;           //5
				else
					Action = KeepState;
			}
			else
				Action = KeepState;*/
				if (Action == GoLeft)
					UAV_action = "����";
				if (Action == GoRight)
					UAV_action = "����";
				if (Action == GoUp)
					UAV_action = "����";
				if (Action == GoDown)
					UAV_action = "����";
				if (Action == KeepState)
					UAV_action = "��ͣ";
				DDX_Text(&dx, IDC_EDIT_TemplateSum, UAV_action);

			
			//����ָ����Ϣ
			DrawObjectBlockInIniImg();
			//DrawObjectCenterInIniImg();
		}
		///////////////////////////////////////////////
		if (m_TargetProperty !=TargetPropertyBasket)
		{
			
			//m_TargetProperty = TargetPropertyQiangZhiShouDong;

			

			m_DeltaCenterColForDisp = 0;
			m_DeltaCenterRowForDisp = 0;

			/*if (m_TargetProperty == TargetPropertyBasket&&ConnerFrameCount3 == 0)
			{
				m_iIsWhichShape = TypeOfNavyLine_StartPoint;
				Action = GoRight;
			}
			if (m_TargetProperty == TargetPropertyBasket&&ConnerFrameCount1 >0&&
				ConnerFrameCount2 >0 && ConnerFrameCount3 >0 && ConnerFrameCount4 >0)  //FrameDownLimit
			{
				m_iIsWhichShape = TypeOfNavyLine_EndPoint;
				Action = Land;
			}
			*/
		}
		else
			Action = KeepState;

		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
		m_ObjectXOffset = JinZiTaCompress * m_DeltaCenterColForDisp;
		m_ObjectYOffset = JinZiTaCompress * m_DeltaCenterRowForDisp;
		if (mhCom != NULL)
		{ // ����Ŀ��ͼ������ƫ����
			//m_ObjectXOffset = 2;
			//m_ObjectYOffset = -19;
			int tempXOffset, tempYOffset;
			tempXOffset = 0 * m_LastTimeObjectXOffset + 1 * m_ObjectXOffset;
			tempYOffset = 0 * m_LastTimeObjectYOffset + 1 * m_ObjectYOffset;
			SendObjectOffset(tempXOffset, tempYOffset);
			m_LastTimeObjectXOffset = m_ObjectXOffset;
			m_LastTimeObjectYOffset = m_ObjectYOffset;
		}
		m_pianyix = m_ObjectXOffset;
		m_pianyiy = m_ObjectYOffset;

		tempBitmapInfo.bmiHeader.biHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;  // ѹ������
		tempBitmapInfo.bmiHeader.biWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
		tempBitmapInfo.bmiHeader.biSizeImage = tempBitmapInfo.bmiHeader.biHeight*
			tempBitmapInfo.bmiHeader.biWidth * 3;
		if (m_ObjectNum >= 1)
		{
			for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
				for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
				{
					if (m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w] != 0)
						ptempImageDIBForScreenShow[z*tempBitmapInfo.bmiHeader.biWidth + w] = 1;
					else
						ptempImageDIBForScreenShow[z*tempBitmapInfo.bmiHeader.biWidth + w] = 0;
				}
			}
			//ThinImage(ptempImageDIBForScreenShow, ptempImageDIBForScreenShow,
			//	0, tempBitmapInfo.bmiHeader.biHeight, tempBitmapInfo.bmiHeader.biWidth);
			for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
				for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
				{
					m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w]
						= ptempImageDIBForScreenShow[z*tempBitmapInfo.bmiHeader.biWidth + w];
				}
			}
		}


		for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
		{
			for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
			{
				if (m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w] != 0)
					m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w] = 0;
				else
					m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w] = 255;

				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
					= m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
					= m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
					= m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
		}
		/*ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - test_row)*tempBitmapInfo.bmiHeader.biWidth * 3 + test_col * 3]
			= 255;
		ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - test_row)*tempBitmapInfo.bmiHeader.biWidth * 3 + test_col * 3 + 1]
			= 0;
		ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - test_row)*tempBitmapInfo.bmiHeader.biWidth * 3 + test_col * 3 + 2]
			= 0;*/
		pwnd1 = GetDlgItem(IDC_VideoReDisp_STATIC);
		pwnd1->GetWindowRect(&temprect);
		ScreenToClient(temprect);
		CClientDC dc(pwnd1);
		HDC hDC = dc.GetSafeHdc();
		SetStretchBltMode(hDC, COLORONCOLOR);

		
		checkval = StretchDIBits(dc.m_hDC,
			0,//temprect.left,
			0,//temprect.top,
			temprect.Width(),
			temprect.Height(),
			0,
			0,
			tempBitmapInfo.bmiHeader.biWidth,
			tempBitmapInfo.bmiHeader.biHeight,
			ptempImageDIBForScreenShow,
			&tempBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);


		/*for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
		{
		for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
		{

		tempR = m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
		tempG = m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
		tempB = m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
		pdc->SetPixel({ w, z }, RGB(tempR, tempG, tempB));
		}
		}*/


			for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
			for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
			{
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
			= m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
			= m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
			= m_pIniCmpImgRChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
			}
			pwnd1 = GetDlgItem(IDC_VideoINIRedPart_STATIC);
			pwnd1->GetWindowRect(&temprect);
			ScreenToClient(temprect);
			CClientDC dcRed(pwnd1);
			hDC = dcRed.GetSafeHdc();
			SetStretchBltMode(hDC, COLORONCOLOR);

			checkval = StretchDIBits(dcRed.m_hDC,
			0,//temprect.left,
			0,//temprect.top,
			temprect.Width(),
			temprect.Height(),
			0,
			0,
			tempBitmapInfo.bmiHeader.biWidth,
			tempBitmapInfo.bmiHeader.biHeight,
			ptempImageDIBForScreenShow,
			&tempBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

			for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
			for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
			{
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
			= m_pIniCmpImgGChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
			= m_pIniCmpImgGChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
			= m_pIniCmpImgGChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
			}
			pwnd1 = GetDlgItem(IDC_VideoINIGreenPart_STATIC);
			pwnd1->GetWindowRect(&temprect);
			ScreenToClient(temprect);
			CClientDC dcGreen(pwnd1);
			hDC = dcGreen.GetSafeHdc();
			SetStretchBltMode(hDC, COLORONCOLOR);

			checkval = StretchDIBits(dcGreen.m_hDC,
			0,//temprect.left,
			0,//temprect.top,
			temprect.Width(),
			temprect.Height(),
			0,
			0,
			tempBitmapInfo.bmiHeader.biWidth,
			tempBitmapInfo.bmiHeader.biHeight,
			ptempImageDIBForScreenShow,
			&tempBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

			for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
			{
			for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
			{
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
			= m_pIniCmpImgBChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
			= m_pIniCmpImgBChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
			= m_pIniCmpImgBChInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
			}
			pwnd1 = GetDlgItem(IDC_VideoINIBluePart_STATIC);
			pwnd1->GetWindowRect(&temprect);
			ScreenToClient(temprect);
			CClientDC dcBlue(pwnd1);
			hDC = dcBlue.GetSafeHdc();
			SetStretchBltMode(hDC, COLORONCOLOR);

			checkval = StretchDIBits(dcBlue.m_hDC,
			0,//temprect.left,
			0,//temprect.top,
			temprect.Width(),
			temprect.Height(),
			0,
			0,
			tempBitmapInfo.bmiHeader.biWidth,
			tempBitmapInfo.bmiHeader.biHeight,
			ptempImageDIBForScreenShow,
			&tempBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
			
		free(ptempImageDIBForScreenShow);
		
		



		currentV = 0;
		for (i = 0; i < errx_rem_num - 1; i++)
			rem_errx[i] = rem_errx[i + 1];
		rem_errx[rem_errx_count] = m_pianyix;

		if (rem_errx_count < errx_rem_num - 1)
			rem_errx_count++;
		if (rem_errx_count == errx_rem_num - 1)
		{
			for (i = 0; i < errx_rem_num / 2; i++)
			{
				tmpV[i] = (rem_errx[errx_rem_num / 2 + i] - rem_errx[i]) / (errx_rem_num / 2 * 0.065);
				currentV += tmpV[i];
			}

			currentV = currentV / (errx_rem_num / 2);
		}



		m_fVx = currentV;

		QueryPerformanceCounter(&m_CurrentImgTime);
		if (m_ImgDispLoop > 1)
		{
			tempdata = (double)(m_CurrentImgTime.QuadPart - m_LastImgTime.QuadPart) / (double)m_SysFreq.QuadPart * 1000;
			if (tempdata > 50)
				m_ImgFreq = tempdata;
			m_LastImgTime = m_CurrentImgTime;
		}

		

		//*******************************ʵ����ĺ���**********************************//
		DDX_Text(&dx, IDC_MouseX_EDIT, m_pianyix);
		DDX_Text(&dx, IDC_MouseY_EDIT, m_pianyiy);
		//DDX_Text(&dx, IDC_MouseX_EDIT, m_direction);               //    GoUp/GoDown/GoLeft/GoRight   

		//****************************************************************************//
		DDX_Text(&dx, IDC_VX_EDIT, m_fVx);
		DDX_Text(&dx, IDC_IMGFREQ_EDIT, m_ImgFreq);
		DDX_Text(&dx, IDC_OBJECTNum_EDIT, m_ObjectNum);
		
	}

	CDialogEx::OnTimer(nIDEvent);
}




void CMFCVideoDispDlg::OpenMemOfImgChData()
{
	int CmpHeight, CmpWidth;
	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;

	m_pIniCmpImgRChInSight = new unsigned char[CmpHeight*CmpWidth];
	m_pIniCmpImgGChInSight = new unsigned char[CmpHeight*CmpWidth];;
	m_pIniCmpImgBChInSight = new unsigned char[CmpHeight*CmpWidth];;
	m_pIniCmpImgGreyInSight = new unsigned char[CmpHeight*CmpWidth];
	m_pEZHImgEvryTimeInSight = new unsigned int[CmpHeight*CmpWidth];
}

void CMFCVideoDispDlg::PyramidCompress()
{
	int CmpHeight, CmpWidth;
	long w, z;
	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	int Y, U, V, C, D, E, R, G, B;
	if (m_BitmapInfo.bmiHeader.biBitCount == 24)
	{
		for (z = 0; z < CmpHeight; z++)
			for (w = 0; w < CmpWidth; w++)
			{
				m_pIniCmpImgRChInSight[z*CmpWidth + w] = m_pImageIniDIB[(CmpHeight - 1 - z)*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 * JinZiTaCompress + 2];
				m_pIniCmpImgGChInSight[z*CmpWidth + w] = m_pImageIniDIB[(CmpHeight - 1 - z)*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 * JinZiTaCompress + 1];
				m_pIniCmpImgBChInSight[z*CmpWidth + w] = m_pImageIniDIB[(CmpHeight - 1 - z)*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 * JinZiTaCompress];
			}
	}
	else
		if (m_BitmapInfo.bmiHeader.biBitCount == 16)
		{
			for (z = 0; z < CmpHeight; z++)
				for (w = 0; w < CmpWidth; w++)
				{
					Y = m_pImageIniDIB[z*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 * JinZiTaCompress + 1];
					if ((w*JinZiTaCompress) % 2 == 0)
					{
						U = m_pImageIniDIB[z*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 * JinZiTaCompress + 1 - 1];// +1];
						V = m_pImageIniDIB[z*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 * JinZiTaCompress + 1 + 1];// +3];
					}
					else
					{
						U = m_pImageIniDIB[z*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 * JinZiTaCompress + 1 - 3];// -1];
						V = m_pImageIniDIB[z*JinZiTaCompress *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 * JinZiTaCompress + 1 - 1];// +1];
					}
					C = Y - 16;
					D = U - 128;
					E = V - 128;
					R = (298 * C + 409 * E + 128) >> 8;
					if (R < 0)R = 0;
					if (R > 255)R = 255;
					G = (298 * C - 100 * D - 208 * E + 128) >> 8;
					if (G < 0)G = 0;
					if (G > 255)G = 255;
					B = (298 * C + 516 * D + 128) >> 8;
					if (B < 0)B = 0;
					if (B > 255)B = 255;
					m_pIniCmpImgRChInSight[z*CmpWidth + w] = R;
					m_pIniCmpImgGChInSight[z*CmpWidth + w] = G;
					m_pIniCmpImgBChInSight[z*CmpWidth + w] = B;
				}

		}
	for (z = 0; z < CmpHeight; z++)
		for (w = 0; w < CmpWidth; w++)
		{
			m_pIniCmpImgGreyInSight[z*CmpWidth + w] = m_pIniCmpImgRChInSight[z*CmpWidth + w] * 0.299 +
				m_pIniCmpImgGChInSight[z*CmpWidth + w] * 0.587 +
				m_pIniCmpImgBChInSight[z*CmpWidth + w] * 0.114;        //Y=0.299R+0.587G+0.114B
		}
}


void CMFCVideoDispDlg::FindRedBlock()                        
{
	int w, z;
	int CmpHeight, CmpWidth, minGrey, maxGrey;
	float tt1, tt2;
	unsigned long DiffImgZft[256];
	unsigned int objectSum;
	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	unsigned char *pDiffImg = new unsigned char[CmpHeight*CmpWidth];
	

	for (z = 0; z < CmpHeight; z++)
	{
		for (w = 0; w < CmpWidth; w++)
		{
			m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
		}
	}

	BOOL p[10] = { 0 };

	for (z = 0; z < 256; z++)
		DiffImgZft[z] = 0;                       //������0

	for (z = 0; z < CmpHeight; z++)
	{
		for (w = 0; w < CmpWidth; w++)
		{
			if (m_CurrentMission == CurrentMissionIsBlue)
				pDiffImg[z*CmpWidth + w] = m_pIniCmpImgBChInSight[z*CmpWidth + w];// abs((int)m_pIniCmpImgRChInSight[z*CmpWidth + w] - (int)m_pIniCmpImgGreyInSight[z*CmpWidth + w]);
			if (m_CurrentMission == CurrentMissionIsRed)
				pDiffImg[z*CmpWidth + w] = m_pIniCmpImgRChInSight[z*CmpWidth + w];// abs((int)m_pIniCmpImgRChInSight[z*CmpWidth + w] - (int)m_pIniCmpImgGreyInSight[z*CmpWidth + w]);
			if (m_CurrentMission == CurrentMissionIsYellow)
			{
				pDiffImg[z*CmpWidth + w] = 0.5*m_pIniCmpImgRChInSight[z*CmpWidth + w] + 0.5*m_pIniCmpImgGChInSight[z*CmpWidth + w];// abs((int)m_pIniCmpImgRChInSight[z*CmpWidth + w] - (int)m_pIniCmpImgGreyInSight[z*CmpWidth + w]);
				//m_pIniCmpImgRChInSight[z*CmpWidth + w] = pDiffImg[z*CmpWidth + w];
			}
			DiffImgZft[pDiffImg[z*CmpWidth + w]] ++;      //ͳ�Ƹ����Ҷȼ��ĸ���
		}
	}
	minGrey = 0;
	maxGrey = 255;
	for (z = 0; z < 256; z++)
	{
		if (DiffImgZft[z] != 0)               
		{
			minGrey = z;
			break;
		}
	}                        //0-255 ����С��

	for (z = 255; z > 0; z--)
	{
		if (DiffImgZft[z] != 0)
		{
			maxGrey = z;
			break;
		}
	}            //0-255������

	tt1 = minGrey - (minGrey - 0) / 1.0;           //???????????
	tt2 = (255 - maxGrey)/1.0 + maxGrey;       //????????????????/

	for (z = 0; z < CmpHeight; z++)
	{
		for (w = 0; w < CmpWidth; w++)
		{
			//pDiffImg[z*CmpWidth + w] = fabs((float)pDiffImg[z*CmpWidth + w] - (float)(minGrey))*(tt2 - tt1 + 1) / (maxGrey - minGrey + 1) + tt1;
			if (pDiffImg[z*CmpWidth + w] >80)            //
			{
				float bi1, bi2;
				if (m_CurrentMission == CurrentMissionIsBlue)
				{
					bi1 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgRChInSight[z*CmpWidth + w];
					bi2 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgGChInSight[z*CmpWidth + w];
				}

				if (m_CurrentMission == CurrentMissionIsRed)
				{
					bi1 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgBChInSight[z*CmpWidth + w];
					bi2 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgGChInSight[z*CmpWidth + w];
				}

				if (m_CurrentMission == CurrentMissionIsYellow)
				{
					bi1 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgBChInSight[z*CmpWidth + w];
					bi2 = pDiffImg[z*CmpWidth + w] * 1.0 / m_pIniCmpImgGChInSight[z*CmpWidth + w];
				
					if (pDiffImg[z*CmpWidth + w]>120
						&& m_pIniCmpImgGChInSight[z*CmpWidth + w]>120
						&& m_pIniCmpImgBChInSight[z*CmpWidth + w]>120)
						m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;

					else
					{
						if (bi1 > 2 || bi2 > 2 && (bi1 > 1 && bi2 > 1))
							m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 1;
						else
							m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;

						//if (m_pIniCmpImgGChInSight[z*CmpWidth + w]>120)
						//m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
					}
				}
				else
				{
					if (m_pIniCmpImgRChInSight[z*CmpWidth + w]>120
						&& m_pIniCmpImgGChInSight[z*CmpWidth + w]>120
						&& m_pIniCmpImgBChInSight[z*CmpWidth + w]>120)
						m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;

					else
					{
						if (m_CurrentMission == CurrentMissionIsBlue)
						{
							if (bi1 > 2 || bi2 > 2 && (bi1 > 1 && bi2 > 1))
								m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 1;
							else
								m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
						}

						if (m_CurrentMission == CurrentMissionIsRed)
						{
							if (bi1 > 1.3 && bi2 >1.2)
								m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 1;
							else
								m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
						}

					}
				}
			}
			else
				m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;// pDiffImg[z*CmpWidth + w]
		}
	}
	///////////////////���û���κ���ɫ���֣������Ƿ���������
	//////���ȼ����Ƿ���ĳ����ɫ����
	int pointCount = 0;
	for (z = 0; z < CmpHeight; z++)
	{
		for (w = 0; w < CmpWidth; w++)
		{
			if (m_pEZHImgEvryTimeInSight[z*CmpWidth + w] != 0)
			{
				m_TargetProperty = TargetPropertyBasket;
				pointCount ++;                             //����Ŀ������������1
				break;
			}
		}
		if (pointCount > 0)
			break;
	}
	if (pointCount==0)//˵��û������Ŀ�꣬��������
	{
		for (z = 0; z < CmpHeight; z++)
		{
			for (w = 0; w < CmpWidth; w++)
			{
				if (m_pIniCmpImgRChInSight[z*CmpWidth + w]<90
					&& m_pIniCmpImgGChInSight[z*CmpWidth + w]<90
					&& m_pIniCmpImgBChInSight[z*CmpWidth + w]<90)
					m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 1;
				else
					m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
			}
		}
		pointCount = 0;
		for (z = 0; z < CmpHeight; z++)
		{
			for (w = 0; w < CmpWidth; w++)
			{
				if (m_pEZHImgEvryTimeInSight[z*CmpWidth + w] != 0)
				{
					m_TargetProperty = TargetPropertyNaveyLine;
					pointCount++;
					break;
				}
			}
			if (pointCount > 0)
				break;
		}
		if (pointCount == 0)//˵��������Ҳδʶ���
		{
			m_TargetProperty = TargetPropertyNothing;

		}
	}	
	
	//BlackWhiteImageDilationOnePixel();
	if (m_TargetProperty != 0)
	{
		for (z = 0; z < CmpHeight; z++)
		{
			for (w = 0; w < 2; w++)
			{
				m_pEZHImgEvryTimeInSight[z*CmpWidth + w] = 0;
			}
		}
		m_ObjectNum = ImageProcessmbfg(m_pEZHImgEvryTimeInSight, CmpHeight, CmpWidth);

		/*FILE *fp = fopen("image.txt", "w+");
		for (int row = 0; row < CmpHeight; row++)
		{
			for (int col = 0; col < CmpWidth; col++)
			{
				fprintf(fp, "%3d", m_pEZHImgEvryTimeInSight[row * CmpWidth + col]);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);*/
		
	}
	//m_fCurrentYaw = m_TargetProperty;
	/*FILE *tempfp = fopen("track.txt", "w+");

	for (z = 0; z < CmpHeight; z++)
	{
		for (w = 0; w < CmpWidth; w++)
		{
			fprintf(tempfp, "%6d", m_pEZHImgEvryTimeInSight[z*CmpWidth + w]);
		}
		fprintf(tempfp, "\n");
	}
	fclose(tempfp);*/

	free(pDiffImg);
}

void CMFCVideoDispDlg::OnBnClickedButtonAddtemplate()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	unsigned long square;		//���
	float sumRow, sumCol;
	int CmpHeight, CmpWidth, row, col;
	float TemplateCenterRow, TemplateCenterCol;
	float D20, D02, D11, D30, D12, D21, D03;
	float Z20, Z02, Z11, Z30, Z12, Z21, Z03;
	float H1, H2, H3, H4;
	float R1, R2, R3;
	double CenterMoment;
	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	square = 0;
	sumRow = 0;
	sumCol = 0;
	D20 = 0;
	D02 = 0;
	D11 = 0;
	D30 = 0;
	D12 = 0;
	D21 = 0;
	D03 = 0;
	for (row = 0; row < CmpHeight; row++)
	{
		for (col = 0; col < CmpWidth; col++)
		{
			if (m_pEZHImgEvryTimeInSight[row*CmpWidth + col] == 0)     //��ʱ��m_pEZHImgEvryTimeInSightֻ�洢0��ɫ��255��ɫ��
			{
				square++;
				sumRow += row;
				sumCol += col;
			}
		}
	}
	TemplateCenterRow = sumRow / double(square);
	TemplateCenterCol = sumCol / double(square);
	for (row = 0; row < CmpHeight; row++)
	{
		for (col = 0; col < CmpWidth; col++)
		{
			D20 += (row - TemplateCenterRow)*(row - TemplateCenterRow);
			D02 += (col - TemplateCenterCol)*(col - TemplateCenterCol);
			D11 += (row - TemplateCenterRow)*(col - TemplateCenterCol);
			D30 += (row - TemplateCenterRow)*(row - TemplateCenterRow)*(row - TemplateCenterRow);
			D12 += (row - TemplateCenterRow)*(col - TemplateCenterCol)*(col - TemplateCenterCol);
			D21 += (row - TemplateCenterRow)*(row - TemplateCenterRow)*(col - TemplateCenterCol);
			D03 += (col - TemplateCenterCol)*(col - TemplateCenterCol)*(col - TemplateCenterCol);
		}
	}
	Z20 = D20 / square / square / square;
	Z02 = D02 / square / square / square;
	Z11 = D11 / square / square / square;
	Z30 = D30 / square / square / square / square;
	Z12 = D12 / square / square / square / square;
	Z21 = D21 / square / square / square / square;
	Z03 = D03 / square / square / square / square;
	
	H1 = Z20 + Z02;
	H2 = (Z20 - Z02)*(Z20 - Z02) + 4 * Z11*Z11;
	H3 = (Z30 - 3 * Z12)*(Z30 - 3 * Z12) + (3 * Z21 - Z03)*(3 * Z21 - Z03);
	H4 = (Z30 + Z12)*(Z30 + Z12) + (Z21 + Z03)*(Z21 + Z03);
	
	R1 = sqrtf(H2) / H1;
	R2 = pow(H3, (1.0 / 8.0)) / pow(H1, (1.0 / 3.0));
	R3 = pow(H4, (1.0 / 8.0)) / pow(H1, (1.0 / 3.0));


	//CenterMoment = R1 + R2 + R3;
	m_iTemplateSum++;
	m_pTemplate[m_iTemplateSum - 1] = new struct TargetTemplateStruct;
	m_pTemplate[m_iTemplateSum - 1]->square = square;
	m_pTemplate[m_iTemplateSum - 1]->R1 = R1;
	m_pTemplate[m_iTemplateSum - 1]->R2 = R2;
	m_pTemplate[m_iTemplateSum - 1]->R3 = R3;
	UpdateData(DipDataOnScreen);
}

void CMFCVideoDispDlg::GetObjectCenter()
{
	int CmpHeight, CmpWidth, row, col;
	float sumRow, sumCol;
	unsigned long square;
	float TemplateCenterRow, TemplateCenterCol;
	double Moment[10];
	BOOL sideLeft, sideRight, sideUp, sideDown;

	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	m_ObjectCenterInShrinkedImage.ROW = CmpHeight / 2;
	m_ObjectCenterInShrinkedImage.COLUMN = CmpWidth / 2;
	m_LimitedObjectCenterInShrinkedImage.ROW = CmpHeight / 2;
	m_LimitedObjectCenterInShrinkedImage.COLUMN = CmpWidth / 2;
	sideLeft = 0;
	sideRight = 0;
	sideUp = 0;
	sideDown = 0;
	square = 0;
	sumRow = 0;
	sumCol = 0;
	if (m_ObjectNum > 0)
	{
		for (row = 0; row < CmpHeight; row++)
		{
			for (col = 0; col < CmpWidth; col++)
			{
				if (m_pEZHImgEvryTimeInSight[row*CmpWidth + col] == 1)
				{
					square++;
					sumRow += row;
					sumCol += col;
				}
			}
		}
		TemplateCenterRow = sumRow / double(square);
		TemplateCenterCol = sumCol / double(square);
		m_ObjectCenterInShrinkedImage.ROW = TemplateCenterRow;
		m_ObjectCenterInShrinkedImage.COLUMN = TemplateCenterCol;
		m_LimitedObjectCenterInShrinkedImage.ROW = m_ObjectCenterInShrinkedImage.ROW;
		m_LimitedObjectCenterInShrinkedImage.COLUMN = m_ObjectCenterInShrinkedImage.COLUMN;
	}
	m_DeltaCenterRowForDisp = m_ObjectCenterInShrinkedImage.ROW - CmpHeight / 2;
	m_DeltaCenterColForDisp = m_ObjectCenterInShrinkedImage.COLUMN - (CmpWidth / 2);

	for (row = 5; row < CmpHeight - 5; row++)
	{
		if (m_pEZHImgEvryTimeInSight[row*CmpWidth + 5] == 1)
		{
			sideLeft = 1;
		}
	}
	for (row = 5; row < CmpHeight - 5; row++)
	{
		if (m_pEZHImgEvryTimeInSight[row*CmpWidth + CmpWidth-5-1] == 1)
		{
			sideRight = 1;
		}
	}
	for (col = 5; col < CmpWidth - 5; col++)
	{
		if (m_pEZHImgEvryTimeInSight[5*CmpWidth + col] == 1)
		{
			sideUp = 1;
		}
	}
	for (col = 5; col < CmpWidth - 5; col++)
	{
		if (m_pEZHImgEvryTimeInSight[(CmpHeight-5-1) * CmpWidth + col] == 1)
		{
			sideDown = 1;
		}
	}
	if (sideLeft == 1 && sideRight == 0 && sideDown == 0 && sideUp == 1)
	{
		m_iIsWhichShape = TypeOfNavyLine_Conner1;
	}
	else if(sideLeft == 1 && sideRight == 0 && sideDown == 1 && sideUp == 0)
	{
		m_iIsWhichShape = TypeOfNavyLine_Conner2;
	}
	else if (sideLeft == 0 && sideRight == 1 && sideDown == 1 && sideUp == 0)
	{
		m_iIsWhichShape = TypeOfNavyLine_Conner3;
	}
	else if (sideLeft == 0 && sideRight == 1 && sideDown == 0 && sideUp == 1)
	{
		m_iIsWhichShape = TypeOfNavyLine_Conner4;
	}
	else if (sideLeft == 0 && sideRight == 0 && sideDown == 1 && sideUp == 1)
	{
		m_iIsWhichShape = TypeOfNavyLine_VerticleStraightLine;//��ֱ��ֱ��
	}
	else if (sideLeft == 1 && sideRight == 1 && sideDown == 0 && sideUp == 0)
	{
		m_iIsWhichShape = TypeOfNavyLine_HorizonStraightLine;//ˮƽ��ֱ��
	}
	else if(sideLeft == 1 && sideRight == 1 && sideDown == 1 && sideUp == 0)
	{
		m_iIsWhichShape = TypeOfNavyLine_HorizonStraightLine;//7;//�����Բ
	}
	else if(sideLeft == 1 && sideRight == 1 && sideDown == 0 && sideUp == 1)
	{
		m_iIsWhichShape = TypeOfNavyLine_HorizonStraightLine; //8;//�����Բ
	}
	else
	{
		m_iIsWhichShape = 0;
	}
		//UpdateData(DipDataOnScreen);
	if (m_iIsWhichShape == 0)
		Template = "��ƥ��ģ��";
	if (m_iIsWhichShape == TypeOfNavyLine_Conner1)
		Template = "�ս�1";
	if (m_iIsWhichShape == TypeOfNavyLine_Conner2)
		Template = "�ս�2";
	if (m_iIsWhichShape == TypeOfNavyLine_Conner3)
		Template = "�ս�3";
	if (m_iIsWhichShape == TypeOfNavyLine_Conner4)
		Template = "�ս�4";
	if (m_iIsWhichShape == TypeOfNavyLine_HorizonStraightLine)
		Template = "ˮƽֱ��";
	if (m_iIsWhichShape == TypeOfNavyLine_VerticleStraightLine)
		Template = "��ֱֱ��";
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_IsAnyTemplate, Template);
}


void CMFCVideoDispDlg::SetPixelInImg(POINT pt, COLORREF color, BITMAPINFO BmpINF, unsigned char *pBmpData)
{
	int lineLen = BmpINF.bmiHeader.biBitCount / 8 * BmpINF.bmiHeader.biWidth;
	int ImgBitCount = BmpINF.bmiHeader.biBitCount / 8;
	pBmpData[(BmpINF.bmiHeader.biHeight - 1 - pt.y)*lineLen + pt.x*ImgBitCount] = GetBValue(color);
	pBmpData[(BmpINF.bmiHeader.biHeight - 1 - pt.y)*lineLen + pt.x*ImgBitCount + 1] = GetGValue(color);
	pBmpData[(BmpINF.bmiHeader.biHeight - 1 - pt.y)*lineLen + pt.x*ImgBitCount + 2] = GetRValue(color);
}

void CMFCVideoDispDlg::DrawLineInImg(POINT start_pt, POINT dest_pt, COLORREF color, BITMAPINFO BmpINF, unsigned char *pBmpData)
{
	int w, z;
	if (start_pt.x == dest_pt.x)
	{
		w = start_pt.x;
		for (z = start_pt.y; z <= dest_pt.y; z++)
		{
			SetPixelInImg({ w, z }, color, BmpINF, pBmpData);
		}

	}
	else
	{
		if (start_pt.y == dest_pt.y)
		{
			z = start_pt.y;
			for (w = start_pt.x; w <= dest_pt.x; w++)
			{
				SetPixelInImg({ w, z }, color, BmpINF, pBmpData);
			}
		}
		else
		{

		}
	}
}


void CMFCVideoDispDlg::DrawObjectCenterInIniImg()
{

	BITMAPINFO tempBitmapInfo = m_BitmapInfo;
	POINT startPoint, endPoint;
	int Y, U, V, C, D, E, R, G, B;
	int z, w;
	unsigned char *ptempImageDIBForScreenShow;
	if (m_BitmapInfo.bmiHeader.biBitCount == 24)
	{
		ptempImageDIBForScreenShow = new unsigned char[m_BitmapInfo.bmiHeader.biSizeImage];
		memcpy(ptempImageDIBForScreenShow, m_pImageIniDIB, m_BitmapInfo.bmiHeader.biSizeImage);
	}
	else
	{
		ptempImageDIBForScreenShow = new unsigned char[m_BitmapInfo.bmiHeader.biHeight*
			m_BitmapInfo.bmiHeader.biWidth * 3];
		for (z = 0; z < m_BitmapInfo.bmiHeader.biHeight; z++)
			for (w = 0; w < m_BitmapInfo.bmiHeader.biWidth; w++)
			{
				Y = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1];
				if ((w) % 2 == 0)
				{
					U = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 1];
					V = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 + 1];
				}
				else
				{
					U = m_pImageIniDIB[z *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 3];
					V = m_pImageIniDIB[z *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 1];
				}
				C = Y - 16;
				D = U - 128;
				E = V - 128;
				R = (298 * C + 409 * E + 128) >> 8;
				if (R < 0)R = 0;
				if (R > 255)R = 255;
				G = (298 * C - 100 * D - 208 * E + 128) >> 8;
				if (G < 0)G = 0;
				if (G > 255)G = 255;
				B = (298 * C + 516 * D + 128) >> 8;
				if (B < 0)B = 0;
				if (B > 255)B = 255;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3] = B;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1] = G;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2] = R;
			}
	}
	tempBitmapInfo.bmiHeader.biBitCount = 24;
	tempBitmapInfo.bmiHeader.biHeight = m_BitmapInfo.bmiHeader.biHeight;
	tempBitmapInfo.bmiHeader.biWidth = m_BitmapInfo.bmiHeader.biWidth;
	tempBitmapInfo.bmiHeader.biSizeImage = tempBitmapInfo.bmiHeader.biHeight*
		tempBitmapInfo.bmiHeader.biWidth * 3;
	tempBitmapInfo.bmiHeader.biCompression = 0;


	startPoint.x = (m_ObjectCenterInShrinkedImage.x) *JinZiTaCompress;
	startPoint.y = m_ObjectCenterInShrinkedImage.y *JinZiTaCompress;
	endPoint.x = (m_ObjectCenterInShrinkedImage.x) * JinZiTaCompress;
	endPoint.y = m_ObjectCenterInShrinkedImage.y * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//----

	startPoint.x = m_ObjectCenterInShrinkedImage.x *JinZiTaCompress;
	startPoint.y = (m_ObjectCenterInShrinkedImage.y) *JinZiTaCompress;
	endPoint.x = m_ObjectCenterInShrinkedImage.x * JinZiTaCompress;
	endPoint.y = (m_ObjectCenterInShrinkedImage.y) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = m_ObjectCenterInShrinkedImage.x *JinZiTaCompress;
	startPoint.y = (m_ObjectCenterInShrinkedImage.y) *JinZiTaCompress;
	endPoint.x = m_ObjectCenterInShrinkedImage.x * JinZiTaCompress;
	endPoint.y = (m_ObjectCenterInShrinkedImage.y) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = (m_ObjectCenterInShrinkedImage.x) *JinZiTaCompress;
	startPoint.y = m_ObjectCenterInShrinkedImage.y *JinZiTaCompress;
	endPoint.x = (m_ObjectCenterInShrinkedImage.x) * JinZiTaCompress;
	endPoint.y = m_ObjectCenterInShrinkedImage.y * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//--

	CRect temprect;
	CWnd* pwnd1;
	int checkval;

	pwnd1 = GetDlgItem(IDC_VideoProedDisp_STATIC);



	pwnd1->GetWindowRect(&temprect);
	ScreenToClient(temprect);


	CClientDC dc(pwnd1);
	HDC hDC = dc.GetSafeHdc();
	SetStretchBltMode(hDC, COLORONCOLOR);

	checkval = StretchDIBits(dc.m_hDC,
		0,//temprect.left,
		0,//temprect.top,
		temprect.Width(),
		temprect.Height(),
		0,
		0,
		m_BitmapInfo.bmiHeader.biWidth,
		m_BitmapInfo.bmiHeader.biHeight,
		ptempImageDIBForScreenShow,
		&tempBitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	free(ptempImageDIBForScreenShow);

}

void CMFCVideoDispDlg::DrawObjectBlockInIniImg()
{
	int i, w, z;
	int CmpHeight, CmpWidth;
	int Y, U, V, C, D, E, R, G, B;
	BITMAPINFO tempBitmapInfo = m_BitmapInfo;
	POINT startPoint, endPoint;

	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;

	//m_pobjectPosLeftUp = new POINT[m_ObjectNum];
	//m_pobjectPosRightDown = new POINT[m_ObjectNum];
	POINT pt1, pt2;

	unsigned char *ptempImageDIBForScreenShow;
	if (m_BitmapInfo.bmiHeader.biBitCount == 24)
	{
		ptempImageDIBForScreenShow = new unsigned char[m_BitmapInfo.bmiHeader.biSizeImage];
		memcpy(ptempImageDIBForScreenShow, m_pImageIniDIB, m_BitmapInfo.bmiHeader.biSizeImage);
	}
	else
	{
		ptempImageDIBForScreenShow = new unsigned char[m_BitmapInfo.bmiHeader.biHeight*
			m_BitmapInfo.bmiHeader.biWidth * 3];
		for (z = 0; z < m_BitmapInfo.bmiHeader.biHeight; z++)
			for (w = 0; w < m_BitmapInfo.bmiHeader.biWidth; w++)
			{
				Y = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1];
				if ((w) % 2 == 0)
				{
					U = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 1];
					V = m_pImageIniDIB[z*m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 + 1];
				}
				else
				{
					U = m_pImageIniDIB[z *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 3];
					V = m_pImageIniDIB[z *m_BitmapInfo.bmiHeader.biWidth * 2 + w * 2 + 1 - 1];
				}
				C = Y - 16;
				D = U - 128;
				E = V - 128;
				R = (298 * C + 409 * E + 128) >> 8;
				if (R < 0)R = 0;
				if (R > 255)R = 255;
				G = (298 * C - 100 * D - 208 * E + 128) >> 8;
				if (G < 0)G = 0;
				if (G > 255)G = 255;
				B = (298 * C + 516 * D + 128) >> 8;
				if (B < 0)B = 0;
				if (B > 255)B = 255;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3] = B;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1] = G;
				ptempImageDIBForScreenShow[(m_BitmapInfo.bmiHeader.biHeight - 1 - z)*m_BitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2] = R;
			}
	}
	tempBitmapInfo.bmiHeader.biBitCount = 24;
	tempBitmapInfo.bmiHeader.biHeight = m_BitmapInfo.bmiHeader.biHeight;
	tempBitmapInfo.bmiHeader.biWidth = m_BitmapInfo.bmiHeader.biWidth;
	tempBitmapInfo.bmiHeader.biSizeImage = tempBitmapInfo.bmiHeader.biHeight*
		tempBitmapInfo.bmiHeader.biWidth * 3;
	tempBitmapInfo.bmiHeader.biCompression = 0;
	///////////////////��������ʮ��//////////////////////////////////////////////////////////
	pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2 - 40;
	pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2 + 40;
	DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);
	pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2+1; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2 - 40;
	pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2+1; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2 + 40;
	DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);

	pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2 - 40; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2;
	pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2 + 40; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2;
	DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);
	pt1.x = tempBitmapInfo.bmiHeader.biWidth / 2 - 40; pt1.y = tempBitmapInfo.bmiHeader.biHeight / 2+1;
	pt2.x = tempBitmapInfo.bmiHeader.biWidth / 2 + 40; pt2.y = tempBitmapInfo.bmiHeader.biHeight / 2+1;
	DrawLineInImg(pt1, pt2, RGB(0, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);

	if (m_ObjectNum >= 1)
	{
		for (i = 0; i < m_ObjectNum; i++)
		{
			m_pobjectPosRightDown[i].x = 0;
			m_pobjectPosRightDown[i].y = 0;

			m_pobjectPosLeftUp[i].x = CmpWidth - 1;
			m_pobjectPosLeftUp[i].y = CmpHeight - 1;

		}

		for (z = 0; z < CmpHeight; z++)//��Ѱÿ��Ŀ������µ�
		{
			for (w = 0; w < CmpWidth; w++)
			{
				if (m_pEZHImgEvryTimeInSight[z*CmpWidth + w] != 0)
				{
					if (m_pobjectPosRightDown[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].x < w)
						m_pobjectPosRightDown[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].x = w;
					if (m_pobjectPosRightDown[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].y < z)
						m_pobjectPosRightDown[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].y = z;
				}
			}
		}
		for (z = CmpHeight - 1; z > 0; z--)//��Ѱÿ��Ŀ������ϵ�
		{
			for (w = CmpWidth - 1; w > 0; w--)
			{
				if (m_pEZHImgEvryTimeInSight[z*CmpWidth + w] != 0)
				{
					if (m_pobjectPosLeftUp[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].x > w)
						m_pobjectPosLeftUp[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].x = w;
					if (m_pobjectPosLeftUp[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].y > z)
						m_pobjectPosLeftUp[m_pEZHImgEvryTimeInSight[z*CmpWidth + w] - 1].y = z;
				}
			}
		}
		

		


		for (i = 0; i < m_ObjectNum; i++)//����ÿ��Ŀ��ı߿�
		{
			startPoint.x = m_pobjectPosLeftUp[i].x *JinZiTaCompress;
			startPoint.y = m_pobjectPosLeftUp[i].y *JinZiTaCompress;
			endPoint.x = m_pobjectPosRightDown[i].x * JinZiTaCompress;
			endPoint.y = m_pobjectPosLeftUp[i].y * JinZiTaCompress;
			DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//----

			startPoint.x = m_pobjectPosLeftUp[i].x *JinZiTaCompress;
			startPoint.y = m_pobjectPosLeftUp[i].y *JinZiTaCompress;
			endPoint.x = m_pobjectPosLeftUp[i].x * JinZiTaCompress;
			endPoint.y = m_pobjectPosRightDown[i].y * JinZiTaCompress;
			DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

			startPoint.x = m_pobjectPosRightDown[i].x *JinZiTaCompress;
			startPoint.y = m_pobjectPosLeftUp[i].y *JinZiTaCompress;
			endPoint.x = m_pobjectPosRightDown[i].x * JinZiTaCompress;
			endPoint.y = m_pobjectPosRightDown[i].y * JinZiTaCompress;
			DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

			startPoint.x = m_pobjectPosLeftUp[i].x *JinZiTaCompress;
			startPoint.y = m_pobjectPosRightDown[i].y *JinZiTaCompress;
			endPoint.x = m_pobjectPosRightDown[i].x * JinZiTaCompress;
			endPoint.y = m_pobjectPosRightDown[i].y * JinZiTaCompress;
			DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//--
		}
	}
	if (m_iIsWhichShape == 0 || m_iIsWhichShape == TypeOfNavyLine_HorizonStraightLine || m_iIsWhichShape == TypeOfNavyLine_VerticleStraightLine)
	{
		if (m_LimitedObjectCenterInShrinkedImage.x < 3)
			m_LimitedObjectCenterInShrinkedImage.x = 3;
		if (m_LimitedObjectCenterInShrinkedImage.x > CmpWidth - 3)
			m_LimitedObjectCenterInShrinkedImage.x = CmpWidth - 3;
		if (m_LimitedObjectCenterInShrinkedImage.y < 3)
			m_LimitedObjectCenterInShrinkedImage.y = 3;
		if (m_LimitedObjectCenterInShrinkedImage.y > CmpHeight - 3)
			m_LimitedObjectCenterInShrinkedImage.y = CmpHeight - 3;

		for (ppt.x = (m_LimitedObjectCenterInShrinkedImage.x - 2)*JinZiTaCompress; ppt.x < (m_LimitedObjectCenterInShrinkedImage.x + 2)*JinZiTaCompress; ++ppt.x)
			for (ppt.y = (m_LimitedObjectCenterInShrinkedImage.y - 2)*JinZiTaCompress; ppt.y < (m_LimitedObjectCenterInShrinkedImage.y + 2)*JinZiTaCompress; ++ppt.y)
				SetPixelInImg(ppt, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);
	}
	/*startPoint.x = (m_LimitedObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_LimitedObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_LimitedObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_LimitedObjectCenterInShrinkedImage.y - 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//----
	
	startPoint.x = (m_LimitedObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_LimitedObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_LimitedObjectCenterInShrinkedImage.x - 3) * JinZiTaCompress;
	endPoint.y = (m_LimitedObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = (m_LimitedObjectCenterInShrinkedImage.x + 3) *JinZiTaCompress;
	startPoint.y = (m_LimitedObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_LimitedObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_LimitedObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = (m_LimitedObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_LimitedObjectCenterInShrinkedImage.y + 3) *JinZiTaCompress;
	endPoint.x = (m_LimitedObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_LimitedObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 255), tempBitmapInfo, ptempImageDIBForScreenShow);//--
	*/
	if (m_iIsWhichShape == TypeOfNavyLine_Conner1 || m_iIsWhichShape == TypeOfNavyLine_Conner2 || m_iIsWhichShape == TypeOfNavyLine_Conner3 || m_iIsWhichShape == TypeOfNavyLine_Conner4)
	{
		if (m_AngleObjectCenterInShrinkedImage.x < 3)
			m_AngleObjectCenterInShrinkedImage.x = 3;
		if (m_AngleObjectCenterInShrinkedImage.x > CmpWidth - 3)
			m_AngleObjectCenterInShrinkedImage.x = CmpWidth - 3;
		if (m_AngleObjectCenterInShrinkedImage.y < 3)
			m_AngleObjectCenterInShrinkedImage.y = 3;
		if (m_AngleObjectCenterInShrinkedImage.y > CmpHeight - 3)
			m_AngleObjectCenterInShrinkedImage.y = CmpHeight - 3;

		for (ppt.x = (m_AngleObjectCenterInShrinkedImage.x - 2)*JinZiTaCompress; ppt.x < (m_AngleObjectCenterInShrinkedImage.x + 2)*JinZiTaCompress; ++ppt.x)
			for (ppt.y = (m_AngleObjectCenterInShrinkedImage.y - 2)*JinZiTaCompress; ppt.y < (m_AngleObjectCenterInShrinkedImage.y + 2)*JinZiTaCompress; ++ppt.y)
				SetPixelInImg(ppt, RGB(255, 0, 0), tempBitmapInfo, ptempImageDIBForScreenShow);
	}
	/*startPoint.x = (m_AngleObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_AngleObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_AngleObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_AngleObjectCenterInShrinkedImage.y - 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 0), tempBitmapInfo, ptempImageDIBForScreenShow);//----

	startPoint.x = (m_AngleObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_AngleObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_AngleObjectCenterInShrinkedImage.x - 3) * JinZiTaCompress;
	endPoint.y = (m_AngleObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 0), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = (m_AngleObjectCenterInShrinkedImage.x + 3) *JinZiTaCompress;
	startPoint.y = (m_AngleObjectCenterInShrinkedImage.y - 3) *JinZiTaCompress;
	endPoint.x = (m_AngleObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_AngleObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 0), tempBitmapInfo, ptempImageDIBForScreenShow);//|

	startPoint.x = (m_AngleObjectCenterInShrinkedImage.x - 3) *JinZiTaCompress;
	startPoint.y = (m_AngleObjectCenterInShrinkedImage.y + 3) *JinZiTaCompress;
	endPoint.x = (m_AngleObjectCenterInShrinkedImage.x + 3) * JinZiTaCompress;
	endPoint.y = (m_AngleObjectCenterInShrinkedImage.y + 3) * JinZiTaCompress;
	DrawLineInImg(startPoint, endPoint, RGB(255, 0, 0), tempBitmapInfo, ptempImageDIBForScreenShow);//--
	*/
	/////////////////////////////////////////////////////////////////////////
	CRect temprect;
	CWnd* pwnd1;
	int checkval;

	pwnd1 = GetDlgItem(IDC_VideoProedDisp_STATIC);



	pwnd1->GetWindowRect(&temprect);
	ScreenToClient(temprect);


	CClientDC dc(pwnd1);
	HDC hDC = dc.GetSafeHdc();
	SetStretchBltMode(hDC, COLORONCOLOR);

	checkval = StretchDIBits(dc.m_hDC,
		0,//temprect.left,
		0,//temprect.top,
		temprect.Width(),
		temprect.Height(),
		0,
		0,
		m_BitmapInfo.bmiHeader.biWidth,
		m_BitmapInfo.bmiHeader.biHeight,
		ptempImageDIBForScreenShow,
		&tempBitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	free(ptempImageDIBForScreenShow);
	/*if (m_pobjectPosLeftUp != NULL)
	{
		free(m_pobjectPosLeftUp);
		m_pobjectPosLeftUp = NULL;
	}
	if (m_pobjectPosRightDown != NULL)
	{
		free(m_pobjectPosRightDown);
		m_pobjectPosRightDown = NULL;
	}*/
}


int CMFCVideoDispDlg::ImageProcessmbfg(unsigned int *b, int row, int col)//Ŀ����ͨ��ָ�
{
	int i, j, x, y, ii, jj;//ѭ������
	int m, n;
	int k = 1;//��¼Ŀ���������
	int win = 1;//3*3���ڸ������������ݹ���ȱ���
	int flag = 0;
	unsigned int *a = new unsigned int[row*col];
	struct WZH
	{
		unsigned int x;
		unsigned int y;};
	WZH *wzh = new WZH[row*col];
	//unsigned char *point[81*81];
	for (i = 0; i<row; i++)
		for (j = 0; j<col; j++)
		{
			a[i*col + j] = b[i*col + j];
			b[i*col + j] = 0;
			//point[i*col+j]=NULL;
		}
	for (ii = 0; ii<row; ii++)
	{
		for (jj = 0; jj<col; jj++)
		{
			if (a[ii*col + jj] == 0 || b[ii*col + jj] != 0)
				continue;
			i = ii;
			j = jj;
		next_win1:
			for (x = i - 1; x <= i + 1; x++)
			{
				for (y = j - 1; y <= j + 1; y++)
				{

					if (x == i&&y == j) continue;
					else if (x<0 || x >= row || y<0 || y >= col) continue;
					else if (a[x*col + y] == 1 && b[x*col + y] == 0)
					{
						flag = 1;
						if (k<256)
							b[x*col + y] = k;
						else
							b[x*col + y] = 255;
						//��¼�ô�����������a�е�λ�ã���i,j
						//��¼�����ڴ˴����е�λ��
						wzh[x*col + y].x = i;
						wzh[x*col + y].y = j;
						//point[x*col+y]=&a[x*col+y];
						i = x;
						j = y;
						win++;
						goto next_win1;
					}
				next_win:		x = x;
				}
			}
			if (win>1)
			{
				//һ��3*3С���ڴ�����
				//�ص��ô��ڵĽ���λ�ã�
				//���ҵ�������(i,j)������һ���ڵ�λ��
				m = i; n = j;
				x = m;
				y = n;
				i = wzh[m*col + n].x;
				j = wzh[m*col + n].y;
				win--;//���ڸ�����1
				goto next_win;
			}
			if (flag == 1)
			{
				k++;
				flag = 0;
			}
		}
	}
	free(a);
	free(wzh);
	return(k - 1);
}

void CMFCVideoDispDlg::Filter(unsigned int *b, int row, int col, int n)//�����Ŀ�����������Ŀ���˵�
{
	int i, j, maxs, object_num;
	int *s = new int[n + 1];   //��¼��ͬĿ�����������
	

	for (i = 0; i < n+1; i++)
		s[i] = 0;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if(b[i*col + j]!=0)
				s[b[i*col + j]]++;
		}
	}
	maxs = s[1];
	object_num = 1;
	for (i = 2; i <= n ; i++)
	{
		if (s[i]>maxs)
		{
			maxs = s[i];
			object_num = i;
		}
	}
	
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if (b[i*col + j] == object_num)
				b[i*col + j] = 1;
			else 
				b[i*col + j] = 0;
		}
	}
	free(s);
	return;
}
void CMFCVideoDispDlg::GetCornerCenter(unsigned int *rniEZHImage, int height, int width)
{
	//Ѱ�ߣ���������Ե��
	struct
	{
		int row;
		int col;
		BOOL havePoint;
	}point1, point2, conerCenter, centerOfTwoPoint;
	int row, col;
	int currentCol, currentRow;
	int flag;
	float kk1, kk2;
	flag = 0;
	point1.row = 0;
	point1.col = 0;
	point1.havePoint = 0;
	point2.row = 0;
	point2.col = 0;
	point2.havePoint = 0;
	conerCenter.row = 0;
	conerCenter.col = 0;
	for (col = 10; col < width - 10; col++)
	{
		if (col == 10)
		{
			if (rniEZHImage[10 * width + col] == 1)
			{
				point1.row = 10;
				point1.col = col;
				point1.havePoint = 1;
			}

		}

		else if (rniEZHImage[10 * width + col] == 1 && rniEZHImage[10 * width + col - 1] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = 10;
				point1.col = col;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = 10;
				point2.col = col;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (row = 10; row < height - 10; row++)
	{
		if (row == 10)
		{
			if (rniEZHImage[10 * width + width - 10 - 1] == 1)
			{
				if (point1.havePoint == 0)
				{
					point1.row = 10;
					point1.col = width - 10 - 1;
					point1.havePoint = 1;
				}
				else
				{
					point2.row = 10;
					point2.col = width - 10 - 1;
					point2.havePoint = 1;
					break;
				}
			}
		}
		else if (rniEZHImage[row * width + width - 10 - 1] == 1 && rniEZHImage[(row - 1) * width + width - 10 - 1] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = row;
				point1.col = width - 10 - 1;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = row;
				point2.col = width - 10 - 1;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (col = width - 10 - 1; col >9; col--)
	{
		if (col == width - 10 - 1)
		{
			if (rniEZHImage[(height - 10 - 1) * width + col] == 1)
			{
				point1.row = height - 10 - 1;
				point1.col = col;
				point1.havePoint = 1;
			}

		}

		else if (rniEZHImage[(height - 10 - 1) * width + col] == 1 && rniEZHImage[(height - 10 - 1) * width + col + 1] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = height - 10 - 1;
				point1.col = col;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = height - 10 - 1;
				point2.col = col;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (row = height - 10 - 1; row > 9; row--)
	{
		if (row == height - 10 - 1)
		{
			if (rniEZHImage[row * width + 10] == 1)
			{
				if (point1.havePoint == 0)
				{
					point1.row = row;
					point1.col = 10;
					point1.havePoint = 1;
				}
				else
				{
					point2.row = row;
					point2.col = 10;
					point2.havePoint = 1;
					break;
				}
			}
		}
		else if (rniEZHImage[row * width + 10] == 1 && rniEZHImage[(row + 1) * width + 10] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = row;
				point1.col = 10;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = row;
				point2.col = 10;
				point2.havePoint = 1;
				break;
			}
		}
	}
	//////�����ķ���ǣ�������õ����ĺ�����������
	centerOfTwoPoint.row = (point1.row + point2.row) / 2;
	centerOfTwoPoint.col = (point1.col + point2.col) / 2;
	float searchAngle, tempAng;
	searchAngle = atan2(m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row,
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col);
	tempAng = searchAngle * 180 / 3.1415926;
	////�ҽǵ�
	int sumrow, sumcol, sum;
	float kk;
	
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row == 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col>0)
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (col = width - 10; col > m_ObjectCenterInShrinkedImage.COLUMN; col--)
		{
			
			for (row = 0; row<height - 10; row++)
			{
				if (rniEZHImage[row * width + col] == 1)
				{
					sumrow += row;
					sumcol += col;
					sum++;
				}
			}
			if (sum >= 1)
				break;
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row == 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col<0)
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (col = 10; col < m_ObjectCenterInShrinkedImage.COLUMN; col++)
		{

			for (row = 0; row<height - 10; row++)
			{
				if (rniEZHImage[row * width + col] == 1)
				{
					sumrow += row;
					sumcol += col;
					sum++;
				}
			}
			if (sum >= 1)
				break;
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row > 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col == 0)//����
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (row = height - 10; row > m_ObjectCenterInShrinkedImage.ROW; row--)
		{

			for (col = 0; col<width - 10; col++)
			{
				if (rniEZHImage[row * width + col] == 1)
				{
					sumrow += row;
					sumcol += col;
					sum++;
				}
			}
			if (sum >= 1)
				break;
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row < 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col == 0)//����
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (row = 10; row < m_ObjectCenterInShrinkedImage.ROW; row++)
		{

			for (col = 0; col<width - 10; col++)
			{
				if (rniEZHImage[row * width + col] == 1)
				{
					sumrow += row;
					sumcol += col;
					sum++;
				}
			}
			if (sum >= 1)
				break;
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	////////////////////////////////////
	if (tempAng>-90 && tempAng < 0)
	{
		kk1 = (float)(point1.row - point2.row) / (float)(point1.col - point2.col);
		kk2 = (float)(point1.col - point2.col) / (float)(point1.row - point2.row);
		flag = 0;
		for (currentCol = width - 11; currentCol > 9; currentCol--)
		{
			for (col = currentCol; col < width - 10; col++)
			{
				row = (int)(kk1*(col - currentCol)) + 10;
				if (row>height - 10)
					row = height - 10;
				if (row < 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 1)
				break;
		}
		if (flag == 0)
		{
			for (currentRow = 11; currentRow < height - 10; currentRow++)
			{
				for (row = currentRow; row <height -10; row++)
				{
					col = (int)(kk2*(row - currentRow)) + 10;
					if (col>width - 10)
						col = width - 10;
					if (col < 10)
						col = 10;
					if (rniEZHImage[row*width + col] == 1)
						break;
				}
				if (rniEZHImage[row*width + col] == 1)
					break;
			}
		}
		conerCenter.row = row;
		conerCenter.col = col;	
	}
	/////////////////////////////////////////////////
	if (tempAng > -180 && tempAng < -90)
	{
		kk1 = (float)(point1.row - point2.row) / (float)(point1.col - point2.col);
		kk2 = (float)(point1.col - point2.col) / (float)(point1.row - point2.row);
		flag = 0;
		for (currentCol = 10; currentCol < width - 11; currentCol++)
		{
			for (col = 10; col < currentCol; col++)
			{
				row = (int)(kk1*(col - currentCol)) + 10;
				if (row>height - 10)
					row = height - 10;
				if (row < 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 1)
				break;
		}
			if (flag == 0)
			{
				for (currentRow = 11; currentRow < height - 10; currentRow++)
				{
					for (row = currentRow; row <height - 10; row++)
					{
						col = (int)(kk2*(row - currentRow)) + width - 10 - 1;
						if (col>width - 10)
							col = width - 10;
						if (col < 10)
							col = 10;
						if (rniEZHImage[row*width + col] == 1)
							break;
					}
					if (rniEZHImage[row*width + col] == 1)
						break;
				}
			}
			conerCenter.row = row;
			conerCenter.col = col;
	}
	////////////////////////////////////////////////
		if (tempAng > 90 && tempAng < 180)
		{
			kk1 = (float)(point1.row - point2.row) / (float)(point1.col - point2.col);
			kk2 = (float)(point1.col - point2.col) / (float)(point1.row - point2.row);
			flag = 0;
			for (currentCol = 10; currentCol < width - 11; currentCol++)
			{
				for (col = 10; col < currentCol; col++)
				{
					row = (int)(kk1*(col - currentCol)) + height - 11;
					if (row>height - 10)
						row = height - 10;
					if (row < 10)
						row = 10;
					if (rniEZHImage[row*width + col] == 1)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
					break;
			}
			if (flag == 0)
			{
				for (currentRow = height - 11; currentRow > 10; currentRow--)
				{
					for (row = currentRow; row <height - 10; row++)
					{
						col = (int)(kk2*(row - currentRow)) + width - 10 - 1;
						if (col>width - 10)
							col = width - 10;
						if (col < 10)
							col = 10;
						if (rniEZHImage[row*width + col] == 1)
							break;
					}
					if (rniEZHImage[row*width + col] == 1)
						break;
				}
			}
			conerCenter.row = row;
			conerCenter.col = col;
		}
   ////////////////////////////////////////////////
			if (tempAng > 0 && tempAng < 90)
			{
				kk1 = (float)(point1.row - point2.row) / (float)(point1.col - point2.col);
				kk2 = (float)(point1.col - point2.col) / (float)(point1.row - point2.row);
				flag = 0;
				for (currentCol = width - 11; currentCol > 9; currentCol--)
				{
					for (col = currentCol; col < width - 11; col++)
					{
						row = (int)(kk1*(col - currentCol)) + height - 11;
						if (row>height - 10)
							row = height - 10;
						if (row < 10)
							row = 10;
						if (rniEZHImage[row*width + col] == 1)
						{
							flag = 1;
							break;
						}
					}
					if (flag == 1)
						break;
				}
				if (flag == 0)
				{
					for (currentRow = height - 11; currentRow > 10; currentRow--)
					{
						for (row = currentRow; row > 10; row--)
						{
							col = (int)(kk2*(row - currentRow)) + 10;
							if (col > width - 10)
								col = width - 10;
							if (col < 10)
								col = 10;
							if (rniEZHImage[row*width + col] == 1)
								break;
						}
						if (rniEZHImage[row*width + col] == 1)
							break;
					}
				}
				conerCenter.row = row;
				conerCenter.col = col;
			}
	m_AngleObjectCenterInShrinkedImage.ROW = conerCenter.row;
	m_AngleObjectCenterInShrinkedImage.COLUMN = conerCenter.col;
	m_DeltaCenterRowForDisp = m_AngleObjectCenterInShrinkedImage.ROW - height / 2;
	m_DeltaCenterColForDisp = m_AngleObjectCenterInShrinkedImage.COLUMN - (width / 2);
}
void CMFCVideoDispDlg::GetCornerCenter2(unsigned int *rniEZHImage, int height, int width)
{
	//Ѱ�ߣ���������Ե��
	struct
	{
		int row;
		int col;
		BOOL havePoint;
	}point1, point2, conerCenter, centerOfTwoPoint;
	int row, col;
	point1.row = 0;
	point1.col = 0;
	point1.havePoint = 0;
	point2.row = 0;
	point2.col = 0;
	point2.havePoint = 0;
	conerCenter.row = 0;
	conerCenter.col = 0;
	for (col = 10; col < width - 10; col++)
	{
		if (col == 10)
		{
			if (rniEZHImage[10 * width + col] == 1)
			{
				point1.row = 10;
				point1.col = col;
				point1.havePoint = 1;
			}

		}

		else if (rniEZHImage[10 * width + col] == 1&&rniEZHImage[10 * width + col-1] == 0)
		{
			if(point1.havePoint ==0)
			{
				point1.row = 10;
				point1.col = col;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = 10;
				point2.col = col;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (row = 10; row < height - 10; row++)
	{
		if (row == 10)
		{
			if (rniEZHImage[10 * width + width-10-1] == 1)
			{
				if (point1.havePoint == 0)
				{
					point1.row = 10;
					point1.col = width - 10 - 1;
					point1.havePoint = 1;
				}
				else
				{
					point2.row = 10;
					point2.col = width - 10 - 1;
					point2.havePoint = 1;
					break;
				}
			}
		}
		else if (rniEZHImage[row * width + width - 10 - 1] == 1 && rniEZHImage[(row - 1) * width + width - 10 - 1] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = row;
				point1.col = width - 10 - 1;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = row;
				point2.col = width - 10 - 1;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (col = width - 10 -1; col >9; col--)
	{
		if (col == width - 10 - 1)
		{
			if (rniEZHImage[(height - 10 - 1) * width + col] == 1)
			{
				point1.row = height - 10 - 1;
				point1.col = col;
				point1.havePoint = 1;
			}

		}

		else if (rniEZHImage[(height - 10 - 1) * width + col] == 1 && rniEZHImage[(height - 10 - 1) * width + col + 1] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = height - 10 - 1;
				point1.col = col;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = height - 10 - 1;
				point2.col = col;
				point2.havePoint = 1;
				break;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (row = height - 10 -1; row > 9; row--)
	{
		if (row == height - 10 - 1)
		{
			if (rniEZHImage[row * width + 10] == 1)
			{
				if (point1.havePoint == 0)
				{
					point1.row = row;
					point1.col = 10;
					point1.havePoint = 1;
				}
				else
				{
					point2.row = row;
					point2.col = 10;
					point2.havePoint = 1;
					break;
				}
			}
		}
		else if (rniEZHImage[row * width + 10] == 1 && rniEZHImage[(row + 1) * width + 10] == 0)
		{
			if (point1.havePoint == 0)
			{
				point1.row = row;
				point1.col = 10;
				point1.havePoint = 1;
			}
			else
			{
				point2.row = row;
				point2.col = 10;
				point2.havePoint = 1;
				break;
			}
		}
	}
	//////�����ķ���ǣ�������õ����ĺ�����������
	centerOfTwoPoint.row = (point1.row + point2.row) / 2;
	centerOfTwoPoint.col = (point1.col + point2.col) / 2;
	float searchAngle, tempAng;
	searchAngle = atan2(m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row,
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col);
	tempAng = searchAngle * 180 / 3.1415926;
	////�ҽǵ�
	int sumrow, sumcol, sum;
	float kk;
	if (tempAng == 0 || (tempAng > 0 && tempAng<90) || (tempAng>-90 && tempAng < 0))
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (col = m_ObjectCenterInShrinkedImage.COLUMN; col < width - 10; col++)
		{
			kk = (float)(m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row) / 
				(float)(m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col);
			row = kk*(col - m_ObjectCenterInShrinkedImage.COLUMN) + m_ObjectCenterInShrinkedImage.ROW;
			if (row > height - 10)
				row = height - 10;
			if (row <10)
				row = 10;
			if (rniEZHImage[row * width + col] == 1)
			{
				sumrow += row;
				sumcol += col;
				sum++;
			}
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if ((m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row==0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col>0)
		|| (tempAng > 0 && tempAng<90) || (tempAng>-90 && tempAng < 0))
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (col = m_ObjectCenterInShrinkedImage.COLUMN; col < width - 10; col++)
		{
			kk = (float)(m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row) /
				(float)(m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col);
			row = kk*(col - m_ObjectCenterInShrinkedImage.COLUMN) + m_ObjectCenterInShrinkedImage.ROW;
			if (row > height - 10)
				row = height - 10;
			if (row <10)
				row = 10;
			if (rniEZHImage[row * width + col] == 1)
			{
				sumrow += row;
				sumcol += col;
				sum++;
			}
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if ((m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row == 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col<0)
		|| (tempAng > 90 && tempAng<180) || (tempAng>-180 && tempAng < -90))
	{ 
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (col = m_ObjectCenterInShrinkedImage.COLUMN; col >=10 ; col--)
		{
			kk = (float)(m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row) /
				(float)(m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col);
			row = kk*(col - m_ObjectCenterInShrinkedImage.COLUMN) + m_ObjectCenterInShrinkedImage.ROW;
			if (row > height - 10)
				row = height - 10;
			if (row <10)
				row = 10;
			if (rniEZHImage[row * width + col] == 1)
			{
				sumrow += row;
				sumcol += col;
				sum++;
			}
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row > 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col == 0)//����
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (row = m_ObjectCenterInShrinkedImage.ROW; row< height-10; row++)
		{
			if (rniEZHImage[row * width + m_ObjectCenterInShrinkedImage.COLUMN] == 1)
			{
				sumrow += row;
				sumcol += col;
				sum++;
			}
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	if (m_ObjectCenterInShrinkedImage.ROW - centerOfTwoPoint.row < 0 &&
		m_ObjectCenterInShrinkedImage.COLUMN - centerOfTwoPoint.col == 0)//����
	{
		sumrow = 0;
		sumcol = 0;
		sum = 0;
		for (row = m_ObjectCenterInShrinkedImage.ROW; row>= 10; row--)
		{
			if (rniEZHImage[row * width + m_ObjectCenterInShrinkedImage.COLUMN] == 1)
			{
				sumrow += row;
				sumcol += col;
				sum++;
			}
		}
		conerCenter.row = int(float(sumrow) / sum);
		conerCenter.col = int(float(sumcol) / sum);
	}
	m_AngleObjectCenterInShrinkedImage.ROW = conerCenter.row;

	m_AngleObjectCenterInShrinkedImage.COLUMN = conerCenter.col;
}

void CMFCVideoDispDlg::GetCornerCenter1(unsigned int *rniEZHImage, int height, int width)
{
	int row, col;
	float kk;
	int currentCol, currentRow;
	//int CmpHeight, CmpWidth;
	//CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	//CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	struct
	{
		int row;
		int col;
		BOOL havePoint;
	}startPoint1, startPoint2, startPoint3, startPoint4;
	for (row = 10; row < height - 10; row++)
	{
		if (rniEZHImage[row*width + 10] == 1)
		{
			startPoint1.row = row;
			startPoint1.col = 10;
			startPoint1.havePoint = 1;
			break;
		}
	}
	if (row == height - 10)
		startPoint1.havePoint = 0;
	for (col = 10; col < width - 10; col++)
	{
		if (rniEZHImage[(height-11)*width + col] == 1)
		{
			startPoint2.row = height - 11;
			startPoint2.col = col;
			startPoint2.havePoint = 1;
			break;
		}
	}
	if (col == width - 10)
		startPoint2.havePoint = 0;
	for (row = 10; row < height - 10; row++)
	{
		if (rniEZHImage[row*width + width-11] == 1)
		{
			startPoint3.row = row;
			startPoint3.col = width - 11;
			startPoint3.havePoint = 1;
			break;
		}
	}
	if (row == height - 10)
		startPoint3.havePoint = 0;
	for (col = 10; col < width - 10; col++)
	{
		if (rniEZHImage[10*width + col] == 1)
		{
			startPoint4.row = 10;
			startPoint4.col = col;
			startPoint4.havePoint = 1;
			break;
		}
	}
	if (col == width - 10)
		startPoint4.havePoint = 0;
	
	if (startPoint1.havePoint == 1 && startPoint2.havePoint == 1 &&
		startPoint3.havePoint == 0 && startPoint4.havePoint == 0)
	{
		kk = (float)(startPoint2.row - startPoint1.row) / (float)(startPoint2.col - startPoint1.col);
		for (currentCol = width - 11; currentCol>9; currentCol--)
		{
			for (col = currentCol; col < width - 10; col++)
			{
				row = (int)(kk*(col - currentCol)) + 10;
				if (row>height - 10)
					row = height - 10;
				if (row< 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
					break;
			}
			if (rniEZHImage[row*width + col] == 1)
				break;
		}
	}
	if (startPoint2.havePoint == 1 && startPoint3.havePoint == 1 &&
		startPoint1.havePoint == 0 && startPoint4.havePoint == 0)
	{
		kk = (float)(startPoint2.row - startPoint3.row) / (float)(startPoint2.col - startPoint3.col);
		for (currentCol = 10; currentCol<width - 11; currentCol++)
		{
			for (col = 10; col < currentCol; col++)
			{
				row = (int)(kk*(col - currentCol)) + 10;
				if (row>height - 10)
					row = height - 10;
				if (row< 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
					break;
			}
			if (rniEZHImage[row*width + col] == 1)
				break;
		}
	}
	if (startPoint1.havePoint == 1 && startPoint4.havePoint == 1 &&
		startPoint2.havePoint == 0 && startPoint3.havePoint == 0)
	{
		kk = (float)(startPoint1.row - startPoint4.row) / (float)(startPoint1.col - startPoint4.col);
		for (currentCol = width - 11; currentCol>9; currentCol--)
		{
			for (col = currentCol; col < width-11; col++)
			{
				row = (int)(kk*(col - currentCol)) + height - 11;
				if (row>height - 10)
					row = height - 10;
				if (row< 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
					break;
			}
			if (rniEZHImage[row*width + col] == 1)
				break;
		}
	}
	if (startPoint3.havePoint == 1 && startPoint4.havePoint == 1 &&
		startPoint1.havePoint == 0 && startPoint2.havePoint == 0)
	{
		kk = (float)(startPoint3.row - startPoint4.row) / (float)(startPoint3.col - startPoint4.col);
		for (currentCol = 10; currentCol<width - 11; currentCol++)
		{
			for (col = 10; col < currentCol; col++)
			{
				row = (int)(kk*(col - currentCol)) + height - 11;
				if (row>height - 10)
					row = height - 10;
				if (row< 10)
					row = 10;
				if (rniEZHImage[row*width + col] == 1)
					break;
			}
			if (rniEZHImage[row*width + col] == 1)
				break;
		}
	}
	m_AngleObjectCenterInShrinkedImage.y = row;
	m_AngleObjectCenterInShrinkedImage.x = col;
	//m_pianyix = col;
	//m_pianyiy = row;
}




UINT CommWatchProc(LPVOID lpParam)
{
	int i = 0, j = 0;
	char oneTimeBuffer[40];
	DWORD dwEvtMask = 0, dwErrorFlags, dwBytesRead, dwError;
	COMSTAT ComStat;
	DWORD dwLength = 10;
	CMFCVideoDispDlg *ptempDlg = (CMFCVideoDispDlg *)lpParam;
	SetCommMask(ptempDlg->mhCom, EV_RXCHAR | EV_TXEMPTY);//����Щ�����¼���Ҫ����
	while (ptempDlg->mhCom != NULL)
	{
		WaitCommEvent(ptempDlg->mhCom, &dwEvtMask, NULL);// �ȴ�����ͨ���¼��ķ���
		//��ⷵ�ص�dwEvtMask��֪��������ʲô�����¼�
		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		{ // �������������ݵ���
			ClearCommError(ptempDlg->mhCom, &dwErrorFlags, &ComStat);
			if (dwErrorFlags == CE_RXOVER)
			{
				ptempDlg->CleanRxBufferRegion();
			}
			dwLength = ComStat.cbInQue; //���뻺�����ж������ݣ�
			if (dwLength > 0)
			{
				BOOL fReadStat;
				//ptempDlg->m_bThereAreDataReadingFromSerial = TRUE;
				fReadStat = ReadFile(ptempDlg->mhCom, oneTimeBuffer, dwLength,
					&dwBytesRead,
					&ptempDlg->m_OverlappedRead); //������
				//ע:������CreareFile()ʱʹ����FILE_FLAG_OVERLAPPED,����ReadFile()Ҳ����ʹ��
				//LPOVERLAPPED�ṹ.����,�����᲻��ȷ�ر���������������.
				//ʹ��LPOVERLAPPED�ṹ, ReadFile()��������,���صȴ����������,ʵ�ַ�����
				//ͨ��.��ʱ, ReadFile()����FALSE, GetLastError()����ERROR_IO_PENDING.
				if (!fReadStat)
				{
					if (GetLastError() == ERROR_IO_PENDING)
					{
						while (!GetOverlappedResult(ptempDlg->mhCom,
							&ptempDlg->m_OverlappedRead,
							&dwBytesRead,
							TRUE))
						{
							dwError = GetLastError();
							if (dwError == ERROR_IO_INCOMPLETE) continue;
							//����������û�ж��꣬����
							//::PostMessage((HWND)hSendWnd,WM_NOTIFYPROCESS,0,0);//֪ͨ���̣߳������յ����ݡ���}
							return 0;
						}
					}
				}
				else
				{
					//FILE *fp=NULL;
					//fp=fopen("data.txt","a+");
					for (i = 0; i < SAVEDATALENGTH-dwLength; i++)
					{
						ptempDlg->mAllRemFrameData[i] = ptempDlg->mAllRemFrameData[i + dwLength];
					}
					for (i = SAVEDATALENGTH - 1; i>=SAVEDATALENGTH - dwLength; i--)
					{
						//���ݰ��Ӻ���ǰ��
						ptempDlg->mAllRemFrameData[i] = oneTimeBuffer[dwLength - 1 -(SAVEDATALENGTH - 1-i)];


					}
					ptempDlg->FindAllFrames();
					//ptempDlg->m_ctlParaIsChanged = 1;
				}
			}
		}
	}
	return dwLength;
}

void CMFCVideoDispDlg::FindAllFrames()
{
	int pos = 0;
	int i, k, p;
	int datasize;
	unsigned char XORdata, CommandType, temp1;
	char temp2;
	int tempPara;
	unsigned long count;
	for (i = 0; i < SAVEDATALENGTH; i++)
	{
		if (mAllRemFrameData[i] == 0x00fa)   //0XfA
		{
			if ((i + 1) < SAVEDATALENGTH && mAllRemFrameData[i + 1] == 'M')   //0X4D
			{
				if ((i + 2) < SAVEDATALENGTH && mAllRemFrameData[i + 2] == 0x003c)   //0X3C
				{//�ҵ�һ֡

					i = i + 3;//˵��һ֡��ʼ,��ʱi��datasize�ֽ���
					datasize = mAllRemFrameData[i];
					if ((i + 1 + datasize + 1)<SAVEDATALENGTH)
					{
						XORdata = 0;

						for (k = 0; k<datasize + 2; k++)
						{
							XORdata ^= mAllRemFrameData[i + k];
						}
						if (XORdata == mAllRemFrameData[i + datasize + 2])//У��λ��ȷ��
						{
							CommandType = mAllRemFrameData[i + 1];
							
							if (CommandType == MsgRecParaFromPx)
							{
								//tempPara = mAllRemFrameData[i + 2];
								//temp1 = mAllRemFrameData[i + 2 + 1];
								//tempPara = (tempPara << 8) + temp1;
								//m_KpOut = tempPara / 100.;

								temp1 = mAllRemFrameData[i + 2] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 1] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_KpOut = tempPara / 100.;

								//tempPara = mAllRemFrameData[i + 2 + 2];
								//temp1 = mAllRemFrameData[i + 2 + 3];
								//tempPara = (tempPara << 8) + temp1;
								//m_KpIn = tempPara / 100.;

								temp1 = mAllRemFrameData[i + 2 + 2] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 3] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_KpIn = tempPara / 100.;

								//tempPara = mAllRemFrameData[i + 2 + 4];
								//temp1 = mAllRemFrameData[i + 2 + 5];
								//tempPara = (tempPara << 8) + temp1;
								//m_KdOut = tempPara / 100.;

								temp1 = mAllRemFrameData[i + 2 + 4] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 5] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_KdOut = tempPara / 100.;

								//tempPara = mAllRemFrameData[i + 2 + 6];
								//temp1 = mAllRemFrameData[i + 2 + 7];
								//tempPara = (tempPara << 8) + temp1;
								//m_KdIn = tempPara / 100.;

								temp1 = mAllRemFrameData[i + 2 + 6] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 7] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_KdIn = tempPara / 100.;

								//tempPara = mAllRemFrameData[i + 2 + 8];
								//temp1 = mAllRemFrameData[i + 2 + 9];
								//tempPara = (tempPara << 8) + temp1;
								//m_Ki = tempPara / 100.;

								temp1 = mAllRemFrameData[i + 2 + 8] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 9] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_Ki = tempPara / 100.;

								//tempPara = mAllRemFrameData[i + 2 + 10];
								//temp1 = mAllRemFrameData[i + 2 + 11];
								//tempPara = (tempPara << 8) + temp1;
								//m_SightArea = tempPara;

								temp1 = mAllRemFrameData[i + 2 + 10] & 0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 11] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_SightArea = tempPara;

								//��ʱiһֱָ��datasize�ֽ��ϡ�

								//m_DataTransRemNum = count;
								//FILE *fp = fopen("data.txt", "a+");
								//fprintf(fp, "%ld	\n", m_DataTransRemNum);
								//fclose(fp);
								//for (p = i-3; p <= i + 2 + datasize; p++)
								for (p = 0; p <= i + 2 + datasize; p++)
								{
									mAllRemFrameData[p] = 0;
								}
								i = i + 2 + datasize;//��ʱiָ��У����ֽ���
								//SendMessage(WM_DisplayDataOnScreen);
								m_ctlParaIsChanged = datasize;
								
							}
							else if (CommandType == MsgReportCurrentYaw)
							{
								temp1 = mAllRemFrameData[i + 2] &  0x0ff;
								tempPara = temp1 & 0x0ff;
								temp1 = mAllRemFrameData[i + 2 + 1] & 0x0ff;
								tempPara = (tempPara << 8) | temp1;
								m_fCurrentYaw = tempPara;// / 100.;

								//��ʱiһֱָ��datasize�ֽ��ϡ�

								//m_DataTransRemNum = count;
								//FILE *fp = fopen("data.txt", "a+");
								//fprintf(fp, "%ld	\n", m_DataTransRemNum);
								//fclose(fp);
								for (p = i - 3; p <= i + 2 + datasize; p++)
								{
									mAllRemFrameData[p] = 0;
								}
								i = i + 2 + datasize;//��ʱiָ��У����ֽ���
								//SendMessage(WM_DisplayDataOnScreen);
								m_ctlParaIsChanged = 2;

							}
							else if (CommandType == MsgSetControlChannal)
							{
								for (p = i - 3; p <= i + 2 + datasize; p++)
								{
									mAllRemFrameData[p] = 0;
								}
								i = i + 2 + datasize;//��ʱiָ��У����ֽ���
							}
						}
					}
				}
			}
		}
	}
}


//LRESULT CMFCVideoDispDlg::DisplayDataOnScreen(WPARAM wParam, LPARAM lParam)
//{
//	UpdateData(DipDataOnScreen);
//	return 0;
//}

void CMFCVideoDispDlg::OnBnClickedOpenserialButton()		//�򿪴���
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CString tempstr = _T("COM1");
	CString tempstr = _T(COMMUNICAT_COMPORT);
	if (mhCom != NULL)
	{
		CString strtemp = tempstr + _T("�����Ѵ򿪣��Ƿ�Ҫ�ر�");
		if (AfxMessageBox(strtemp, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			CloseHandle(mhCom);
			mhCom = NULL;
		}
		else
		{
			return;
		}
	}

	mhCom = CreateFile(tempstr, GENERIC_READ | GENERIC_WRITE, // �����д
		0,								//�������Ϊ0
		NULL,// no security attrs
		OPEN_EXISTING,//���ò�����ʽ
		FILE_FLAG_OVERLAPPED,// ����׼��ʹ���첽ͨ��
		NULL);

	//����ע�⣬����ʹ����FILE_FLAG_OVERLAPPED�ṹ��������ʹ��APIʵ�ַ�����ͨ�ŵĹؼ����ڡ�  ����Ǵ򿪴���
	if (GetLastError() != 0)
	{
		MessageBox(_T("�޷��򿪴��ڣ�"), _T("����"), MB_OK | MB_ICONEXCLAMATION);
		if (mhCom != NULL)
		{
			CloseHandle(mhCom);
			mhCom = NULL;
		}
		return;
	}

	ASSERT(mhCom != INVALID_HANDLE_VALUE); //���򿪴��ڲ����Ƿ�ɹ�

	SetCommMask(mhCom, EV_RXCHAR | EV_TXEMPTY);//�����¼�����������

	SetupComm(mhCom, 2048, 2048); //�������롢����������Ĵ�С

	PurgeComm(mhCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
		| PURGE_RXCLEAR); //��ɾ����롢���������

	COMMTIMEOUTS CommTimeOuts; //���峬ʱ�ṹ������д�ýṹ
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(mhCom, &CommTimeOuts);//���ö�д����������ĳ�ʱ

	DCB dcb;// �������ݿ��ƿ�ṹ
	GetCommState(mhCom, &dcb); //������ԭ���Ĳ�������
	dcb.BaudRate = 57600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;

	dcb.StopBits = ONESTOPBIT; dcb.fBinary = TRUE; dcb.fParity = FALSE;

	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.XonLim = 0;
	dcb.XoffLim = 0;

	BOOL errtemp;
	errtemp = SetCommState(mhCom, &dcb); //���ڲ�������

	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	//m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//������COMMTIMEOUTS�ṹ��DCB������Ҫ��ʵ�ʹ�������Ҫ��ϸѡ�����

	//////////////////
	DWORD dwThreadID;
	mhCommWatchThread = CreateThread((LPSECURITY_ATTRIBUTES)NULL, //��ȫ����
		0,//��ʼ���߳�ջ�Ĵ�С��ȱʡΪ�����̴߳�С��ͬ
		(LPTHREAD_START_ROUTINE)CommWatchProc, //�̵߳�ȫ�ֺ���,��h�ļ���˵��
		(LPVOID)this,//�˴�����������ܣ����Ի����ָ��
		0, &dwThreadID);
	ASSERT(mhCommWatchThread != NULL);
	//////////////////
	SetTimer(DataUpdataTimer, 500, 0);
	if (mhCom!=NULL)
	{
		OnBnClickedButtonReadcontrolparafrompx4();//��ɿ�ѯ�ʲ�����Ϣ
	}
	GetDlgItem(IDC_COMBO_ControlChannals)->EnableWindow(TRUE);
}


void CMFCVideoDispDlg::SendObjectOffset(int Offsetx,int Offsety)
{
	BOOL	fWriteStat;
	DWORD dwTmp;
	char chr1[11] = { 0xfa, 0x4d, 0x3C, 0x04, MsgObjImgOffset, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
	if (m_ObjectNum == 0)
		return;

	chr1[5] = (Offsetx >> 8) & 0x00ff;
	chr1[6] = Offsetx & 0x00ff;
	chr1[7] = (Offsety >> 8) & 0x00ff;
	chr1[8] = Offsety & 0x00ff;
	chr1[9] = (m_TargetProperty << 4) & 0x00ff;
	chr1[9] = chr1[9] + (Action & 0x000f);
	chr1[10] = chr1[3] ^ chr1[4] ^ chr1[5] ^ chr1[6] ^ chr1[7] ^ chr1[8] ^ chr1[9];
	PurgeComm(mhCom, PURGE_TXABORT |  PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 11, &dwTmp, &m_OverlappedWrite); //д����
	//Sleep(2);
}
void CMFCVideoDispDlg::CleanRxBufferRegion()
{
	//PurgeComm( mhCom, PURGE_RXABORT |PURGE_RXCLEAR);
	PurgeComm(mhCom, PURGE_RXCLEAR);
}

void CMFCVideoDispDlg::BlackWhiteImageDilationOnePixel()
//ʮ������һ������
{
	int x, y, w, z;
	
	int CmpHeight, CmpWidth, minGrey, maxGrey;
	float tt1, tt2;
	unsigned long DiffImgZft[256];
	unsigned int objectSum;
	CmpHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;
	CmpWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;

	for (x = 1; x<CmpHeight - 1; x++)
	{
		for (y = 1; y<CmpWidth - 1; y++)
		{
			if (m_pEZHImgEvryTimeInSight[x*CmpWidth + y] == 1)
			{
				w = x;
				z = y;
				if (m_pEZHImgEvryTimeInSight[(w - 1)*CmpWidth + z] == 0)
					m_pEZHImgEvryTimeInSight[(w - 1)*CmpWidth + z] = 2;
				if (m_pEZHImgEvryTimeInSight[(w + 1)*CmpWidth + z] == 0)
					m_pEZHImgEvryTimeInSight[(w + 1)*CmpWidth + z] = 2;

				if (m_pEZHImgEvryTimeInSight[w*CmpWidth + z - 1] == 0)
					m_pEZHImgEvryTimeInSight[w*CmpWidth + z - 1] = 2;
				if (m_pEZHImgEvryTimeInSight[w*CmpWidth + z + 1] == 0)
					m_pEZHImgEvryTimeInSight[w*CmpWidth + z + 1] = 2;


			}
		}
	}
	for (x = 1; x<CmpHeight - 1; x++)
	{
		for (y = 1; y<CmpWidth - 1; y++)
		{
			if (m_pEZHImgEvryTimeInSight[x*CmpWidth + y] != 0)
				m_pEZHImgEvryTimeInSight[x*CmpWidth + y] = 1;
		}
	}
}

void CMFCVideoDispDlg::OnBnClickedButtonReadcontrolparafrompx4()//��ɿ�ѯ�ʲ�����Ϣ
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL	fWriteStat;
	DWORD dwTmp;
	char chr1[6] = { 0xfa, 0x4d, 0x3C, 0x00, MsgAskingPxSendPara, 0x03 };
	if (mhCom == NULL)
	{
		MessageBox(_T("����û�򿪣�"));
		return;
	}
	PurgeComm(mhCom, PURGE_TXABORT | PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 6, &dwTmp, &m_OverlappedWrite); //д����
}

void CMFCVideoDispDlg::OnBnClickedButtonWritecontrolparatopx4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL	fWriteStat;
	DWORD dwTmp;
	int para1, para2, para3, para4, para5, para6;
	if (mhCom == NULL)
	{
		MessageBox(_T("����û�򿪣�"));
		return;
	}
	if (AfxMessageBox(_T("ȷ��д��"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	UpdateData(ReadFromScreen);

	para1 = m_KpOut * 100;
	para2 = m_KpIn * 100;
	para3 = m_KdOut * 100;
	para4 = m_KdIn * 100;
	para5 = m_Ki * 100;
	para6 = m_SightArea;
	char chr1[18] = { 0xfa, 0x4d, 0x3C, 12, MsgSendParaToPX
		, 0x00, 0x00
		, 0x00, 0x00
		, 0x00, 0x00
		, 0x00, 0x00
		, 0x00, 0x00
		, 0x00, 0x00
		, 0x01 };

	

	chr1[5] = (para1 >> 8) & 0x00ff;
	chr1[6] = para1 & 0x00ff;

	chr1[7] = (para2 >> 8) & 0x00ff;
	chr1[8] = para2 & 0x00ff;

	chr1[9] = (para3 >> 8) & 0x00ff;
	chr1[10] = para3 & 0x00ff;

	chr1[11] = (para4 >> 8) & 0x00ff;
	chr1[12] = para4 & 0x00ff;

	chr1[13] = (para5 >> 8) & 0x00ff;
	chr1[14] = para5 & 0x00ff;

	chr1[15] = (para6 >> 8) & 0x00ff;
	chr1[16] = para6 & 0x00ff;

	chr1[17] = chr1[3] ^ chr1[4] ^ chr1[5] ^ chr1[6] ^ chr1[7] ^ chr1[8] ^ chr1[9]
		^ chr1[10] ^ chr1[11] ^ chr1[12] ^ chr1[13] ^ chr1[14] ^ chr1[15] ^ chr1[16];
	PurgeComm(mhCom, PURGE_TXABORT | PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 18, &dwTmp, &m_OverlappedWrite); //д����
}


void CMFCVideoDispDlg::OnSelchangeComboControlchannals()			//��MFC��ѡ�����ͨ����Ϣ
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char channalNum;
	BOOL	fWriteStat;
	DWORD dwTmp;
	if (mhCom == NULL)
	{
		MessageBox(_T("����û�򿪣�"));
		return;
	}
	char chr1[7] = { 0xfa, 0x4d, 0x3C, 0x01, MsgSetControlChannal, 0x00, 0x03 };
	channalNum = m_ChannalSelectComboBoxCtrl.GetCurSel();

	chr1[5] = channalNum;
	chr1[6] = chr1[3] ^ chr1[4] ^ chr1[5];

	PurgeComm(mhCom, PURGE_TXABORT | PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 7, &dwTmp, &m_OverlappedWrite); //д����
	
}




void CMFCVideoDispDlg::OnClickedCheckInputparaenable()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char sel;
	
	sel = ((CButton    *)GetDlgItem(IDC_CHECK_InputParaEnable))->GetCheck();
	if (sel == 1)
	{
		GetDlgItem(IDC_EDIT_KPOUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KPIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KDOUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KDIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_KI)->EnableWindow(FALSE);
		GetDlgItem(ID_EDIT_SIGHTAREA)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_KPOUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KPIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KDOUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KDIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_KI)->EnableWindow(TRUE);
		GetDlgItem(ID_EDIT_SIGHTAREA)->EnableWindow(TRUE);
	}
}


void CMFCVideoDispDlg::OnBnClickedButtonSavecurimg()           //���浱ǰ֡��currentIMG.bmp��
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_ImgDispLoop < 1)
		return;
	int w, z;
	BITMAPINFO tempBitmapInfo = m_BitmapInfo;

	unsigned char *ptempImageDIBForScreenShow;

	tempBitmapInfo.bmiHeader.biHeight = m_BitmapInfo.bmiHeader.biHeight / JinZiTaCompress;  // ѹ������
	tempBitmapInfo.bmiHeader.biWidth = m_BitmapInfo.bmiHeader.biWidth / JinZiTaCompress;
	tempBitmapInfo.bmiHeader.biSizeImage = tempBitmapInfo.bmiHeader.biHeight*
		tempBitmapInfo.bmiHeader.biWidth * 3;
	tempBitmapInfo.bmiHeader.biCompression = 0;
	ptempImageDIBForScreenShow = new unsigned char[tempBitmapInfo.bmiHeader.biSizeImage];



	for (z = 0; z < tempBitmapInfo.bmiHeader.biHeight; z++)
	{
		for (w = 0; w < tempBitmapInfo.bmiHeader.biWidth; w++)
		{
			if (m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w] != 0)
			{
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
					= 255;
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
					= 0;// m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
					= 0;// m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}
			else
			{
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3]
					= 255;
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 1]
					= 255;// m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
				ptempImageDIBForScreenShow[(tempBitmapInfo.bmiHeader.biHeight - 1 - z)*tempBitmapInfo.bmiHeader.biWidth * 3 + w * 3 + 2]
					= 255;// m_pEZHImgEvryTimeInSight[z*tempBitmapInfo.bmiHeader.biWidth + w];
			}

			
		}
	}

	CFile file;
	CString str;
	str.Format(_T("currentIMG.bmp"));
	if (!file.Open(str, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		str.Format(_T("Can not write BMP file."));
		MessageBox(str);
		return;
	}
	/*char name[] = "currentIMG.bmp";

	capFileSaveDIB(m_hWndCap, name);
	return;
	*/
	//����ͼ�����Ϣͷ������λͼ�ļ�ͷ��λͼ��Ϣ��
	//λͼ�ļ�ͷռ14�ֽڣ�λͼ��Ϣͷռ40�ֽ�
	//���ɫλͼû�е�ɫ�壬����ͼ���ƫ�ƾ�Ϊ54�ֽ�
	BITMAPFILEHEADER bf;
	bf.bfOffBits = 54;
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
	//˵���ļ���С��λͼ�ļ�ͷ+λͼ��Ϣͷ+ͼƬ������ռ�ֽ�����
	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + tempBitmapInfo.bmiHeader.biWidth*tempBitmapInfo.bmiHeader.biHeight * 3;
	//˵���ļ�����ΪBM
	bf.bfType = ((WORD)'M' << 8 | 'B');
	BITMAPINFOHEADER bi;
	//˵��ÿ��������ռ�ֽ�������ֵΪ1��4��8��16��24��32
	bi.biBitCount = 24;
	//˵��ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ
	bi.biClrImportant = 0;
	//˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bi.biClrUsed = 0;
	//˵��ͼ������ѹ��������
	bi.biCompression = 0L;
	//˵��ͼ��Ŀ�ȣ�������Ϊ��λ
	bi.biWidth = tempBitmapInfo.bmiHeader.biWidth;
	//˵��ͼ��ĸ߶ȣ�������Ϊ��λ
	bi.biHeight = tempBitmapInfo.bmiHeader.biHeight;
	//Ŀ���豸˵��λ��������ֵ���Ǳ���Ϊ1
	bi.biPlanes = 1;
	//˵��BITMAPINFOHEADER�ṹ����Ҫ���ֽ���
	bi.biSize = sizeof(BITMAPINFOHEADER);
	//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0
	bi.biSizeImage = 0;
	//˵��ͼ���ˮƽ�ֱ��ʣ�������/������ʾ
	bi.biXPelsPerMeter = 0;
	//˵��ͼ��Ĵ�ֱ�ֱ��ʣ�������/������ʾ
	bi.biYPelsPerMeter = 0;
	//�򿪱���Ի��򣬻�ȡ����·�����ļ���
	//����CFile����׼������ͼƬ

	//д��λͼ�ļ�ͷ
	file.Write(&bf, 14);
	//д��λͼ��Ϣͷ
	file.Write(&bi, sizeof(BITMAPINFOHEADER));
	//д�����������ؾ����е�����
	file.Write(ptempImageDIBForScreenShow, tempBitmapInfo.bmiHeader.biWidth*tempBitmapInfo.bmiHeader.biHeight * 3);
	//д����ϣ��ر��ļ�
	file.Close();
	free(ptempImageDIBForScreenShow);
}



void CMFCVideoDispDlg::OnBnClickedButtonSavectrlparatofile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(ReadFromScreen);
	FILE *fp = fopen("ControlPara.dat", "w+");
	fprintf(fp, "[Pk1]=%0.2f\n", m_KpOut);
	fprintf(fp, "[Pk2]=%0.2f\n", m_KpIn);
	fprintf(fp, "[Pk2]=%0.2f\n", m_KdOut);
	fprintf(fp, "[Pk2]=%0.2f\n", m_KdIn);
	fprintf(fp, "[Pk2]=%0.2f\n", m_Ki);
	fprintf(fp, "[Pk2]=%d\n", m_SightArea);
	fclose(fp);
}


void CMFCVideoDispDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();	
	OnBnClickedButtonSavectrlparatofile();
}


void CMFCVideoDispDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (AfxMessageBox(_T("����δ���档���豣�棬������񡱣��ٴε����ȷ������"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	CDialogEx::OnCancel();
}
void CMFCVideoDispDlg::ReadParaFromFile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	FILE *fp = NULL;
	fp = fopen("ControlPara.dat", "r");
	if (fp == NULL)
		return;
	fscanf(fp, "[Pk1]=%f\n", &m_KpOut);
	fscanf(fp, "[Pk2]=%f\n", &m_KpIn);
	fscanf(fp, "[Pk2]=%f\n", &m_KdOut);
	fscanf(fp, "[Pk2]=%f\n", &m_KdIn);
	fscanf(fp, "[Pk2]=%f\n", &m_Ki);
	fscanf(fp, "[Pk2]=%d", &m_SightArea);
	fclose(fp);
	UpdateData(DipDataOnScreen);
}
void CMFCVideoDispDlg::ReadTemplateFromFile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	FILE *fp = NULL;
	fp = fopen("ImageTemplate.dat", "r");
	if (fp == NULL)
		return;
	fscanf(fp, "[ImageTemplateSum]=%d\n", &m_iTemplateSum);
	for (int i = 0; i < m_iTemplateSum; i++)
	{
		
		m_pTemplate[i] = new struct TargetTemplateStruct; 
		fscanf(fp, "[ImageSquare]=%f\n", &m_pTemplate[i]->square);
		fscanf(fp, "[ImageR1]=%f\n", &m_pTemplate[i]->R1);
		fscanf(fp, "[ImageR2]=%f\n", &m_pTemplate[i]->R2);
		fscanf(fp, "[ImageR3]=%f\n", &m_pTemplate[i]->R3);
	}
	fclose(fp);
	UpdateData(DipDataOnScreen);
}

void CMFCVideoDispDlg::OnBnClickedOpenvideoButton()      //������Ƶ
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CRect  temprect;
	CWnd* pwnd;//=GetDlgItem(IDC_DisplayRect_STATIC);
	HWND  hWnd;
	//pwnd->GetWindowRect(&rect);
	hWnd = GetSafeHwnd();
	if (m_hWndCap != NULL)
	{
		pwnd = FromHandle(m_hWndCap);
		pwnd->DestroyWindow();
		m_hWndCap = NULL;
	}
	pwnd = GetDlgItem(IDC_VideoDisplayRgn_STATIC);
	pwnd->GetWindowRect(&temprect);
	m_DiplayRGN = temprect;
	ScreenToClient(m_DiplayRGN);
	m_hWndCap = capCreateCaptureWindow(NULL,
		WS_DISABLED | WS_EX_WINDOWEDGE | WS_CHILDWINDOW | WS_VISIBLE,//| WS_THICKFRAME ,
		m_DiplayRGN.left, m_DiplayRGN.top,
		m_DiplayRGN.Width(), m_DiplayRGN.Height(),
		hWnd,//parent  window  
		NULL//chile  window  id  
		);//������Ƶ�ɼ�����
	ASSERT(m_hWndCap);
	//�жϲɼ������Ƿ���0�Ųɼ�������������
	if (capDriverConnect(m_hWndCap, m_ctlCameraDeviceSel.GetCurSel()))
	{

		CAPDRIVERCAPS	CapDriverCaps;
		//��ȡ��Ƶ�豸�ɼ�����
		capDriverGetCaps(m_hWndCap, &CapDriverCaps, sizeof(CAPDRIVERCAPS));



		if (CapDriverCaps.fCaptureInitialized)//��ʼ���ɹ�
		{
			//���֧����ƵԴѡ���򵯳���ƵԴѡ��Ի���  
			if (CapDriverCaps.fHasDlgVideoSource)
			{
				capDlgVideoSource(m_hWndCap);

				//put_TVFormat
			}
			//���֧����Ƶ��ʽѡ���򵯳���Ƶ��ʽѡ��Ի���  
			if (CapDriverCaps.fHasDlgVideoFormat)
				capDlgVideoFormat(m_hWndCap);
			//���֧����Ƶ��ʾ��ʽѡ���򵯳���Ƶ��ʾ��ʽѡ��Ի���  
			if (CapDriverCaps.fHasDlgVideoDisplay)
				capDlgVideoDisplay(m_hWndCap);
			//CWnd *pwnd1 = FromHandle(m_hWndCap);

			capGetVideoFormat(m_hWndCap, &m_BitmapInfo, sizeof(BITMAPINFO));

			m_iCameraheight = m_BitmapInfo.bmiHeader.biHeight;
			m_iCamerawidth = m_BitmapInfo.bmiHeader.biWidth;
			m_pImageIniDIB = new unsigned char[m_BitmapInfo.bmiHeader.biSizeImage];
			OpenMemOfImgChData();
			BOOL errorornot;
			//errorornot=capSetVideoFormat(m_hWndCap,m_pBitmapInfo,sizeof(BITMAPINFO));
			if (m_iCameraheight <= 0)
			{
				capDriverDisconnect(m_hWndCap);
				m_hWndCap = NULL;
				return;
			}


			//  ����Previewģʽ��Ƶ֡
			capPreviewRate(m_hWndCap, 10);
			capPreviewScale(m_hWndCap, TRUE);
			//����Previewģʽ
			capPreview(m_hWndCap, TRUE);

			//����ÿ֡�����������õĻص�����  
			capSetCallbackOnFrame(m_hWndCap, FrameCallbackProc);
			SetTimer(DrawCrossOnVedioPictureTimer, 20, 0);
			m_CenterOnImage.COLUMN = m_iCamerawidth / 2 - 1;
			m_CenterOnImage.ROW = m_iCameraheight / 2 - 1;

			if (mhCom != NULL)
			{
				OnBnClickedButtonReadcontrolparafrompx4();//��ɿ�ѯ�ʲ�����Ϣ
			}

		}
		else//��ʼ�����ɹ�  
		{
			MessageBox(_T("���񿨳�ʼ��ʧ��"), _T("������Ϣ"), MB_ICONSTOP | MB_OK);
			pwnd = FromHandle(m_hWndCap);
			capDriverDisconnect(m_hWndCap);
			pwnd->DestroyWindow();
			m_hWndCap = NULL;
		}

	}
	else
		MessageBox(_T("��������ʧ��"), _T("������Ϣ"), MB_ICONSTOP | MB_OK);
}


void CMFCVideoDispDlg::OnBnClickedButtonReadparasfromfile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ReadParaFromFile();
}

void CMFCVideoDispDlg::ThinImage(unsigned char *psrcImg, unsigned char *pdstImg, int iterations, int height, int width)
{
	//memcpy(pdstImg, psrcImg, height*width);
	unsigned char *ptemp_image = new unsigned char[height*width];
	BOOL stop = 0;
	int count = 0;
	int p2, p3, p4, p5, p6, p7, p8, p9, ap;
	int row, col;
	while (stop == 0)
	{
		memcpy(ptemp_image, pdstImg, height*width);
		count = 0;
		for (row = 0; row < height; row++)
		{
			for (col = 0; col < width; col++)
			{
				if (ptemp_image[row*width + col] == 1)
				{
					ap = 0;
					p2 = (row == 0) ? 0 : ptemp_image[(row - 1)*width + col];
					p3 = (row == 0 || col == width - 1) ? 0 : ptemp_image[(row - 1)*width + col + 1];
					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}
					///////////////////////////////////////////////////////////////////////////////////
					p4 = (col == width - 1) ? 0 : ptemp_image[row*width + col + 1];
					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}
					///////////////////////////////////////////////////////////////////////////////////
					p5 = (row == height - 1 || col == width - 1) ? 0 : ptemp_image[(row + 1)*width + col + 1];
					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}
					///////////////////////////////////////////////////////////////////////////////////
					p6 = (row == height - 1) ? 0 : ptemp_image[(row + 1)*width + col];
					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}
					///////////////////////////////////////////////////////////////////////////////////
					p7 = (row == height - 1 || col == 0) ? 0 : ptemp_image[(row + 1)*width + col - 1];
					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}
					p8 = (col == 0) ? 0 : ptemp_image[row*width + col - 1];
					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}
					p9 = (row == 0 || col == 0) ? 0 : ptemp_image[(row - 1)*width + col - 1];
					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if (p2*p4*p8 == 0)
							{
								if (p2*p6*p8 == 0)
								{
									pdstImg[row*width + col] = 0;
									count = count + 1;
								}
							}
						}
					}
				}

			}
		}
		//count++;
		if (count == 0)
			stop = 1;
	}
	delete(ptemp_image);
}




void CMFCVideoDispDlg::OnBnClickedButtonCleartemplate()         //��մ��̵�ģ������
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (AfxMessageBox(_T("ȷ��ɾ����"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	for (int i = 0; i < m_iTemplateSum; i++)
	{
		if (m_pTemplate[i] != NULL)
		{
			free(m_pTemplate[i]);
			m_iTemplateSum = 0;
			CDataExchange dx(this, FALSE);
			DDX_Text(&dx, IDC_EDIT_TemplateSum, m_iTemplateSum);
		}
	}
	FILE *fp = NULL;
	fp = fopen("ImageTemplate.dat", "w+");
	if (fp == NULL)
		return;
	fprintf(fp, "[ImageTemplateSum]=%d\n", 0);
	fclose(fp);

}


void CMFCVideoDispDlg::OnBnClickedButtonSavetemplatetodisk()        //ģ�����ݴ���
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	FILE *fp = NULL;
	fp = fopen("ImageTemplate.dat", "a+");
	if (fp == NULL)
		return;
	fprintf(fp, "[ImageTemplateSum]=%d\n", m_iTemplateSum);
	for (int i = 0; i < m_iTemplateSum; i++)
	{

		fprintf(fp, "[ImageSquare]=%f\n", m_pTemplate[i]->square);
		fprintf(fp, "[ImageR1]=%f\n", m_pTemplate[i]->R1);
		fprintf(fp, "[ImageR2]=%f\n", m_pTemplate[i]->R2);
		fprintf(fp, "[ImageR3]=%f\n", m_pTemplate[i]->R3);
	}
	fclose(fp);
	UpdateData(DipDataOnScreen);
}


void CMFCVideoDispDlg::OnBnClickedButtonAskingcurrentyaw()   //��PX���Ͳ�ѯ��ǰ���������
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	BOOL	fWriteStat;
	DWORD dwTmp;
	char chr1[6] = { 0xfa, 0x4d, 0x3C, 0x00, MsgAskingCurrentYaw, MsgAskingCurrentYaw };
	if (mhCom == NULL)
	{
		MessageBox(_T("����û�򿪣�"));
		return;
	}
	PurgeComm(mhCom, PURGE_TXABORT | PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 6, &dwTmp, &m_OverlappedWrite); //д����
}




void CMFCVideoDispDlg::OnBnClickedButtonSendstartchangyawcmd()			 //��PX���Ϳ�ʼת������
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL	fWriteStat;
	DWORD dwTmp;
	char chr1[6] = { 0xfa, 0x4d, 0x3C, 0x00, MsgCmdSartChangYaw, MsgCmdSartChangYaw };
	if (mhCom == NULL)
	{
		MessageBox(_T("����û�򿪣�"));
		return;
	}
	PurgeComm(mhCom, PURGE_TXABORT | PURGE_TXCLEAR);
	fWriteStat = WriteFile(mhCom, chr1, 6, &dwTmp, &m_OverlappedWrite); //д����
}


void CMFCVideoDispDlg::OnUpdateRadioMission1Blue(CCmdUI *pCmdUI)
{
	// TODO:  �ڴ������������û����洦��������
	m_CurrentMission = CurrentMissionIsBlue;
}


void CMFCVideoDispDlg::OnUpdateRadioMission2Red(CCmdUI *pCmdUI)
{
	// TODO:  �ڴ������������û����洦��������
	m_CurrentMission = CurrentMissionIsRed;
}


void CMFCVideoDispDlg::OnBnClickedRadioMission1Blue()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CurrentMission = CurrentMissionIsBlue;
}


void CMFCVideoDispDlg::OnBnClickedRadioMission2Red()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CurrentMission = CurrentMissionIsRed;
}


void CMFCVideoDispDlg::OnBnClickedRadioMission3Yellow()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CurrentMission = CurrentMissionIsYellow;
}


void CMFCVideoDispDlg::OnEnChangeEditCurrentyaw()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeEditIsanytemplate()     //��MFC����ȡ��Ƶ��������ģ���ģ���  ���ؼ���
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeEditTemplatesum()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnCbnSelchangeCombo()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeImgfreqEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnBnClickedTextofdsicribevideoinputStatic()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnStnClickedMousexStatic()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeMousexEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeMouseyEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnEnChangeVxEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnBnClickedTextofdsicribevideodispStatic2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnBnClickedButtonClearcountingvariable()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	StartGoRightStepsCount = 0;
	StartGoLeftStepsCount = 0;
	StartGoUpStepsCount = 0;
	StartGoDownStepsCount = 0;
	ConnerFrameCount1 = 0;
	ConnerFrameCount2 = 0;
	ConnerFrameCount3 = 0;
	ConnerFrameCount4 = 0;
}


void CMFCVideoDispDlg::OnStnClickedMouseyStatic()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnBnClickedTextofdsicribevideodispStatic3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMFCVideoDispDlg::OnStnClickedVideoproeddispStatic()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
