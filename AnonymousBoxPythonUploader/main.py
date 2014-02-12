import MultipartPostHandler, urllib2, cookielib
params = {"message" :"hjeres mfdasf", "file": open("/home/yggdrasil/Pictures/AiHjHYL.jpg")}
opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookies), MultipartPostHandler.MultipartPostHandler)
opener.open("http://127.0.0.1:3000/post", params)


