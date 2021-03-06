//----------------------------------------------------------------------------
// Agg2D - Version 1.0
// Based on Anti-Grain Geometry
// Copyright (C) 2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//	25 Jan 2007 - Ported to AGG 2.4 Jerry Evans (jerry@novadsp.com)
//
//----------------------------------------------------------------------------

#ifndef AGG2D_INCLUDED
#define AGG2D_INCLUDED


// With this define uncommented you can use FreeType font engine
#define AGG2D_USE_FREETYPE
#define AGG2D_USE_FONTS

// JME
#include "../include/agg_basics.h"
#include "../include/agg_trans_affine.h"
#include "../include/agg_trans_viewport.h"
#include "../include/agg_path_storage.h"
#include "../include/agg_conv_stroke.h"
#include "../include/agg_conv_transform.h"
#include "../include/agg_conv_curve.h"
#include "../include/agg_rendering_buffer.h"
#include "../include/agg_renderer_base.h"
#include "../include/agg_renderer_scanline.h"
#include "../include/agg_span_gradient.h"
#include "../include/agg_span_image_filter_rgba.h"
//#include "agg_span_image_resample_rgba.h"
//+ JME
#include "../include/agg_span_allocator.h"
#include "../include/agg_span_converter.h"
#include "../include/agg_span_interpolator_linear.h"
#include "../include/agg_rasterizer_scanline_aa.h"
#include "../include/agg_gamma_functions.h"
#include "../include/agg_scanline_u.h"
#include "../include/agg_scanline_p.h"
#include "../include/agg_bezier_arc.h"
#include "../include/agg_rounded_rect.h"
#include "../include/agg_font_cache_manager.h"

#ifdef AGG2D_USE_FREETYPE
//#include "../font_freetype/agg_font_freetype.h"
#else
//	#ifdef WINDOWS
//		#include "../font_win32_tt/agg_font_win32_tt.h"
//	#endif
#endif

#include "../include/agg_pixfmt_rgba.h"
#include "../include/agg_pixfmt_gray.h"
#include "../include/agg_pixfmt_amask_adaptor.h"
#include "../include/agg_alpha_mask_u8.h"

//+ JME
#include "../include/agg_image_accessors.h"

