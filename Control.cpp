/********************************************
  TControl
                          M.K
*********************************************/
#include <Control.h>

TControlManager *gcm;
/********************************************
  TControlManager process event
*********************************************/
TControlManager::TControlManager(long *ms,void *owner)
{
	Owner=owner;
	msec=ms;
	ts = new Adafruit_STMPE610(STMPE_CS);
	tft = new Adafruit_ILI9341(TFT_CS, TFT_DC);
	tft->begin();
	ts_state=ts->begin();
	if( !ts_state ) {
		return;
	}
	tft->fillScreen(ILI9341_BLACK);
	tft->setRotation(1); 
}

void TControlManager::Clock()
{
	if (!ts->bufferEmpty()) {   
     
    	TS_Point p = ts->getPoint(); // Retrieve a point 
    // Scale using the calibration #'s
    // and rotate coordinate system
    	p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft->height());
    	p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft->width());
    	int y = tft->height() - p.x;
    	int x = p.y;
		for( int c=0;c<Controls.Count;c++) {
			TControl *Control=(TControl *)Controls.Items[c];
      		if((x > Control->X) && (x < ( Control->X +  Control->Width))) {
        		if ((y > Control->Y) && (y <= (Control->Y + Control->Height))) {
          			if( Control->OnClick ) {
						Control->OnClick(Control,0);
					}
        		}
      		}
		}
		for(;!ts->bufferEmpty();) {
		  ts->getPoint(); // Retrieve a point
		}
	}
}
/********************************************
  TList
*********************************************/
TList::TList()
{
	Count=0;
	Capacity=4;
	Items=(void**)malloc(sizeof(void*)*Capacity);
}

void TList::Add(void *item)
{
	if( Capacity<=Count ) {
		Capacity*=2;
		void **Items0=Items;
		Items=(void**)malloc(sizeof(void*)*Capacity);
		memcpy(Items,Items0,sizeof(void*)*Count);
		free(Items0);
	}
	Items[Count++]=item;
}

/********************************************
  TControl
*********************************************/
void TControl::Init(TControl *owner)
{
	this->Owner=owner;
	X=Y=0;
	Width=80; Height=28;
	BorderColor=ILI9341_LIGHTGREY;
	Color=ILI9341_BLACK;
	Text=NULL;
	FontSize=2;
	FontColor=ILI9341_LIGHTGREY;
	OnClick=NULL;
	Tag=0;
	gcm->Controls.Add(this);
}
TControl::TControl(TControl *owner)
{
	Init(owner);
}
TControl::TControl(int text_size,TControl *owner)
{
	Init(owner);
	TextSize=text_size;
	Text=(char*)malloc(text_size);
	Text[0]=0;
}
TControl::TControl(char *text,TControl *owner)
{
	Init(owner);
	this->Text=text;
}

TControl::~TControl()
{

}

void TControl::Draw()
{
	gcm->tft->fillRect(X+1, Y+1, Width-2, Height-2, Color);
	if( BorderColor!=Color ) {
		gcm->tft->drawRect(X, Y, Width, Height, BorderColor);
	}
	if( Text ) {
		gcm->tft->setCursor(X + 6 , Y + 6);
		gcm->tft->setTextColor(FontColor);
		gcm->tft->setTextSize(FontSize);
		gcm->tft->println(Text);
	}
}
void TControl::DrawCaption(int off_x,int off_y,char *caption) //(X,Y)からオフセットした位置にcaption文字列を描画
{
	gcm->tft->setCursor(X+off_x , Y + off_y); 
	gcm->tft->setTextColor(FontColor); 
	gcm->tft->setTextSize(1); 
	gcm->tft->println(caption); //説明ラベル

}

TButton::TButton(TControl *owner):TControl(owner)
{
}
TButton::TButton(char *text,TControl *owner):TControl(owner)
{
	Text=text;
}
TButton::TButton(int text_size,TControl *owner):TControl(text_size,owner)
{

}

