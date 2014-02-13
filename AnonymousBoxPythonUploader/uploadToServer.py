import MultipartPostHandler, urllib2, cookielib
def uploadeverything(params, filedir):
    paramstosend = params
    paramstosend['file'] = filedir

    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies), MultipartPostHandler.MultipartPostHandler)
    opener.open("http://127.0.0.1:3000/post", paramstosend)




