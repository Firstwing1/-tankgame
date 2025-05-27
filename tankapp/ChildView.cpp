// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// ChildView.cpp: CChildView 类的实现
//

#include "pch.h"
#include "framework.h"
#include "tankapp.h"
#include "ChildView.h"

#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool end = true;
static bool pause;

// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
    ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BTN1, OnBtn1Clicked)
	ON_BN_CLICKED(ID_BTN2, OnBtn2Clicked)
	ON_BN_CLICKED(ID_BTN3, OnBtn3Clicked)
	ON_BN_CLICKED(ID_BTN4, OnBtn4Clicked)
	ON_BN_CLICKED(ID_BTN5, OnBtn5Clicked)
	ON_BN_CLICKED(ID_BTN6, OnBtn6Clicked)

END_MESSAGE_MAP()


//将png贴图透明
static void TransparentPNG(CImage *png)
{
  for(int i = 0; i <png->GetWidth(); i++)
  {
    for(int j = 0; j <png->GetHeight(); j++)
    {
      unsigned char* pucColor = reinterpret_cast<unsigned char *>(png->GetPixelAddress(i , j));
      pucColor[0] = pucColor[0] * pucColor[3] / 255;
      pucColor[1] = pucColor[1] * pucColor[3] / 255;
      pucColor[2] = pucColor[2] * pucColor[3] / 255;
    }
  }
}

// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);
	
  //加载背景
  //m_bg.Load(_T("grass.png"));

  //加载英雄图片
  //m_hero.Load(_T("tank.png"));
  //TransparentPNG(&m_hero);

  //设置英雄初始位置
  m_heroPos.left=100;    //人物左边贴在100的位置
  m_heroPos.right=100+60; //人物的右边等于左边加上人物的宽度
  m_heroPos.top=400;
  m_heroPos.bottom=400+60;

  return TRUE;
}

void CChildView::OnPaint() 
{
	//CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
#if 0
	// 不要为绘制消息而调用 CWnd::OnPaint()
	  //获取窗口DC指针
	  CDC *cDC=this->GetDC();
	  //获取窗口大小
	  GetClientRect(&m_client);
	  //贴背景
	  m_bg.Draw(*cDC,m_client);
	  //贴英雄
	  m_hero.Draw(*cDC,m_heroPos);

	  //释放DC
	  ReleaseDC(cDC);

#else 
      CPaintDC dc(this);
      CDC memDC;
      memDC.CreateCompatibleDC(&dc);
      CBitmap bmp;
      GetClientRect(&m_client);
      bmp.CreateCompatibleBitmap(&dc, m_client.Width(), m_client.Height());
      CBitmap* pOldBitmap = memDC.SelectObject(&bmp);
	  CDC* pDC = &memDC;

      // 绘制到内存 DC
      //m_bg.Draw(memDC, m_client);
      //m_hero.Draw(memDC, m_heroPos);

      // 绘制背景
	  pDC->FillSolidRect(m_client, RGB(0, 120, 0));  // 或用背景刷填充

      //m_hero.Draw(memDC, m_heroPos);

      for (auto& s1 : m_vecShell)
      {
          if (end != false)
          {
              s1.Draw(pDC);
          }
      }
      
      for (auto& w : m_vecWall)
      {
          w.Draw(w.m_flag, pDC);
		  if (!m_tank.IsOverlap(w))
			  //我方坦克
			  m_tank.Draw(pDC);
		  else
			  continue;
		  CString str3;
		  str3.Format(L"内部障碍 %d", m_innerWall.size());
		  pDC->TextOutW(100, 520, str3);
      }

	  int num = m_EnemyTank.size();

	  CString str7;
	  if (num == 4)
	  {
		  str7.Format(L"难度 %s", L"easy 模式");
	  }
	  if (num == 6)
	  {
		  str7.Format(L"难度 %s", L"normal 模式");
	  }
	  if (num == 8)
	  {
		  str7.Format(L"难度 %s", L"hard 模式");
	  }
	  pDC->TextOutW(300, 320, str7);

	  int _0Armor = 0;
	  bool win = false;
	  for (auto& t : m_EnemyTank)
	  {
		  int iniArmor = 2;
		  t.Draw(pDC);
		  CString str1;
		  str1.Format(L"敌方装甲 %d", iniArmor);
		  pDC->TextOutW(100, 220, str1);
		  if (t.m_nArmor <= 0)
		  {
			  num -= 1;
			  _0Armor += 1;
		  }
		  CString str2;
		  str2.Format(L"敌方坦克 %d", num);
		  pDC->TextOutW(100, 320, str2);
		  if (_0Armor == m_EnemyTank.size())
			  win = true;
	  }
	  //提示
	  if (win)
	  {
		  pDC->TextOutW(620, 400, L"Game Over!");
		  pDC->TextOutW(620, 550, L"You Win!");
		  if (m_pVoice)
			  m_pVoice->Speak(L"You have won the game", SPF_ASYNC, NULL);
		  end = true;
		  //m_vecWall.clear();
	  }
	  else if (m_EnemyTank.size() != 0 && m_tank.m_nArmor <= 0)
	  {
		  pDC->TextOutW(620, 400, L"Game Over!");
		  pDC->TextOutW(620, 550, L"You Fail!");
		  end = false;
	  }
	  if (pause == true)
	  {
		  pDC->TextOutW(620, 475, L"游戏暂停");
	  }
	  //属性
	  CString str;
	  if (m_tank.m_nArmor >= 0 && m_tank.m_nArmor <= 100)
	  {
		  str.Format(L"我方装甲 %d", m_tank.m_nArmor);
		  pDC->TextOutW(100, 120, str);
	  }
	  else
	  {
		  pDC->TextOutW(100, 120, L"我方状态 0");
	  }
	  pDC->TextOutW(100, 420, L"我方状态 1");


	  int h = m_client.Width();
	  int w = m_client.Height();

      // 一次性拷贝到屏幕
      dc.BitBlt(0, 0, m_client.Width(), m_client.Height(), &memDC, 0, 0, SRCCOPY);

      //清理资源
      memDC.SelectObject(pOldBitmap);
      bmp.DeleteObject();

#endif
}