/****************************************************************************
TR50共通日付け時間レコード定義 は以下のrCommonDateTimeの８バイト構造体とする
 [19][99]年[12]月[31]日[23]時[59]分[59]秒[99]x10msec
*****************************************************************************/
//************** 年月日時分秒整数を共通日付けに変換******************
rCommonDateTime  ToCommonDateTime(int year,int month,int day,int hour,int min,int sec,int m10sec)
{   rCommonDateTime Result;

  Result.Dt =ToCommonDate(year,month,day);
  Result.Tm =ToCommonTime(hour,min,sec,m10sec);

 return Result;
};
//************** 年月日整数を共通日付けに変換******************
rCommonTime  ToCommonTime(int hour,int min,int sec,int m10sec)
{   rCommonTime Result;

  Result.byHour   =((unsigned char)(hour));
  Result.byMin    =((unsigned char)(min));
  Result.bySec    =((unsigned char)(sec));
  Result.by10mSec =((unsigned char)(m10sec));

 return Result;
};
//************** 年月日整数を共通日付けに変換******************
rCommonDate  ToCommonDate(int year,int month,int day)
{   rCommonDate Result;

  Result.by100Year =((unsigned char)(year / 100));
  Result.byYear    =((unsigned char)(year % 100));
  Result.byMonth   =((unsigned char)(month));
  Result.byDay     =((unsigned char)(day));

 return Result;
};
//************** 共通日付けを年月日整数に分解******************
void  CommonDateTo(rCommonDate dates,int &year,int &month,int &day)
{ 
  year  =((int)(dates.by100Year*100+dates.byYear));
  month =((int)(dates.byMonth));
  day   =((int)(dates.byDay));
};
//************** 共通時間を時間整数に変換******************
void  CommonTimeto(rCommonTime times,int &hour,int &min,int &sec,int &m10sec)
{ 
  hour   =((int)(times.byHour));
  min    =((int)(times.byMin));
  sec    =((int)(times.bySec));
  m10sec =((int)(times.by10mSec));
};
//************** 共通日時を整数に変換******************
void  CommonDateTimeTo(rCommonDateTime datetimes,int &year,int &month,int &day,int &hour,int &min,int &sec,int &m10sec)
{ 
  CommonDateTo(datetimes.Dt, year,month,day);
  CommonTimeto(datetimes.Tm, hour,min,sec,m10sec);
};
/******************************************************
  共通時間へのミリ秒加算（減算）
  オーバーアンダーフローは切り捨てられる（日付は変わらない）
*******************************************************/
void IncCommonTimeMSec(rCommonTime &t,long msec)//共通時間へのミリ秒加算（減算）
{ long ms,s,m;
  ms=msec+(t.by10mSec*10)+1000*(t.bySec+60*(t.byMin+60*t.byHour));
  t.by10mSec=((unsigned char)((ms % 1000) / 10)); //msec
  s=ms / 1000;
  t.bySec=((unsigned char)(s % 60));
  m=s / 60;
  t.byMin=((unsigned char)(m % 60));
  t.byHour=((unsigned char)(m / 60));
};
/******************************************************
  通算秒を日、時間、分、秒に変換
*******************************************************/
void SecToDayHourMin(long sec,int *d,int *h,int *m,int *s)//通算秒を日、時間、分、秒に変換
{
	*s=sec%60;
	long tm=sec/60; 
	*m=tm%60;
	long th=tm/60;
	*h=th%60;
	*d=th/24;
};
//************** 共通時間に加算******************
rCommonTime  CommonTimeAdd(rCommonTime t,int hour,int min,int sec,int m10sec)//加算
{  int h,m,s,m10s;
  rCommonTime Result;

  m10s=t.by10mSec+m10sec;
  if( 99<m10s)   {
    s=m10s / 100;
    m10s=m10s % 100;
  }  else  s=0;
  s=s+t.bySec+sec;
  if( 59<s)   {
    m=s / 60;
    s=s % 60;
  }  else  m=0;
  m=m+t.byMin+min;
  if( 59<m)   {
    h=m / 60;
    m=m % 60;
  }  else  h=0;
  h=h+t.byHour+hour;

  Result=ToCommonTime(h,m,s,m10s);


 return Result;
};
//************** 共通日比較 t1<t2:1 t1=t2:0 t1>t2:-1******************
int  CommonDateCompare(rCommonDate t1,rCommonDate t2)//比較
{   int Result;

  if( t1.by100Year<t2.by100Year)   { Result=1; return Result; }
   else  if( t1.by100Year>t2.by100Year)   { Result=-1; return Result; }
   else  if( t1.byYear<t2.byYear)   { Result=1; return Result; }
   else  if( t1.byYear>t2.byYear)   { Result=-1; return Result; }
   else  if( t1.byMonth<t2.byMonth)   { Result=1; return Result; }
   else  if( t1.byMonth>t2.byMonth)   { Result=-1; return Result; }
   else  if( t1.byDay<t2.byDay)   { Result=1; return Result; }
   else  if( t1.byDay>t2.byDay)   { Result=-1; return Result; };
  Result=0;

 return Result;
};