template<class ColorT, class Order> struct canvas_blender
{
	typedef ColorT color_type;
	typedef Order order_type;
	typedef typename color_type::value_type value_type;
	typedef typename color_type::calc_type calc_type;
	enum base_scale_e
	{ 
		base_shift = color_type::base_shift,
		base_mask  = color_type::base_mask
	};

	// Blend pixels using the premultiplied form of Alvy-Ray Smith's
	// compositing function. 

	//--------------------------------------------------------------------
	static AGG_INLINE void blend_pix(value_type* p, 
		unsigned cr, unsigned cg, unsigned cb,
		unsigned alpha,
		unsigned cover)
	{
		alpha = color_type::int_mult_cover(alpha, cover);

		if(alpha == 0) return;

		double const Aa = alpha/255.0;
		double const Ab = p[Order::A]/255.0;

		double const bfac = Ab *(1-Aa);
		double const c = Aa + bfac;

		double const Ra = cr;
		double const Rb = p[Order::R];
		double const Ga = cg;
		double const Gb = p[Order::G];
		double const Ba = cb;
		double const Bb = p[Order::B];

		double const ic = 1.0/c;

		p[Order::R] = (value_type)((Aa* Ra + bfac * Rb)*ic);
		p[Order::G] = (value_type)((Aa* Ga + bfac * Gb)*ic);
		p[Order::B] = (value_type)((Aa* Ba + bfac * Bb)*ic);

		p[Order::A] = (value_type)((c)*255);

	}

	//--------------------------------------------------------------------
	static AGG_INLINE void blend_pix(value_type* p, 
		unsigned cr, unsigned cg, unsigned cb,
		unsigned alpha)
	{
		if(alpha == 0) return;
		/*	double b = p[Order::A]/255.0;
		double a = alpha/255.0;

		double ca = (1-b+a*b);
		double ica = 1-ca;

		double A = cr;
		double B = p[Order::R];
		double AA = cg;
		double BB = p[Order::G];
		double AAA = cb;
		double BBB = p[Order::B];

		double outA = ca * A + ica *B;			
		double outAA = ca*AA  + ica * BB;
		double outAAA = ca*AAA + ica * BBB;

		p[Order::R] = (value_type)(outA);
		p[Order::G] = (value_type)(outAA);
		p[Order::B] = (value_type)(outAAA);
		p[Order::A] = (value_type)((a*ca + b*ica)*255);
		*/


		double a = alpha/255.0;
		double b = p[Order::A]/255.0;

		double ica = b *(1-a);

		double ao = a + ica;

		double A = cr;
		double B = p[Order::R];
		double AA = cg;
		double BB = p[Order::G];
		double AAA = cb;
		double BBB = p[Order::B];


		double outA = (a * A + ica *B)/ao;			
		double outAA = (a*AA  + ica * BB)/ao;
		double outAAA = (a*AAA + ica * BBB)/ao;

		p[Order::R] = (value_type)(outA);
		p[Order::G] = (value_type)(outAA);
		p[Order::B] = (value_type)(outAAA);


		p[Order::A] = (value_type)((ao)*255);
		/*            calc_type a = p[Order::A];
		calc_type r = color_type::int_mult(p[Order::R], a);
		calc_type g = color_type::int_mult(p[Order::G], a);
		calc_type b = color_type::int_mult(p[Order::B], a);
		p[Order::R] = (value_type)(color_type::int_lerp(r, cr, alpha));
		p[Order::G] = (value_type)(color_type::int_lerp(g, cg, alpha));
		p[Order::B] = (value_type)(color_type::int_lerp(b, cb, alpha));
		p[Order::A] = (value_type)(color_type::int_prelerp(a, alpha, alpha));
		agg::multiplier_rgba<ColorT,Order>::demultiply(p);*/
	}
};


class Agg2D
{
	typedef agg::order_bgra ComponentOrder; // Platform dependent!

public:
	typedef agg::rgba8                                               ColorType;
private:
	typedef canvas_blender<ColorType, ComponentOrder>             Blender;
	typedef agg::comp_op_adaptor_rgba<ColorType, ComponentOrder>     BlenderComp;


	//	typedef agg::pixfmt_bgra32		PixFormat32;
	typedef agg::pixfmt_alpha_blend_rgba<Blender , agg::rendering_buffer, agg::pixel32_type> PixFormat;
	//typedef agg::pixfmt_custom_blend_rgba<Blender,agg::rendering_buffer>        PixFormat_test;

	typedef agg::pixfmt_custom_blend_rgba<BlenderComp,agg::rendering_buffer>        PixFormat_Comp;

	typedef agg::pixfmt_amask_adaptor<PixFormat, agg::amask_no_clip_gray8>			PixFormatAlpha;
	typedef agg::pixfmt_amask_adaptor<PixFormat_Comp, agg::amask_no_clip_gray8>		PixFormatAlpha_Comp;

	typedef agg::renderer_base<PixFormat>         RendererBase;
	typedef agg::renderer_base<PixFormat_Comp>    RendererBase_Comp;

	typedef agg::renderer_scanline_aa_solid<RendererBase>     Renderer_Solid;
	typedef agg::renderer_scanline_aa_solid<RendererBase_Comp> Renderer_SolidComp;


	typedef agg::renderer_base<PixFormatAlpha>         RendererBaseAlpha;
	typedef agg::renderer_base<PixFormatAlpha_Comp>    RendererBaseAlpha_Comp;

	typedef agg::renderer_scanline_aa_solid<RendererBaseAlpha>     RendererAlpha_Solid;
	typedef agg::renderer_scanline_aa_solid<RendererBaseAlpha_Comp> RendererAlpha_SolidComp;

	//  typedef agg::pixfmt_bgra32_pre													PixFormat_Pre;
	//	premultiplied alpha types... these were only used by image-to-image compositing?

