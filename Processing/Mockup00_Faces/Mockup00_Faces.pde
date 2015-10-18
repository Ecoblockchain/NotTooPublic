String url = "http://api.tumblr.com/v2/tagged?tag=selfie&api_key=SVYcV0UUSSgF5CsuYkiNc7KSfEaRqsYuwEgtr3GYG0Owqb5lsw";
int oldestPost = -1;
long lastRedraw = 0;
long lastRequest = 0;
ArrayList<PImage> photos;

int getPics(String url) {
  return getPics(url, -1, 20);
}
int getPics(String url, int before) {
  return getPics(url, before, 20);
}

int getPics(String url, int before, int limit) {
  println("beef: "+before);
  url = url+((before > -1)?("&before="+before):(""));
  url = url+((limit > 0)?("&limit="+limit):(""));
  int returnValue = -1;
  JSONArray response = loadJSONObject(url).getJSONArray("response");
  for (int i=0; i<response.size(); i++) {
    JSONObject post = response.getJSONObject(i);
    returnValue = post.getInt("timestamp");
    if (post.getString("type").equals("photo")) {
      JSONObject original_size = post.getJSONArray("photos").getJSONObject(0).getJSONObject("original_size");
      int originalWidth = original_size.getInt("width");
      int originalHeight = original_size.getInt("height");
      if (originalWidth == originalHeight) {
        photos.add(loadImage(original_size.getString("url")));
      }
    }
  }
  return returnValue;
}

void setup() {
  size(1280, 800, P2D);
  background(255);
  textFont(createFont("ArialRoundedMTBold", 64));
  frameRate(30);
  photos = new ArrayList<PImage>();
  while (photos.size () < 1) {
    oldestPost = getPics(url, oldestPost, 1);
  }
}

void draw() { 
  int period = (photos.size() < 200)?1000:500;
  if (millis()-lastRedraw > period) {
    background(0);
    float c = sqrt(float(photos.size())*float(width)/(float)(0.8*height));
    float r = photos.size()/c;
    int cols = round(c);
    int rows = round(r);
    int pw = min(width/cols, (int)(0.8*height/rows));
    pushMatrix();
    translate(0, 0.2*height);
    translate(abs(width-pw*cols)/2, abs(0.8*height-pw*rows)/2);

    for (int i=0; i<photos.size(); i++) {
      PImage p = photos.get(i);
      p.resize(pw, 0);
      image(p, (i%cols)*pw, (i/cols)*pw);
    }

    /*
    for (int i=0, b=0; i<photos.size(); i++) {
      if ((random(1.0)+float(photos.size())/512) > 1.0) {
        PImage p = photos.get(i);
        int sizeMult = int(random(2, min(4, min(cols, rows))));
        pushMatrix();
        translate((i%cols)*pw, (i/cols)*pw);
        scale((float)(pw*sizeMult)/p.width);
        image(p, 0, 0);
        popMatrix();
        i += sizeMult*cols;
      }
    }
    */
    popMatrix();
    println(photos.size());
    lastRedraw = millis();
  }
  else if ((photos.size() < 512) && (millis()-lastRequest > 1000)) {
    lastRequest = millis();
    oldestPost = getPics(url, oldestPost, 4);
  }
  pushStyle();
  noStroke();
  fill(0);
  rect(0, 0, width, 0.2*height);
  popStyle();
  textSize(64);

  float textHeight = textAscent()+textDescent();
  //text("OAKLAND, SHOW US YOUR FACE!", 30, textHeight);
  textSize(48);
  //text(frameRate, 30, 2*textHeight);
  //text("@NotTooPublic", width-textWidth("@NotTooPublic")-20, 2*textHeight);
}