//************** 共通時間比較 t1<t2:1 t1=t2:0 t1>t2:-1******************
int  CommonTimeCompare(rCommonTime t1,rCommonTime t2)//比較
{   int Result;

  if( t1.byHour<t2.byHour)   { Result=1; return Result; }
   else  if( t1.byHour>t2.byHour)   { Result=-1; return Result; }
   else  if( t1.byMin<t2.byMin)   { Result=1; return Result; }
   else  if( t1.byMin>t2.byMin)   { Result=-1; return Result; }
   else  if( t1.bySec<t2.bySec)   { Result=1; return Result; }
   else  if( t1.bySec>t2.bySec)   { Result=-1; return Result; }
   else  if( t1.by10mSec<t2.by10mSec)   { Result=1; return Result; }
   else  if( t1.by10mSec>t2.by10mSec)   { Result=-1; return Result; };
  Result=0;

 return Result;
};

//************** 共通日付時間比較 t1<t2:1 t1=t2:0 t1>t2:-1******************
int  CommonDateTimeCompare(rCommonDateTime t1,rCommonDateTime t2)//比較
{   int Result;

  Result=CommonDateCompare(t1.Dt,t2.Dt);
  if( Result==0)   Result=CommonTimeCompare(t1.Tm,t2.Tm);

 return Result;
};

//時間インクリメント
void  IncCommonDateHour(int &y,int &m,int &d,int &h)
{ 
  h++;
  if( 23<h)   {
    h=9;
    d++;
    if( 31<d)   {
      d=1;
      m++;
      if( 12<m)   {
        m=1;
        y++;
      };
    };
  };
}
//************** 共通時を文字列に変換******************
void CommonTimeToStr(rCommonTime t,char *buf)//時間を文字列に変換
{

  sprintf(buf,"%.2d:%.2d:%.2d:%.3d",t.byHour,t.byMin,t.bySec,t.by10mSec*10);
};
//************** 共通日を文字列に変換（区切り文字/）******************
void CommonDateToStr(rCommonDate dates,char *buf)
{

  sprintf(buf,"%.4d/%.2d/%.2d",dates.by100Year*100+dates.byYear,
                                 dates.byMonth,
                                 dates.byDay);

};
//************** 共通時を文字列に変換(区切り無し)******************
void CommonTimeToStr1(rCommonTime t,char *buf)//時間を(区切り無し)文字列に変換
{

  sprintf(buf,"%.2d%.2d%.2d%.3d",t.byHour,t.byMin,t.bySec,t.by10mSec*10);
}
//************** 共通日時を文字列に変換(区切り無し)******************
void CommonDateTimeToStr1(rCommonDateTime d,char *buf)//日付時間を(区切り無し)文字列に変換
{

    sprintf(buf,"%.4d%.2d%.2d%.2d%.2d%.2d%.3d",d.Dt.by100Year*100+d.Dt.byYear,
                                   d.Dt.byMonth,
                                   d.Dt.byDay,
                                   d.Tm.byHour,d.Tm.byMin,d.Tm.bySec,d.Tm.by10mSec*10);

}
//************** 共通日を文字列に変換（区切り文字無し版）******************
void CommonDateToStr1(rCommonDate dates,char *buf) //区切り文字無し
{
  sprintf(buf,"%.4d%.2d%.2d",dates.by100Year*100+dates.byYear,
                                 dates.byMonth,
                                 dates.byDay);
};