	//  typedef agg::blender_rgba_pre<ColorType, ComponentOrder>         BlenderPre;
	//  typedef agg::comp_op_adaptor_rgba_pre<ColorType, ComponentOrder> BlenderCompPre;
	//	typedef agg::pixfmt_custom_blend_rgba<BlenderCompPre,agg::rendering_buffer>     PixFormat_CompPre;
	//	typedef agg::pixfmt_amask_adaptor<PixFormat_Pre, agg::amask_no_clip_gray8>		PixFormatAlpha_Pre;
	//	typedef agg::pixfmt_amask_adaptor<PixFormat_CompPre, agg::amask_no_clip_gray8>	PixFormatAlpha_CompPre;
	//	typedef agg::renderer_base<PixFormat_Pre>     RendererBase_Pre;
	//	typedef agg::renderer_base<PixFormat_CompPre> RendererBase_CompPre;
	//  typedef agg::renderer_base<PixFormatAlpha_Pre>     RendererBaseAlpha_Pre;
	//  typedef agg::renderer_base<PixFormatAlpha_CompPre> RendererBaseAlpha_CompPre;



	typedef agg::span_allocator<ColorType> SpanAllocator;
public:
	typedef agg::pod_auto_array<ColorType, 256> GradientArray;
private:
	typedef agg::span_gradient<ColorType, agg::span_interpolator_linear<>, agg::gradient_x,      GradientArray> LinearGradientSpan;
	typedef agg::span_gradient<ColorType, agg::span_interpolator_linear<>, agg::gradient_radial_focus, GradientArray> RadialGradientSpan;
#ifdef AGG2D_USE_FONTS

#ifdef AGG2D_USE_FREETYPE
	typedef agg::font_engine_freetype_int32       FontEngine;
#else

	typedef agg::font_engine_win32_tt_int32       FontEngine;
#endif
	typedef agg::font_cache_manager<FontEngine>   FontCacheManager;
	typedef FontCacheManager::gray8_adaptor_type  FontRasterizer;
	typedef FontCacheManager::gray8_scanline_type FontScanline;
#endif

	typedef agg::conv_curve<agg::path_storage>    ConvCurve;
	typedef agg::conv_stroke<ConvCurve>           ConvStroke;
	typedef agg::conv_transform<ConvCurve>        PathTransform;
	typedef agg::conv_transform<ConvStroke>       StrokeTransform;


public:
	enum Gradient
	{
		Solid,
		Linear,
		Radial
	};

	friend class Agg2DRenderer;

	typedef ColorType         Color;
	// JME
	//typedef agg::rect         Rect;
	typedef agg::rect_i       Rect;
	typedef agg::rect_d       RectD;
	typedef agg::trans_affine Affine;

	enum LineJoin
	{
		JoinMiter = agg::miter_join,
		JoinRound = agg::round_join,
		JoinBevel = agg::bevel_join
	};

	enum LineCap
	{
		CapButt   = agg::butt_cap,
		CapSquare = agg::square_cap,
		CapRound  = agg::round_cap
	};

	enum TextAlignment
	{
		AlignLeft,
		AlignRight,
		AlignCenter,
		AlignTop = AlignRight,
		AlignBottom = AlignLeft
	};


	enum DrawPathFlag
	{
		FillOnly,
		StrokeOnly,
		FillAndStroke,
		FillWithLineColor
	};

	enum ViewportOption
	{
		Anisotropic,
		XMinYMin,
		XMidYMin,
		XMaxYMin,
		XMinYMid,
		XMidYMid,
		XMaxYMid,
		XMinYMax,
		XMidYMax,
		XMaxYMax
	};

	struct Transformations
	{
		double affineMatrix[6];
	};


	struct Image
	{
		agg::rendering_buffer renBuf;

		Image() {}
		Image(unsigned char* buf, unsigned width, unsigned height, int stride) :
		renBuf(buf, width, height, stride) {}
		void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
		{
			renBuf.attach(buf, width, height, stride);
		}
		int width()  const { return renBuf.width(); }
		int height() const { return renBuf.height(); }
		void premultiply();
		void demultiply();
	};

