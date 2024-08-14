#include "pch.h"
#include "framework.h"
#include "VideoClient.h"
#include "VideoClientDlg.h"
#include "afxdialogex.h"
#include "VideoClientController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* 设置播放状态为false， 此时hWnd为空 */
CVideoClientDlg::CVideoClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_status = false;
	m_length = 0.0f;
}

void CVideoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLAY, m_video);
	DDX_Control(pDX, IDC_SLIDER_POS, m_pos);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_volume);
	DDX_Control(pDX, IDC_EDIT_URL, m_url);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
}

BEGIN_MESSAGE_MAP(CVideoClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CVideoClientDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVideoClientDlg::OnBnClickedBtnStop)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_POS, &CVideoClientDlg::OnTRBNThumbPosChangingSliderPos)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOLUME, &CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume)
	// 当用户单击滚动条或者是拖动滑块时，Windows会向窗口过程发送WM_VSCROLL消息（垂直滚动时）或者是WM_HSCROLL消息（水平滚动时）。
	ON_WM_HSCROLL()   
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CVideoClientDlg 消息处理程序
// 1.设置计数器
// 2.初始化控件
// 3.将窗口句柄传递给vlc类
BOOL CVideoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(0, 500, NULL);  // 设置计数器500毫秒
	m_pos.SetRange(0, 1);
	m_volume.SetRange(0, 100);
	m_volume.SetTicFreq(20); // 设置刻度20一个刻度
	SetDlgItemText(IDC_STATIC_VOLUME, _T("100%"));              // 初始音量显示100%
	SetDlgItemText(IDC_STATIC_TIME, _T("--:--:--/--:--:--"));   // 初始播放进度
	// 只有在此时才有窗口句柄，才可以传递
	m_controller->SetWnd(m_video.GetSafeHwnd());
	m_url.SetWindowText(_T("file:///D:\\vs2022 code\\Av (audio and video)\\Av-audio-and-video-\\VideoClient\\股市讨论.mp4"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CVideoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVideoClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0) {
		//控制层，获取播放状态，进度信息
		float pos = m_controller->VideoCtrl(EVLC_GET_POSITION); // 拿到的是一个比例
		if (pos != -1.0f) {//更新播放时间
			if (m_length <= 0.0f) m_length = m_controller->VideoCtrl(EVLC_GET_LENGTH);
			if (m_pos.GetRangeMax() <= 1) {
				// 设置播放条的范围
				m_pos.SetRange(0, int(m_length));
			}
			// 更新进度条进度和底下文本框的显示
			CString strPos;
			strPos.Format(_T("%f/%f"), pos * m_length, m_length);
			SetDlgItemText(IDC_STATIC_TIME, strPos);
			m_pos.SetPos(int(m_length * pos));
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

/* 窗口销毁的响应函数 */
void CVideoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(0);
	// TODO: 在此处添加消息处理程序代码
}

/* 播放/暂停按钮的响应函数 */
void CVideoClientDlg::OnBnClickedBtnPlay()
{
	if (m_status == false) {
		CString url;
		m_url.GetWindowText(url);
		// unicode -> utf-8
		// SetMedia增加了m_url进行比对，只在第一次点击播放的时候创建媒体和播放器，剩下的情况直接返回。
		m_controller->SetMedia(m_controller->Unicode2Utf8((LPCTSTR)url));
		m_btnPlay.SetWindowText(_T("暂停"));
		m_status = true;
		m_controller->VideoCtrl(EVLC_PLAY);
	}
	else {
		m_btnPlay.SetWindowText(_T("播放"));
		m_status = false;
		m_controller->VideoCtrl(EVLC_PAUSE);
	}
}

/* 停止按钮的响应函数 */
void CVideoClientDlg::OnBnClickedBtnStop()
{
	m_btnPlay.SetWindowText(_T("播放"));
	m_status = false;
	m_controller->VideoCtrl(EVLC_STOP);
}

/* 滚动条位置变化的响应函数 没反应 */
void CVideoClientDlg::OnTRBNThumbPosChangingSliderPos(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos %d reason %d\r\n", pNMTPC->dwPos, pNMTPC->nReason);
	*pResult = 0;
}

/* 音量条位置变化的响应函数 没反应 */
void CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos %d reason %d\r\n", pNMTPC->dwPos, pNMTPC->nReason);
	*pResult = 0;
}

/* 播放条拖动的响应函数，响应消息ON_WM_HSCROLL */
void CVideoClientDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 如果有多个水平滑动条，会通过pScrollBar来辨别是哪一个滑动条发出的消息 
	// 通过比较&m_pos与pScrollBar来判断是不是这个控件
	//TRACE("pos %p volume %p cur %p pos %d code %d\r\n", &m_pos, &m_volume, pScrollBar, nPos, nSBCode);
	if (nSBCode == 5) {
		CString strPosition;
		strPosition.Format(_T("%d%%"), nPos);
		SetDlgItemText(IDC_STATIC_TIME, strPosition);
		m_controller->SetPosition(float(nPos) / m_length); // vlc 拿到的需要是比例
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

/* 音量条拖动的响应函数，响应消息ON_WM_VSCROLL */
void CVideoClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{//音量被拖动了
	// 如果有多个垂直滑动条，会通过pScrollBar来辨别是哪一个滑动条发出的消息
	if (nSBCode == 5) {
		TRACE("pos %p volume %p cur %p pos %d code %d\r\n", &m_pos, &m_volume, pScrollBar, nPos, nSBCode);
		CString strVolume;
		strVolume.Format(_T("%d%%"), 100 - nPos);
		SetDlgItemText(IDC_STATIC_VOLUME, strVolume);
		m_controller->SetVolume(100 - nPos);
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}