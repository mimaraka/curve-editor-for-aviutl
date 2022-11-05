//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_class.hpp"



//----------------------------------------------------------------------------------
//		extern宣言
//----------------------------------------------------------------------------------
extern cve::Curve							g_curve_normal,
											g_curve_normal_previous,
											g_curve_mb[CVE_CURVE_MAX],
											g_curve_mb_previous;

extern cve::Static_Array<cve::Preset, CVE_PRESET_NUM_DEFAULT> g_presets_default;
extern cve::Static_Array<cve::Preset, CVE_PRESET_NUM_CUSTOM> g_presets_custom;

extern const cve::Theme						g_theme[2];

extern cve::Config							g_config;

extern cve::Window							g_window_main,
											g_window_header,
											g_window_editor,
											g_window_library,
											g_window_preset_list;

extern cve::Graph_View_Info					g_view_info;

extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;

//フック
extern BOOL									(WINAPI* TrackPopupMenu_original)(HMENU, UINT, int, int, int, HWND, const RECT*);
extern INT_PTR								(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
extern INT_PTR								(WINAPI* dialogproc_original)(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		フィルタ関数
//----------------------------------------------------------------------------------

//フィルタ構造体のポインタを渡す
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//初期化・終了
BOOL				filter_initialize(FILTER*);
BOOL				filter_exit(FILTER*);

// aviutl.iniを用いて設定を読み書き
void				ini_load_configs(FILTER* fp);
void				ini_write_configs(FILTER* fp);

// プロジェクト読み込み・書き出し時に呼び出される関数
BOOL				on_project_load(FILTER* fp, void* editp, void* data, int size);
BOOL				on_project_save(FILTER* fp, void* editp, void* data, int* size);

// ウィンドウプロシージャ
BOOL				filter_wndproc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);



//----------------------------------------------------------------------------------
//		その他の関数
//----------------------------------------------------------------------------------

namespace cve {
	// Direct2D初期化
	void						d2d_init();

	//文字列の分割
	std::vector<std::string>	split(const std::string& s, TCHAR c);

	//クリップボードにテキストをコピー
	BOOL						copy_to_clipboard(HWND, LPCTSTR);

	// 長さを減算
	void						subtract_length(cve::Float_Point* pt, const cve::Float_Point& st, const cve::Float_Point& ed, float length);

	// 設定をメニューに反映
	void						apply_config_to_menu(HMENU menu, MENUITEMINFO* mi);

	// キー押下時の処理
	LRESULT						on_keydown(WPARAM wparam);
}



//----------------------------------------------------------------------------------
//		ウィンドウプロシージャ
//----------------------------------------------------------------------------------
LRESULT CALLBACK	wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_header(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_library(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_preset_list(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		ダイアログプロシージャ
//----------------------------------------------------------------------------------
BOOL CALLBACK		dialogproc_config(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_param_normal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_save(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_id(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		フック
//----------------------------------------------------------------------------------
BOOL WINAPI			TrackPopupMenu_hooked(HMENU menu, UINT flags, int x, int y, int reserved, HWND hwnd, const RECT* rect);
INT_PTR WINAPI		DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param);
BOOL CALLBACK		dialogproc_hooked(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		インライン関数
//----------------------------------------------------------------------------------

//グラフ -> クライアント
inline cve::Float_Point to_client(int gr_x, int gr_y)
{
	return {
		g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline cve::Float_Point to_client(const POINT& pt_graph)
{
	return {
		to_client(pt_graph.x, pt_graph.y).x,
		to_client(pt_graph.x, pt_graph.y).y
	};
}

//クライアント -> グラフ
inline POINT to_graph(double cl_x, double cl_y)
{
	return {
		(int)((cl_x - g_view_info.origin.x) / g_view_info.scale.x),
		(int)((-cl_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

inline POINT to_graph(const POINT& pt_client)
{
	return {
		to_graph(pt_client.x, pt_client.y).x,
		to_graph(pt_client.x, pt_client.y).y
	};
}

//グラフ -> プリセット
inline cve::Float_Point to_preset(int gr_x, int gr_y)
{
	return {
		(float)CVE_MARGIN_PRESET + (gr_x / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2),
		g_config.preset_size - (float)CVE_MARGIN_PRESET - (gr_y / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2)
	};
}

inline cve::Float_Point to_preset(const POINT& pt_graph)
{
	return {
		to_preset(pt_graph.x, pt_graph.y).x,
		to_preset(pt_graph.x, pt_graph.y).y
	};
}