	enum ImageFilter
	{
		NoFilter,
		Bilinear,
		Hanning,
		Hermite,
		Quadric,
		Bicubic,
		Catrom,
		Spline16,
		Spline36,
		Blackman144
	};

	enum ImageResample
	{
		NoResample,
		ResampleAlways,
		ResampleOnZoomOut
	};

	enum FontCacheType
	{
		RasterFontCache,
		VectorFontCache
	};

	enum BlendMode
	{
		BlendAlpha      = agg::end_of_comp_op_e,
		BlendClear      = agg::comp_op_clear,
		BlendSrc        = agg::comp_op_src,
		BlendDst        = agg::comp_op_dst,
		BlendSrcOver    = agg::comp_op_src_over,
		BlendDstOver    = agg::comp_op_dst_over,
		BlendSrcIn      = agg::comp_op_src_in,
		BlendDstIn      = agg::comp_op_dst_in,
		BlendSrcOut     = agg::comp_op_src_out,
		BlendDstOut     = agg::comp_op_dst_out,
		BlendSrcAtop    = agg::comp_op_src_atop,
		BlendDstAtop    = agg::comp_op_dst_atop,
		BlendXor        = agg::comp_op_xor,
		BlendAdd        = agg::comp_op_plus,
		BlendSub        = agg::comp_op_minus,
		BlendMultiply   = agg::comp_op_multiply,
		BlendScreen     = agg::comp_op_screen,
		BlendOverlay    = agg::comp_op_overlay,
		BlendDarken     = agg::comp_op_darken,
		BlendLighten    = agg::comp_op_lighten,
		BlendColorDodge = agg::comp_op_color_dodge,
		BlendColorBurn  = agg::comp_op_color_burn,
		BlendHardLight  = agg::comp_op_hard_light,
		BlendSoftLight  = agg::comp_op_soft_light,
		BlendDifference = agg::comp_op_difference,
		BlendExclusion  = agg::comp_op_exclusion,
		BlendContrast   = agg::comp_op_contrast
	};

	enum Direction
	{
		CW, CCW
	};

	~Agg2D();
	Agg2D();

	// Setup
	//-----------------------
	//	void  attachshadow(unsigned char* buf, unsigned width, unsigned height, int stride);
	void  attachalpha(unsigned char* buf, unsigned width, unsigned height, int stride);
	void  attach(unsigned char* buf, unsigned width, unsigned height, int stride);
	void  attach(Image& img);

	agg::rendering_buffer *GetAlphaBuffer();
	//	agg::rendering_buffer *GetShadowBuffer();

	void  clipBox(double x1, double y1, double x2, double y2);
	RectD clipBox() const;

	void  clearAll(Color c);
	void  clearAll(unsigned r, unsigned g, unsigned b, unsigned a = 255);

	void  clearClipBox(Color c);
	void  clearClipBox(unsigned r, unsigned g, unsigned b, unsigned a = 255);

	// Conversions
	//-----------------------
	void   worldToScreen(double& x, double& y) const;
	void   screenToWorld(double& x, double& y) const;
	double worldToScreen(double scalar) const;
	double screenToWorld(double scalar) const;
	void   alignPoint(double& x, double& y) const;
	bool   inBox(double worldX, double worldY) const;

	// General Attributes
	//-----------------------
	void blendMode(BlendMode m);
	BlendMode blendMode() const;

	void imageBlendMode(BlendMode m);
	BlendMode imageBlendMode() const;

	void imageBlendColor(Color c);
	void imageBlendColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
	Color imageBlendColor() const;

	void masterAlpha(double a);
	double masterAlpha() const;

	void antiAliasGamma(double g);
	double antiAliasGamma() const;

	void fillColor(Color c);
	void fillColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
	void noFill();

	void lineColor(Color c);
	void lineColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
	void noLine();

