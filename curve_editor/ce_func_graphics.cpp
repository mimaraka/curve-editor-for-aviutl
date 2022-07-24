//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（グラフィック）
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		Direct2Dの初期化
//---------------------------------------------------------------------
void D2D1_Init(HDC hdc, LPRECT rect_wnd, COLORREF cr)
{
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BindDC(hdc, rect_wnd);
		g_render_target->BeginDraw();
		g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
		g_render_target->Clear(D2D1::ColorF(cr));
		g_render_target->EndDraw();
	}
}


//---------------------------------------------------------------------
//		グラフのグリッドを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawGrid(ID2D1SolidColorBrush* pBrush, LPRECT rect_wnd) {
	pBrush->SetColor(D2D1::ColorF(BRIGHTEN(ToBGR(g_theme[g_cfg.theme].gr_bg), CE_BR_GRID)));
	int kx = std::floor(std::log(CE_GR_RES * g_disp_info.scale.x / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int ky = std::floor(std::log(CE_GR_RES * g_disp_info.scale.y / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int nx = std::pow(CE_GR_GRID_N, kx);
	int ny = std::pow(CE_GR_GRID_N, ky);
	double dx = (CE_GR_RES * g_disp_info.scale.x) / nx;
	double dy = (CE_GR_RES * g_disp_info.scale.y) / ny;
	int lx, ly;

	if (toGraph(0, 0).x >= 0)
		lx = std::floor(toGraph(0, 0).x * nx / (double)CE_GR_RES);
	else
		lx = std::ceil(toGraph(0, 0).x * nx / (double)CE_GR_RES);

	if (toGraph(0, 0).y >= 0)
		ly = std::floor(toGraph(0, 0).y * ny / (double)CE_GR_RES);
	else
		ly = std::ceil(toGraph(0, 0).y * ny / (double)CE_GR_RES);

	double ax = toClient(lx * CE_GR_RES / (double)nx, 0).x;
	double ay = toClient(0, ly * CE_GR_RES / (double)ny).y;
	float thickness;

	for (int i = 0; ax + dx * i <= rect_wnd->right; i++) {
		if ((lx + i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, rect_wnd->bottom),
			pBrush, thickness, NULL
		);
	}

	for (int i = 0; ay + dy * i <= rect_wnd->bottom; i++) {
		if ((ly - i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F(rect_wnd->right, ay + dy * i),
			pBrush, thickness, NULL
		);
	}
}


//---------------------------------------------------------------------
//		ベジェカーブを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawBezier(ID2D1SolidColorBrush* pBrush,
	DoublePoint stpt, DoublePoint ctpt1, DoublePoint ctpt2, DoublePoint edpt, float thickness)
{
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* bezier;
	g_d2d1_factory->CreatePathGeometry(&bezier);
	bezier->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(stpt.x, stpt.y), D2D1_FIGURE_BEGIN_HOLLOW);
	sink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(ctpt1.x, ctpt1.y),
		D2D1::Point2F(ctpt2.x, ctpt2.y),
		D2D1::Point2F(edpt.x, edpt.y)
	));
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	if (bezier)
		g_render_target->DrawGeometry(bezier, pBrush, thickness);
}


//---------------------------------------------------------------------
//		グラフのハンドルを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawHandle(ID2D1SolidColorBrush* pBrush, DoublePoint stpt, DoublePoint edpt)
{
	g_render_target->DrawLine(
		D2D1::Point2F(stpt.x, stpt.y),
		D2D1::Point2F(edpt.x, edpt.y),
		pBrush, CE_HANDLE_TH
	);
	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(edpt.x, edpt.y),
			CE_HANDLE_SIZE, CE_HANDLE_SIZE),
		pBrush, CE_HANDLE_SIZE * 2
	);
}


