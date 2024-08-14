#include "pch.h"
#include "EVlc.h"

/* 构造函数中创建实例 */
EVlc::EVlc()
{
	m_instance = libvlc_new(0, NULL);
	m_media = NULL;
	m_player = NULL;
	m_hwnd = NULL;
}

/* 析构函数中释放播放器、媒体、实例 */
EVlc::~EVlc()
{
	if (m_player != NULL) {
		libvlc_media_player_t* temp = m_player;
		m_player = NULL;
		libvlc_media_player_release(temp);

	}
	if (m_media != NULL) {
		libvlc_media_t* temp = m_media;
		m_media = NULL;
		libvlc_media_release(temp);
	}
	if (m_instance != NULL) {
		libvlc_instance_t* temp = m_instance;
		m_instance = NULL;
		libvlc_release(temp);
	}
}

/* 加载媒体、创建播放器、设置播放宽高、设置播放窗口 */
int EVlc::SetMedia(const std::string& strUrl)
{
	// 加载媒体
	if (m_instance == NULL || (m_hwnd == NULL))return -1;
	// 如果没有更改音频地址，就返回。避免重新创建播放器
	if (strUrl == m_url)return 0;
	m_url = strUrl;
	if (m_media != NULL) {
		libvlc_media_release(m_media);
		m_media = NULL;
	}
	m_media = libvlc_media_new_location(m_instance, strUrl.c_str());
	if (!m_media) return -2;
	// 创建播放器
	if (m_player != NULL) {
		libvlc_media_player_release(m_player);
		m_player = NULL;
	}
	m_player = libvlc_media_player_new_from_media(m_media);
	if (!m_player)return -3;
	// 设置播放宽高
	CRect rect;
	GetWindowRect(m_hwnd, rect);
	std::string strRatio="";
	strRatio.resize(32);
	sprintf((char*)strRatio.c_str(), "%d:%d", rect.Width(), rect.Height());
	libvlc_video_set_aspect_ratio(m_player, strRatio.c_str());
	// 根据传递进来的窗口句柄，设置播放窗口
	libvlc_media_player_set_hwnd(m_player, m_hwnd);
	return 0;
}

/* 将播放窗口的句柄传递进来 */
#ifdef WIN32
int EVlc::SetHwnd(HWND hWnd)
{
	m_hwnd = hWnd;
	return 0;
}
#endif

/* 封装libvlc_media_player_play，开始播放 */
int EVlc::Play()
{
	if (!m_player || !m_instance || !m_media)return -1;
	return libvlc_media_player_play(m_player);
}

int EVlc::Pause()
{
	if (!m_player || !m_instance || !m_media)return -1;
	libvlc_media_player_pause(m_player);
	return 0;
}

int EVlc::Stop()
{
	if (!m_player || !m_instance || !m_media)return -1;
	libvlc_media_player_stop(m_player);
	return 0;
}

float EVlc::GetPostion()
{
	if (!m_player || !m_instance || !m_media)return -1.0;
	return libvlc_media_player_get_position(m_player);
}

void EVlc::SetPostion(float pos)
{
	if (!m_player || !m_instance || !m_media)return;
	libvlc_media_player_set_position(m_player, pos);
}

int EVlc::GetVolume()
{
	if (!m_player || !m_instance || !m_media)return -1;
	return libvlc_audio_get_volume(m_player);;
}

int EVlc::SetVolume(int volume)
{
	if (!m_player || !m_instance || !m_media)return -1;
	return libvlc_audio_set_volume(m_player, volume);
}

/* 获取音频信息，长*宽 */
VlcSize EVlc::GetMediaInfo()
{
	if (!m_player || !m_instance || !m_media)return VlcSize(-1, -1);
	return VlcSize(
		libvlc_video_get_width(m_player),
		libvlc_video_get_height(m_player)
	);
}

float EVlc::GetLength()
{
	if (!m_player || !m_instance || !m_media)return -1.0f;
	libvlc_time_t tm = libvlc_media_player_get_length(m_player);  // 毫秒
	float ret = tm / 1000.0f;
	return ret;
}

std::string EVlc::Unicode2Utf8(const std::wstring& strIn)
{
	std::string str;
	int length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
	str.resize(length + 1);
	::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), length, NULL, NULL);
	return str;
}