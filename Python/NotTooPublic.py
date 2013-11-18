# -*- coding: utf-8 -*-

import sys, time, getopt
from Queue import Queue
from threading import Thread
from json import loads, dumps
from cPickle import dump, load
from nltk import pos_tag, word_tokenize
from OSC import OSCClient, OSCMessage, OSCClientError
from twython import TwythonStreamer

## What to search for
SEARCH_TERMS = ["#aeLab", "#aeffect", "#aeffectLab", "#nottoopublic"]

class TwitterStreamReceiver(TwythonStreamer):
    """A class for receiving a Twitter stream"""
    def __init__(self, *args, **kwargs):
        super(TwitterStreamReceiver, self).__init__(*args, **kwargs)
        self.tweetQ = Queue()
    def on_success(self, data):
        if 'text' in data:
            self.tweetQ.put(data['text'].encode('utf-8'))
            print "received %s" % (data['text'].encode('utf-8'))
    def on_error(self, status_code, data):
        print status_code
    def empty(self):
        return self.tweetQ.empty()
    def get(self):
        return self.tweetQ.get()

def setup():
    global secrets, lastTwitterCheck, myTwitterStream, enTagger, streamThread
    secrets = {}
    lastTwitterCheck = time.time()
    ## read secrets from file
    inFile = open('oauth.txt', 'r')
    for line in inFile:
        (k,v) = line.split()
        secrets[k] = v
    myTwitterStream = TwitterStreamReceiver(app_key = secrets['CONSUMER_KEY'],
                                            app_secret = secrets['CONSUMER_SECRET'],
                                            oauth_token = secrets['ACCESS_TOKEN'],
                                            oauth_token_secret = secrets['ACCESS_SECRET'])
    streamThread = Thread(target=myTwitterStream.statuses.filter, kwargs={'track':','.join(SEARCH_TERMS)})
    streamThread.start()

def loop():
    global secrets, lastTwitterCheck, myTwitterStream, enTagger, streamThread
    ## check twitter queue
    if((time.time()-lastTwitterCheck > 10) and
       (not myTwitterStream.empty())):
        tweet = myTwitterStream.get().lower()
        for st in SEARCH_TERMS:
            tweet = tweet.replace(st,"")
        tweet = tweet.replace(",","").replace(".","").replace("?","").replace("!","")
        for (word,tag) in pos_tag(word_tokenize(tweet)):
            print "%s : %s" % (word,tag)

        ## TODO: forward somewhere
        lastTwitterCheck = time.time()

if __name__=="__main__":
    (inIp, inPort) = ("127.0.0.1", 8888)
    opts, args = getopt.getopt(sys.argv[1:],"i:p:",["inip=","inport="])
    for opt, arg in opts:
        if(opt in ("--inip","-i")):
            inIp = str(arg)
        elif(opt in ("--inport","-p")):
            inPort = int(arg)

    ## setup the twitter stream
    setup()

    ## run loop
    try:
        while(True):
            ## keep it from looping faster than ~60 times per second
            loopStart = time.time()
            loop()
            loopTime = time.time()-loopStart
            if (loopTime < 0.016):
                time.sleep(0.016 - loopTime)
    except KeyboardInterrupt :
        print "exiting, bye!"
        myTwitterStream.disconnect()
        streamThread.join()
