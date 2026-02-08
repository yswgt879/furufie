/*
  Program Furu¥Fie
  File    furufie.c
  Create  2024/06/04
  Update  2024/06/17
  Version 0.9.2.0
  By      Kenoh
*/
#include "extlib.h"
#define MAX_X 18
#define MAX_Y 6
#define MAX_STAGE 20
#define C_BLUNK 0 /* ¸³Ê¸ */
#define C_WALL 1 /* ¶ÍÞ */
#define C_UP 2 /* ³´ */
#define C_DOWN 3 /* ¼À */
#define C_LEFT 4 /* ËÀÞØ */
#define C_RIGHT 5 /* Ð·Þ */
#define C_FOOD 6 /* ÀÍÞÓÉ */
#define C_PLAYER 15 /* ÌßÚ²Ô° */
#define S_CLEAR 1 /* ½Ã°¼Þ¸Ø± */
#define S_MISS 2 /* Ð½ */
#define S_OVER 3 /* ¹Þ°Ñµ°ÊÞ° */
#define S_END 4 /* ´ÝÃÞ¨Ý¸Þ */
#define ON "ON "
#define OFF "OFF"
/*
const char *C_player[]={
  "92FF7B7BFF92",
  "0AFF7B7FFFA2",
  "A2FF7F7BFF0A",
  "8AFF7F7FFF8A",
};
*/
const char *C_dest[]={
  "54AA55AA552A",
  "244912810452",
  "240049000012",
};
const char *C_map[]={
  "000000000000",
  "777077770777",
  "76F38181F376",
  "6ECF8181CF6E",
  "66C381E7E77E",
  "7EE7E781C366",
  "6060300DC7F8",
  "0C7AD6AB760C",
  "60E0F0793F0D",
  "387CFFF57C38",
  "78848C8E8478",
  "E0F87C7D3F19",
  "B8C47E7EC4B8",
  "1875ABD76D18",
  "78D5AFD5AC78",
};
char *C_player[]={
  "92FF7B7BFF92",
  "0AFF7B7FFFA2",
  "A2FF7F7BFF0A",
  "8AFF7F7FFF8A",
};
char C_etc[13];
/*
char C_dest[3][13];
char C_map[15][13];
*/
const int adr_s[]={
  0x00fb, /* »³ÝÄÞOFF/ON */
  0x00fc, /* ÏÁ¼Þ¶Ý */
  0x00fd,0x00fe,0x00ff /* Ê²½º± */
};
const int adr_d[]={
  0x0100,0x0118,0x012a,0x0130, /* ÄÞ¯Ä´ ÃÞ°À */
  0x01a0,0x01b2,0x01c4,0x01d6,0x01e2, /* »³ÝÄÞ ÃÞ°À */
  0x0200 /* ½Ã°¼Þ ÃÞ°À */
};
unsigned char Stage=1;
int Player;
unsigned long Score=0;
unsigned long HiScore=0;
unsigned int Step;
char Food;
char Sound=0;
int Waittime;
/* ÃÞÊÞ¯¸ÞÖ³
gotoxy(0,0);
printf("w=%d x=%d x1=%d y=%d y1=%d m=%d",w,x,*x1,y,*y1,*(map+w));
getch();
breakpt();
*/
/* ÊÞ²ÅØ ¶× ASCIIº°ÄÞ Í ÍÝ¶Ý */
unsigned char hexToAsc(unsigned char bin) {
  if(bin<=0x09) {
    bin+=0x30;
  } else {
    bin+=0x37;
  }
  return bin;
}
/* ¼®·¾¯Ã² */
void init() {
  int p,p1;
  unsigned char m;
/*
  for(p1=0;p1<6;p1++) {
gotoxy(0,0);
printf("adr=%d     ",*(adr_d+p1*6));
getch();
    for(p=0;p<6;p++) {
      m=peek(*(adr_d+p1*6)+p);
      *(C_player+p1*6+p*2)=hexToAsc((m&0xf0)>>4);
      *(C_player+p1*6+p*2+1)=hexToAsc(m&0x0f);
    }
  }
*/
  for(p=0;p<6;p++) {
    m=peek(*(adr_d+2)+p);
    *(C_etc+p*2)=hexToAsc((m&0xf0)>>4);
    *(C_etc+p*2+1)=hexToAsc(m&0x0f);
  }
}
/* ½Ã°¼ÞÃÞ°À ÖÐºÐ */
void stageRead(char *map,unsigned char stage) {
  int p;
  int i=0;
  unsigned char m;
  for(p=(stage-1)*(MAX_X*MAX_Y)/2;p<stage*(MAX_X*MAX_Y)/2;p++) {
    /* ½Ã°¼ÞÃÞ°À ¦ ÒÓØ ¶× Ì¸¹ÞÝ */
    m=peek(*(adr_d+9)+p);
    *(map+i*2)=(m&0xf0)>>4;
    *(map+i*2+1)=m&0x0f;
    i++;
  }
}
/* ¶ÞÒÝ ÊÝ² Áª¯¸ */
char range_chk(int x,int y) {
  char clrSw;
  clrSw=x<0||x>=MAX_X||y<0||y>=MAX_Y;
  return clrSw;
}
/* Ï¯ÌßÁ¯Ìß ËÞ®³¶Þ */
void map_put(double x,double y,char chip) {
  gcursor(x*6,y*8+8-1);
  gprint(C_map[chip]);
}
/* Ï¯Ìß ËÞ®³¶Þ */
void map_draw(char *map,int mline,int mlen,int *px,int *py) {
  int x,y;
  char w;
  Food=0;
  for(y=0;y<mline;y++) {
    for(x=0;x<mlen;x++) {
      w=(char)*(map+y*mlen+x);
      if(w==C_PLAYER) {
        gcursor(x*6,(y+1)*8-1);
        gprint(*C_player);
        *(map+y*mlen+x)=C_BLUNK;
        *px=x;
        *py=y;
      } else {
        if(w>=C_FOOD)
          Food++;
        map_put((double)x,(double)y,w);
      }
      if(Sound)
        beep(200,5,1);
    }
  }
  gotoxy(19,0);
  printf("STAGE");
  gotoxy(21,1);
  printf("%02d",Stage);
  gotoxy(19,2);
  printf("STEPS");
  gotoxy(19,3);
  printf("%4d",Step);
  gotoxy(20,4);
  printf("REM");
  for(w=Player-1;w>0;w--) {
    gcursor(23*6-w*6,5*8+8-1);
    gprint(*C_player);
  }
}
/* ½Ã°À½ ËÞ®³¶Þ */
void status_draw() {
  Step++;
  gotoxy(20,3);
  printf("%3d",Step);
}
/* ½Ìß×²Ä ²ÄÞ³ ¼®Ø */
void sprite_move(int x,int y,int x1,int y1,int step,char **obj,unsigned char pattern,unsigned char *wk,unsigned char wkpattern,char *map,int mlen,unsigned char *bgm,int bwait,int *bp) {
  int i,i1,wx,wy;
  i1=(6*(x1!=0)+8*(y1!=0))/step;
  for(i=1;i<=i1;i++) {
    map_put((double)x,(double)y,(int)*(map+y*mlen+x));
    wx=x*6+i*step*x1;
    wy=y*8+8+i*step*y1;
    gcursor(wx,wy-1);
    gprint(obj[*wk+pattern]);
    *wk=(++*wk)%wkpattern;
    if(x1==-1) {
      wx+=i1;
    } else if(x1==1) {
      wx-=1;
    } else if(y1==-1) {
      wy+=1;
    } else if(y1==1) {
      wy-=i1;
    }
    if(Sound) {
      bwait+=(bwait<1);
      beep(*(bgm+*bp),bwait,1);
      *bp=(++*bp)%(sizeof(bgm));
    } else {
        wait(Waittime/i1);
    }
  }
}
/* ÌßÚ²Ô° ²ÄÞ³ ¼®Ø */
void player_move(int *x,int *y,int x1,int y1,char *map,int mline,int mlen) {
  static unsigned char wk=0;
  static int bp=0;
/*  const unsigned char bgm[4]={219,143,172,194};*/
  const unsigned char bgm[4]={194,127,152,172};
  sprite_move(*x,*y,x1,y1,1,C_player,y1==-1,&wk,3,map,mlen,bgm,Waittime/50,&bp);
  *x+=x1;
  *y+=y1;
  status_draw();
}
/* ÌÞÛ¯¸ ²ÄÞ³ ¼®Ø */
void block_move(int x,int y,char *map,int mlen) {
  int x1,y1;
  int wx,wy;
  int w;
  static unsigned char wk=0;
  static int bp=0;
  const unsigned char bgm[5]={143,172,219,194,232};
  /* ÌÞÛ¯¸ É Î³º³ Í ²ÄÞ³ */
  w=*(map+y*mlen+x);
  while(1) {
    x1=-(w==C_LEFT)+(w==C_RIGHT);
    y1=-(w==C_UP)+(w==C_DOWN);
    wx=x+x1;
    wy=y+y1;
    /* ÊÝ²¶Þ²¥Ñº³ÅÎ³º³¥¸³Ê¸²¶Þ² ÏÃÞ ÌÞÛ¯¸ ²ÄÞ³ */
    if(range_chk(wx,wy)) {
      break;
    } else if(*(map+wy*mlen+wx)!=C_BLUNK) {
      break;
    }
    *(map+wy*mlen+wx)=w;
    *(map+y*mlen+x)=C_BLUNK;
    sprite_move(x,y,x1,y1,2,C_map,w,&wk,1,map,mlen,bgm,Waittime/100,&bp);
    x=wx;
    y=wy;
  }
}
/* ÌÞÛ¯¸ Ê¶² ¼®Ø */
void block_broke(int x,int y,char *map,int mlen) {
  int i;
  static int bp=0;
  const unsigned char bgm[5]={143,172,219,194,232};
  /* ÌÞÛ¯¸ Ê¶² ±ÆÒ°¼®Ý */
  *(map+y*mlen+x)=C_BLUNK;
  for(i=0;i<30;i++) {
    gcursor(x*6,y*8+8-1);
    gprint(C_dest[i/10]);
    if(Sound) {
      beep(*(bgm+bp),Waittime/100,1);
      bp=(++bp)%(sizeof(bgm));
    } else {
        wait(Waittime/10);
    }
  }
  map_put((double)x,(double)y,C_BLUNK);
}
/* ÌßÚ²Ô° ²ÄÞ³ Áª¯¸ */
void player_move_chk(int key,int x,int y,int *x1,int *y1,int *x2,int *y2,char *map,int mline,int mlen) {
  int wx,wy;
  char *mp;
  /* ·°¿³»¥²ÄÞ³ÊÝ² Áª¯¸ */
  *x1=-(key==0x34)+(key==0x36);
  *y1=-(key==0x38)+(key==0x32);
  wx=x+*x1;
  *x1*=(wx>=0)&&(wx<mlen);
  wy=y+*y1;
  *y1*=(wy>=0)&&(wy<mline);
  /* Ñ· ¦ Î¿ÞÝ */
  if(*x1||*y1) {
    gcursor(x*6,y*8+8-1);
    gprint(C_player[(*x1==-1)+(2*(*x1==1))+(3*(*y1==-1))]);
    *x2=*x1;
    *y2=*y1;
  }
  /* ²ÄÞ³»· Áª¯¸ */
  mp=map+y*mlen+x;
  if(*x1||*y1) {
    mp=map+wy*mlen+wx;
    switch(*mp) {
      case C_WALL:
        *x1=0;
        *y1=0;
        break;
      case C_UP:
      case C_DOWN:
      case C_LEFT:
      case C_RIGHT:
        *x1=0;
        *y1=0;
        break;
      default:
        if(*mp>=C_FOOD) {
          Food--;
          Score+=(unsigned long)*mp*100;
          *mp=C_BLUNK;
          if(Sound) {
            beep(232,15,1);
            beep(127,15,1);
          }
        }
        break;
    }
  }
}
/* ÌÞÛ¯¸ ²ÄÞ³»· Áª¯¸ */
char block_move_chk(int x,int y,int x1,int y1,char *map,int mlen) {
  char moveChk;
  char brokeChk;
  int wx;
  int wy;
  char w=(char)*(map+(y+y1)*mlen+(x+x1));
  char w1;
  /* ²ÄÞ³ Áª¯¸ : Ô¼ÞÙ¼ É Ñ· Ä Ñ¶²±Ü¾ Ê Ñº³ ((Ú²)P<¥¥¥NG P>¥¥¥OK P^¥¥¥OK) */
  moveChk=(w==C_UP&&y1!=1)+(w==C_DOWN&&y1!=-1)+(w==C_LEFT&&x1!=1)+(w==C_RIGHT&&x1!=-1);
  /* Ê¶² Áª¯¸ : Ô¼ÞÙ¼ É Ñ· É ³¼Û¶×µ¾ÊÞ Õ³º³ ((Ú²)P>¥¥¥OK P<¥¥¥NG P^¥¥¥NG) */
  brokeChk=(w==C_UP&&y1==-1)+(w==C_DOWN&&y1==1)+(w==C_LEFT&&x1==-1)+(w==C_RIGHT&&x1==1);
  if(brokeChk) {
    wx=x+x1+(w==C_RIGHT)-(w==C_LEFT);
    wy=y+y1+(w==C_DOWN)-(w==C_UP);
    if(range_chk(wx,wy)==NULL) {
      /* ²ÄÞ³»· ¶Þ Ô¼ÞÙ¼ Ä Ñ¶²±Ü¾¥¸³Ê¸ Ê Ñº³ ((Ú²)><¥¥¥NG >^¥¥¥OK >F¥¥¥OK >B¥¥¥NG) */
      w1=(char)*(map+wy*mlen+wx);
      brokeChk=(w1!=C_BLUNK)&&((w==C_UP&&w1!=C_DOWN)+(w==C_DOWN&&w1!=C_UP)+(w==C_LEFT&&w1!=C_RIGHT)+(w==C_RIGHT&&w1!=C_LEFT));
    }
    moveChk+=brokeChk;
  }
  return moveChk;
}
/* ¼Þ®³·®³ Áª¯¸ */
char status_chk(int key,int x,int y,char *map) {
  char clrSw=NULL;
  if(toupper(key)==0x47) {
    Player--;
    if (Player>0) {
      clrSw=S_MISS;
    } else {
      clrSw=S_OVER;
    }
  } else {
    /* ½ÍÞÃÉ ÀÍÞÓÉ ¦ ¼­Ä¸¼ÀÊÞ±² ¸Ø± */
    if(Food<=0) {
      Score+=1000000/(unsigned long)(Step+(Step<=0));
      Stage++;
      /* »²ÀÞ²½Ã°¼Þ ¦ º´À× ´ÝÃÞ¨Ý¸Þ */
      if (Stage<=MAX_STAGE) {
        clrSw=S_CLEAR;
      } else {
        clrSw=S_END;
      }
    }
  }
  return clrSw;
}
/* À²ÄÙ */
void title() {
  int key;
  /* ¶¸¼­¾¯Ã² ¦ ÒÓØ ¶× Ì¸¹ÞÝ */
  Sound=peek(*adr_s);
  Waittime=(int)peek(*(adr_s+1))*10;
  /* Ë®³¼Þ */
  clrscr();
  map_put(0,0,C_FOOD);
  map_put(2,0,C_FOOD+1);
  map_put(4,0,C_FOOD+2);
  map_put(6,0,C_FOOD+3);
  gotoxy(8,0);
  printf("Furu¥Fie");
  map_put(17,0,C_FOOD+4);
  map_put(19,0,C_FOOD+5);
  map_put(21,0,C_FOOD+6);
  map_put(23,0,C_FOOD+7);
  gotoxy(0,2);
  printf("SPACE=START");
  gotoxy(0,3);
  printf("82=STAGE ");
  printf("%2d",Stage);
  gotoxy(13,2);
  printf("S=SOUND");
  gotoxy(21,2);
  if(Sound<0||Sound>1)
    Sound=0;
  if(Sound) {
    printf(ON);
  } else {
    printf(OFF);
  }
  if(Waittime<=0||Waittime>990)
    Waittime=500;
  gotoxy(13,3);
  printf("+-=SPEED ");
  printf("%2d",100-Waittime/10);
  gcursor(6*6,5*8+8-1);
  gprint(C_etc);
  gotoxy(8,5);
  printf("2024 Kenoh");
  do {
    key=getch();
    if(toupper(key)==0x53) {
      /* »³ÝÄÞ ·Ø¶´ */
      Sound=!Sound;
      gotoxy(21,2);
      if(Sound) {
        printf(ON);
        beep(49,80,1);
        beep(30,200,1);
      } else {
        printf(OFF);
      }
    } else if(key==0x2b||key==0x2d) {
      /* ÏÁ¼Þ¶Ý ¾¯Ã² */
      Waittime+=(-(key==0x2b)+(key==0x2d))*10;
      if(Waittime<10) {
        Waittime=10;
      }
      if(Waittime>990) {
        Waittime=990;
      }
      gotoxy(22,3);
      printf("%2d",100-Waittime/10);
    } else if(key==0x32||key==0x38) {
      /* ½Ã°¼Þ ¾ÝÀ¸ */
      Stage+=(-(key==0x32)+(key==0x38));
      if(Stage<1) {
        Stage=1;
      }
      if(Stage>MAX_STAGE) {
        Stage=MAX_STAGE;
      }
      gotoxy(9,3);
      printf("%2d",Stage);
    }
  } while(key!=0x20);
  /* ¼®·¾¯Ã² */
  Score=0;
  Player=3;
  /* Ê²½º± ¦ ÒÓØ ¶× Ì¸¹ÞÝ */
  HiScore=(unsigned long)peek(*(adr_s+2))*0x10000+(unsigned long)peek(*(adr_s+3))*0x100+(unsigned long)peek(*(adr_s+4));
  /* ¶¸¼­¾¯Ã² ¦ ÒÓØ Í ·µ¸ */
  poke(*adr_s,Sound);
  poke(*(adr_s+1),Waittime/10);
}
/* ¹Þ°Ñ */
char game() {
  int key;
  int x,y;
  int x1,y1;
  int x2=0;
  int y2=0;
  char moveChk;
  char statusSw=0;
  char *map=malloc(sizeof(char)*(MAX_Y*MAX_X));
  Step=0;
  /* ¼®·Ë®³¼Þ */
  clrscr();
  stageRead(map,Stage);
  map_draw(map,MAX_Y,MAX_X,&x,&y);
  if(Sound)
    bgm_play(*(adr_d+4),9,16);
  /* ¹Þ°ÑÒ²Ý */
  do {
    /* ¿³»ÊÝÃ² */
    key=inkey();
    player_move_chk(key,x,y,&x1,&y1,&x2,&y2,map,MAX_Y,MAX_X);
    if(x1||y1) {
      player_move(&x,&y,x1,y1,map,MAX_Y,MAX_X);
      statusSw = status_chk(key,x,y,map);
      if(statusSw)
        break;
    } else if(key==0x20) {
      moveChk=block_move_chk(x,y,x2,y2,map,MAX_X);
      switch (moveChk) {
        case 1:
          block_move(x+x2,y+y2,map,MAX_X);
          break;
        case 2:
          block_broke(x+x2,y+y2,map,MAX_X);
          break;
      }
    }
    /* ½Ã°¼Þ¸Ø±¥·ÞÌÞ±¯Ìß ÏÃÞ ¹²¿Þ¸ */
    if(statusSw==NULL)
      statusSw = status_chk(key,x,y,map);
  } while(!statusSw);
  /* ½Ã°À½ Ë®³¼Þ */
  switch(statusSw) {
    case S_MISS:
    case S_OVER:
      gotoxy(4,2);
      printf(" GIVE UP ! ");
      if(Sound)
        bgm_play(*(adr_d+6),9,16);
      break;
    case S_CLEAR:
    case S_END:
      gotoxy(2,2);
      printf(" STAGE CLEAR ! ");
      if(Sound)
        bgm_play(*(adr_d+5),9,16);
      break;
  }
  stopEnter();
  free(map);
  return statusSw;
}
/* ¼­³Ø®³ */
void end(char statusSw) {
  int key;
  clrscr();
  gotoxy(5,3);
  printf("SCORE %7lu",Score);
  if(Score>HiScore) {
    HiScore=Score;
    /* Ê²½º± ¦ ÒÓØ Í ·µ¸ */
    poke(*(adr_s+2),HiScore/0x10000);
    poke(*(adr_s+3),HiScore/0x100);
    poke(*(adr_s+4),HiScore%0x100);
  }
  gotoxy(3,4);
  printf("HISCORE %7lu",HiScore);
  switch(statusSw) {
    case S_OVER:
      gotoxy(7,1);
      printf("GAME OVER");
      if(Sound)
        bgm_play(*(adr_d+7),6,16);
      break;
    case S_END:
      gotoxy(3,1);
      printf("CONGRATULATIONS !!");
      if(Sound)
        bgm_play(*(adr_d+8),7,16);
      Stage=1;
      break;
  }
  stopEnter();
}
/* Ò²Ý */
 void main() {
  char statusSw;
  init();
  while(1) {
    title();
    do {
      statusSw=game();
    } while(statusSw==S_CLEAR||statusSw==S_MISS);
    end(statusSw);
  }
}
