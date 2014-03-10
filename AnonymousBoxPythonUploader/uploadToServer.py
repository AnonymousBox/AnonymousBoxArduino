import MultipartPostHandler, urllib2, cookielib
def uploadeverything(params, filedir):
    cookies = cookielib.CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies),
              MultipartPostHandler.MultipartPostHandler)
    paramstosend = params
    paramstosend["file"] = open(filedir, "rb")
    print paramstosend
    opener.open("http://127.0.0.1:3000/post", paramstosend)
#   paramstosend = params
#   paramstosend["file"] = open(filename, "rb")
#   print paramstosend
#   cookies = cookielib.CookieJar()
#   opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies), MultipartPostHandler.MultipartPostHandler)
#   opener.open("http://127.0.0.1:3000/post", paramstosend)




