# -*- coding: utf-8 -*-

import sys, time, getopt, re
from threading import Thread
from Queue import Queue
from cPickle import dump, load
from OSC import OSCClient, OSCMessage, OSCClientError, OSCServer, getUrlStr
from nltk import pos_tag, word_tokenize
from twython import TwythonStreamer

## What to search for
SEARCH_TERMS = ["@nottoopublic", "#nottoopublic"]

class TwitterStreamReceiver(TwythonStreamer):
    def __init__(self, *args, **kwargs):
        super(TwitterStreamReceiver, self).__init__(*args, **kwargs)
        self.tweetQ = Queue()
    def on_success(self, data):
        ## no re-tweets
        if ('text' in data) and (not data['text'].startswith(('rt','RT'))):
            self.tweetQ.put(data['text'].encode('utf-8'))
            print "received %s" % (data['text'].encode('utf-8'))
    def on_error(self, status_code, data):
        print status_code
    def empty(self):
        return self.tweetQ.empty()
    def get(self):
        return self.tweetQ.get()

def oscSubscribeHandler(addr, tags, args, source):
    ip = getUrlStr(source).split(":")[0]
    port = int(args[0])
    print "subscribing %s:%s" % (ip,port)
    myOscSubscribers[(ip,port)] = (ip,port)

def setup():
    global lastTwitterCheck, myTwitterStream, streamThread
    global myOscSubscribers, myOscServer, oscThread, myOscClient
    secrets = {}
    myOscSubscribers = {}
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

    myOscClient = OSCClient()
    myOscServer = OSCServer(('127.0.0.1', 8888))
    myOscServer.addMsgHandler('/NotTooPublic/subscribe', oscSubscribeHandler)
    myOscServer.addMsgHandler('default', lambda addr, tags, args, source:None)
    oscThread = Thread(target=myOscServer.serve_forever)
    oscThread.start()

def loop():
    global lastTwitterCheck, myTwitterStream, streamThread, myOscSubscribers
    ## check twitter queue
    if((time.time()-lastTwitterCheck > 10) and (not myTwitterStream.empty())):
        tweet = myTwitterStream.get().lower()
        ## removes hashtags, arrobas and links
        tweet = re.sub(r'(#\S+)|(@\S+)|(http://\S+)', '', tweet)
        ## removes punctuation
        tweet = re.sub(r'[.,;:!?*/+=\-&%^/\\_$~()<>{}\[\]]', ' ', tweet)
        ## removes some bad words
        tweet = re.sub(r'(f *u *c *k)', 'tuck', tweet)
        tweet = re.sub(r'(s *h *i *t)', 'isht', tweet)
        tweet = re.sub(r'(c *o *c *k)', 'dock', tweet)
        tweet = re.sub(r'(d *i *c *k)', 'wick', tweet)
        tweet = re.sub(r'(c *u *n *t)', 'grunt', tweet)
        tweet = re.sub(r'(p *u *s *s *y)', 'juicy', tweet)
        tweet = re.sub(r'(b *i *t *c *h)', 'itch', tweet)
        tweet = re.sub(r'(a *s *s)', 'grass', tweet)
        ## replaces double-spaces with single space
        tweet = re.sub(r'( +)', ' ', tweet)
        taggedTweet = pos_tag(word_tokenize(tweet))
        for (word,tag) in taggedTweet:
            print "%s : %s" % (word,tag)

        ## forward to all subscribers
        msg = OSCMessage()
        msg.setAddress("/NotTooPublic/message")
        msg.append(" ".join([str(i[0]) for i in taggedTweet]))
        msg.append(" ".join([str(i[1]) for i in taggedTweet]))
        for (ip,port) in myOscSubscribers:
            try:
                myOscClient.sendto(msg, (ip, port))
            except OSCClientError:
                print "no connection to %s : %s, can't send message" % (ip, port)

        lastTwitterCheck = time.time()

if __name__=="__main__":
    setup()

    try:
        while(True):
            ## keep it from looping faster than ~60 times per second
            loopStart = time.time()
            loop()
            loopTime = time.time()-loopStart
            if (loopTime < 0.016):
                time.sleep(0.016 - loopTime)
    except KeyboardInterrupt :
        myTwitterStream.disconnect()
        myOscServer.close()
        streamThread.join()
        oscThread.join()
