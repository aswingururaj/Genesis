
/* Program to write RMI */

#include <avr/io.h>
#include<stdlib.h>
#include<avr/interrupt.h>
#include<math.h>


int cnt=0,chkm=0,j;
char a,c,ch;
int i,k=0,a1o=0,a2o=0; 
int param[4]={0,0,0,0}; 
int p;
int flag=0; 
float l1=15,l2=15; 
int val_a,val_b;
int sall;

void straight(float c,float d,float a,float b)
{
  c=2*c/3;
  d=2*d/3;
  a=2*a/3;
  b=2*b/3;
  //float c=10,d=10;
  ik(c,d);
  float X=a-c;
  float sw=0.5;
  int n=X/sw;
  int q=c;
  if(X==0)
  {
    float Y = b- d;
    n = Y/sw;
    if(n>0)
    {
     for(int h=0;h<n;h++)
     {
      float y1 = d + (h*sw);
      Serial.println(c);
      Serial.println(y1);
      ik(c,y1);
     }
    }
    else
    {
      for(int h=0;h>n;h--)
      {
        float y1 = d + (h*sw);
        Serial.println(c);
        Serial.println(y1);
        ik(c,y1);
      }
    }
  }
  else
  {
    float m=(b-d)/(a-c);
    if(n>0)
    {
      for(int h=0;h<n;h++)
      {
      float x1=q+(h*sw);
      float y1=m*(x1-c)+d;
      Serial.println(x1);
      Serial.println(y1);
      ik(x1,y1);
      }
    }
    else
    {
      for(int h=0;h>n;h--)
      {
        float x1=q+(h*sw);
        float y1=m*(x1-c)+d;
        Serial.println(x1);
        Serial.println(y1);
        ik(x1,y1); 
      }
    }
  }
  ik(a,b);
}
void angle(float a,float b)
{
  a+=90;
  val_a=round((110+((422.0/180.0)*a))*(76.0/90.0)*(32.0/30.0));//PLEASE NOTE THE CORRECTION FACTORS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  val_b=round(110+((425.0/180.0)*b)*(75.0/90.0));
  OCR1A=val_a;
  OCR1B=val_b;
}
void sweep(int a1,int a2){
  int rot1,rot2;
  if(a1o>a1){
    rot1=0;
  }
  else rot1=1;
  if(a2o>a2){
    rot2=0;
  }
  else rot2=1;
  i=a1o;
  j=a2o;
  while(!((i==a1)&&(j==a2))){
    if(rot1==0&&rot2==0){
      if(i>a1){
        i--;
      }
      if(j>a2){
        j--;
      }
    }
    if(rot1==0&&rot2==1){
      if(i>a1){
        i--;
      }
      if(j<a2){
        j++;
      }
    }
    if(rot1==1&&rot2==0){
      if(i<a1){
        i++;
      }
      if(j>a2){
        j--;
      }
    }
    if(rot1==1&&rot2==1){
      if(i<a1){
        i++;
      }
      if(j<a2){
        j++;
      }
    }
    angle(i,j);
    _delay_ms(50);
  }
  a1o = a1;
  a2o = a2;
}


void ik(float a,float b)
{    
  float A1=0,A2=0,cA2,sA2; 
  float u=0,l=0,m=0,n=0; 
  if(a==0&&b==0)
     flag=1;
 else
  {    
    u=-(l2*l2)-(l1*l1)+(a*a)+(b*b);
    l=(2*l1*l2);
   // A2= acos(u/l);    //// it will always return + ve, cuz cos(-Q)=cos(Q)
    cA2=(u/l);
    sA2=sqrt(1-((u*u)/(l*l)));
   //A2=atan2(sA2,cA2);
    A2=atan2(sA2,cA2);
               
    if(a==0)
      m=3.14/2;
    else
      m=atan2(b,a);

                      
    
    n=atan2((l2*sin(A2)),(l1+(l2*cos(A2))));   
    A1= m-n;
    A2=A2*180/3.14;   
    A1=A1*180/3.14;                  
    

    A1=round(A1);
    A2=round(A2);
        
   if(A1>90||A1<(-90)||A2>180||A2<0)
      flag=1;
} 
 
  if(flag==1)  //// if coor values are inappropriate
    {
      char str[]="\n out of reach \n";
      for(j=0;str[j]!='\0';j++)
      {
        ch=str[j];
        UART_send(ch);
      }
      flag=0; 
    }
    
  else    //// if angle values are available
    {  
        sweep(A1,A2);
        UART_send((A1/10+'0'));
        UART_send(((int)A1%10+'0'));
        UART_send(',');
        UART_send((A2/10+'0'));
        UART_send(((int)A2%10+'0'));
        UART_send(' ');   
                          
    }
}

void add(char a)
{
    chkm++;
    if(a==',')
    {        
        k++;
    }
    else if(a!=':')
    {
       p=a;
       param[k]=(param[k]*10)+(p-48); //ASCII TO DECIMAL       
    }
    else if(a==':')      
    {       
       straight(param[0],param[1],param[2],param[3]);
       for(i=0;i<3;i++)
         param[i]=0;
       k=0; 
       chkm=0;      
    }
}

void UART_init()
{
  UBRR0=103;  // BAUD RATE
  UCSR0B|=(1<<RXEN0)|(1<<TXEN0);  //ENABLING RECIEVE INTERRUPT ENEBLE,TRANSMITTER AND RECIEVER
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);//8BIT TRANSFER
}

char UART_receivechar()
{
  char ch;
  while(UCSR0A!= (UCSR0A|(1<<RXC0)));
  ch=UDR0;
  return(ch);  
}

void UART_send(char X)
{
  while(!(UCSR0A&(1<<UDRE0)));
  UDR0=X;
  UCSR0A|=1<<UDRE0;
}


void timer_init()
{
  DDRB |=(1<<PB1)|(1<<PB2);
  TCCR1A |= (1<<WGM11) |(1<<COM1A1)|( 1<<COM1B1);
  TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11)|(1<<CS10);//Prescalar 64
  ICR1=4999;
}


int main(void)
{
  sei();
  UART_init();   
  timer_init();
  
  while (1)
  {
    straight(0,12,0,12);
    straight(0,12,0,24);
    straight(0,24,6,24);
    straight(6,24,6,18);
    straight(6,18,0,18);
    straight(0,18,6,12);
    straight(6,12,9,12);
    straight(9,12,9,24);
    straight(9,24,15,18);
    straight(15,18,21,24);
    straight(21,24,21,12);
    straight(21,12,24,12);
    straight(24,12,30,12);
    straight(30,12,27,12);
    straight(27,12,27,24);
    straight(27,24,24,24);
    straight(24,24,30,24);
    straight(30,24,24,24);
    break;
    /*if(chkm==0) /// it is 0 when the conversion of coor to angle is done and given to servo
    {
      char str[]="\nEnter the coordinates: (x,y:)  \n";
      for(j=0;str[j]!='\0';j++)
      {
        ch=str[j];
        UART_send(ch);
      }
    }
    c=UART_receivechar();
    add(c);*/
  }
  
  return 0;
  
} /// end of mai
