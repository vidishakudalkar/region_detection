// kingimageView.cpp : implementation of the CKingimageView class
//

#include "stdafx.h"
#include "kingimage.h"
#include <iostream>
#include "kingimageDoc.h"
#include "kingimageView.h"
#include <fstream>
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CKingimageView

IMPLEMENT_DYNCREATE(CKingimageView, CScrollView)

BEGIN_MESSAGE_MAP(CKingimageView, CScrollView)
	//{{AFX_MSG_MAP(CKingimageView)
	ON_WM_MOUSEMOVE()
	
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_NEGATIVEIMAGE, &CKingimageView::OnNegativeimage)
	ON_COMMAND(ID_HISTOGRAMEQUALIZATION, &CKingimageView::OnHistogramequalization)
	ON_COMMAND(ID_REGIONDETECTION, &CKingimageView::OnRegiondetection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKingimageView construction/destruction
int get_Threshold(int[], int, int);
CKingimageView::CKingimageView()
{
	// TODO: add construction code here

}

CKingimageView::~CKingimageView()
{
}

BOOL CKingimageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView drawing

void CKingimageView::OnDraw(CDC* pDC)
{
	CKingimageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
//	pDC->SetStretchBltMode(COLORONCOLOR);
//	int x,y,bytes;
	if (pDoc->imagetype==PCX)
		(pDoc->_pcx)->draw(pDC);
	if (pDoc->imagetype==BMP)
		(pDoc->_bmp)->draw(pDC);
	if (pDoc->imagetype==GIF)
		(pDoc->_gif)->draw(pDC);
	if (pDoc->imagetype==JPG)
		(pDoc->_jpg)->draw(pDC);

}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView printing

BOOL CKingimageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKingimageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKingimageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView diagnostics

#ifdef _DEBUG
void CKingimageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CKingimageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CKingimageDoc* CKingimageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKingimageDoc)));
	return (CKingimageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKingimageView message handlers



void CKingimageView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CSize totalSize=CSize(::GetSystemMetrics(SM_CXSCREEN),
		                  ::GetSystemMetrics(SM_CYSCREEN));
	CSize pageSize=CSize(totalSize.cx/2,totalSize.cy/2);
	CSize lineSize=CSize(totalSize.cx/10,totalSize.cy/10);

	SetScrollSizes(MM_TEXT,totalSize,pageSize,lineSize);
}

void CKingimageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CScrollView::OnMouseMove(nFlags, point);
}





