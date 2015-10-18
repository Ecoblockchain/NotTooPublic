import java.util.Arrays;

String[] past = {
  "made", "did", "ate", "killed", "stole", "slept"
};
String[] noun = {
  "friends", "car", "pastries", "money", "wife"
};

String[] phrases = {
  "I am a superhero", 
  "I did it in your mom's car", 
  "I ate all the pastries", 
  "I slept with my boss's wife for money"
};

String DFTA = "Don't\nFucking Tell\nAnyone!";

String[] currentPhrase;

int currentPhraseCount, currentWordCount, maxWordIndex;
int currentVerbCount;

long nextUpdate;
long UPDATEPERIOD = 500;
boolean showDFTA, showVerb;

float alpha;

void setup() {
  size(1024, 600);
  textFont(createFont("Helvetica", 500));
  currentPhraseCount = currentWordCount = currentVerbCount = 0; 
  currentPhrase = phrases[currentPhraseCount].split(" ");
  maxWordIndex = currentPhrase.length;
  nextUpdate = millis();
  nextUpdate += UPDATEPERIOD;
  showDFTA = false;
  showVerb = false;
  alpha = 0;
}

void draw() {
  background(0);

  if (millis() > nextUpdate) {
    nextUpdate += UPDATEPERIOD;

    if (showVerb) {
      currentVerbCount++;
      if (currentVerbCount >= past.length) {
        showVerb = false;
        currentWordCount++;
        alpha = 0;
      }
    }
    else {
      currentWordCount++;
      alpha = 0;
    }

    // new phrase
    if (currentWordCount == maxWordIndex) {
      showDFTA = true;
      nextUpdate += 400;
      currentWordCount = 0;
      currentPhraseCount = (currentPhraseCount+1)%(phrases.length);
      currentPhrase = phrases[currentPhraseCount].split(" ");
      maxWordIndex = currentPhrase.length;
    }
    else if (showDFTA) {
      showDFTA = false;
      currentWordCount = 0;
    }

    if (!showVerb && Arrays.asList(past).contains(currentPhrase[currentWordCount])) {
      showVerb = true;
      currentVerbCount = 0;
    }
  }

  // have valid word
  if (showDFTA) {
    textSize(170);
    fill(255,alpha);
    text(DFTA, 0, 0, width, height+50);
  }
  else {
    textSize(500);
    float td = textDescent();
    if (textWidth(currentPhrase[currentWordCount]) > width) {
      textSize(500/(textWidth(currentPhrase[currentWordCount])/width));
    }

    if (showVerb) {
      fill(255, min(alpha,100));
      text(currentPhrase[currentWordCount], (width-textWidth(currentPhrase[currentWordCount]))/2, height-td);
      for (int i=0; i<currentVerbCount; i++) {
        text(past[i], (width-textWidth(past[i]))/2, height-td);
      }
    }
    else {
      fill(255,alpha);
      text(currentPhrase[currentWordCount], (width-textWidth(currentPhrase[currentWordCount]))/2, height-td);
    }
  }
  
  alpha = min((alpha+16),255);
}

