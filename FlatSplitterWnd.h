#pragma once


// CFlatSplitterWnd

class CFlatSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CFlatSplitterWnd)

public:
	CFlatSplitterWnd();
	virtual ~CFlatSplitterWnd();

	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);
	virtual void RecalcLayout();    // call after changing sizes
	static void DeferClientPos(AFX_SIZEPARENTPARAMS* lpLayout,CWnd* pWnd, int x, int y, int cx, int cy, BOOL bScrollBar);
	static void LayoutRowCol(CSplitterWnd::CRowColInfo* pInfoArray,int nMax, int nSize, int nSizeSplitter);

protected:
	DECLARE_MESSAGE_MAP()
};