//按键响应函数
void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*
  //nChar表示按下的键值
  switch(nChar)
  {
  case 'd':         //游戏中按下的键当然应该不区分大小写了
  case 'D':
    m_heroPos.left+=10;    //向右移动10个像素的单位
    m_heroPos.right+=10;   //左边和右边都要移动哦
    break;
  case 'a':
  case 'A':
    m_heroPos.left-=10;
    m_heroPos.right-=10;
    break;
  case 'w':
  case 'W':
    m_heroPos.top-=10;
    m_heroPos.bottom-=10;
    break;
  case 's':
  case 'S':
    m_heroPos.top+=10;
    m_heroPos.bottom+=10;
    break;
  }
  Invalidate(FALSE);

  */
}

//鼠标左键单击响应函数
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	static __int64 tmLast = 0;
	__int64 tmCurrent = GetTickCount64();
	if (tmCurrent - tmLast > 500)//炮管冷却，开炮间隔时间要大于0.5秒
	{
		if (m_pVoice)
			m_pVoice->Speak(L"Fire", SPF_ASYNC, NULL);
		TCHAR cFilePathName[1024];
		GetModuleFileName(NULL, cFilePathName, 1024);
		CString str = cFilePathName;
		int nPos = str.ReverseFind('\\');//反向寻找第一个反斜杠
		CString strPathName = str.Left(nPos);
		strPathName = strPathName + L"\\data";
		CString strImagePathName;
		strImagePathName = strPathName + L"\\Fire.wav";
		PlaySound(strImagePathName, 0, SND_FILENAME | SND_ASYNC);
		if (m_EnemyTank.size() != 0 && m_tank.m_nArmor != 0)
		{
			Shell shell = m_tank.Fire();
			shell.SetSpeed(m_ShellSpeed);
			int shellsize = shell.GetSize();
			shell.SetSize(m_ShellDamage * shellsize);
			m_vecShell.push_back(shell);
		}
		tmLast = tmCurrent;
	}
	Invalidate(FALSE);
}

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 800;
const int BORDER_SIZE = 64;
const int WALL_SIZE = 64;
const int TANK_SIZE = 64;