void CKingimageView::OnRegiondetection()
{
	CKingimageDoc* pDoc = GetDocument();


	int iBitPerPixel = pDoc->_bmp->bitsperpixel;
	int iWidth = pDoc->_bmp->width;
	int iHeight = pDoc->_bmp->height;
	BYTE *pImg = pDoc->_bmp->point;
	BYTE *pVal = pDoc->_bmp->point;
	BYTE  *poriginal = pDoc->_bmp->point;
	int Wp = iWidth;
	int fourconnectedmatrix[200000] = { 0 }; //----------------------------------
	int initial_components_label = 1;
	int unique_label[200] = { 0 };
	int minimum = 0;
	int maximum = 0;
	int threshold = 40;
	int modifiedthreshold = 0;
	int computedthreshold = 0;

	if (iBitPerPixel == 8)  ////Grey scale 8 bits image
	{
		int r = iWidth % 4;
		int p = (4 - r) % 4;
		Wp = iWidth + p;
	}
	else if (iBitPerPixel == 24)	// RGB image
	{
		int r = (3 * iWidth) % 4;
		int p = (4 - r) % 4;
		Wp = 3 * iWidth + p;
	}

	if (iBitPerPixel == 8)  ////Grey scale 8 bits image
	{

		int pixelcount[256] = { 0 };
		int pixelvalue = 0;
		for (int i = 0; i<iHeight; i++)
			for (int j = 0; j<iWidth; j++)
			{
				pixelvalue = int(pImg[i*Wp + j]);
				pixelcount[pixelvalue]++;
			}


		computedthreshold = get_Threshold(pixelcount, threshold, modifiedthreshold);

		std::ofstream ofs1;
		ofs1.open("finalthreshold.txt", std::ofstream::out | std::ofstream::app);
		ofs1 << computedthreshold << "\n";
		for (int i = 0; i<iHeight; i++)
			for (int j = 0; j<iWidth; j++)
			{
				if (int(pImg[i*Wp + j]) < computedthreshold) {
					pImg[i*Wp + j] = 0;
					pVal[i*Wp + j] = 0;
				}
				else
				{
					pImg[i*Wp + j] = 255;
					pVal[i*Wp + j] = 1;
				}
			}
		for (int i = 0; i < iHeight; i++)
			for (int j = 0; j < iWidth; j++)
			{
				if (int(pVal[i*Wp + j]) == 1)   //Checking if the pixel is foreground
				{


					if (i == 0 && j != 0)
					{
						if (pVal[i*Wp + j - 1] == pVal[i*Wp + j])
							fourconnectedmatrix[i*Wp + j] = fourconnectedmatrix[(i*Wp + j - 1)];
						else
						{
							fourconnectedmatrix[i*Wp + j] = initial_components_label;
							initial_components_label++;
						}

					}


					else if (i != 0 && j != 0 && pVal[i*Wp + j - 1] != pVal[i*Wp + j] && pVal[(i - 1)*Wp + j] != pVal[i*Wp + j])
					{
						fourconnectedmatrix[i*Wp + j] = initial_components_label;
						initial_components_label++;
					}

					else if (j != 0 && pVal[i*Wp + j - 1] == pVal[i*Wp + j])
					{
						if (pVal[(i - 1)*Wp + j] == pVal[i*Wp + j])
						{

							if (fourconnectedmatrix[(i*Wp + j - 1)] < fourconnectedmatrix[(i - 1)*Wp + j])
							{
								fourconnectedmatrix[i*Wp + j] = fourconnectedmatrix[(i*Wp + j - 1)];

							}
							else
							{
								fourconnectedmatrix[i*Wp + j] = fourconnectedmatrix[(i - 1)*Wp + j];

							}

							if (pVal[i*Wp + j - 1] == pVal[(i - 1)*Wp + j])
							{
								if (fourconnectedmatrix[(i*Wp + j - 1)] < fourconnectedmatrix[(i - 1)*Wp + j])//minimum left
								{
									minimum = fourconnectedmatrix[(i*Wp + j - 1)];
									maximum = fourconnectedmatrix[(i - 1)*Wp + j];
									for (int k = 0; k < i*Wp + j; k++)
									{
										if (fourconnectedmatrix[k] == maximum)
											fourconnectedmatrix[k] = minimum;
									}
								}
								else
								{
									minimum = fourconnectedmatrix[(i - 1)*Wp + j];
									maximum = fourconnectedmatrix[(i*Wp + j - 1)];
									for (int k = 0; k < i*Wp + j; k++)
									{
										if (fourconnectedmatrix[k] == maximum)
											fourconnectedmatrix[k] = minimum;
									}
								}
							}
						}

						else
						{
							fourconnectedmatrix[i*Wp + j] = fourconnectedmatrix[(i*Wp + j - 1)];
						}
					}


					else if (i != 0 && pVal[(i - 1)*Wp + j] == pVal[i*Wp + j])
					{
						fourconnectedmatrix[i*Wp + j] = fourconnectedmatrix[(i - 1)*Wp + j];
					}
					else if (i == 0 && j == 0) // First Pixel is 1 where i and j will be zero
					{
						fourconnectedmatrix[i*Wp + j] = initial_components_label;
						initial_components_label++;
					}
				}

			}

		int imagepixels[200000] = { 0 };
		int individual_components_frequency[100000] = { 0 };
		for (int i = 0; i < iHeight; i++)
			for (int j = 0; j < iWidth; j++)
			{
				individual_components_frequency[fourconnectedmatrix[i*Wp + j]]++;
				imagepixels[fourconnectedmatrix[i*Wp + j]] = fourconnectedmatrix[i*Wp + j]; 
			}
		int k = 0, m = 0;
		int finalcounts[20] = { 0 };
		int finalpixels[20] = { 0 };

		for (int i = 0; i < iHeight; i++)
			for (int j = 0; j < iWidth; j++)
			{

				if (individual_components_frequency[i*Wp + j] != 0 && imagepixels[i*Wp + j] != 0)
				{
					finalcounts[m] = individual_components_frequency[i*Wp + j];
					m++;
				}
				if (imagepixels[i*Wp + j] != 0)
				{
					finalpixels[k] = imagepixels[i*Wp + j];
					k++;

				}
			}


		int var1 = 0;
		int var2 = 0;
		for (int i = 0; i<k; i++)
		{
			for (int j = i + 1; j<k; j++)
			{
				if (finalcounts[i]<finalcounts[j])
				{
					var1 = finalcounts[i];
					var2 = finalpixels[i];
					finalcounts[i] = finalcounts[j];
					finalpixels[i] = finalpixels[j];
					finalcounts[j] = var1;
					finalpixels[j] = var2;
				}
			}
		}


		for (int i = 0; i < iHeight; i++)
			for (int j = 0; j < iWidth; j++)
			{

				if (fourconnectedmatrix[i*Wp + j] == 0)
					pImg[i*Wp + j] = 0;
				else if (fourconnectedmatrix[i*Wp + j] == finalpixels[0])
					pImg[i*Wp + j] = 200;
				else if (fourconnectedmatrix[i*Wp + j] == finalpixels[k - 1])
					pImg[i*Wp + j] = 60;
				else if (fourconnectedmatrix[i*Wp + j] == finalpixels[(k) / 2])
					pImg[i*Wp + j] = 120;
				else
					pImg[i*Wp + j] = 255;


			}



	}
	////redraw the screen
	OnDraw(GetDC());


}


int get_Threshold(int intensityfrequency[], int threshold, int newthreshold)
{
	if (newthreshold == threshold)
	{
		return threshold;
	}
	else
	{
		float region1 = 0, region2 = 0, region1mean = 0, region2mean = 0, region1sum = 0, region2sum = 0;

		for (int i = 0; i < threshold; i++)
			region1 += intensityfrequency[i];

		for (int i = int(threshold + 1); i <= 255; i++)
			region2 += intensityfrequency[i];

		for (int i = 0; i < threshold; i++)
			region1sum += i*intensityfrequency[i];

		region1mean = region1sum / region1;

		for (int i = int(threshold + 1); i <= 255; i++)
			region2sum += i * intensityfrequency[i];

		region2mean = region2sum / region2;

		newthreshold = (region1mean + region2mean) / 2;

		if (newthreshold == threshold)
			return threshold;

		else
		{
			threshold = newthreshold;
			newthreshold = 0;
		}

		return get_Threshold(intensityfrequency, threshold, newthreshold);
	}
}