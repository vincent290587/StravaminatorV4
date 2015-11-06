void basicTone() {
  noTone(speakerPin); 
  tone(speakerPin, 6440, 250);
}

void basicShort() {
  noTone(speakerPin); 
  tone(speakerPin, 6440, 200);
}

void basicLong() {
  noTone(speakerPin); 
  tone(speakerPin, 6440, 500);
}

void otherTone() {
  noTone(speakerPin); 
  tone(speakerPin, 6800, 250);
}


void victoryTone () {
  
  //int delay_=0;
  
  basicLong();
}

void segStartTone () {

  //int delay_=0;
  
  basicTone();
}

void segEndTone () {

  //int delay_=0;
  
  basicLong();
}