	Color fillColor() const;
	Color lineColor() const;

	void fillLinearGradient(double x1, double y1, double x2, double y2, Color c1, Color c2, double profile=1.0);
	void lineLinearGradient(double x1, double y1, double x2, double y2, Color c1, Color c2, double profile=1.0);

	void fillRadialGradient(double x, double y, double r, Color c1, Color c2, double profile=1.0);
	void lineRadialGradient(double x, double y, double r, Color c1, Color c2, double profile=1.0);

	void fillRadialGradient(double x, double y, double r, Color c1, Color c2, Color c3);
	void lineRadialGradient(double x, double y, double r, Color c1, Color c2, Color c3);

	void fillRadialGradient(double x, double y, double r);
	void lineRadialGradient(double x, double y, double r);

	void lineWidth(double w);
	double lineWidth(double w) const;

	void lineCap(LineCap cap);
	LineCap lineCap() const;

	void lineJoin(LineJoin join);
	LineJoin lineJoin() const;

	void fillEvenOdd(bool evenOddFlag);
	bool fillEvenOdd() const;

	// Transformations
	//-----------------------
	Transformations transformations() const;
	void transformations(const Transformations& tr);
	void resetTransformations();
	void affine(const Affine& tr);
	void affine(const Transformations& tr);
	void rotate(double angle);
	void scale(double sx, double sy);
	void skew(double sx, double sy);
	void translate(double x, double y);
	void parallelogram(double x1, double y1, double x2, double y2, const double* para);
	void viewport(double worldX1,  double worldY1,  double worldX2,  double worldY2,
		double screenX1, double screenY1, double screenX2, double screenY2,
		ViewportOption opt=XMidYMid);

