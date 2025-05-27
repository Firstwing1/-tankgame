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

// ChildView.h: CChildView 类的接口
//


#pragma once
#include "Tank.h"
#include "Shell.h"
#include "Wall.h"
#include "AutoTank.h"
#include <vector>
#include <sapi.h>
#include <mutex>


// CChildView 窗口

class CChildView : public CWnd
{
// 构造
public:
	CChildView();

// 特性
public:
  CRect m_client;    //保存客户区大小
  CRect m_heroPos;    //保存英雄的位置
  CImage m_hero;   //英雄
  CImage m_bg;      //背景图片
  int m_ShellSpeed;    //我方子弹速度
  int m_ShellDamage;    //我方子弹级别
  ISpVoice* m_pVoice;//语音合成
  Tank m_tank;//增加
  std::vector<Shell> m_vecShell;
  std::vector<Wall>m_vecWall;
  std::vector<Wall>m_innerWall;
  std::vector<AutoTank> m_EnemyTank;//敌方坦克,自行实现。受到阻碍后自动改变方向，自行开炮
  CButton m_btn1, m_btn2, m_btn3 ;
  CButton m_btn4, m_btn5, m_btn6;
  enum { ID_BTN1 = 1000, ID_BTN2 = 1001, ID_BTN3 = 1002 ,
	  ID_BTN4 = 1003, ID_BTN5 = 1004, ID_BTN6 = 1005,
  };
// 操作
public:
	//多线程相关函数
	static void Judgy(CChildView* pThis);//碰撞测试，多线程方式运行
	std::mutex m_mtxJudgy;//碰撞锁

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnBtn1Clicked();
  afx_msg void OnBtn2Clicked();
  afx_msg void OnBtn3Clicked();
  afx_msg void OnBtn4Clicked();
  afx_msg void OnBtn5Clicked();
  afx_msg void OnBtn6Clicked();
  void InitEnemyTanks(int difficulty);

};