bool IsPositionValid(const CPoint& pt,
	const std::vector<Wall>& existingWalls,
	const CPoint forbiddenPoints[],
	int num,
	const CPoint& tankPos) {
	// 检查边界冲突
	if (pt.x < BORDER_SIZE || pt.x > MAP_WIDTH - BORDER_SIZE - WALL_SIZE ||
		pt.y < BORDER_SIZE || pt.y > MAP_HEIGHT - BORDER_SIZE - WALL_SIZE) {
		return false;
	}

	// 检查坦克区域冲突
	if (abs(pt.x - tankPos.x) < TANK_SIZE + WALL_SIZE &&
		abs(pt.y - tankPos.y) < TANK_SIZE + WALL_SIZE) {
		return false;
	}

	// 检查特殊禁止点冲突
	for (int i = 0; i < num; ++i) {
		if (abs(pt.x - forbiddenPoints[i].x) < WALL_SIZE &&
			abs(pt.y - forbiddenPoints[i].y) < WALL_SIZE) {
			return false;
		}
	}

	// 检查现有障碍物冲突
	for (const auto& wall : existingWalls) {
		if (abs(pt.x - wall.m_ptPos.x) < WALL_SIZE * 2 &&
			abs(pt.y - wall.m_ptPos.y) < WALL_SIZE * 2) {
			return false;
		}
	}

	return true;
}

// 游戏区域边界
const int MIN_X = 64;
const int MAX_X = 1280-2*64;
const int MIN_Y = 64;
const int MAX_Y = 800-2*64;

