

class ReaderThread : public QThread {
void run(){
    char * bp;
    while(1){
      bp = readSock(sd);
      //mw->setText(bp);
      //SIGNAL(setText(bp));
      emit setText(bp);
      printf ("%s\n", bp);
      fflush(stdout);
    }
}
    signals:
        void seText(QString text);
};
