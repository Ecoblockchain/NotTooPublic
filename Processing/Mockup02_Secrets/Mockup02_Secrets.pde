class Secret {
  private String thePhrase;
  private int importantWordIndex; 
  public Secret(String s, int viw) {
    importantWordIndex = viw;
    thePhrase = s;
  }
  public int getImportantIndex(){ return importantWordIndex;}
  public String getPhrase(){ return thePhrase;}
};

ArrayList<Secret> secrets;
String[] words;
int currentImportantWordIndex;
long lastRestart;
float fontSize;
int numWordsPlaced;

void setup() {
  size(800, 600);
  secrets = new ArrayList<Secret>();
  secrets.add(new Secret("Don't Tell Anyone", -1));
  secrets.add(new Secret("Ko Has Tons of Underwear",4));
  secrets.add(new Secret("Don't Tell Anyone", -1));
  secrets.add(new Secret("I steal office supplies",3));
  secrets.add(new Secret("Don't Tell Anyone", -1));
  secrets.add(new Secret("I pooped",1));
  secrets.add(new Secret("Don't Tell Anyone", -1));
  secrets.add(new Secret("I had sex with my boss's wife for money",8));
  //textFont(createFont("Helvetica", height));
  textFont(createFont("TrajanPro-Regular", height));

  smooth();
  frameRate(60);
  restart();
}

void draw() {
  long timeNow = millis();
  pushMatrix();
  for (int i=0; i<words.length; i++) {
    pushMatrix();
    float xoff = max(textWidth("I"), min(abs((width-textWidth(words[i]))/2), textWidth(words[i])/2));
    translate((width-textWidth(words[i]))/2, (i+1)*textAscent()+textDescent());
    if (random(1) < 0.5) {
      numWordsPlaced++;
      if ((abs(timeNow-lastRestart-9000) < 255) && (currentImportantWordIndex != i)) {
        fill(255, 8);
        text(words[i], 0, 0);
      }
      else if ((timeNow-lastRestart) > 13000) {
        restart();
      }
      else {
        fill(255, 4);
        text(words[i], random(-xoff, xoff), 0);
      }
    }
    popMatrix();
  }
  popMatrix();

  if ((timeNow-lastRestart) > 12000) {
    fill(0, 16);
    rect(0, 0, width, height);
  }
}

void restart() {
  Secret secret = secrets.remove(0);
  words = secret.getPhrase().split(" ");
  currentImportantWordIndex = secret.getImportantIndex();
  secrets.add(secret);
  fontSize = height/(words.length*1.1);
  textSize(fontSize);
  background(0);
  println(numWordsPlaced);
  numWordsPlaced = 0;
  lastRestart = millis();
}

void keyPressed() {
  if (key == ' ')
    restart();
}
void mousePressed() {
  restart();
}