void CChildView::InitEnemyTanks(int difficulty) {
	std::srand(std::time(0)); // 初始化随机种子

	int tankCount = 4; // easy模式默认值
	if (difficulty == 1) tankCount = 6; // normal
	else if (difficulty == 2) tankCount = 8; // hard

	for (int k = 0; k < tankCount; k++) {
		AutoTank autotank;
		// 生成随机位置（确保不重叠）
		bool positionValid;
		do {
			positionValid = true;
			autotank.m_ptPos.x = MIN_X + (std::rand() % (MAX_X - MIN_X));
			autotank.m_ptPos.y = MIN_Y + (std::rand() % (MAX_Y - MIN_Y));

			// 检查是否与现有坦克位置太近
			for (const auto& tank : m_EnemyTank) {
				if (abs(tank.m_ptPos.x - autotank.m_ptPos.x) < 50 &&
					abs(tank.m_ptPos.y - autotank.m_ptPos.y) < 50) {
					positionValid = false;
					break;
				}
			}
		} while (!positionValid);

		autotank.SetDirection((std::rand() % 4) + 1); // 随机方向1-4
		autotank.SetArmor(3);
		m_EnemyTank.push_back(autotank);
	}
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

	//我方坦克发射炮弹速度
	m_ShellSpeed = 1;
	//我方坦克发射级别
	m_ShellDamage = 1;
    // TODO:  在此添加您专用的创建代码
	CRect rect(10 + 150 * 0, 10, 120 + 150 * 0, 40);
	m_btn1.Create(_T("进行速度+"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN1);
	rect.SetRect(10 + 150 * 1, 10, 120 + 150 * 1, 40);  // 原x坐标10→160（+150）
	m_btn2.Create(_T("进行速度-"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN2);
	rect.SetRect(10 + 150 * 2, 10, 120 + 150 * 2, 40);
	m_btn3.Create(_T("子弹速度+"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN3);
	rect.SetRect(10 + 150 * 3, 10, 120 + 150 * 3, 40);
	m_btn4.Create(_T("子弹速度-"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN4);
	rect.SetRect(10 + 150 * 4, 10, 120 + 150 * 4, 40);
	m_btn5.Create(_T("子弹级别+"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN5);
	rect.SetRect(10 + 150 * 5, 10, 120 + 150 * 5, 40);
	m_btn6.Create(_T("子弹级别-"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, this, ID_BTN6);

	int nTimerID1 = 1;
	int nTimerID2 = 2;
	int nTimerID3 = 3;
	SetTimer(nTimerID1, 4, NULL);//一号定时器，4ms，全体发送 增加
	SetTimer(nTimerID2, 2, NULL);
	SetTimer(nTimerID3, 2000, NULL);//自动开火

	std::srand(std::time(nullptr)); // 初始化随机种子
	int mode = std::rand() % 3; // 生成0-2的随机数
	//mode = 0;
	InitEnemyTanks(mode);
	CPoint gdpt[8];
	for (int i = 0; i < (mode+2)*2; i++)
	{
		gdpt[i] = m_EnemyTank[i].m_ptPos;
	}
#if 0

	//敌方坦克
	CPoint gdpt[4];
	gdpt[0].x = 300;
	gdpt[0].y = 200;
	gdpt[1].x = 800;
	gdpt[1].y = 600;
	gdpt[2].x = 400;
	gdpt[2].y = 300;
	gdpt[3].x = 700;
	gdpt[3].y = 300;
	for (int k = 0; k < 4; k++)
	{

		AutoTank autotank;
		autotank.m_ptPos.x = gdpt[k].x;
		autotank.m_ptPos.y = gdpt[k].y;
		autotank.SetDirection(k + 1);
		autotank.SetArmor(3);
		m_EnemyTank.push_back(autotank);
	}
#endif 
	//随机墙
	srand(time(NULL));//一次
	for (auto& w : m_vecWall)
	{
		if (w.m_ptPos.x >= 2 * w.GetSize() || w.m_ptPos.y >= 2 * w.GetSize() || w.m_ptPos.x <= 20 * w.GetSize() || w.m_ptPos.y <= 13 * w.GetSize())
			m_vecWall.clear();
	}
	//CRect rct;
	//GetDlgItem(IDC_BACKGROUND)->GetClientRect(rct);
	//GetClientRect(&rct);

	CFrameWnd* pFrame = GetParentFrame();
	CRect rct;
	if (pFrame) {		
		pFrame->GetWindowRect(&rct);  // 获取框架窗口尺寸
		ScreenToClient(&rct);
	}

	//墙		
	for (int i = 0; i < 63; i++) {
		Wall brick;
		brick.SetArmor(999);
		brick.m_flag = 3;//金属墙

		if (i < 13) { // 左边（从下到上）
			brick.SetPosition(0, 64 * i);
		}
		else if (i < 32) { // 下边（从左到右）
			brick.SetPosition(64 * (i - 13), 736); // 800-64=736
		}
		else if (i < 44) { // 右边（从上到下）
			brick.SetPosition(1216, 736 - 64 * (i - 32)); // 1280-64=1216
		}
		else { // 上边（从右到左）
			brick.SetPosition(1216 - 64 * (i - 44), 0);
		}
		m_vecWall.push_back(brick);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> xDist(BORDER_SIZE, MAP_WIDTH - BORDER_SIZE - WALL_SIZE);
	std::uniform_int_distribution<> yDist(BORDER_SIZE, MAP_HEIGHT - BORDER_SIZE - WALL_SIZE);

	for (int i = 0; i < 20; i++) 
	{
		CPoint pt(xDist(gen), yDist(gen));

		if (IsPositionValid(pt, m_innerWall, gdpt, (mode+2)*2, m_tank.m_ptPos)) {
			Wall w;
			w.m_ptPos = pt;
			w.SetArmor(2);
			w.SetSize(32);
			w.m_flag = 2; // 障碍物类型标识
			m_innerWall.push_back(w);
		}
	}
	m_vecWall.insert(m_vecWall.end(), m_innerWall.begin(), m_innerWall.end());
	/*
	for (int i = 0; i < 20; i++)
	{
		//随机数
		int nX = rand();//调用n次0~RAND_MAX(为32767)
		int nY = rand();
		CPoint pt;
		pt.x = double(nX) / RAND_MAX * 10 + 1;//1~10随机数
		pt.y = double(nY) / RAND_MAX * 10 + 1;
		//程序窗口
		
		int nWidth = rct.Width() - 20;
		int nHeight = rct.Height() - 43;
		if (nWidth == 0) nWidth = 1280;
		if (nHeight == 0) nHeight = 800;

		//建墙
		Wall w1;
		w1.m_ptPos = pt;
		w1.m_flag = 2;
		//w1.m_Img.Draw((int)pt.x * nWidth / 10 + 2 * i, (int)pt.y * nHeight / 10);
		w1.SetArmor(2);
		w1.SetSize(32);
		if (pt.x * nWidth / 10 + w1.GetSize() <= nWidth)
			w1.SetPosition(pt.x * nWidth / 10 - 2 * w1.GetSize() + 6, pt.y * nHeight / 10);
		else
			w1.SetPosition(pt.x * nWidth / 10 - w1.GetSize(), pt.y * nHeight / 10);
		m_vecWall.push_back(w1);
	}
	*/
	m_pVoice = NULL;
	::CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&m_pVoice);
	if (SUCCEEDED(hr))
	{
		m_pVoice->SetVolume((USHORT)100);//设置音量，范围为0~100
		m_pVoice->SetRate(2);//设置速度，范围为-10~10
	}
	else if (m_pVoice)
	{
		m_pVoice->Release();
		m_pVoice = NULL;
		::CoUninitialize();//释放com资源
	}

	if (m_pVoice)
		m_pVoice->Speak(L"游戏开始", SPF_ASYNC, NULL);
	TCHAR cFilePathName[1024];
	GetModuleFileName(NULL, cFilePathName, 1024);
	CString str = cFilePathName;
	int nPos = str.ReverseFind('\\');//反向寻找第一个反斜杠
	CString strPathName = str.Left(nPos);
	strPathName = strPathName + L"\\data";
	CString strImagePathName;
	strImagePathName = strPathName + L"\\BGM.wav";
	PlaySound(strImagePathName, 0, SND_FILENAME | SND_ASYNC);

    return 0;
}



void CChildView::Judgy(CChildView* pThis)
{
	int i, j;
	std::vector<Shell>& vecShell = pThis->m_vecShell;
	std::vector<Wall>& vecWall = pThis->m_vecWall;
	std::vector<AutoTank>& vecEnemyTank = pThis->m_EnemyTank;
	Tank& MyTank = pThis->m_tank;

	TCHAR cFilePathName[1024];
	GetModuleFileName(NULL, cFilePathName, 1024);
	CString str = cFilePathName;
	int nPos = str.ReverseFind('\\');//反向寻找第一个反斜杠
	CString strPathName = str.Left(nPos);
	strPathName = strPathName + L"\\data";
	CString strImagePathName1;
	strImagePathName1 = strPathName + L"\\explode.wav";
	CString strImagePathName2;
	strImagePathName2 = strPathName + L"\\BGM.wav";
	CString strImagePathName3;
	strImagePathName3 = strPathName + L"\\GameOver.wav";
	//炮弹与其他炮弹、我方坦克、墙壁、敌方坦克间的碰撞测试
	for (i = 0; i < vecShell.size(); i++)
	{
		//炮弹与其它炮弹碰撞
		if (vecShell[i].m_nArmor <= 0)
			continue;
		for (j = i + 1; j < vecShell.size(); j++)
		{
			//友军无伤害，无效炮弹无伤害
			if (vecShell[j].m_nArmor <= 0 || vecShell[i].GetFriend() == vecShell[j].GetFriend())
				continue;
			if (vecShell[i].IsOverlap(vecShell[j]))
				vecShell[i].m_nArmor = vecShell[j].m_nArmor = 0;
		}
		//炮弹与墙碰撞
		if (vecShell[i].m_nArmor <= 0)
			continue;
		for (j = 0; j < vecWall.size(); j++)
		{
			if (vecWall[j].m_nArmor > 0 && vecShell[i].IsOverlap(vecWall[j]))
			{
				vecWall[j].m_nArmor -= vecShell[i].m_nArmor;
				vecShell[i].m_nArmor = 0;
			}
		}
		//炮弹与我方坦克碰撞,要判断敌友, m_bFriend
		if (vecShell[i].m_nArmor <= 0)
			continue;
		if (MyTank.m_nArmor > 0 && vecShell[i].IsOverlap(MyTank) && vecShell[i].GetFriend() != MyTank.GetFriend())
		{
			MyTank.m_nArmor -= vecShell[i].m_nArmor;
			vecShell[i].m_nArmor = 0;
			PlaySound(strImagePathName1, 0, SND_FILENAME | SND_ASYNC);
			if (vecEnemyTank.size() != 0 && MyTank.m_nArmor <= 0)
			{
				PlaySound(strImagePathName3, 0, SND_FILENAME | SND_ASYNC);
			}
		}

		//炮弹与敌方坦克碰撞,要判断敌友, m_bFriend
		if (vecShell[i].m_nArmor <= 0)
			continue;
		for (int k = 0; k < vecEnemyTank.size(); k++)
		{
			if (vecEnemyTank[k].m_nArmor >= 0
				&& vecShell[i].IsOverlap(vecEnemyTank[k])
				&& vecShell[i].GetFriend() != vecEnemyTank[k].GetFriend())
			{
				//vecEnemyTank[k].m_nArmor -= vecShell[i].m_nArmor;
				vecEnemyTank[k].m_nArmor -= pThis->m_ShellDamage;
				vecShell[i].m_nArmor = 0;
			}
		}
	}

	//敌方坦克与墙壁、我方坦克、敌方坦克的碰撞测试及自动追踪我方坦克
	for (i = 0; i < vecEnemyTank.size(); i++)
	{
		//自动追踪
		if (!vecEnemyTank[i].m_nArmor)
			continue;
		vecEnemyTank[i].pre_pt = vecEnemyTank[i].m_ptPos;
		if (end != false)
		{
			vecEnemyTank[i].ChangeDirection(MyTank);
			if (vecEnemyTank[i].m_dDis <= 1000)
				vecEnemyTank[i].Move(vecEnemyTank[i].GetDirection());
		}
		//敌方坦克与墙碰撞
		for (j = 0; j < vecWall.size(); j++)
		{
			if (vecWall[j].GetArmor() > 0 && vecWall[j].IsOverLapRect_(vecWall[j], vecEnemyTank[i]))
			{
				if (!vecEnemyTank[i].m_nArmor)
					continue;
				vecEnemyTank[i].m_ptPos = vecEnemyTank[i].pre_pt;
				int d = vecEnemyTank[i].GetDirection();
				if (end != false)
					switch (d)
					{
					case 1:
						d = 4;
						break;
					case 2:
						d = 3;
						break;
					case 3:
						d = 1;
						break;
					case 4:
						d = 2;
						break;
					}
				vecEnemyTank[i].SetDirection(d);
				//m_FoeTank.WhichDirection();
			}
		}
		//敌方坦克与我方坦克碰撞
		if (vecEnemyTank[i].m_nArmor >= 0
			&& vecEnemyTank[i].IsOverlap(MyTank)
			&& vecEnemyTank[i].GetFriend() != MyTank.GetFriend())
		{
			int temp;
			temp = MyTank.m_nArmor;
			MyTank.m_nArmor -= vecEnemyTank[i].m_nArmor;
			vecEnemyTank[i].m_nArmor -= temp;
			if (vecEnemyTank.size() != 0 && MyTank.m_nArmor <= 0)
			{
				PlaySound(strImagePathName3, 0, SND_FILENAME | SND_ASYNC);
			}
		}
		//敌方坦克与敌方坦克碰撞
		if (vecEnemyTank[i].m_nArmor <= 0)
			continue;
		for (j = i + 1; j < vecEnemyTank.size(); j++)
		{
			//友军无伤害，无效炮弹无伤害
			if (vecEnemyTank[i].GetFriend() == vecEnemyTank[j].GetFriend()
				&& vecEnemyTank[i].IsOverLapRect_(vecEnemyTank[i], vecEnemyTank[j]))
			{
				if (!vecEnemyTank[i].m_nArmor)
					continue;
				vecEnemyTank[i].m_ptPos = vecEnemyTank[i].pre_pt;
			}
		}
	}

	//删除amr小于等于0的炮弹
	for (i = vecShell.size() - 1; i >= 0; i--)
	{
		if (vecShell[i].m_nArmor <= 0)
			vecShell.erase(vecShell.begin() + i);
	}
	//删除arm小于等于0的墙
	for (i = vecWall.size() - 1; i >= 0; i--)
	{
		if (vecWall[i].m_nArmor <= 0)
			vecWall.erase(vecWall.begin() + i);
	}
	//删除arm小于等于0的坦克
	//for (i = vecEnemyTank.size() - 1; i >= 0; i--)
	//{
	//	if (vecEnemyTank[i].m_nArmor <= 0)
	//		vecEnemyTank.erase(vecEnemyTank.begin() + i);
	//}
	//......
}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	switch (nIDEvent)
	{
	case 1:
		Invalidate(FALSE);
		break;
	case 2://碰撞测试
	{
		m_mtxJudgy.try_lock();
		std::thread Overlay(Judgy, this);
		Overlay.detach();
		m_mtxJudgy.unlock();
	}
	break;
	case 3://自动开火
	{
		for (int i = 0; i < m_EnemyTank.size(); i++)
		{
			if (end != false && m_EnemyTank[i].m_nArmor > 0)
			{
				Shell shell = m_EnemyTank[i].AutoFire();
				m_vecShell.push_back(shell);
			}
		}
	}
	break;
	default:
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}


BOOL CChildView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (end != false)
	{
		CPoint tmp = m_tank.m_ptPos;
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == VK_UP || pMsg->wParam == 'W')
			{
				if (m_tank.IsEnableMove(1))
					m_tank.Move(1);
			}
			else if (pMsg->wParam == VK_DOWN || pMsg->wParam == 'S')
			{
				if (m_tank.IsEnableMove(2))
					m_tank.Move(2);
			}
			else if (pMsg->wParam == VK_LEFT || pMsg->wParam == 'A')
			{
				if (m_tank.IsEnableMove(3))
					m_tank.Move(3);
			}
			else if (pMsg->wParam == VK_RIGHT || pMsg->wParam == 'D')
			{
				if (m_tank.IsEnableMove(4))
					m_tank.Move(4);
			}
			//射击
			else if (pMsg->wParam == 'J')
			{
				static __int64 tmLast = 0;
				__int64 tmCurrent = GetTickCount64();
				if (tmCurrent - tmLast > 500)//炮管冷却，开炮间隔时间要大于0.5秒
				{
					if (m_pVoice)
						m_pVoice->Speak(L"Fire", SPF_ASYNC, NULL);
					TCHAR cFilePathName[1024];
					GetModuleFileName(NULL, cFilePathName, 1024);
					CString str = cFilePathName;
					int nPos = str.ReverseFind('\\');//反向寻找第一个反斜杠
					CString strPathName = str.Left(nPos);
					strPathName = strPathName + L"\\data";
					CString strImagePathName;
					strImagePathName = strPathName + L"\\Fire.wav";
					PlaySound(strImagePathName, 0, SND_FILENAME | SND_ASYNC);
					if (m_EnemyTank.size() != 0 && m_tank.m_nArmor != 0)
					{
						Shell shell = m_tank.Fire();
						shell.SetSpeed(m_ShellSpeed);
						int shellsize = shell.GetSize();
						shell.SetSize(m_ShellDamage* shellsize);
						m_vecShell.push_back(shell);
					}
					tmLast = tmCurrent;
				}
			}

			for (auto& w : m_vecWall)
			{
				if (w.GetArmor() > 0 && m_tank.IsOverlap(w))
				{
					m_tank.m_ptPos = tmp;
					break;
				}
			}
			if (m_tank.m_ptPos.x <= -10 || m_tank.m_ptPos.y <= -10 || m_tank.m_ptPos.x >= 1290 || m_tank.m_ptPos.y >= 900)
				m_tank.m_ptPos = tmp;
			Invalidate(FALSE);
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CChildView::OnBtn1Clicked()
{
    //AfxMessageBox(_T("按钮1被点击"));
	int speed = m_tank.GetSpeed();
	if (speed < 20)
	{
		speed = speed + 5;
		m_tank.SetSpeed(speed);
	}
}

void CChildView::OnBtn2Clicked()
{
    //AfxMessageBox(_T("按钮2被点击"));
	int speed = m_tank.GetSpeed();
	if (speed > 0)
	{
		speed = speed - 5;
		if (speed <= 0)speed = 1;
		m_tank.SetSpeed(speed);
	}
}

void CChildView::OnBtn3Clicked()
{
	//AfxMessageBox(_T("按钮3被点击"));
	if (m_ShellSpeed < 5)
	{
		m_ShellSpeed = m_ShellSpeed + 1;
	}	
}

void CChildView::OnBtn4Clicked()
{
	//AfxMessageBox(_T("按钮4被点击"));
	if (m_ShellSpeed > 0)
	{
		m_ShellSpeed = m_ShellSpeed - 1;
		if (m_ShellSpeed == 0)m_ShellSpeed = 1;
	}

}

void CChildView::OnBtn5Clicked()
{
	//AfxMessageBox(_T("按钮5被点击"));
	if (m_ShellDamage < 3) // 1 2 3
	{
		m_ShellDamage = m_ShellDamage + 1;
	}
}

void CChildView::OnBtn6Clicked()
{
	//AfxMessageBox(_T("按钮6被点击"));
	if (m_ShellDamage > 0)
	{
		m_ShellDamage = m_ShellDamage - 1;
		if (m_ShellDamage == 0)m_ShellDamage = 1;
	}
}