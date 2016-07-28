#ifndef CONTROL_H
#define CONTROL_H
/********************************************
  Arduino graphics control
*********************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

#define STMPE_CS 8
#define TFT_CS 10
#define TFT_DC 9


class TControl;

/********************************************
  TList
*********************************************/
class TList {
  public:
	int Count;
	int Capacity;
	void **Items;
	TList();
	void Add(void *item);
};
/********************************************
  TControlManager
*********************************************/
typedef void (*TOnClick)(TControl *Self,int ev);
class TControlManager {
  public:
	void *Owner;
	long *msec;
	TList Controls;
	Adafruit_STMPE610 *ts;
	Adafruit_ILI9341 *tft;
	bool ts_state;
	TControlManager(long *ms,void *owner=NULL);
	void Clock();
};
extern TControlManager *gcm;

class TControl {
  public:
    TControl *Owner;
	int X,Y;
	int Width,Height;
	int BorderColor;
	int Color;
	int TextSize; //以下のText領域サイズ
	char *Text;
	char FontSize;
	int FontColor;
	int Tag;
	TOnClick OnClick;
    TControl(TControl *owner=NULL);
    TControl(int text_size,TControl *owner=NULL);
    TControl(char *text,TControl *owner=NULL);
	~TControl();
    void Init(TControl *owner);
	virtual void Draw();
	virtual void DrawCaption(int off_x,int off_y,char *caption); //(X,Y)からオフセットした位置にcaption文字列をFontColor,FontSizeで描画
};
class TButton:public TControl {
  public:
    TButton(TControl *owner=NULL);
	TButton(char *text,TControl *owner=NULL);
	TButton(int text_size,TControl *owner=NULL);
};

extern Adafruit_ILI9341 tft;
extern Adafruit_STMPE610 ts;
void ControlManager();

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000



/****************************************************************************
TR50共通日付け時間レコード定義 は以下のrCommonDateTimeの８バイト構造体とする
ただしDWordサイズのrCommonDate日付けとrCommonTime時間に分解して表現する

 [19][99]年[12]月[31]日[23]時[59]分[59]秒[99]x10msec
*****************************************************************************/

  struct rCommonDate {       //共通日付け型
    unsigned char by100Year;
    unsigned char byYear;
    unsigned char byMonth;
    unsigned char byDay;
  };
  struct rCommonTime {       //共通時間型
    unsigned char byHour;
    unsigned char byMin;
    unsigned char bySec;
    unsigned char by10mSec;
  };

  struct rCommonDateTime {   //共通日付け時間型
    rCommonDate Dt;        //日部分
    rCommonTime Tm;        //時間部分
  };
  rCommonTime  ToCommonTime(int hour,int min,int sec,int m10sec);
  rCommonDate  ToCommonDate(int year,int month,int day);
  rCommonDateTime  ToCommonDateTime(int year,int month,int day,int hour,int min,int sec,int m10sec);
  void  CommonDateTo(rCommonDate dates,int &year,int &month,int &day);
  void  CommonTimeTo(rCommonTime times,int &hour,int &min,int &sec,int &m10sec);
  void  CommonDateTimeTo(rCommonDateTime datetimes,int &year,int &month,int &day,int &hour,int &min,int &sec,int &m10sec);
  int  CommonDateCompare(rCommonDate t1,rCommonDate t2);//比較
  int  CommonTimeCompare(rCommonTime t1,rCommonTime t2);//比較
  int  CommonDateTimeCompare(rCommonDateTime t1,rCommonDateTime t2);//比較
  void  IncCommonDate(rCommonDate &d,int OffsetDays); //基準日＋日数の増減
  void  IncCommonDateHour(int &y,int &m,int &d,int &h);
  void  IncCommonSec(rCommonDateTime &d,int sec);//共通日時への秒加算（減算）
  void  IncCommonTimeMSec(rCommonTime &t,long msec);//共通時間へのミリ秒加算（減算）
  void CommonTimeToStr(rCommonTime t,char *buf);//時間を文字列に変換
  void CommonDateToStr(rCommonDate dates,char *buf);    //共通日付を文字列に変換区切り文字/
  void CommonDateTimeToStr1(rCommonDateTime d,char *buf); //日付時間を(区切り無し)文字列に変換
  void SecToDayHourMin(long sec,int *d,int *h,int *m,int *s); //通算秒を日、時間、分、秒に変換


#endif //CONTROL_H

