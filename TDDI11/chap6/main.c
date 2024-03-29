#include <mtc.h>
#include <dos.h>
#include <libepc.h>

#include "serial.h"

/* Function prototypes for the threads */
void OutBound(void) ;
void InBound(void) ;
void DisplayElapsedTime(void) ;

PRIVATE void SerialInit(void) ;

QUEUE *inbound_queue ;

int main()
{
  inbound_queue = QueueCreate(sizeof(char), 20) ;

  ClearScreen(0x07) ;
  SetCursorVisible(TRUE) ;
  SerialInit() ;

  MtCCoroutine(OutBound()) ;
  MtCCoroutine(InBound()) ;
  MtCCoroutine(DisplayElapsedTime());


  return 0 ;
}


void DisplayElapsedTime(void)
{
  //thread-specific initialization (if any) goes here;
  DWORD32 timer;
  WINDOW *w;
  w = WindowCreate("Time since program start", 22, 24, 0, 79);

  for( ;;)
  {
    //timer := 1 second;
    timer = Now_Plus(1);

    DWORD32 tmp = Milliseconds() / 1000; // Convert to seconds
    DWORD32 sec = tmp % 60;              //compute hh:mm:ss;
    DWORD32 min = (tmp / 60) % 60;
    DWORD32 hor = (tmp / 3600) % 24;

    //display hh:mm:ss
    WindowSelect(w);
    PutUnsigned(hor, 10, 2);
    PutString(":");
    PutUnsigned(min, 10, 2);
    PutString(":");
    PutUnsigned(sec, 10, 2);

    while (Milliseconds() < timer)
    {
      MtCYield(); //yield to other threads;
    }
  }
}

PRIVATE void SerialInit(void)
{
  /* Disable interrupts during initialization */
  disable() ;

  /* 9600 baud */
  outportb(SER_LCR, SER_LCR_DLAB) ;
  outportb(SER_DLO, 12) ;
  outportb(SER_DHI, 0) ;

  /* 8 data bits, even parity, 1 stop bit */
  outportb(SER_LCR, 0x1B) ;

  /* Enable only receiver data ready interrupts */
  outportb(SER_IER, 0x01) ;

  /* Request to send, data terminal ready, enable interrupts */
  outportb(SER_MCR, SER_MCR_RTS|SER_MCR_DTR|SER_MCR_OUT2) ;

  /* Store address of ISR in IDT */
  SetISR(IRQ2INT(SER_IRQ), SerialISR) ;

  /* Unmask the UART's IRQ line */
  outportb(0x21, inportb(0x21) & ~SER_MSK) ;

  /* Re-enable interrupts */
  enable() ;
}