//---------------------------------------------------------------------
//		矩形を描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawSquare(ID2D1SolidColorBrush* pBrush, DoublePoint cl_pt)
{
	D2D1_RECT_F rect = {
		cl_pt.x - CE_POINT_SIZE,
		cl_pt.y - CE_POINT_SIZE,
		cl_pt.x + CE_POINT_SIZE,
		cl_pt.y + CE_POINT_SIZE,
	};
	pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].handle)));
	g_render_target->FillRectangle(rect, pBrush);
}


//---------------------------------------------------------------------
//		ウィンドウの端を丸める処理(Direct2D)
//---------------------------------------------------------------------
void D2D1_FillWndEdge(ID2D1SolidColorBrush* pBrush, LPRECT rect_wnd, int flags)
{
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* edge;
	g_d2d1_factory->CreatePathGeometry(&edge);
	edge->Open(&sink);

	//左上
	if (flags & 1 << 0) {
		sink->BeginFigure(D2D1::Point2F(rect_wnd->left, rect_wnd->top), D2D1_FIGURE_BEGIN_FILLED);
		sink->AddLine(D2D1::Point2F(rect_wnd->left, rect_wnd->top + CE_BORDER_RADIUS));
		sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect_wnd->left + CE_BORDER_RADIUS, rect_wnd->top),
				D2D1::SizeF(CE_BORDER_RADIUS, CE_BORDER_RADIUS),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	//左下
	if (flags & 1 << 1) {
		sink->BeginFigure(D2D1::Point2F(rect_wnd->left, rect_wnd->bottom), D2D1_FIGURE_BEGIN_FILLED);
		sink->AddLine(D2D1::Point2F(rect_wnd->left + CE_BORDER_RADIUS, rect_wnd->bottom));
		sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect_wnd->left, rect_wnd->bottom - CE_BORDER_RADIUS),
				D2D1::SizeF(CE_BORDER_RADIUS, CE_BORDER_RADIUS),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	//右上
	if (flags & 1 << 2) {
		sink->BeginFigure(D2D1::Point2F(rect_wnd->right, rect_wnd->top), D2D1_FIGURE_BEGIN_FILLED);
		sink->AddLine(D2D1::Point2F(rect_wnd->right - CE_BORDER_RADIUS, rect_wnd->top));
		sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect_wnd->right, rect_wnd->top + CE_BORDER_RADIUS),
				D2D1::SizeF(CE_BORDER_RADIUS, CE_BORDER_RADIUS),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	//右下
	if (flags & 1 << 3) {
		sink->BeginFigure(D2D1::Point2F(rect_wnd->right, rect_wnd->bottom), D2D1_FIGURE_BEGIN_FILLED);
		sink->AddLine(D2D1::Point2F(rect_wnd->right, rect_wnd->bottom - CE_BORDER_RADIUS));
		sink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect_wnd->right - CE_BORDER_RADIUS, rect_wnd->bottom),
				D2D1::SizeF(CE_BORDER_RADIUS, CE_BORDER_RADIUS),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}
	sink->Close();
	pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].bg)));
	if (edge)
		g_render_target->FillGeometry(edge, pBrush, NULL);
	
}


