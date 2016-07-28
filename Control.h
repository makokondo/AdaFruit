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
	int TextSize; //�ȉ���Text�̈�T�C�Y
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
	virtual void DrawCaption(int off_x,int off_y,char *caption); //(X,Y)����I�t�Z�b�g�����ʒu��caption�������FontColor,FontSize�ŕ`��
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
TR50���ʓ��t�����ԃ��R�[�h��` �͈ȉ���rCommonDateTime�̂W�o�C�g�\���̂Ƃ���
������DWord�T�C�Y��rCommonDate���t����rCommonTime���Ԃɕ������ĕ\������

 [19][99]�N[12]��[31]��[23]��[59]��[59]�b[99]x10msec
*****************************************************************************/

  struct rCommonDate {       //���ʓ��t���^
    unsigned char by100Year;
    unsigned char byYear;
    unsigned char byMonth;
    unsigned char byDay;
  };
  struct rCommonTime {       //���ʎ��Ԍ^
    unsigned char byHour;
    unsigned char byMin;
    unsigned char bySec;
    unsigned char by10mSec;
  };

  struct rCommonDateTime {   //���ʓ��t�����Ԍ^
    rCommonDate Dt;        //������
    rCommonTime Tm;        //���ԕ���
  };
  rCommonTime  ToCommonTime(int hour,int min,int sec,int m10sec);
  rCommonDate  ToCommonDate(int year,int month,int day);
  rCommonDateTime  ToCommonDateTime(int year,int month,int day,int hour,int min,int sec,int m10sec);
  void  CommonDateTo(rCommonDate dates,int &year,int &month,int &day);
  void  CommonTimeTo(rCommonTime times,int &hour,int &min,int &sec,int &m10sec);
  void  CommonDateTimeTo(rCommonDateTime datetimes,int &year,int &month,int &day,int &hour,int &min,int &sec,int &m10sec);
  int  CommonDateCompare(rCommonDate t1,rCommonDate t2);//��r
  int  CommonTimeCompare(rCommonTime t1,rCommonTime t2);//��r
  int  CommonDateTimeCompare(rCommonDateTime t1,rCommonDateTime t2);//��r
  void  IncCommonDate(rCommonDate &d,int OffsetDays); //����{�����̑���
  void  IncCommonDateHour(int &y,int &m,int &d,int &h);
  void  IncCommonSec(rCommonDateTime &d,int sec);//���ʓ����ւ̕b���Z�i���Z�j
  void  IncCommonTimeMSec(rCommonTime &t,long msec);//���ʎ��Ԃւ̃~���b���Z�i���Z�j
  void CommonTimeToStr(rCommonTime t,char *buf);//���Ԃ𕶎���ɕϊ�
  void CommonDateToStr(rCommonDate dates,char *buf);    //���ʓ��t�𕶎���ɕϊ���؂蕶��/
  void CommonDateTimeToStr1(rCommonDateTime d,char *buf); //���t���Ԃ�(��؂薳��)������ɕϊ�
  void SecToDayHourMin(long sec,int *d,int *h,int *m,int *s); //�ʎZ�b����A���ԁA���A�b�ɕϊ�


#endif //CONTROL_H

