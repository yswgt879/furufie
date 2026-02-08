/*
  Program ¶¸Á®³¶Ý½³ ×²ÌÞ×Ø
  File    extlib.h
  Create  2024/05/18
  Update  2024/06/21
  Version 1.0.2.0
  By      Kenoh
*/
/* µ»ÚÀ ·° É Å²Ö³ ¦ ÖÐºÑ*/
int inkey() {
  static char *p="ÍS¾ÍV¾oÉ";
  return call((unsigned)p,0);
}
/* ¼Ã²¼À ¼Þ¶Ý ÃÞ ÌßÛ¸Þ×Ñ ¦ Ã²¼ */
void wait(int t) {
  int i;
  for(i=0;i<t;i++) {
  }
}
/* ENTER¥SPACE µ³¶ÏÃÞ Ã²¼ */
void stopEnter() {
  int key;
  do {
    key=getch();
  } while(key!=0x0a&&key!=0x20);
}
/* BGM »²¾² */
void bgm_play(int adr,int len,int temp) {
  char i;
  int b;
  temp*=2;
  for(i=0;i<len;i++) {
    b=peek(adr+len+i);
    beep(b,5200000/(166+22*b)/peek(adr+i),1);
    wait(3000/temp);
  }
}
/* ÊÞ²ÅØ ¶× ASCIIº°ÄÞ Í ÍÝ¶Ý */
unsigned char hexToAsc(unsigned char bin) {
  if(bin<=0x09) {
    bin+=0x30;
  } else {
    bin+=0x37;
  }
  return bin;
}