//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void drawMain(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	RECT rect_sepr;
	static ID2D1SolidColorBrush* pBrush = NULL;

	rect_sepr = {
	g_cfg.sepr - CE_SEPR_W,
	0,
	g_cfg.sepr + CE_SEPR_W,
	rect_wnd->bottom
	};

	//Direct2D初期化
	D2D1_Init(hdc_mem, rect_wnd, ToBGR(g_theme[g_cfg.theme].bg_wnd));

	if (g_render_target != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
		pBrush->SetColor(D2D1::ColorF(ToBGR(BRIGHTEN(g_theme[g_cfg.theme].bg_wnd, CE_BR_SEPR))));
		if (pBrush) g_render_target->DrawLine(
			D2D1::Point2F(rect_sepr.left + CE_SEPR_W, (rect_sepr.top + rect_sepr.bottom) * 0.5 - CE_SEPR_LINE_L),
			D2D1::Point2F(rect_sepr.left + CE_SEPR_W, (rect_sepr.top + rect_sepr.bottom) * 0.5 + CE_SEPR_LINE_L),
			pBrush, CE_SEPR_LINE_W, NULL
		);
		g_render_target->EndDraw();
	}

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		サイドパネルを描画
//---------------------------------------------------------------------
void drawSide(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Init(hdc_mem, rect_wnd, ToBGR(g_theme[g_cfg.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		ライブラリを描画
//---------------------------------------------------------------------
void drawLib(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Init(hdc_mem, rect_wnd, ToBGR(g_theme[g_cfg.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		エディタパネルを描画
//---------------------------------------------------------------------
void drawEditor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Init(hdc_mem, rect_wnd, ToBGR(g_theme[g_cfg.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		グラフパネルを描画
//---------------------------------------------------------------------
void drawGraph(HWND hwnd, HDC hdc_mem, POINT* pt_trace, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;
	ID2D1StrokeStyle* pStyle = NULL;
	float dashes[] = { CE_GR_POINT_DASH, CE_GR_POINT_DASH };

	DoublePoint ctpt_cl[] = {//クライアント
		{
			g_disp_info.o.x,
			g_disp_info.o.y
		},
		{
			toClient(g_cv_vl.ctpt[0]).x,
			toClient(g_cv_vl.ctpt[0]).y
		},
		{
			toClient(g_cv_vl.ctpt[1]).x,
			toClient(g_cv_vl.ctpt[1]).y
		},
		{
			g_disp_info.o.x + g_disp_info.scale.x * CE_GR_RES,
			g_disp_info.o.y - g_disp_info.scale.y * CE_GR_RES
		}
	};
	DoublePoint ctpt_hs_cl[] = {
		ctpt_cl[0],
		{
			toClient(pt_trace[0]).x,
			toClient(pt_trace[0]).y
		},
		{
			toClient(pt_trace[1]).x,
			toClient(pt_trace[1]).y
		},
		ctpt_cl[3]
	};

	D2D1_RECT_F rect_left = {
		0, 0,
		ctpt_cl[0].x,
		rect_wnd->bottom
	};

	D2D1_RECT_F rect_right = {
		ctpt_cl[3].x, 0,
		rect_wnd->right,
		rect_wnd->bottom,
	};

	//Direct2D初期化
	D2D1_Init(hdc_mem, rect_wnd, ToBGR(g_theme[g_cfg.theme].gr_bg));

	//描画
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);

		//グリッド
		D2D1_DrawGrid(pBrush, rect_wnd);

		pBrush->SetColor(D2D1::ColorF(BRIGHTEN(ToBGR(g_theme[g_cfg.theme].gr_bg), CE_BR_GR_INVALID)));
		pBrush->SetOpacity(0.5);
		if (pBrush) {
			g_render_target->FillRectangle(&rect_left, pBrush);
			g_render_target->FillRectangle(&rect_right, pBrush);
		}
		pBrush->SetOpacity(1);


		//値モードのとき
		if (!g_cfg.mode) {
			//bezier curve (trace)
			if (g_cfg.trace) {
				pBrush->SetColor(D2D1::ColorF(BRIGHTEN(ToBGR(g_theme[g_cfg.theme].gr_bg), CE_BR_TRACE)));
				D2D1_DrawBezier(pBrush, ctpt_hs_cl[0], ctpt_hs_cl[1], ctpt_hs_cl[2], ctpt_hs_cl[3], CE_CURVE_TH);
			}
			//ベジェ
			pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].curve)));
			D2D1_DrawBezier(pBrush, ctpt_cl[0], ctpt_cl[1], ctpt_cl[2], ctpt_cl[3], CE_CURVE_TH);

			//ハンドルの色
			pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].handle)));

			//ハンドル（左）
			D2D1_DrawHandle(pBrush, ctpt_cl[0], ctpt_cl[1]);
			//ハンドル（右）
			D2D1_DrawHandle(pBrush, ctpt_cl[3], ctpt_cl[2]);
		}

		//IDモードのとき
		else {
			for (int i = 0; i < g_cv_id[g_cfg.id_current].ctpts.size() - 1; i++)
			{
				pBrush->SetColor(D2D1::ColorF(ToBGR(CONTRAST(INVERT(g_theme[g_cfg.theme].gr_bg), CE_GR_POINT_CONTRAST))));
				g_d2d1_factory->CreateStrokeStyle(
					D2D1::StrokeStyleProperties(
						D2D1_CAP_STYLE_FLAT,
						D2D1_CAP_STYLE_FLAT,
						D2D1_CAP_STYLE_ROUND,
						D2D1_LINE_JOIN_MITER,
						10.0f,
						D2D1_DASH_STYLE_CUSTOM,
						0.0f),
					dashes,
					ARRAYSIZE(dashes),
					&pStyle
				);
				if (i > 0)
					g_render_target->DrawLine(
						D2D1::Point2F(toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_center).x, 0),
						D2D1::Point2F(toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_center).x, rect_wnd->bottom),
						pBrush, CE_GR_POINT_TH, pStyle
					);

				pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].curve)));
				D2D1_DrawBezier(pBrush,
					toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_center),
					toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_right),
					toClient(g_cv_id[g_cfg.id_current].ctpts[i + 1].pt_left),
					toClient(g_cv_id[g_cfg.id_current].ctpts[i + 1].pt_center),
					CE_CURVE_TH
				);

				pBrush->SetColor(D2D1::ColorF(ToBGR(g_theme[g_cfg.theme].handle)));
				D2D1_DrawHandle(pBrush,
					toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_center),
					toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_right)
				);
				D2D1_DrawHandle(pBrush,
					toClient(g_cv_id[g_cfg.id_current].ctpts[i + 1].pt_center),
					toClient(g_cv_id[g_cfg.id_current].ctpts[i + 1].pt_left)
				);

				if (!g_theme[g_cfg.theme].shape && pBrush) {
					g_render_target->DrawEllipse(
						D2D1::Ellipse(
							D2D1::Point2F(
								g_cv_id[g_cfg.id_current].ctpts[i].pt_center.x,
								g_cv_id[g_cfg.id_current].ctpts[i].pt_center.y),
							CE_POINT_SIZE, CE_POINT_SIZE),
						pBrush, CE_POINT_SIZE * 2
					);
				}
				else if(pBrush) D2D1_DrawSquare(pBrush, toClient(g_cv_id[g_cfg.id_current].ctpts[i].pt_center));
			}
			if (!g_theme[g_cfg.theme].shape && pBrush) {
				g_render_target->DrawEllipse(
					D2D1::Ellipse(
						D2D1::Point2F(
							g_cv_id[g_cfg.id_current].ctpts[g_cv_id[g_cfg.id_current].ctpts.size() - 1].pt_center.x,
							g_cv_id[g_cfg.id_current].ctpts[g_cv_id[g_cfg.id_current].ctpts.size() - 1].pt_center.y),
						CE_POINT_SIZE, CE_POINT_SIZE),
					pBrush, CE_POINT_SIZE * 2
				);
			}
			else if (pBrush) D2D1_DrawSquare(pBrush, toClient(g_cv_id[g_cfg.id_current].ctpts[g_cv_id[g_cfg.id_current].ctpts.size() - 1].pt_center));
		}

		if (!g_theme[g_cfg.theme].shape && pBrush) {
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ctpt_cl[0].x, ctpt_cl[0].y),
					2.5f, 2.5f),
				pBrush, 4
			);
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ctpt_cl[3].x, ctpt_cl[3].y),
					2.5f, 2.5f), pBrush, 4
			);
		}

		D2D1_FillWndEdge(pBrush, rect_wnd, CE_EDGE_LT | CE_EDGE_LB | CE_EDGE_RT | CE_EDGE_RB);

		g_render_target->EndDraw();
	}


	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}