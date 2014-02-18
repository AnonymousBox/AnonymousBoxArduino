import MultipartPostHandler, urllib2, cookielib
def uploadeverything(PARAMS, filedir):
    cookies = cookielib.CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies),
              MultipartPostHandler.MultipartPostHandler)
    PARAMStosend = params
    PARAMStosend["file"] = open(filedir, "rb")
    print PARAMStosend
    opener.open("http://127.0.0.1:3000/post", PARAMStosend)
#   PARAMStosend = params
#   PARAMStosend["file"] = open(filename, "rb")
#   print PARAMStosend
#   cookies = cookielib.CookieJar()
#   opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies), MultipartPostHandler.MultipartPostHandler)
#   opener.open("http://127.0.0.1:3000/post", PARAMStosend)




