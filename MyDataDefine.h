#define  BORDER_WIDTH		1
#define  BORDER_COLOR		RGB(255, 0, 0)
#define  BK_COLOR			RGB(100, 100, 100)
#define  SNAPLINE_COLOR		RGB(0, 0, 0)
#define  TXTBG_COLOR		RGB(0, 0, 0)
#define  TXT_COLOR			RGB(255, 255, 255)

//#define  CROSS_LINE_OFFSET	0//10

#define  ERASE_THEN_DRAW	1
#define  ERASE_ONLY			2
#define  DRAW_ONLY			3

#define  WAVE_ENDSNAP		"endSnap.WAV"

#define  CAP_VIDEO_SWITCH	true//false
#define  CAP_VIDEO_FILE		"CapVideo.req"

#define  SLEEP_TIME			150

#define  GET_APP			((CMySnapApp *)AfxGetApp())

#define	 PRN_HEADER_POS		3.5 * 100
#define  PRN_BLK_UND_HDTXT	0.5 * 100
#define  PRN_BLK_UND_HDLN	2.0 * 100

#define  PRN_BLK_LEFT		6.0 * 100
#define  PRN_BLK_RIGHT		6.0 * 100
#define  PRN_BLK_BOTTOM		6.0  * 100

#define  MOUSE_SENSITIVITY	40
#define  ZOOMIN_REGION_SIZE	30

#define  STATUS_FREEMOVING	1		//move mouse only, ready to click left button to start capturing
#define	 STATUS_CAPTURING	2		//left button has been clicked, select the end point

#define  DRAW_TXT_OUT_X	100
#define  DRAW_TXT_OUT_Y	40			//标识当选择区域多大的时候在区域外面显示区域大小