	// Basic Shapes
	//-----------------------
	void line(double x1, double y1, double x2, double y2);
	void triangle(double x1, double y1, double x2, double y2, double x3, double y3);
	void rectangle(double x1, double y1, double x2, double y2);
	void roundedRect(double x1, double y1, double x2, double y2, double r);
	void roundedRect(double x1, double y1, double x2, double y2, double rx, double ry);
	void roundedRect(double x1, double y1, double x2, double y2,
		double rxBottom, double ryBottom,
		double rxTop,    double ryTop);
	void ellipse(double cx, double cy, double rx, double ry);
	void arc(double cx, double cy, double rx, double ry, double start, double sweep);
	void star(double cx, double cy, double r1, double r2, double startAngle, int numRays);
	void curve(double x1, double y1, double x2, double y2, double x3, double y3);
	void curve(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
	void polygon(double* xy, int numPoints);
	void polyline(double* xy, int numPoints);

	//#ifdef AGG2D_USE_FONTS
	// Text
	//-----------------------
	void   flipText(bool flip);
	void   font(const char* fileName, double height,
		bool bold = false,
		bool italic = false,
		FontCacheType ch = RasterFontCache,
		double angle = 0.0);
	double fontHeight() const;
	void   textAlignment(TextAlignment alignX, TextAlignment alignY);
	bool   textHints() const;
	void   textHints(bool hints);
	double textWidth(const char* str);
	void   text(double x, double y, const char* str, DrawPathFlag flag, bool roundOff=false, double dx=0.0, double dy=0.0);
	void   textpath(double x, double y, const char* str, bool roundOff=false, double dx=0.0, double dy=0.0);
	//#endif

	// Path commands
	//-----------------------
	void resetPath();

	void moveTo(double x, double y);
	void moveRel(double dx, double dy);

	void lineTo(double x, double y);
	void lineRel(double dx, double dy);

	void horLineTo(double x);
	void horLineRel(double dx);

	void verLineTo(double y);
	void verLineRel(double dy);

	void arcTo(double rx, double ry,
		double angle,
		bool largeArcFlag,
		bool sweepFlag,
		double x, double y);

	void arcRel(double rx, double ry,
		double angle,
		bool largeArcFlag,
		bool sweepFlag,
		double dx, double dy);

	void quadricCurveTo(double xCtrl, double yCtrl,
		double xTo,   double yTo);
	void quadricCurveRel(double dxCtrl, double dyCtrl,
		double dxTo,   double dyTo);
	void quadricCurveTo(double xTo, double yTo);
	void quadricCurveRel(double dxTo, double dyTo);

	void cubicCurveTo(double xCtrl1, double yCtrl1,
		double xCtrl2, double yCtrl2,
		double xTo,    double yTo);

	void cubicCurveRel(double dxCtrl1, double dyCtrl1,
		double dxCtrl2, double dyCtrl2,
		double dxTo,    double dyTo);

	void cubicCurveTo(double xCtrl2, double yCtrl2,
		double xTo,    double yTo);

	void cubicCurveRel(double xCtrl2, double yCtrl2,
		double xTo,    double yTo);

	void addEllipse(double cx, double cy, double rx, double ry, Direction dir);
	void closePolygon();

	void drawPath(DrawPathFlag flag = FillAndStroke, bool text = false);
	void drawPathNoTransform(DrawPathFlag flag = FillAndStroke);


	// Image Transformations
	//-----------------------
	void imageFilter(ImageFilter f);
	ImageFilter imageFilter() const;

	void imageResample(ImageResample f);
	ImageResample imageResample() const;

	void transformImage(const Image& img,
		int imgX1,    int imgY1,    int imgX2,    int imgY2,
		double dstX1, double dstY1, double dstX2, double dstY2);

	void transformImage(const Image& img,
		double dstX1, double dstY1, double dstX2, double dstY2);

	void transformImage(const Image& img,
		int imgX1, int imgY1, int imgX2, int imgY2,
		const double* parallelogram);

	void transformImage(const Image& img, const double* parallelogram);


	void transformImagePath(const Image& img,
		int imgX1,    int imgY1,    int imgX2,    int imgY2,
		double dstX1, double dstY1, double dstX2, double dstY2);

	void transformImagePath(const Image& img,
		double dstX1, double dstY1, double dstX2, double dstY2);

	void transformImagePath(const Image& img,
		int imgX1, int imgY1, int imgX2, int imgY2,
		const double* parallelogram);

	void transformImagePath(const Image& img, const double* parallelogram);


	// Image Blending (no transformations available)
	void blendImage(Image& img,
		int imgX1, int imgY1, int imgX2, int imgY2,
		double dstX, double dstY, unsigned alpha=255);
	void blendImage(Image& img, double dstX, double dstY, unsigned alpha=255);


	// Copy image directly, together with alpha-channel
	void copyImage(Image& img,
		int imgX1, int imgY1, int imgX2, int imgY2,
		double dstX, double dstY);
	void copyImage(Image& img, double dstX, double dstY);


	// Auxiliary
	//-----------------------
	static double pi() { return agg::pi; }
	static double deg2Rad(double v) { return v * agg::pi / 180.0; }
	static double rad2Deg(double v) { return v * 180.0 / agg::pi; }

	void EnableAlphaMask(bool enable = true) {m_alphamaskenabled = enable;};


private:
	void render(bool fillColor);

	//#ifdef AGG2D_USE_FONTS
	void render(FontRasterizer& ras, FontScanline& sl);
	//#endif

	void addLine(double x1, double y1, double x2, double y2);
	void updateRasterizerGamma();
public:
	void renderImage(const Image& img, int x1, int y1, int x2, int y2, const double* parl, bool keeprect = false);

	agg::rendering_buffer           m_rbuf;
	agg::rendering_buffer           m_rbuf_alpha;
	//    agg::rendering_buffer           m_rbuf_shadow;

	agg::amask_no_clip_gray8 m_alphamask;
	PixFormat                       m_pixFormat;
	PixFormat_Comp                  m_pixFormat_Comp;
private:
	//PixFormat_Pre                   m_pixFormat_Pre;
	//PixFormat_CompPre               m_pixFormat_CompPre;

	PixFormatAlpha                       m_pixFormatAlpha;
	PixFormatAlpha_Comp                  m_pixFormatAlpha_Comp;
	//    PixFormatAlpha_Pre                   m_pixFormatAlpha_Pre;
	//   PixFormatAlpha_CompPre               m_pixFormatAlpha_CompPre;

	RendererBase                    m_renBase;
	RendererBase_Comp               m_renBase_Comp;
	//    RendererBase_Pre                m_renBase_Pre;
	//RendererBase_CompPre            m_renBase_CompPre;
	Renderer_Solid                  m_ren_Solid;
	Renderer_SolidComp              m_ren_SolidComp;

	RendererBaseAlpha                    m_renBaseAlpha;
	RendererBaseAlpha_Comp               m_renBaseAlpha_Comp;
	//    RendererBaseAlpha_Pre                m_renBaseAlpha_Pre;
	//RendererBaseAlpha_CompPre            m_renBaseAlpha_CompPre;
	RendererAlpha_Solid				     m_renAlpha_Solid;
	RendererAlpha_SolidComp              m_renAlpha_SolidComp;


	SpanAllocator                   m_allocator;
	RectD                           m_clipBox;

	bool m_alphamaskenabled;
	void BuildAlphaMaskFromPath();

	BlendMode                       m_blendMode;
	BlendMode                       m_imageBlendMode;
	Color                           m_imageBlendColor;

	agg::scanline_p8                m_scanline;
	agg::rasterizer_scanline_aa<>   m_rasterizer;

	double                          m_masterAlpha;
	double                          m_antiAliasGamma;

	Color                           m_fillColor;
	Color                           m_lineColor;
public:
	GradientArray                   m_fillGradient;
	GradientArray                   m_lineGradient;

	LineCap                         m_lineCap;
	LineJoin                        m_lineJoin;

	// sorry for the public mod.. all the relevant bits need to be moved to cCanvas later on anyway.
	Gradient                        m_fillGradientFlag;
	Gradient                        m_lineGradientFlag;
	agg::trans_affine               m_fillGradientMatrix;
	agg::trans_affine               m_lineGradientMatrix;
	double                          m_fillGradientD1;
	double                          m_lineGradientD1;
	double                          m_fillGradientD2;
	double                          m_lineGradientD2;

private:
	double                          m_textAngle;
	TextAlignment                   m_textAlignX;
	TextAlignment                   m_textAlignY;
	bool                            m_textHints;

	double                          m_fontHeight;
	double                          m_fontAscent;
	double                          m_fontDescent;
	FontCacheType                   m_fontCacheType;

	ImageFilter                     m_imageFilter;
	ImageResample                   m_imageResample;
	agg::image_filter_lut           m_imageFilterLut;

	agg::span_interpolator_linear<> m_fillGradientInterpolator;
	agg::span_interpolator_linear<> m_lineGradientInterpolator;

	agg::gradient_x                 m_linearGradientFunction;
public:
	agg::gradient_radial_focus      m_radialGradientFunction;
private:
	double                          m_lineWidth;
	bool                            m_evenOddFlag;

public:
	agg::path_storage               m_path;
	agg::path_storage               m_textpath;
	agg::trans_affine               m_transform;
	ConvCurve                       m_convCurve;
	ConvStroke                      m_convStroke;
	ConvCurve                       m_convCurveText;
	ConvStroke                      m_convStrokeText;

	PathTransform                   m_pathTransformText;
	PathTransform                   m_pathTransform;
	StrokeTransform                 m_strokeTransformText;
	StrokeTransform                 m_strokeTransform;
private:
	//#ifdef AGG2D_USE_FONTS

#ifndef AGG2D_USE_FREETYPE
	HDC                             m_fontDC;
#endif

public:
	FontEngine                      m_fontEngine;
	FontCacheManager                m_fontCacheManager;
	//#endif
};


inline bool operator == (const Agg2D::Color& c1, const Agg2D::Color& c2)
{
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

inline bool operator != (const Agg2D::Color& c1, const Agg2D::Color& c2)
{
	return !(c1 == c2);
}


#endif


